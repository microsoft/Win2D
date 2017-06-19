// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "CanvasSvgDocument.h"
#include "CanvasSvgElement.h"
#include "CanvasSvgPaintAttribute.h"
#include "CanvasSvgPathAttribute.h"
#include "CanvasSvgPointsAttribute.h"
#include "CanvasSvgStrokeDashArrayAttribute.h"
#include "BufferStreamWrapper.h"

using namespace Microsoft::WRL::Wrappers;

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{

ActivatableStaticOnlyFactory(CanvasSvgDocumentStatics);

// static
ComPtr<CanvasSvgDocument> CanvasSvgDocument::CreateNew(ICanvasResourceCreator* resourceCreator, IStream* stream)
{
    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    auto deviceInternal = As<ICanvasDeviceInternal>(device);

    ComPtr<ID2D1SvgDocument> d2dSvgDocument = deviceInternal->CreateSvgDocument(stream);

    auto canvasSvgDocument = Make<CanvasSvgDocument>(
        device.Get(),
        d2dSvgDocument.Get());
    CheckMakeResult(canvasSvgDocument);

    return canvasSvgDocument;
}

CanvasSvgDocument::CanvasSvgDocument(
    ICanvasDevice* canvasDevice,
    ID2D1SvgDocument* d2dSvgDocument)
    : ResourceWrapper(d2dSvgDocument)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgDocument::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasSvgDocument::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}

IFACEMETHODIMP CanvasSvgDocument::GetXml(HSTRING* result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(result);

            auto& resource = GetResource();

            ComPtr<IStream> outputStream;
            ThrowIfFailed(CreateStreamOnHGlobal(nullptr, TRUE, &outputStream));

            ThrowIfFailed(resource->Serialize(outputStream.Get()));
            
            HGLOBAL globalHandle;
            ThrowIfFailed(GetHGlobalFromStream(outputStream.Get(), &globalHandle));

            void* lockedBytes = GlobalLock(globalHandle);
            if (!lockedBytes)
            {
                ThrowHR(E_INVALIDARG);
            }

            auto unlockBytes = MakeScopeWarden([&] { GlobalUnlock(globalHandle); });

            auto lockedBytesLength = GlobalSize(globalHandle);
            if (lockedBytesLength == 0)
            {
                ThrowHR(E_INVALIDARG);
            }
            
            // The string is returned from D2D in a multi-byte UTF-8 format. We need to convert it to a wide
            // character format to return it through the platform.

            // Note that the string pointed at by lockedBytes is not null terminated. Therefore, we need to 
            // pass the locked size to the function below.
            
            assert(lockedBytesLength <= INT_MAX);
            int wideCharacterCount = MultiByteToWideChar(
                CP_UTF8,
                0, // Default flags
                reinterpret_cast<char* const>(lockedBytes),
                static_cast<int32_t>(lockedBytesLength),
                nullptr,
                0); // Write nothing, and retrieve size

            WinStringBuilder stringBuilder;
            wideCharacterCount++; // Account for null terminator
            auto buffer = stringBuilder.Allocate(wideCharacterCount);

            if (MultiByteToWideChar(
                CP_UTF8,
                0, // Default flags
                reinterpret_cast<char* const>(lockedBytes),
                static_cast<int32_t>(lockedBytesLength),
                buffer,
                wideCharacterCount) == 0)
            {
                ThrowHR(E_INVALIDARG);
            }

            // MultiByteToWideChar does not null-terminate the string, so we do it ourselves here.
            buffer[wideCharacterCount - 1] = 0;

            stringBuilder.Get().CopyTo(result);
        });
}

IFACEMETHODIMP CanvasSvgDocument::SaveAsync(IRandomAccessStream* rawStream, IAsyncAction** resultAsyncAction)
{
    return ExceptionBoundary(
        [=]
        {
            CheckInPointer(rawStream);
            CheckAndClearOutPointer(resultAsyncAction);

            auto& resource = GetResource();

            ComPtr<IRandomAccessStream> randomAccessStream = rawStream;

            auto asyncAction = Make<AsyncAction>(
                [=]
                {
                    ComPtr<IStream> stream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));

                    ThrowIfFailed(resource->Serialize(stream.Get()));
                });

            CheckMakeResult(asyncAction);
            ThrowIfFailed(asyncAction.CopyTo(resultAsyncAction));
        });
}

IFACEMETHODIMP CanvasSvgDocument::put_Root(ICanvasSvgNamedElement* root)
{
    return ExceptionBoundary(
        [=]
        {
            CheckInPointer(root);

            auto& resource = GetResource();

            CanvasSvgNamedElement* implementation = static_cast<CanvasSvgNamedElement*>(root);

            VerifyDeviceBoundary(m_canvasDevice, implementation);

            resource->SetRoot(implementation->GetResource().Get());
        });
}

IFACEMETHODIMP CanvasSvgDocument::get_Root(ICanvasSvgNamedElement** result)
{
    return ExceptionBoundary(
        [=]
        {
            CheckAndClearOutPointer(result);
            
            auto& resource = GetResource();

            ComPtr<ID2D1SvgElement> d2dSvgElement;
            resource->GetRoot(&d2dSvgElement);

            auto& device = m_canvasDevice.EnsureNotClosed();

            auto rootElement = ResourceManager::GetOrCreate<ICanvasSvgElement>(device.Get(), d2dSvgElement.Get());
            ThrowIfFailed(rootElement.CopyTo(result));
        });
}


IFACEMETHODIMP CanvasSvgDocument::LoadElementFromXml(
    HSTRING xmlString,
    ICanvasSvgNamedElement** svgElement)
{ 
    return ExceptionBoundary(
        [=]
        {
            CheckAndClearOutPointer(svgElement);

            m_canvasDevice.EnsureNotClosed();

            ComPtr<IStream> stream = WrapSvgStringInStream(xmlString);
                        
            ComPtr<ICanvasSvgElement> newElement = CreateNewElementFromStream(this, stream.Get());
            ThrowIfFailed(newElement.CopyTo(svgElement));
        });
}

IFACEMETHODIMP CanvasSvgDocument::LoadElementAsync(
    IRandomAccessStream *rawStream,
    IAsyncOperation<CanvasSvgNamedElement*>** resultAsyncOperation)
{
    return ExceptionBoundary(
        [=]
        { 
            CheckInPointer(rawStream);
            CheckAndClearOutPointer(resultAsyncOperation);

            m_canvasDevice.EnsureNotClosed();

            ComPtr<IRandomAccessStream> randomAccessStream = rawStream;

            auto asyncOperation = Make<AsyncOperation<CanvasSvgNamedElement>>(
                [=]
                {
                    ComPtr<IStream> stream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));

                    auto svgElement = CreateNewNamedElementFromStream(this, stream.Get());
                    CheckMakeResult(svgElement);

                    return svgElement;
                });

            CheckMakeResult(asyncOperation);
            ThrowIfFailed(asyncOperation.CopyTo(resultAsyncOperation));
        });
}

IFACEMETHODIMP CanvasSvgDocument::FindElementById(HSTRING elementName, ICanvasSvgNamedElement** foundElement)
{ 
    return ExceptionBoundary(
        [=]
        { 
            CheckAndClearOutPointer(foundElement);

            auto& resource = GetResource();

            auto& device = m_canvasDevice.EnsureNotClosed();

            ComPtr<ID2D1SvgElement> d2dFoundElement;
            ThrowIfFailed(resource->FindElementById(GetStringBuffer(elementName), &d2dFoundElement));

            ComPtr<ICanvasSvgNamedElement> wrapped = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(device.Get(), d2dFoundElement.Get());
            ThrowIfFailed(wrapped.CopyTo(foundElement));
        });
}

void CanvasSvgDocument::CreatePaintAttributeImpl(D2D1_SVG_PAINT_TYPE d2dSvgPaintType, D2D1_COLOR_F d2dColor, wchar_t const* id, ICanvasSvgPaintAttribute** result)
{
    auto& resource = GetResource();

    auto& device = m_canvasDevice.EnsureNotClosed();

    ComPtr<ID2D1SvgPaint> d2dPaint;
    ThrowIfFailed(resource->CreatePaint(d2dSvgPaintType, d2dColor, id, &d2dPaint));

    auto newAttribute = Make<CanvasSvgPaintAttribute>(device.Get(), d2dPaint.Get());

    ThrowIfFailed(newAttribute.CopyTo(result));
}

IFACEMETHODIMP CanvasSvgDocument::CreatePaintAttributeWithDefaults(ICanvasSvgPaintAttribute** result)
{
    return ExceptionBoundary(
        [=]
        {
            CreatePaintAttributeImpl(D2D1_SVG_PAINT_TYPE_NONE, D2D1::ColorF(D2D1::ColorF::Black), L"", result);
        });
}

IFACEMETHODIMP CanvasSvgDocument::CreatePaintAttribute(CanvasSvgPaintType paintType, Color color, HSTRING id, ICanvasSvgPaintAttribute** result)
{
    return ExceptionBoundary(
        [=]
        {
            CreatePaintAttributeImpl(StaticCastAs<D2D1_SVG_PAINT_TYPE>(paintType), ToD2DColor(color), GetStringBuffer(id), result);
        });
}

void CanvasSvgDocument::CreatePathDataAttributeImpl(
    const float* segmentData, 
    uint32_t segmentDataCount, 
    D2D1_SVG_PATH_COMMAND const* commands, 
    uint32_t commandsCount, 
    ICanvasSvgPathAttribute** result)
{
    auto& resource = GetResource();

    auto& device = m_canvasDevice.EnsureNotClosed();

    ComPtr<ID2D1SvgPathData> d2dPathData;
    ThrowIfFailed(resource->CreatePathData(segmentData, segmentDataCount, commands, commandsCount, &d2dPathData));

    auto newAttribute = Make<CanvasSvgPathAttribute>(device.Get(), d2dPathData.Get());

    ThrowIfFailed(newAttribute.CopyTo(result));
}

IFACEMETHODIMP CanvasSvgDocument::CreatePathAttributeWithDefaults(ICanvasSvgPathAttribute** result)
{
    return ExceptionBoundary(
        [=]
        {
            CreatePathDataAttributeImpl(nullptr, 0, nullptr, 0, result);
        });
}

IFACEMETHODIMP CanvasSvgDocument::CreatePathAttribute(
    UINT32 segmentDataCount, 
    float* segmentData, 
    UINT32 commandCount, 
    CanvasSvgPathCommand* commands, 
    ICanvasSvgPathAttribute** result)
{
    return ExceptionBoundary(
        [=]
        {
            CreatePathDataAttributeImpl(segmentData, segmentDataCount, ReinterpretAs<D2D1_SVG_PATH_COMMAND const*>(commands), commandCount, result);
        });
}

void CanvasSvgDocument::CreatePointsAttributeImpl(
    D2D1_POINT_2F* points,
    uint32_t pointsCount,
    ICanvasSvgPointsAttribute** result)
{
    auto& resource = GetResource();

    auto& device = m_canvasDevice.EnsureNotClosed();

    ComPtr<ID2D1SvgPointCollection> d2dPointCollection;
    ThrowIfFailed(resource->CreatePointCollection(points, pointsCount, &d2dPointCollection));

    auto newAttribute = Make<CanvasSvgPointsAttribute>(device.Get(), d2dPointCollection.Get());

    ThrowIfFailed(newAttribute.CopyTo(result));
}

IFACEMETHODIMP CanvasSvgDocument::CreatePointsAttributeWithDefaults(ICanvasSvgPointsAttribute** result)
{
    return ExceptionBoundary(
        [=]
        {
            CreatePointsAttributeImpl(nullptr, 0, result);
        });
}

IFACEMETHODIMP CanvasSvgDocument::CreatePointsAttribute(uint32_t pointCount, Vector2* points, ICanvasSvgPointsAttribute** result)
{
    return ExceptionBoundary(
        [=]
        {
            CreatePointsAttributeImpl(ReinterpretAs<D2D1_POINT_2F*>(points), pointCount, result);
        });
}

void CanvasSvgDocument::CreateStrokeDashArrayAttributeImpl(
    D2D1_SVG_LENGTH* dashes,
    uint32_t dashCount,
    ICanvasSvgStrokeDashArrayAttribute** result)
{
    auto& resource = GetResource();

    auto& device = m_canvasDevice.EnsureNotClosed();

    ComPtr<ID2D1SvgStrokeDashArray> d2dStrokeDashArray;
    ThrowIfFailed(resource->CreateStrokeDashArray(dashes, dashCount, &d2dStrokeDashArray));

    auto newAttribute = Make<CanvasSvgStrokeDashArrayAttribute>(device.Get(), d2dStrokeDashArray.Get());

    ThrowIfFailed(newAttribute.CopyTo(result));
}

IFACEMETHODIMP CanvasSvgDocument::CreateStrokeDashArrayAttributeWithDefaults(ICanvasSvgStrokeDashArrayAttribute** result)
{
    return ExceptionBoundary(
        [=]
        {            
            CreateStrokeDashArrayAttributeImpl(nullptr, 0, result);
        });
}

IFACEMETHODIMP CanvasSvgDocument::CreateStrokeDashArrayAttribute(
    UINT32 valueCount, 
    float* values, 
    UINT32 unitsCount, 
    CanvasSvgLengthUnits* units, 
    ICanvasSvgStrokeDashArrayAttribute** result)
{
    return ExceptionBoundary(
        [=]
        {
            if (valueCount != unitsCount)
            {
                ThrowHR(E_INVALIDARG);
            }

            std::vector<D2D1_SVG_LENGTH> d2dSvgLengths = CanvasSvgStrokeDashArrayAttribute::GetD2DSvgLengths(valueCount, values, units);

            CreateStrokeDashArrayAttributeImpl(d2dSvgLengths.data(), valueCount, result);
        });
    }

IFACEMETHODIMP CanvasSvgDocumentStatics::CreateEmpty(
    ICanvasResourceCreator *resourceCreator,
    ICanvasSvgDocument **canvasSvgDocument)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(canvasSvgDocument);

            ComPtr<CanvasSvgDocument> newDocument = CanvasSvgDocument::CreateNew(resourceCreator, nullptr);
            ThrowIfFailed(newDocument.CopyTo(canvasSvgDocument));
        });
}

IFACEMETHODIMP CanvasSvgDocumentStatics::LoadFromXml(
    ICanvasResourceCreator* resourceCreator,
    HSTRING xmlString,
    ICanvasSvgDocument** svgDocument)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(svgDocument);

            // Wrap the input text into an IStream.
            ComPtr<IStream> inputXmlStream = WrapSvgStringInStream(xmlString);

            ComPtr<CanvasSvgDocument> newDocument = CanvasSvgDocument::CreateNew(resourceCreator, inputXmlStream.Get());
            ThrowIfFailed(newDocument.CopyTo(svgDocument));
        });
}

IFACEMETHODIMP CanvasSvgDocumentStatics::LoadAsync(
    ICanvasResourceCreator* resourceCreator,
    IRandomAccessStream* randomAccessStream,
    IAsyncOperation<CanvasSvgDocument*>** svgDocument)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckInPointer(randomAccessStream);
            CheckAndClearOutPointer(svgDocument);

            ComPtr<IStream> stream;
            ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream, IID_PPV_ARGS(&stream)));
            
            auto asyncOperation = Make<AsyncOperation<CanvasSvgDocument>>(
                [=]
                {
                    auto svgDocument = CanvasSvgDocument::CreateNew(resourceCreator, stream.Get());
                    CheckMakeResult(svgDocument);

                    return svgDocument;
                });

            CheckMakeResult(asyncOperation);
            ThrowIfFailed(asyncOperation.CopyTo(svgDocument));
        });
}

IFACEMETHODIMP CanvasSvgDocumentStatics::IsSupported(ICanvasDevice* device, boolean* isSupported)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(device);
            CheckInPointer(isSupported);
        
            auto lease = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();
            auto deviceContext5 = MaybeAs<ID2D1DeviceContext5>(lease.Get());
        
            *isSupported = static_cast<bool>(deviceContext5);
        });
}

}}}}}

#endif
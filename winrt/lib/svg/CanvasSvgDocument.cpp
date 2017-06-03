// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "CanvasSvgDocument.h"
#include "CanvasSvgElement.h"
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

IFACEMETHODIMP CanvasSvgDocument::put_Root(ICanvasSvgElement* root)
{
    return ExceptionBoundary(
        [=]
        {
            CheckInPointer(root);

            auto& resource = GetResource();

            CanvasSvgElement* implementation = static_cast<CanvasSvgElement*>(root);

            VerifyDeviceBoundary(m_canvasDevice, implementation);

            resource->SetRoot(implementation->GetResource().Get());
        });
}

IFACEMETHODIMP CanvasSvgDocument::get_Root(ICanvasSvgElement** result)
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
    ICanvasSvgElement** svgElement)
{ 
    return ExceptionBoundary(
        [=]
        {
            CheckAndClearOutPointer(svgElement);

            m_canvasDevice.EnsureNotClosed();

            ComPtr<IStream> stream = WrapSvgStringInStream(xmlString);
                        
            ComPtr<CanvasSvgElement> newElement = CanvasSvgElement::CreateNew(
                this,
                stream.Get());
            ThrowIfFailed(newElement.CopyTo(svgElement));
        });
}

IFACEMETHODIMP CanvasSvgDocument::LoadElementAsync(
    IRandomAccessStream *rawStream,
    IAsyncOperation<CanvasSvgElement*>** resultAsyncOperation)
{
    return ExceptionBoundary(
        [=]
        { 
            CheckInPointer(rawStream);
            CheckAndClearOutPointer(resultAsyncOperation);

            m_canvasDevice.EnsureNotClosed();

            ComPtr<IRandomAccessStream> randomAccessStream = rawStream;

            auto asyncOperation = Make<AsyncOperation<CanvasSvgElement>>(
                [=]
                {
                    ComPtr<IStream> stream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));

                    auto svgElement = CanvasSvgElement::CreateNew(this, stream.Get());
                    CheckMakeResult(svgElement);

                    return svgElement;
                });

            CheckMakeResult(asyncOperation);
            ThrowIfFailed(asyncOperation.CopyTo(resultAsyncOperation));
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

template<typename IMPLEMENTATION_TYPE>
void VerifyDeviceBoundary(
    ClosablePtr<ICanvasDevice> thisDeviceClosable,
    IMPLEMENTATION_TYPE* otherObjectImplementationType)
{
    auto thisDevice = thisDeviceClosable.EnsureNotClosed();
    auto otherDevice = otherObjectImplementationType->GetDevice();
    if (!IsSameInstance(thisDevice.Get(), otherDevice.Get()))
    {
        ThrowHR(E_INVALIDARG, Strings::SvgDocumentTreeMustHaveConsistentDevice);
    }
}

}}}}}

#endif
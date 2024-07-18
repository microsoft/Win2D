// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasSvgElement.h"
#include "CanvasSvgDocument.h"
#include "BufferStreamWrapper.h"
#include "AttributeHelpers.h"

using namespace ABI::Microsoft::Graphics::Canvas::Svg;
using namespace Microsoft::WRL::Wrappers;

static 
ComPtr<ICanvasSvgElement> WrapElementFromD2DResource(ICanvasDevice* canvasDevice, ID2D1SvgElement* d2dResource)
{
    ComPtr<ICanvasSvgElement> result;

    if (d2dResource->IsTextContent())
    {
        auto textElement = ResourceManager::GetOrCreate<ICanvasSvgTextElement>(canvasDevice, d2dResource);
        ThrowIfFailed(textElement.As(&result));
    }
    else
    {
        auto namedElement = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(canvasDevice, d2dResource);
        ThrowIfFailed(namedElement.As(&result));
    }

    CheckMakeResult(result);

    return result;
}

static
ComPtr<ICanvasSvgAttribute> WrapAttributeFromD2DResource(ICanvasDevice* canvasDevice, ComPtr<ID2D1SvgAttribute> d2dResource)
{
    ComPtr<ICanvasSvgAttribute> result;

    ComPtr<ID2D1SvgPaint> d2dPaintAttribute = MaybeAs<ID2D1SvgPaint>(d2dResource);
    if (d2dPaintAttribute)
    {
        auto attribute = ResourceManager::GetOrCreate<ICanvasSvgPaintAttribute>(canvasDevice, d2dResource.Get());
        ThrowIfFailed(attribute.As(&result));
        return result;
    }

    ComPtr<ID2D1SvgPathData> d2dPathAttribute = MaybeAs<ID2D1SvgPathData>(d2dResource);
    if (d2dPathAttribute)
    {
        auto attribute = ResourceManager::GetOrCreate<ICanvasSvgPathAttribute>(canvasDevice, d2dResource.Get());
        ThrowIfFailed(attribute.As(&result));
        return result;
    }

    ComPtr<ID2D1SvgPointCollection> d2dPointsAttribute = MaybeAs<ID2D1SvgPointCollection>(d2dResource);
    if (d2dPointsAttribute)
    {
        auto attribute = ResourceManager::GetOrCreate<ICanvasSvgPointsAttribute>(canvasDevice, d2dResource.Get());
        ThrowIfFailed(attribute.As(&result));
        return result;
    }

    ComPtr<ID2D1SvgStrokeDashArray> d2dStrokeDashArrayAttribute = As<ID2D1SvgStrokeDashArray>(d2dResource);

    auto attribute = ResourceManager::GetOrCreate<ICanvasSvgStrokeDashArrayAttribute>(canvasDevice, d2dResource.Get());
    ThrowIfFailed(attribute.As(&result));
    return result;
}

ComPtr<ICanvasSvgElement> ABI::Microsoft::Graphics::Canvas::Svg::CreateNewElementFromStream(CanvasSvgDocument* parent, IStream* stream)
{
    ComPtr<ID2D1SvgElement> d2dSvgElement;
    ThrowIfFailed(parent->GetResource()->Deserialize(stream, &d2dSvgElement));

    auto device = parent->GetDevice().Get();

    return WrapElementFromD2DResource(device, d2dSvgElement.Get());
}

ComPtr<CanvasSvgNamedElement> ABI::Microsoft::Graphics::Canvas::Svg::CreateNewNamedElementFromStream(CanvasSvgDocument* parent, IStream* stream)
{
    ComPtr<ID2D1SvgElement> d2dSvgElement;
    ThrowIfFailed(parent->GetResource()->Deserialize(stream, &d2dSvgElement));

    auto device = parent->GetDevice().Get();

    return Make<CanvasSvgNamedElement>(device, d2dSvgElement.Get());
}

CanvasSvgNamedElement::CanvasSvgNamedElement(
    ICanvasDevice* canvasDevice,
    ID2D1SvgElement* d2dSvgElement)
    : ResourceWrapper(d2dSvgElement)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgNamedElement::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

static ComPtr<ID2D1SvgElement> VerifyDeviceBoundaryAndGetNativeResource(
    ClosablePtr<ICanvasDevice> thisDevice,
    ICanvasSvgElement* svgElement)
{
    ComPtr<ID2D1SvgElement> nativeResource;
    auto namedElement = MaybeAs<ICanvasSvgNamedElement>(svgElement);
    if (namedElement)
    {
        auto implementation = static_cast<CanvasSvgNamedElement*>(namedElement.Get());
        VerifyDeviceBoundary(thisDevice, implementation);
        nativeResource = implementation->GetResource();
    }
    else
    {
        auto textElement = MaybeAs<ICanvasSvgTextElement>(svgElement);
        auto implementation = static_cast<CanvasSvgTextElement*>(textElement.Get());
        VerifyDeviceBoundary(thisDevice, implementation);
        nativeResource = implementation->GetResource();
    }

    return nativeResource;
}

IFACEMETHODIMP CanvasSvgNamedElement::AppendChild(ICanvasSvgElement* child)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(child);

            auto& resource = GetResource();

            auto nativeResource = VerifyDeviceBoundaryAndGetNativeResource(m_canvasDevice, child);
            
            ThrowIfFailed(resource->AppendChild(nativeResource.Get()));
        });
}

IFACEMETHODIMP CanvasSvgNamedElement::CreateAndAppendNamedChildElement(HSTRING elementName, ICanvasSvgNamedElement** newElement)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(newElement);

            auto& device = m_canvasDevice.EnsureNotClosed();

            auto& resource = GetResource();

            ComPtr<ID2D1SvgElement> newD2DChild;
            ThrowIfFailed(resource->CreateChild(GetStringBuffer(elementName), &newD2DChild));

            auto newCanvasElement = WrapElementFromD2DResource(device.Get(), newD2DChild.Get());
            ThrowIfFailed(newCanvasElement.CopyTo(newElement));
        });
}

IFACEMETHODIMP CanvasSvgNamedElement::CreateAndAppendTextChildElement(HSTRING text, ICanvasSvgTextElement** newElement)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(newElement);

            auto& device = m_canvasDevice.EnsureNotClosed();

            auto& resource = GetResource();

            ComPtr<ID2D1SvgElement> newD2DChild;
            ThrowIfFailed(resource->CreateChild(L"", &newD2DChild));

            uint32_t textLength;
            auto textBuffer = GetStringBuffer(text, &textLength);
            ThrowIfFailed(newD2DChild->SetTextValue(textBuffer, textLength));

            auto newCanvasElement = WrapElementFromD2DResource(device.Get(), newD2DChild.Get());
            ThrowIfFailed(newCanvasElement.CopyTo(newElement));
        });
}

IFACEMETHODIMP CanvasSvgNamedElement::get_FirstChild(ICanvasSvgElement** result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(result);

            auto& device = m_canvasDevice.EnsureNotClosed();

            auto& resource = GetResource();

            ComPtr<ID2D1SvgElement> d2dElement;
            resource->GetFirstChild(&d2dElement);

            if (d2dElement)
            {
                auto newCanvasElement = WrapElementFromD2DResource(device.Get(), d2dElement.Get());
                ThrowIfFailed(newCanvasElement.CopyTo(result));
            }
        });
}

    IFACEMETHODIMP CanvasSvgNamedElement::get_LastChild(ICanvasSvgElement** result)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(result);

                auto& device = m_canvasDevice.EnsureNotClosed();

                auto& resource = GetResource();

                ComPtr<ID2D1SvgElement> d2dElement;
                resource->GetLastChild(&d2dElement);

                if (d2dElement)
                {
                    auto newCanvasElement = WrapElementFromD2DResource(device.Get(), d2dElement.Get());
                    ThrowIfFailed(newCanvasElement.CopyTo(result));
                }
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::GetPreviousSibling(ICanvasSvgElement* reference, ICanvasSvgElement** result)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(reference);
                CheckAndClearOutPointer(result);

                auto& device = m_canvasDevice.EnsureNotClosed();

                auto& resource = GetResource();

                auto d2dReference = VerifyDeviceBoundaryAndGetNativeResource(m_canvasDevice, reference);

                ComPtr<ID2D1SvgElement> d2dElement;
                ThrowIfFailed(resource->GetPreviousChild(d2dReference.Get(), &d2dElement));

                if (d2dElement)
                {
                    auto newCanvasElement = WrapElementFromD2DResource(device.Get(), d2dElement.Get());
                    ThrowIfFailed(newCanvasElement.CopyTo(result));
                }
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::GetNextSibling(ICanvasSvgElement* reference, ICanvasSvgElement** result)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(reference);
                CheckAndClearOutPointer(result);

                auto& device = m_canvasDevice.EnsureNotClosed();

                auto& resource = GetResource();

                auto d2dReference = VerifyDeviceBoundaryAndGetNativeResource(m_canvasDevice, reference);

                ComPtr<ID2D1SvgElement> d2dElement;
                ThrowIfFailed(resource->GetNextChild(d2dReference.Get(), &d2dElement));

                if (d2dElement)
                {
                    auto newCanvasElement = WrapElementFromD2DResource(device.Get(), d2dElement.Get());
                    ThrowIfFailed(newCanvasElement.CopyTo(result));
                }
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::get_SpecifiedAttributes(UINT32* valueCount, HSTRING** valueElements)
    {
        return ExceptionBoundary(
            [&]
            {            
                CheckInPointer(valueCount);
                CheckAndClearOutPointer(valueElements);

                auto& resource = GetResource();
                uint32_t specifiedAttributeCount = resource->GetSpecifiedAttributeCount();

                ComArray<HSTRING> array(specifiedAttributeCount);

                for (uint32_t i = 0; i < specifiedAttributeCount; ++i)
                {
                    uint32_t attributeNameLength;
                    ThrowIfFailed(resource->GetSpecifiedAttributeNameLength(i, &attributeNameLength, FALSE));

                    WinStringBuilder stringBuilder;
                    uint32_t bufferSize = attributeNameLength + 1; // Account for null terminator
                    auto buffer = stringBuilder.Allocate(bufferSize);

                    ThrowIfFailed(resource->GetSpecifiedAttributeName(i, buffer, bufferSize, FALSE));

                    stringBuilder.Get().CopyTo(&array[i]);
                }

                array.Detach(valueCount, valueElements);
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::get_Tag(HSTRING* result)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(result);

                auto& resource = GetResource();

                uint32_t tagNameLength = resource->GetTagNameLength();
                uint32_t bufferSize = tagNameLength + 1;
                WinStringBuilder stringBuilder;
                auto buffer = stringBuilder.Allocate(bufferSize);

                ThrowIfFailed(resource->GetTagName(buffer, bufferSize));

                stringBuilder.Get().CopyTo(result);
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::get_HasChildren(boolean* result)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(result);

                auto& resource = GetResource();

                *result = !!resource->HasChildren();
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::InsertChildBefore(ICanvasSvgElement* newChild, ICanvasSvgElement* referenceChild)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(newChild);
                CheckInPointer(referenceChild);

                auto& resource = GetResource();

                auto d2dNewChild = VerifyDeviceBoundaryAndGetNativeResource(m_canvasDevice, newChild);
                auto d2dReference = VerifyDeviceBoundaryAndGetNativeResource(m_canvasDevice, referenceChild);

                ComPtr<ID2D1SvgElement> d2dElement;
                ThrowIfFailed(resource->InsertChildBefore(d2dNewChild.Get(), d2dReference.Get()));
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::IsAttributeSpecified(HSTRING attributeName, boolean* isSpecified)
    {
        return ExceptionBoundary(
            [&]
            {
                IsAttributeSpecifiedImpl(attributeName, isSpecified, nullptr);
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::IsAttributeSpecifiedWithInherhited(HSTRING attributeName, boolean isInherited, boolean* isSpecified)
    {
        return ExceptionBoundary(
            [&]
            {
                IsAttributeSpecifiedImpl(attributeName, isSpecified, &isInherited);
            });
    }

    void CanvasSvgNamedElement::IsAttributeSpecifiedImpl(HSTRING attributeName, boolean* isSpecified, boolean* isInherited)
    {
        CheckInPointer(isSpecified);

        auto& resource = GetResource();

        ComPtr<ID2D1SvgElement> newD2DChild;
        BOOL isInheritedNative = isInherited? *isInherited : FALSE;
        *isSpecified = !!resource->IsAttributeSpecified(GetStringBuffer(attributeName), &isInheritedNative);
    }

    IFACEMETHODIMP CanvasSvgNamedElement::RemoveAttribute(HSTRING attributeName)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                ThrowIfFailed(resource->RemoveAttribute(GetStringBuffer(attributeName)));
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::RemoveChild(ICanvasSvgElement* child)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(child);

                auto& resource = GetResource();

                auto d2dChild = VerifyDeviceBoundaryAndGetNativeResource(m_canvasDevice, child);

                ComPtr<ID2D1SvgElement> d2dElement;
                ThrowIfFailed(resource->RemoveChild(d2dChild.Get()));
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::ReplaceChild(ICanvasSvgElement* newChild, ICanvasSvgElement* oldChild)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(newChild);
                CheckInPointer(oldChild);

                auto& resource = GetResource();

                auto d2dNewChild = VerifyDeviceBoundaryAndGetNativeResource(m_canvasDevice, newChild);
                auto d2dOldChild = VerifyDeviceBoundaryAndGetNativeResource(m_canvasDevice, oldChild);

                ComPtr<ID2D1SvgElement> d2dElement;
                ThrowIfFailed(resource->ReplaceChild(d2dNewChild.Get(), d2dOldChild.Get()));
            });
    }

    static void GetContainingDocumentImpl(
        ICanvasSvgDocument** containingDocument,
        ID2D1SvgElement* thisElement,
        ICanvasDevice* thisDevice)
    {
        CheckAndClearOutPointer(containingDocument);

        ComPtr<ID2D1SvgDocument> d2dDocument;
        thisElement->GetDocument(&d2dDocument);

        if (d2dDocument) // Document is allowed to be null
        {
            ComPtr<ICanvasSvgDocument> wrapped;

            wrapped = ResourceManager::GetOrCreate<ICanvasSvgDocument>(thisDevice, d2dDocument.Get());
            ThrowIfFailed(wrapped.CopyTo(containingDocument));
        }
    }

    IFACEMETHODIMP CanvasSvgNamedElement::get_ContainingDocument(ICanvasSvgDocument** containingDocument)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(containingDocument);

                auto& device = m_canvasDevice.EnsureNotClosed();

                GetContainingDocumentImpl(containingDocument, GetResource().Get(), device.Get());
            });
    }

    static void GetParentImpl(ICanvasSvgNamedElement** parent, ID2D1SvgElement* d2dResource, ICanvasDevice* device)
    {
        ComPtr<ID2D1SvgElement> d2dElement;
        d2dResource->GetParent(&d2dElement);

        if (d2dElement) // Allowed to be null
        {
            ComPtr<ICanvasSvgNamedElement> wrapped = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(device, d2dElement.Get());
            ThrowIfFailed(wrapped.CopyTo(parent));
        }
    }

    IFACEMETHODIMP CanvasSvgNamedElement::get_Parent(ICanvasSvgNamedElement** parent)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(parent);

                auto& device = m_canvasDevice.EnsureNotClosed();

                GetParentImpl(parent, GetResource().Get(), device.Get());
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::SetAttribute(HSTRING attributeName, ICanvasSvgAttribute* attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(attributeValue);

                auto& resource = GetResource();

                auto attributeInternal = As<AttributeHelpers::IAttributeInternal>(attributeValue);

                ComPtr<ID2D1SvgAttribute> d2dAttribute = attributeInternal->GetD2DBaseAttribute();

                ThrowIfFailed(resource->SetAttributeValue(GetStringBuffer(attributeName), d2dAttribute.Get()));
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::GetAttribute(HSTRING attributeName, ICanvasSvgAttribute** attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(attributeValue);

                auto& resource = GetResource();

                auto& device = m_canvasDevice.EnsureNotClosed();

                ComPtr<ID2D1SvgAttribute> d2dAttribute;
                ThrowIfFailed(resource->GetAttributeValue(GetStringBuffer(attributeName), &d2dAttribute));

                ComPtr<ICanvasSvgAttribute> wrapped = WrapAttributeFromD2DResource(device.Get(), d2dAttribute);
                ThrowIfFailed(wrapped.CopyTo(attributeValue));
            });
    }

    void CanvasSvgNamedElement::GetStringAttributeImpl(HSTRING attributeName, D2D1_SVG_ATTRIBUTE_STRING_TYPE stringType, HSTRING* attributeValue)
    {
        CheckInPointer(attributeValue);

        auto& resource = GetResource();

        uint32_t attributeValueLength;

        auto attributeNameBuffer = GetStringBuffer(attributeName);

        ThrowIfFailed(resource->GetAttributeValueLength(
            attributeNameBuffer,
            stringType,
            &attributeValueLength));

        auto bufferSize = attributeValueLength + 1; // Account for null
        WinStringBuilder stringBuilder;
        auto buffer = stringBuilder.Allocate(bufferSize);

        ThrowIfFailed(resource->GetAttributeValue(
            attributeNameBuffer,
            stringType,
            buffer,
            bufferSize));

        stringBuilder.Get().CopyTo(attributeValue);
    }

    void CanvasSvgNamedElement::SetStringAttributeImpl(HSTRING attributeName, D2D1_SVG_ATTRIBUTE_STRING_TYPE stringType, HSTRING attributeValue)
    {
        auto& resource = GetResource();

        ThrowIfFailed(resource->SetAttributeValue(
            GetStringBuffer(attributeName),
            stringType,
            GetStringBuffer(attributeValue)));
    }

    IFACEMETHODIMP CanvasSvgNamedElement::SetIdAttribute(HSTRING attributeName, HSTRING attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                SetStringAttributeImpl(attributeName, D2D1_SVG_ATTRIBUTE_STRING_TYPE_ID, attributeValue);
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::GetIdAttribute(HSTRING attributeName, HSTRING* attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                GetStringAttributeImpl(attributeName, D2D1_SVG_ATTRIBUTE_STRING_TYPE_ID, attributeValue);
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::SetStringAttribute(HSTRING attributeName, HSTRING attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                SetStringAttributeImpl(attributeName, D2D1_SVG_ATTRIBUTE_STRING_TYPE_SVG, attributeValue);
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::GetStringAttribute(HSTRING attributeName, HSTRING* attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                GetStringAttributeImpl(attributeName, D2D1_SVG_ATTRIBUTE_STRING_TYPE_SVG, attributeValue);
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::SetFloatAttribute(HSTRING attributeName, float attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                ThrowIfFailed(resource->SetAttributeValue(GetStringBuffer(attributeName), attributeValue));
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::GetFloatAttribute(HSTRING attributeName, float* attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(attributeValue);
                auto& resource = GetResource();

                ThrowIfFailed(resource->GetAttributeValue(GetStringBuffer(attributeName), attributeValue));
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::SetColorAttribute(HSTRING attributeName, Color attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                ThrowIfFailed(resource->SetAttributeValue(GetStringBuffer(attributeName), ToD2DColor(attributeValue)));
            });
    }
    IFACEMETHODIMP CanvasSvgNamedElement::GetColorAttribute(HSTRING attributeName, Color* attributeValue)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(attributeValue);
                auto& resource = GetResource();

                D2D1_COLOR_F d2dColor;
                ThrowIfFailed(resource->GetAttributeValue(GetStringBuffer(attributeName), &d2dColor));

                *attributeValue = ToWindowsColor(d2dColor);
            });
    }

    inline CanvasCapStyle FromD2DSvgLineCap(D2D1_SVG_LINE_CAP lineCap)
    {
        switch (lineCap)
        {
            case D2D1_SVG_LINE_CAP_BUTT: return CanvasCapStyle::Flat;
            case D2D1_SVG_LINE_CAP_SQUARE: return CanvasCapStyle::Square;
            case D2D1_SVG_LINE_CAP_ROUND: return CanvasCapStyle::Round;
            default: assert(false); return CanvasCapStyle::Flat;
        }
    }

    inline D2D1_SVG_LINE_CAP ToD2DSvgLineCap(CanvasCapStyle lineCap)
    {
        switch (lineCap)
        {
            case CanvasCapStyle::Flat: return D2D1_SVG_LINE_CAP_BUTT;
            case CanvasCapStyle::Square: return D2D1_SVG_LINE_CAP_SQUARE;
            case CanvasCapStyle::Round: return D2D1_SVG_LINE_CAP_ROUND;
            case CanvasCapStyle::Triangle:
            {
                ThrowHR(E_INVALIDARG, Strings::SvgLineCapTriangleNotAllowed);
            }
            default: assert(false); return D2D1_SVG_LINE_CAP_BUTT;
        }
    }

    inline CanvasLineJoin FromD2DSvgLineJoin(D2D1_SVG_LINE_JOIN lineJoin)
    {
        switch (lineJoin)
        {
            case D2D1_SVG_LINE_JOIN_BEVEL: return CanvasLineJoin::Bevel;

            // Note that the functionally of this is "Miter or bevel", but the terminology it should use is "Miter" to be
            // consistent with SVG. D2D does the typedefing to treat SVG_LINE_JOIN_MITER as miter or bevel.
            case D2D1_SVG_LINE_JOIN_MITER: return CanvasLineJoin::Miter; 

            case D2D1_SVG_LINE_JOIN_ROUND: return CanvasLineJoin::Round;
            default: assert(false); return CanvasLineJoin::Miter;
        }
    }

    inline D2D1_SVG_LINE_JOIN ToD2DSvgLineJoin(CanvasLineJoin lineJoin)
    {
        switch (lineJoin)
        {
            // SVG's only supported mitering is actually "Miter or bevel", so they both are mapped to the same thing here.
            case CanvasLineJoin::Miter:
            case CanvasLineJoin::MiterOrBevel: return D2D1_SVG_LINE_JOIN_MITER;

            case CanvasLineJoin::Bevel: return D2D1_SVG_LINE_JOIN_BEVEL;
            case CanvasLineJoin::Round: return D2D1_SVG_LINE_JOIN_ROUND;
            default: assert(false); return D2D1_SVG_LINE_JOIN_MITER;
        }
    }

    inline D2D1_MATRIX_3X2_F ToD2DMatrix3x2F(Matrix3x2 m)
    {
        return *ReinterpretAs<D2D1_MATRIX_3X2_F const*>(&m);
    }

    inline Matrix3x2 FromD2DMatrix3x2F(D2D1_MATRIX_3X2_F m)
    {
        return *ReinterpretAs<Matrix3x2 const*>(&m);
    }

    inline Rect FromD2DSvgViewbox(D2D1_SVG_VIEWBOX viewBox)
    {
        Rect r;
        r.X = viewBox.x;
        r.Y = viewBox.y;
        r.Width = viewBox.width;
        r.Height = viewBox.height;
        return r;
    }

    inline D2D1_SVG_VIEWBOX ToD2DSvgViewbox(Rect r)
    {
        D2D1_SVG_VIEWBOX viewBox;
        viewBox.x = r.X;
        viewBox.y = r.Y;
        viewBox.width = r.Width;
        viewBox.height = r.Height;
        return viewBox;
    }

#define SIMPLE_ATTRIBUTE(name, nativeType, wrappedType, nativeConversion, wrappedConversion) \
IFACEMETHODIMP CanvasSvgNamedElement::Set##name##Attribute(HSTRING attributeName, wrappedType value)                                \
{                                                                                                                                   \
    return ExceptionBoundary(                                                                                                       \
        [&]                                                                                                                         \
        {                                                                                                                           \
            auto& resource = GetResource();                                                                                         \
            ThrowIfFailed(resource->SetAttributeValue(GetStringBuffer(attributeName), nativeConversion(value)));                    \
        });                                                                                                                         \
}                                                                                                                                   \
                                                                                                                                    \
IFACEMETHODIMP CanvasSvgNamedElement::Get##name##Attribute(HSTRING attributeName, wrappedType* value)                               \
{                                                                                                                                   \
    return ExceptionBoundary(                                                                                                       \
        [&]                                                                                                                         \
        {                                                                                                                           \
            CheckInPointer(value);                                                                                                  \
            auto& resource = GetResource();                                                                                         \
            nativeType nativeValue;                                                                                                 \
            ThrowIfFailed(resource->GetAttributeValue(GetStringBuffer(attributeName), &nativeValue));                               \
            *value = wrappedConversion(nativeValue);                                                                                \
        });                                                                                                                         \
}
    
#define SIMPLE_ATTRIBUTE_CAST_CONVERSION(name, nativeType, wrappedType) SIMPLE_ATTRIBUTE(name, nativeType, wrappedType, StaticCastAs<nativeType>, StaticCastAs<wrappedType>)

    SIMPLE_ATTRIBUTE_CAST_CONVERSION(FilledRegionDetermination, D2D1_FILL_MODE, CanvasFilledRegionDetermination)
    SIMPLE_ATTRIBUTE_CAST_CONVERSION(Display, D2D1_SVG_DISPLAY, CanvasSvgDisplay)
    SIMPLE_ATTRIBUTE_CAST_CONVERSION(Overflow, D2D1_SVG_OVERFLOW, CanvasSvgOverflow)
    SIMPLE_ATTRIBUTE_CAST_CONVERSION(Visibility, D2D1_SVG_VISIBILITY, CanvasSvgVisibility)
    SIMPLE_ATTRIBUTE_CAST_CONVERSION(Units, D2D1_SVG_UNIT_TYPE, CanvasSvgUnits)
    SIMPLE_ATTRIBUTE_CAST_CONVERSION(EdgeBehavior, D2D1_EXTEND_MODE, CanvasEdgeBehavior)
        
    SIMPLE_ATTRIBUTE(CapStyle, D2D1_SVG_LINE_CAP, CanvasCapStyle, ToD2DSvgLineCap, FromD2DSvgLineCap)
    SIMPLE_ATTRIBUTE(LineJoin, D2D1_SVG_LINE_JOIN, CanvasLineJoin, ToD2DSvgLineJoin, FromD2DSvgLineJoin)
    SIMPLE_ATTRIBUTE(Transform, D2D1_MATRIX_3X2_F, Matrix3x2, ToD2DMatrix3x2F, FromD2DMatrix3x2F)


IFACEMETHODIMP CanvasSvgNamedElement::SetLengthAttribute(HSTRING attributeName, float value, CanvasSvgLengthUnits units)
{   
    return ExceptionBoundary(       
        [&]                         
        {
            auto& resource = GetResource();

            D2D1_SVG_LENGTH d2dSvgLength;
            d2dSvgLength.units = StaticCastAs<D2D1_SVG_LENGTH_UNITS>(units);
            d2dSvgLength.value = value;

            ThrowIfFailed(resource->SetAttributeValue(GetStringBuffer(attributeName), d2dSvgLength));
        });                         
}

IFACEMETHODIMP CanvasSvgNamedElement::GetLengthAttribute(HSTRING attributeName, CanvasSvgLengthUnits* units, float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(units);
            CheckInPointer(value);
            auto& resource = GetResource();

            D2D1_SVG_LENGTH d2dSvgLength;
            ThrowIfFailed(resource->GetAttributeValue(GetStringBuffer(attributeName), &d2dSvgLength));

            *units = StaticCastAs<CanvasSvgLengthUnits>(d2dSvgLength.units);
            *value = d2dSvgLength.value;
        });
}

    IFACEMETHODIMP CanvasSvgNamedElement::SetAspectRatioAttribute(HSTRING attributeName, CanvasSvgAspectAlignment alignment, CanvasSvgAspectScaling meetOrSlice)
    {
        return ExceptionBoundary(       
            [&]                         
            {
                auto& resource = GetResource();

                D2D1_SVG_PRESERVE_ASPECT_RATIO d2dSvgAspectRatio;
                d2dSvgAspectRatio.align = StaticCastAs<D2D1_SVG_ASPECT_ALIGN>(alignment);
                d2dSvgAspectRatio.meetOrSlice = StaticCastAs<D2D1_SVG_ASPECT_SCALING>(meetOrSlice);

                ThrowIfFailed(resource->SetAttributeValue(GetStringBuffer(attributeName), d2dSvgAspectRatio));
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::GetAspectRatioAttribute(HSTRING attributeName, CanvasSvgAspectScaling* meetOrSlice, CanvasSvgAspectAlignment* alignment)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(meetOrSlice);
                CheckInPointer(alignment);

                auto& resource = GetResource();

                D2D1_SVG_PRESERVE_ASPECT_RATIO d2dSvgAspectRatio;
                ThrowIfFailed(resource->GetAttributeValue(GetStringBuffer(attributeName), &d2dSvgAspectRatio));

                *alignment = StaticCastAs<CanvasSvgAspectAlignment>(d2dSvgAspectRatio.align);
                *meetOrSlice = StaticCastAs<CanvasSvgAspectScaling>(d2dSvgAspectRatio.meetOrSlice);
            });
    }

    IFACEMETHODIMP CanvasSvgNamedElement::get_Device(ICanvasDevice** device)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(device);

                ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
            });
    }
    
IFACEMETHODIMP CanvasSvgNamedElement::SetRectangleAttribute(HSTRING attributeName, Rect value)
{   
    return ExceptionBoundary(       
        [&]                         
        {                           
            auto& resource = GetResource();

            auto d2dSvgViewbox = ToD2DSvgViewbox(value);

            ThrowIfFailed(resource->SetAttributeValue(
                GetStringBuffer(attributeName), 
                D2D1_SVG_ATTRIBUTE_POD_TYPE_VIEWBOX, 
                static_cast<void*>(&d2dSvgViewbox),
                sizeof(d2dSvgViewbox)));
        });                         
}   
    
IFACEMETHODIMP CanvasSvgNamedElement::GetRectangleAttribute(HSTRING attributeName, Rect* value)
{   
    return ExceptionBoundary(       
        [&]                         
        {                  
            CheckInPointer(value);

            auto& resource = GetResource();

            D2D1_SVG_VIEWBOX nativeValue; 

            ThrowIfFailed(resource->GetAttributeValue(
                GetStringBuffer(attributeName), 
                D2D1_SVG_ATTRIBUTE_POD_TYPE_VIEWBOX, 
                static_cast<void*>(&nativeValue),
                sizeof(nativeValue)));

            *value = FromD2DSvgViewbox(nativeValue);                
        });                         
}
    
CanvasSvgTextElement::CanvasSvgTextElement(
    ICanvasDevice* canvasDevice,
    ID2D1SvgElement* d2dSvgElement)
    : ResourceWrapper(d2dSvgElement)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgTextElement::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasSvgTextElement::get_Text(HSTRING* result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(result);

            auto& resource = GetResource();

            uint32_t textLength = resource->GetTextValueLength();
            WinStringBuilder stringBuilder;
            auto bufferSize = textLength + 1; // Account for null
            auto buffer = stringBuilder.Allocate(bufferSize);

            ThrowIfFailed(resource->GetTextValue(buffer, bufferSize));

            stringBuilder.Get().CopyTo(result);
        });
}

IFACEMETHODIMP CanvasSvgTextElement::put_Text(HSTRING text)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            uint32_t textLength;
            auto textBuffer = GetStringBuffer(text, &textLength);

            ThrowIfFailed(resource->SetTextValue(textBuffer, textLength));
        });
}

IFACEMETHODIMP CanvasSvgTextElement::get_ContainingDocument(ICanvasSvgDocument** containingDocument)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(containingDocument);

            auto& device = m_canvasDevice.EnsureNotClosed();

            GetContainingDocumentImpl(containingDocument, GetResource().Get(), device.Get());
        });
}

IFACEMETHODIMP CanvasSvgTextElement::get_Parent(ICanvasSvgNamedElement** parent)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(parent);

            auto& device = m_canvasDevice.EnsureNotClosed();

            GetParentImpl(parent, GetResource().Get(), device.Get());
        });
}

IFACEMETHODIMP CanvasSvgTextElement::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}

bool ABI::Microsoft::Graphics::Canvas::Svg::IsSvgTextElement(ID2D1SvgElement* d2dSvgElement)
{
    return !!d2dSvgElement->IsTextContent();
}
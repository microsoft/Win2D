// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace ABI::Microsoft::Graphics::Canvas::Geometry;

CanvasStrokeStyle::CanvasStrokeStyle()
    : ResourceWrapper(nullptr)
    , m_startCap(CanvasCapStyle::Flat)
    , m_endCap(CanvasCapStyle::Flat)
    , m_dashCap(CanvasCapStyle::Square)
    , m_lineJoin(CanvasLineJoin::Miter)
    , m_miterLimit(10.0f)
    , m_dashStyle(CanvasDashStyle::Solid)
    , m_dashOffset(0)
    , m_transformBehavior(CanvasStrokeTransformBehavior::Normal)
    , m_closed(false)
{
}


CanvasStrokeStyle::CanvasStrokeStyle(ID2D1StrokeStyle1* d2dStrokeStyle)
    : ResourceWrapper(d2dStrokeStyle)
    , m_closed(false)
    , m_startCap(static_cast<CanvasCapStyle>(d2dStrokeStyle->GetStartCap()))
    , m_endCap(static_cast<CanvasCapStyle>(d2dStrokeStyle->GetEndCap()))
    , m_dashCap(static_cast<CanvasCapStyle>(d2dStrokeStyle->GetDashCap()))
    , m_lineJoin(static_cast<CanvasLineJoin>(d2dStrokeStyle->GetLineJoin()))
    , m_miterLimit(d2dStrokeStyle->GetMiterLimit())
    , m_dashStyle(static_cast<CanvasDashStyle>(d2dStrokeStyle->GetDashStyle()))
    , m_dashOffset(d2dStrokeStyle->GetDashOffset())
    , m_transformBehavior(static_cast<CanvasStrokeTransformBehavior>(d2dStrokeStyle->GetStrokeTransformType()))
{
    //
    // Canvas stroke styles created from native stroke styles are made 
    // to default out to dash style Solid because there is no projected Custom. 
    // The dash array is enough to infer whether it's actually custom.
    //
    // One implication of this is that stroke styles are not 'round-trippable'
    // between native and Canvas if it uses dash style custom but no dash
    // array. This is a very minor edge case, though.
    //
    if (d2dStrokeStyle->GetDashStyle() == D2D1_DASH_STYLE_CUSTOM)
    {
        m_dashStyle = CanvasDashStyle::Solid;

        uint32_t customDashElementCount = d2dStrokeStyle->GetDashesCount();
        if (customDashElementCount > 0)
        {
            m_customDashElements.resize(customDashElementCount);
            d2dStrokeStyle->GetDashes(&(m_customDashElements[0]), customDashElementCount);
        }
    }
}

IFACEMETHODIMP CanvasStrokeStyle::get_StartCap(_Out_ CanvasCapStyle* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_startCap;
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_StartCap(_In_ CanvasCapStyle value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();
            if (m_startCap != value)
            {
                ReleaseResource();
                m_startCap = value;
            }
        });
}

IFACEMETHODIMP CanvasStrokeStyle::get_EndCap(_Out_ CanvasCapStyle* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_endCap;
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_EndCap(_In_ CanvasCapStyle value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();
            if (m_endCap != value)
            {
                ReleaseResource();
                m_endCap = value;
            }
        });
}

IFACEMETHODIMP CanvasStrokeStyle::get_DashCap(_Out_ CanvasCapStyle* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_dashCap;
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_DashCap(_In_ CanvasCapStyle value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();
            if (m_dashCap != value)
            {
                ReleaseResource();
                m_dashCap = value;
            }
        });
}

IFACEMETHODIMP CanvasStrokeStyle::get_LineJoin(_Out_ CanvasLineJoin* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_lineJoin;
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_LineJoin(_In_ CanvasLineJoin value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();
            if (m_lineJoin != value)
            {
                ReleaseResource();
                m_lineJoin = value;
            }
        });
}

IFACEMETHODIMP CanvasStrokeStyle::get_MiterLimit(_Out_ float* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_miterLimit;
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_MiterLimit(_In_ float value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();
            if (m_miterLimit != value)
            {
                ReleaseResource();
                m_miterLimit = value;
            }
        });
}

IFACEMETHODIMP CanvasStrokeStyle::get_DashStyle(_Out_ CanvasDashStyle* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_dashStyle;
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_DashStyle(_In_ CanvasDashStyle value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();
            if (m_dashStyle != value)
            {
                ReleaseResource();
                m_dashStyle = value;
            }
        });
}

IFACEMETHODIMP CanvasStrokeStyle::get_DashOffset(_Out_ float* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_dashOffset;
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_DashOffset(float value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();
            if (m_dashOffset != value)
            {
                ReleaseResource();
                m_dashOffset = value;
            }
        });
}

IFACEMETHODIMP CanvasStrokeStyle::get_CustomDashStyle(
    uint32_t* valueCount,
    float** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);
            ThrowIfClosed();

            assert(m_customDashElements.size() <= UINT_MAX);

            ComArray<float> array(m_customDashElements.begin(), m_customDashElements.end());
            array.Detach(valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_CustomDashStyle(
    uint32_t valueCount,
    float* valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();

            if (valueCount)
                CheckInPointer(valueElements);

            if ((m_customDashElements.size() != valueCount) ||
                (valueCount && !std::equal(m_customDashElements.begin(),
                                           m_customDashElements.end(),
                                           stdext::checked_array_iterator<float*>(valueElements, valueCount))))
            {
                ReleaseResource();
                m_customDashElements.assign(valueElements, valueElements + valueCount);
            }
        });
}

IFACEMETHODIMP CanvasStrokeStyle::get_TransformBehavior(_Out_ CanvasStrokeTransformBehavior* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_transformBehavior;
        });
}

IFACEMETHODIMP CanvasStrokeStyle::put_TransformBehavior(_In_ CanvasStrokeTransformBehavior value)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ThrowIfClosed();

            if (m_transformBehavior != value)
            {
                ReleaseResource();
                m_transformBehavior = value;
            }
        });
}

//
// IClosable
//
IFACEMETHODIMP CanvasStrokeStyle::Close()
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            
            ReleaseResource();
            m_closed = true;
        });
}


//
// ICanvasStrokeStyleInternal
//

ComPtr<ID2D1StrokeStyle1> CanvasStrokeStyle::GetRealizedD2DStrokeStyle(ID2D1Factory* d2dFactory)
{
    auto lock = GetLock();
            
    //
    // If there is already a realization, ensure its factory matches the target factory.
    //
    auto& resource = MaybeGetResource();

    if (resource)
    {
        ComPtr<ID2D1Factory> realizationFactory;
        resource->GetFactory(&realizationFactory);

        if (IsSameInstance(realizationFactory.Get(), d2dFactory))
        {
            return resource;
        }
    }

    //
    // We must re-realize the D2D resource.
    //
    D2D1_STROKE_STYLE_PROPERTIES1 strokeStyleProperties = D2D1::StrokeStyleProperties1(
        static_cast<D2D1_CAP_STYLE>(m_startCap),
        static_cast<D2D1_CAP_STYLE>(m_endCap),
        static_cast<D2D1_CAP_STYLE>(m_dashCap),
        static_cast<D2D1_LINE_JOIN>(m_lineJoin),
        m_miterLimit,
        static_cast<D2D1_DASH_STYLE>(m_dashStyle),
        m_dashOffset,
        static_cast<D2D1_STROKE_TRANSFORM_TYPE>(m_transformBehavior));

    float* dashArray = NULL;
    if (m_customDashElements.size() > 0)
    {
        dashArray = &(m_customDashElements[0]);
        strokeStyleProperties.dashStyle = D2D1_DASH_STYLE_CUSTOM;
    }

    assert(m_customDashElements.size() <= UINT_MAX);

    ComPtr<ID2D1Factory2> d2dFactory2;
    ThrowIfFailed(d2dFactory->QueryInterface(IID_PPV_ARGS(d2dFactory2.GetAddressOf())));

    ComPtr<ID2D1StrokeStyle1> d2dStrokeStyle;
    ThrowIfFailed(d2dFactory2->CreateStrokeStyle(
        strokeStyleProperties,
        dashArray,
        static_cast<uint32_t>(m_customDashElements.size()),
        &d2dStrokeStyle));

    SetResource(d2dStrokeStyle.Get());

    return d2dStrokeStyle;
}


//
// ICanvasResourceWrapperNative
//

IFACEMETHODIMP CanvasStrokeStyle::GetNativeResource(ICanvasDevice* device, float dpi, REFIID iid, void** outResource)
{
    UNREFERENCED_PARAMETER(dpi);

    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(outResource);
            ThrowIfClosed();

            if (!device)
                ThrowHR(E_INVALIDARG, Strings::GetResourceNoDevice);

            ComPtr<ID2D1Factory> d2dFactory;
            As<ICanvasDeviceInternal>(device)->GetD2DDevice()->GetFactory(&d2dFactory);

            auto resource = GetRealizedD2DStrokeStyle(d2dFactory.Get());

            ThrowIfFailed(resource.CopyTo(iid, outResource));
        });
}


void CanvasStrokeStyle::ThrowIfClosed()
{
    if (m_closed)
    {
        ThrowHR(RO_E_CLOSED);
    }
}

//
// CanvasStrokeStyleFactory
//

_Use_decl_annotations_
IFACEMETHODIMP CanvasStrokeStyleFactory::ActivateInstance(IInspectable** object)
{
    return ExceptionBoundary(
        [&]
        {
            auto newCanvasStrokeStyle = Make<CanvasStrokeStyle>();

            CheckMakeResult(newCanvasStrokeStyle);

            ThrowIfFailed(newCanvasStrokeStyle.CopyTo(object));
        });
}

ActivatableClassWithFactory(CanvasStrokeStyle, CanvasStrokeStyleFactory);

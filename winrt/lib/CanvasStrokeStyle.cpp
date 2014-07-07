// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include "pch.h"
#include "CanvasStrokeStyle.h"
#include "CanvasDevice.h"

namespace canvas
{
    CanvasStrokeStyle::CanvasStrokeStyle()
        : m_startCap(CanvasCapStyle::Flat)
        , m_endCap(CanvasCapStyle::Flat)
        , m_dashCap(CanvasCapStyle::Flat)
        , m_lineJoin(CanvasLineJoin::Miter)
        , m_miterLimit(10.0f)
        , m_dashStyle(CanvasDashStyle::Solid)
        , m_dashOffset(0)
        , m_transformBehavior(CanvasStrokeTransformBehavior::Normal)
        , m_closed(false)
    {
    }


    CanvasStrokeStyle::CanvasStrokeStyle(ID2D1StrokeStyle1* d2dStrokeStyle)
        : m_closed(false)
        , m_startCap(static_cast<CanvasCapStyle>(d2dStrokeStyle->GetStartCap()))
        , m_endCap(static_cast<CanvasCapStyle>(d2dStrokeStyle->GetEndCap()))
        , m_dashCap(static_cast<CanvasCapStyle>(d2dStrokeStyle->GetDashCap()))
        , m_lineJoin(static_cast<CanvasLineJoin>(d2dStrokeStyle->GetLineJoin()))
        , m_miterLimit(d2dStrokeStyle->GetMiterLimit())
        , m_dashStyle(static_cast<CanvasDashStyle>(d2dStrokeStyle->GetDashStyle()))
        , m_dashOffset(d2dStrokeStyle->GetDashOffset())
        , m_transformBehavior(static_cast<CanvasStrokeTransformBehavior>(d2dStrokeStyle->GetStrokeTransformType()))
        , m_d2dStrokeStyle(d2dStrokeStyle)
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

            UINT32 customDashElementCount = d2dStrokeStyle->GetDashesCount();
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
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_startCap;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_StartCap(_In_ CanvasCapStyle value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_startCap = value;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::get_EndCap(_Out_ CanvasCapStyle* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_endCap;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_EndCap(_In_ CanvasCapStyle value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_endCap = value;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::get_DashCap(_Out_ CanvasCapStyle* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_dashCap;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_DashCap(_In_ CanvasCapStyle value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_dashCap = value;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::get_LineJoin(_Out_ CanvasLineJoin* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_lineJoin;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_LineJoin(_In_ CanvasLineJoin value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_lineJoin = value;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::get_MiterLimit(_Out_ float* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_miterLimit;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_MiterLimit(_In_ float value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_miterLimit = value;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::get_DashStyle(_Out_ CanvasDashStyle* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_dashStyle;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_DashStyle(_In_ CanvasDashStyle value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_dashStyle = value;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::get_DashOffset(_Out_ float* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_dashOffset;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_DashOffset(_In_ float value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_dashOffset = value;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::get_CustomDashStyle(
        UINT32 *valueCount,
        float **valueElements)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(valueCount);
                CheckAndClearOutPointer(valueElements);
                ThrowIfClosed();

                assert(m_customDashElements.size() <= UINT_MAX);

                (*valueCount) = static_cast<UINT32>(m_customDashElements.size());
                (*valueElements) = static_cast<float*>(CoTaskMemAlloc(m_customDashElements.size() * sizeof(float)));
                ThrowIfNullPointer(*valueElements, E_OUTOFMEMORY);

                if (m_customDashElements.size() > 0)
                {
                    memcpy(*valueElements, &(m_customDashElements[0]), m_customDashElements.size() * sizeof(float));
                }
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_CustomDashStyle(
        UINT32 valueCount,
        float *valueElements)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_customDashElements.assign(valueElements, valueElements + valueCount);
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::get_TransformBehavior(_Out_ CanvasStrokeTransformBehavior* value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                *value = m_transformBehavior;
            });
    }

    IFACEMETHODIMP CanvasStrokeStyle::put_TransformBehavior(_In_ CanvasStrokeTransformBehavior value)
    {
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                m_d2dStrokeStyle.Reset();
                m_transformBehavior = value;
            });
    }

    //
    // IClosable
    //
    IFACEMETHODIMP CanvasStrokeStyle::Close()
    {
        m_d2dStrokeStyle.Reset();
        m_closed = true;

        return S_OK;
    }


    //
    // ICanvasStrokeStyleInternal
    //

    ComPtr<ID2D1StrokeStyle1> CanvasStrokeStyle::GetRealizedD2DStrokeStyle(ID2D1Factory2* d2dFactory)
    {
        //
        // If there is already a realization, ensure its factory matches the target factory.
        // If not, invalidate and re-realize.
        //
        // May be possible to forego re-realizing in some more cases. See item #1589.
        //
        if (m_d2dStrokeStyle)
        {
            ComPtr<ID2D1Factory> realizationFactoryBase;
            m_d2dStrokeStyle->GetFactory(&realizationFactoryBase);

            ComPtr<ID2D1Factory2> realizationFactory;
            ThrowIfFailed(realizationFactoryBase.As(&realizationFactory));

            if (realizationFactory.Get() != d2dFactory)
            {
                m_d2dStrokeStyle.Reset();
            }
        }

        if (!m_d2dStrokeStyle)
        {

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

            // Potential thread safety problem here. Need to ensure resource creation, including
            // device-independent resource creation, is per-thread. See #802.

            assert(m_customDashElements.size() <= UINT_MAX);

            ThrowIfFailed(d2dFactory->CreateStrokeStyle(
                strokeStyleProperties,
                dashArray,
                static_cast<UINT32>(m_customDashElements.size()),
                &m_d2dStrokeStyle));
        }

        return m_d2dStrokeStyle;
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
    IFACEMETHODIMP CanvasStrokeStyleFactory::ActivateInstance(IInspectable **object)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto newCanvasStrokeStyle = Make<CanvasStrokeStyle>();

                CheckMakeResult(newCanvasStrokeStyle);

                ThrowIfFailed(newCanvasStrokeStyle.CopyTo(object));
            });
    }


    IFACEMETHODIMP CanvasStrokeStyleFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        // Call Create here
        return ExceptionBoundary(
            [&]()
        {
            ComPtr<ID2D1StrokeStyle1> d2dStrokeStyle;

            ThrowIfFailed(resource->QueryInterface(d2dStrokeStyle.GetAddressOf()));

            auto newCanvasStrokeStyle = Make<CanvasStrokeStyle>(d2dStrokeStyle.Get());

            CheckMakeResult(newCanvasStrokeStyle);

            ThrowIfFailed(newCanvasStrokeStyle.CopyTo(wrapper));
        });
    }

    ActivatableClassWithFactory(CanvasStrokeStyle, CanvasStrokeStyleFactory);
}

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

#include "CanvasDrawingSession.h"
#include "CanvasStrokeStyle.h"
#include "CanvasTextFormat.h"
#include "CanvasImage.h"
#include "CanvasDevice.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    inline ComPtr<ID2D1StrokeStyle1> ToD2DStrokeStyle(ICanvasStrokeStyle* strokeStyle, ID2D1DeviceContext* deviceContext)
    {
        if (!strokeStyle) return nullptr;

        ComPtr<ID2D1Factory> d2dBaseFactory;
        deviceContext->GetFactory(&d2dBaseFactory);

        ComPtr<ID2D1Factory2> d2dFactory;
        ThrowIfFailed(d2dBaseFactory.As(&d2dFactory));

        ComPtr<ICanvasStrokeStyleInternal> internal;
        ThrowIfFailed(strokeStyle->QueryInterface(internal.GetAddressOf()));

        return internal->GetRealizedD2DStrokeStyle(d2dFactory.Get());
    }
    
    IFACEMETHODIMP CanvasDrawingSessionFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<ID2D1DeviceContext1> deviceContext;

                ThrowIfFailed(resource->QueryInterface(deviceContext.GetAddressOf()));

                auto newDrawingSession = GetManager()->GetOrCreate(deviceContext.Get());

                ThrowIfFailed(newDrawingSession.CopyTo(wrapper));
            });
    }


    //
    // This drawing session adapter is used when wrapping an existing
    // ID2D1DeviceContext.  In this wrapper, interop, case we don't want
    // CanvasDrawingSession to call any additional methods in the device context.
    //
    class NoopCanvasDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
    public:

        virtual D2D1_POINT_2F GetRenderingSurfaceOffset() override
        {
            return D2D1::Point2F(0, 0);
        }

        virtual void EndDraw() override
        {
            // nothing
        }
    };


    CanvasDrawingSessionManager::CanvasDrawingSessionManager()
        : m_adapter(std::make_shared<NoopCanvasDrawingSessionAdapter>())
    {
    }

    ComPtr<CanvasDrawingSession> CanvasDrawingSessionManager::CreateNew(
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter)
    {
        return CreateNew(nullptr, deviceContext, drawingSessionAdapter);
    }

    ComPtr<CanvasDrawingSession> CanvasDrawingSessionManager::CreateNew(
        ICanvasDevice* owner,
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter)
    {
        return Make<CanvasDrawingSession>(
            shared_from_this(),
            owner,
            deviceContext,
            drawingSessionAdapter);
    }


    ComPtr<CanvasDrawingSession> CanvasDrawingSessionManager::CreateWrapper(
        ID2D1DeviceContext1* resource)
    {
        auto drawingSession = Make<CanvasDrawingSession>(
            shared_from_this(), 
            nullptr,
            resource, 
            m_adapter);
        CheckMakeResult(drawingSession);
        return drawingSession;
    }


    CanvasDrawingSession::CanvasDrawingSession(
        std::shared_ptr<CanvasDrawingSessionManager> manager,
        ICanvasDevice* owner,
        ID2D1DeviceContext1* deviceContext,
        std::shared_ptr<ICanvasDrawingSessionAdapter> adapter)
        : ResourceWrapper(manager, deviceContext)
        , m_owner(owner)
        , m_adapter(adapter)
    {
        CheckInPointer(adapter.get());
    }


    CanvasDrawingSession::~CanvasDrawingSession()
    {
        // Ignore any errors when closing during destruction
        (void)Close();
    }


    IFACEMETHODIMP CanvasDrawingSession::Close()
    {
        // Base class Close() called outside of ExceptionBoundary since this
        // already has it's own boundary.
        HRESULT hr = ResourceWrapper::Close();
        if (FAILED(hr))
            return hr;

        return ExceptionBoundary(
            [&]
            {
                if (m_adapter)
                {
                    // Arrange it so that m_adapter will always get
                    // reset, even if EndDraw throws.
                    auto adapter = m_adapter;
                    m_adapter.reset();

                    adapter->EndDraw();
                }        
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::Clear(
        ABI::Windows::UI::Color color)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                auto d2dColor = ToD2DColor(color);
                deviceContext->Clear(&d2dColor);
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImage(
        ICanvasImage* image)
    {
        ABI::Windows::Foundation::Point point;
        point.X = 0;
        point.Y = 0;

        return DrawImageWithOffset(
            image,
            point);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawImageWithOffset(
        ICanvasImage* image,
        ABI::Windows::Foundation::Point offset)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(image);
                auto& deviceContext = GetResource();

                ComPtr<ICanvasImageInternal> internal;
                ThrowIfFailed(image->QueryInterface(IID_PPV_ARGS(&internal)));

                deviceContext->DrawImage(internal->GetD2DImage(deviceContext.Get()).Get(), ToD2DPoint(offset));
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawLine(
        ABI::Windows::Foundation::Point point0,
        ABI::Windows::Foundation::Point point1,
        ICanvasBrush* brush) 
    {
        return DrawLineWithStrokeWidthAndStrokeStyle(
            point0,
            point1,
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithStrokeWidth(
        ABI::Windows::Foundation::Point point0,
        ABI::Windows::Foundation::Point point1,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return DrawLineWithStrokeWidthAndStrokeStyle(
            point0,
            point1,
            brush,
            strokeWidth,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawLineWithStrokeWidthAndStrokeStyle(
        ABI::Windows::Foundation::Point point0,
        ABI::Windows::Foundation::Point point1,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
        {
            auto& deviceContext = GetResource();
            CheckInPointer(brush);

            deviceContext->DrawLine(
                ToD2DPoint(point0),
                ToD2DPoint(point1),
                ToD2DBrush(brush).Get(),
                strokeWidth,
                ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
        });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangle(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush) 
    {
        return DrawRectangleWithStrokeWidthAndStrokeStyle(
            rect,
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithStrokeWidth(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return DrawRectangleWithStrokeWidthAndStrokeStyle(
            rect,
            brush,
            strokeWidth,
            nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawRectangleWithStrokeWidthAndStrokeStyle(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
        {
            auto& deviceContext = GetResource();
            CheckInPointer(brush);

            deviceContext->DrawRectangle(
                ToD2DRect(rect),
                ToD2DBrush(brush).Get(),
                strokeWidth,
                ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
        });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRectangle(
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(brush);

                deviceContext->FillRectangle(
                    ToD2DRect(rect),
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangle(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush) 
    {
        return DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
            roundedRectangle,
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithStrokeWidth(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
            roundedRectangle,
            brush,
            strokeWidth,
            nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
        {
            auto& deviceContext = GetResource();  
            CheckInPointer(brush);

            deviceContext->DrawRoundedRectangle(
                ToD2DRoundedRect(roundedRectangle),
                ToD2DBrush(brush).Get(),
                strokeWidth,
                ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
        });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillRoundedRectangle(
        CanvasRoundedRectangle roundedRectangle,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(brush);

                deviceContext->FillRoundedRectangle(
                    ToD2DRoundedRect(roundedRectangle),
                    ToD2DBrush(brush).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipse(
        CanvasEllipse ellipse,
        ICanvasBrush* brush) 
    {
        return DrawEllipseWithStrokeWidthAndStrokeStyle(
            ellipse,
            brush,
            1.0f,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithStrokeWidth(
        CanvasEllipse ellipse,
        ICanvasBrush* brush,
        float strokeWidth) 
    {
        return DrawEllipseWithStrokeWidthAndStrokeStyle(
            ellipse,
            brush,
            strokeWidth,
            nullptr);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawEllipseWithStrokeWidthAndStrokeStyle(
        CanvasEllipse ellipse,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource(); 
                CheckInPointer(brush);

                deviceContext->DrawEllipse(
                    ReinterpretAs<D2D1_ELLIPSE*>(&ellipse),
                    ToD2DBrush(brush).Get(),
                    strokeWidth,
                    ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::FillEllipse(
        CanvasEllipse ellipse,
        ICanvasBrush* brush) 
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(brush);

                deviceContext->FillEllipse(
                    ReinterpretAs<D2D1_ELLIPSE*>(&ellipse),
                    ToD2DBrush(brush).Get());
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawCircle(
        ABI::Windows::Foundation::Point centerPoint,
        float radius,
        ICanvasBrush* brush)
    {
        return DrawCircleWithStrokeWidthAndStrokeStyle(
            centerPoint,
            radius,
            brush,
            1.0f,
            nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawCircleWithStrokeWidth(
        ABI::Windows::Foundation::Point centerPoint,
        float radius,
        ICanvasBrush* brush,
        float strokeWidth)
    {
        return DrawCircleWithStrokeWidthAndStrokeStyle(
            centerPoint,
            radius,
            brush,
            strokeWidth,
            nullptr);
    }

    IFACEMETHODIMP CanvasDrawingSession::DrawCircleWithStrokeWidthAndStrokeStyle(
        ABI::Windows::Foundation::Point centerPoint,
        float radius,
        ICanvasBrush* brush,
        float strokeWidth,
        ICanvasStrokeStyle* strokeStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource(); 
                CheckInPointer(brush);

                deviceContext->DrawEllipse(
                    ToD2DEllipse(centerPoint, radius),
                    ToD2DBrush(brush).Get(),
                    strokeWidth,
                    ToD2DStrokeStyle(strokeStyle, deviceContext.Get()).Get());
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::FillCircle(
        ABI::Windows::Foundation::Point centerPoint,
        float radius,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(brush);

                deviceContext->FillEllipse(
                    ToD2DEllipse(centerPoint, radius),
                    ToD2DBrush(brush).Get());
            });
    }

    void CanvasDrawingSession::DrawTextImpl(
        ID2D1DeviceContext* deviceContext,
        HSTRING text,
        const ABI::Windows::Foundation::Rect& rect,
        ICanvasBrush* brush,
        ICanvasTextFormat* format)
    {
        CheckInPointer(text);
        CheckInPointer(brush);
        CheckInPointer(format);

        ComPtr<ICanvasTextFormatInternal> formatInternal;
        ThrowIfFailed(format->QueryInterface(formatInternal.GetAddressOf()));

        uint32_t textLength;
        auto textBuffer = WindowsGetStringRawBuffer(text, &textLength);
        ThrowIfNullPointer(textBuffer, E_INVALIDARG);

        deviceContext->DrawText(
            textBuffer,
            textLength,
            formatInternal->GetRealizedTextFormat().Get(),
            &ToD2DRect(rect),
            ToD2DBrush(brush).Get(),
            static_cast<D2D1_DRAW_TEXT_OPTIONS>(formatInternal->GetDrawTextOptions()));
    }

    
    void CanvasDrawingSession::DrawTextAtPointImpl(
        ID2D1DeviceContext* deviceContext,
        HSTRING text,
        const ABI::Windows::Foundation::Point& point,
        ICanvasBrush* brush,
        ICanvasTextFormat* format)
    {
        CheckInPointer(format);

        // When drawing using just a point we specify a zero sized rectangle and
        // disable word wrapping.
        ABI::Windows::Foundation::Rect rect{point.X, point.Y, 0, 0};
        
        //
        // TODO #802: there's a thread-safety implication since we're modifying state
        // on something that _may_ be being used on another thread.
        //
        CanvasWordWrapping oldWordWrapping{};
        ThrowIfFailed(format->get_WordWrapping(&oldWordWrapping));

        ThrowIfFailed(format->put_WordWrapping(CanvasWordWrapping::NoWrap));

        auto restoreWordWrapping = MakeScopeWarden(
            [&]
            {
                format->put_WordWrapping(oldWordWrapping);
            });

        DrawTextImpl(deviceContext, text, rect, brush, format);
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtPoint(
        HSTRING text,
        ABI::Windows::Foundation::Point point,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto defaultFormat = Make<CanvasTextFormat>();
                DrawTextAtPointImpl(GetResource().Get(), text, point, brush, defaultFormat.Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextAtPointWithFormat(
        HSTRING text,
        ABI::Windows::Foundation::Point point,
        ICanvasBrush* brush,
        ICanvasTextFormat* format)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawTextAtPointImpl(GetResource().Get(), text, point, brush, format);
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawText(
        HSTRING text,
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush)
    {
        return ExceptionBoundary(
            [&]
            {
                auto defaultFormat = Make<CanvasTextFormat>();
                DrawTextImpl(GetResource().Get(), text, rect, brush, defaultFormat.Get());
            });
    }


    IFACEMETHODIMP CanvasDrawingSession::DrawTextWithFormat(
        HSTRING text,
        ABI::Windows::Foundation::Rect rect,
        ICanvasBrush* brush,
        ICanvasTextFormat* format)
    {
        return ExceptionBoundary(
            [&]
            {
                DrawTextImpl(GetResource().Get(), text, rect, brush, format);
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::get_Antialiasing(CanvasAntialiasing* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = static_cast<CanvasAntialiasing>(deviceContext->GetAntialiasMode());
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_Antialiasing(CanvasAntialiasing value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                deviceContext->SetAntialiasMode(static_cast<D2D1_ANTIALIAS_MODE>(value));
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::get_Blend(CanvasBlend* value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = static_cast<CanvasBlend>(deviceContext->GetPrimitiveBlend());
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_Blend(CanvasBlend value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                deviceContext->SetPrimitiveBlend(static_cast<D2D1_PRIMITIVE_BLEND>(value));
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::get_TextAntialiasing(CanvasTextAntialiasing* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = static_cast<CanvasTextAntialiasing>(deviceContext->GetTextAntialiasMode());
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_TextAntialiasing(CanvasTextAntialiasing value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                deviceContext->SetTextAntialiasMode(static_cast<D2D1_TEXT_ANTIALIAS_MODE>(value));
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::get_Transform(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                D2D1_MATRIX_3X2_F transform;
                deviceContext->GetTransform(&transform);
                
                //
                // Un-apply the offset transform. This could be done with a matrix invert, 
                // but this is cheaper.
                //
                // This polls from the wrapped object, rather than stores a local transform 
                // member. This ensures that any transforms performed in native interop
                // will be retrievable here.
                //
                const D2D1_POINT_2F renderingSurfaceOffset = m_adapter->GetRenderingSurfaceOffset();
                transform._31 -= renderingSurfaceOffset.x;
                transform._32 -= renderingSurfaceOffset.y;

                *value = *reinterpret_cast<ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2*>(&transform);
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_Transform(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2 value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                
                D2D1_POINT_2F offset = m_adapter->GetRenderingSurfaceOffset();

                D2D1_MATRIX_3X2_F transform = *(ReinterpretAs<D2D1_MATRIX_3X2_F*>(&value));
                transform._31 += offset.x;
                transform._32 += offset.y;

                deviceContext->SetTransform(transform);
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::get_Units(CanvasUnits* value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();
                CheckInPointer(value);

                *value = static_cast<CanvasUnits>(deviceContext->GetUnitMode());
            });
	}

    IFACEMETHODIMP CanvasDrawingSession::put_Units(CanvasUnits value)
	{
        return ExceptionBoundary(
            [&]
            {
                auto& deviceContext = GetResource();

                deviceContext->SetUnitMode(static_cast<D2D1_UNIT_MODE>(value));
            });
    }

    IFACEMETHODIMP CanvasDrawingSession::get_Device(ICanvasDevice** value)
    {
        using namespace ::Microsoft::WRL::Wrappers;

        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                if (!m_owner)
                {
                    auto& deviceContext = GetResource();

                    ComPtr<ID2D1Device> d2dDeviceBase;
                    deviceContext->GetDevice(&d2dDeviceBase);

                    ComPtr<ID2D1Device1> d2dDevice;
                    ThrowIfFailed(d2dDeviceBase.As(&d2dDevice));

                    auto canvasDeviceManager = CanvasDeviceFactory::GetOrCreateManager();

                    m_owner = canvasDeviceManager->GetOrCreate(d2dDevice.Get());
                }

                ThrowIfFailed(m_owner.CopyTo(value));
        });
    }

    ActivatableStaticOnlyFactory(CanvasDrawingSessionFactory);
}}}}

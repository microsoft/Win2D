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

#pragma once

#include "ClosablePtr.h"
#include "ErrorHandling.h"

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ABI::Windows::Foundation;
    using namespace Microsoft::WRL;

    class ICanvasDrawingSessionAdapter
    {
    public:
        virtual ~ICanvasDrawingSessionAdapter() = default;
        virtual D2D1_POINT_2F GetRenderingSurfaceOffset() = 0;
        virtual void EndDraw() = 0;
    };

    class CanvasDrawingSessionManager;
    class CanvasDrawingSession;

    class CanvasDrawingSessionFactory
        : public ActivationFactory<ICanvasDrawingSessionStatics, CloakedIid<ICanvasFactoryNative>>,
          public FactoryWithResourceManager<CanvasDrawingSessionFactory, CanvasDrawingSessionManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawingSession, BaseTrust);

    public:
        //
        // ICanvasFactoryNative
        //

        IFACEMETHOD(GetOrCreate)(
            IUnknown* resource,
            IInspectable** wrapper) override;        
    };

    struct CanvasDrawingSessionTraits
    {
        typedef ID2D1DeviceContext1 resource_t;
        typedef CanvasDrawingSession wrapper_t;
        typedef ICanvasDrawingSession wrapper_interface_t;
        typedef CanvasDrawingSessionManager manager_t;
    };

    class CanvasDrawingSession : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasDrawingSessionTraits,
        ICanvasResourceCreator)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasDrawingSession, BaseTrust);

        std::shared_ptr<ICanvasDrawingSessionAdapter> m_adapter;

        //
        // Contract:
        //     Drawing sessions created conventionally initialize this member.
        //     Drawing sessions created through interop set this member to null.
        //
        //     The thing this affects is DrawingSession's use as an ICanvasResourceCreator.
        //     If the backpointer is initialized, that is the resource creator's device.
        //     If the backpointer is null, a CanvasDevice wrapper is produced based on 
        //     this drawing session's device context. That wrapper is created on demand 
        //     by get_Device.
        //
        ComPtr<ICanvasDevice> m_owner;

    public:
        CanvasDrawingSession(
            std::shared_ptr<CanvasDrawingSessionManager> manager,
            ICanvasDevice* owner,
            ID2D1DeviceContext1* deviceContext,
            std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter);

        virtual ~CanvasDrawingSession();

        // IClosable

        IFACEMETHOD(Close)() override;

        // ICanvasDrawingSession

        IFACEMETHOD(Clear)(
            ABI::Windows::UI::Color color) override;

        IFACEMETHOD(DrawImage)(
            ICanvasImage* image) override;

        IFACEMETHOD(DrawImageWithOffset)(
            ICanvasImage* image,
            ABI::Windows::Foundation::Point offset) override;

        IFACEMETHOD(DrawLine)(
            ABI::Windows::Foundation::Point point0,
            ABI::Windows::Foundation::Point point1,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawLineWithStrokeWidth)(
            ABI::Windows::Foundation::Point point0,
            ABI::Windows::Foundation::Point point1,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawLineWithStrokeWidthAndStrokeStyle)(
            ABI::Windows::Foundation::Point point0,
            ABI::Windows::Foundation::Point point1,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(DrawRectangle)(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawRectangleWithStrokeWidth)(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawRectangleWithStrokeWidthAndStrokeStyle)(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(FillRectangle)(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawRoundedRectangle)(
            CanvasRoundedRectangle roundedRectangle,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawRoundedRectangleWithStrokeWidth)(
            CanvasRoundedRectangle roundedRectangle,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawRoundedRectangleWithStrokeWidthAndStrokeStyle)(
            CanvasRoundedRectangle roundedRectangle,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(FillRoundedRectangle)(
            CanvasRoundedRectangle roundedRectangle,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawEllipse)(
            CanvasEllipse ellipse,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawEllipseWithStrokeWidth)(
            CanvasEllipse ellipse,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawEllipseWithStrokeWidthAndStrokeStyle)(
            CanvasEllipse ellipse,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(FillEllipse)(
            CanvasEllipse ellipse,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawCircle)(
            ABI::Windows::Foundation::Point centerPoint,
            float radius,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawCircleWithStrokeWidth)(
            ABI::Windows::Foundation::Point centerPoint,
            float radius,
            ICanvasBrush* brush,
            float strokeWidth) override;

        IFACEMETHOD(DrawCircleWithStrokeWidthAndStrokeStyle)(
            ABI::Windows::Foundation::Point centerPoint,
            float radius,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override;

        IFACEMETHOD(FillCircle)(
            ABI::Windows::Foundation::Point centerPoint,
            float radius,
            ICanvasBrush* brush) override;

        IFACEMETHOD(DrawTextAtPoint)(
            HSTRING text,
            ABI::Windows::Foundation::Point point,
            ICanvasBrush* brush) override;
        
        IFACEMETHOD(DrawTextAtPointWithFormat)(
            HSTRING text,
            ABI::Windows::Foundation::Point point,
            ICanvasBrush* brush,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(DrawText)(
            HSTRING text,
            ABI::Windows::Foundation::Rect rectangle,
            ICanvasBrush* brush) override;
        
        IFACEMETHOD(DrawTextWithFormat)(
            HSTRING text,
            ABI::Windows::Foundation::Rect rectangle,
            ICanvasBrush* brush,
            ICanvasTextFormat* format) override;

        IFACEMETHOD(get_Antialiasing)(CanvasAntialiasing* value);
        IFACEMETHOD(put_Antialiasing)(CanvasAntialiasing value);

        IFACEMETHOD(get_Blend)(CanvasBlend* value);
        IFACEMETHOD(put_Blend)(CanvasBlend value);

        IFACEMETHOD(get_TextAntialiasing)(CanvasTextAntialiasing* value);
        IFACEMETHOD(put_TextAntialiasing)(CanvasTextAntialiasing value);

        IFACEMETHOD(get_Transform)(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2* value);
        IFACEMETHOD(put_Transform)(ABI::Microsoft::Graphics::Canvas::Numerics::Matrix3x2 value);

        IFACEMETHOD(get_Units)(CanvasUnits* value);
        IFACEMETHOD(put_Units)(CanvasUnits value);

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value);

    private:
        void DrawTextAtPointImpl(
            ID2D1DeviceContext* deviceContext,
            HSTRING text,
            const ABI::Windows::Foundation::Point& point,
            ICanvasBrush* brush,
            ICanvasTextFormat* format);

        void DrawTextImpl(
            ID2D1DeviceContext* deviceContext,
            HSTRING text,
            const ABI::Windows::Foundation::Rect& rect,
            ICanvasBrush* brush,
            ICanvasTextFormat* format);
    };


    class CanvasDrawingSessionManager : public ResourceManager<CanvasDrawingSessionTraits>
    {
        std::shared_ptr<ICanvasDrawingSessionAdapter> m_adapter;

    public:
        CanvasDrawingSessionManager();

        ComPtr<CanvasDrawingSession> CreateNew(
            ICanvasDevice* owner,
            ID2D1DeviceContext1* deviceContext,
            std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter);

        ComPtr<CanvasDrawingSession> CreateNew(
            ID2D1DeviceContext1* deviceContext,
            std::shared_ptr<ICanvasDrawingSessionAdapter> drawingSessionAdapter);

        ComPtr<CanvasDrawingSession> CreateWrapper(
            ID2D1DeviceContext1* resource);
    };
}

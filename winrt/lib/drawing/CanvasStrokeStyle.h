// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "utils/LockUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    using namespace ::Microsoft::WRL;

    class CanvasStrokeStyle;

    [uuid(45BBB5CD-F9FB-4786-9491-D9BF0354DFF6)]
    class ICanvasStrokeStyleInternal : public IUnknown
    {
    public:
        // This realizes the stroke style if necessary.
        virtual ComPtr<ID2D1StrokeStyle1> GetRealizedD2DStrokeStyle(ID2D1Factory* d2dFactory) = 0;
    };

    class CanvasStrokeStyleFactory
        : public ActivationFactory<>
        , private LifespanTracker<CanvasStrokeStyleFactory>
    {
    public:
        IFACEMETHOD(ActivateInstance)(_COM_Outptr_ IInspectable** ppvObject) override;
    };

    class CanvasStrokeStyle : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1StrokeStyle1,
        CanvasStrokeStyle,
        ICanvasStrokeStyle,
        CloakedIid<ICanvasStrokeStyleInternal>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Geometry_CanvasStrokeStyle, BaseTrust);

        std::mutex m_mutex;

        CanvasCapStyle m_startCap;
        CanvasCapStyle m_endCap;
        CanvasCapStyle m_dashCap;
        CanvasLineJoin m_lineJoin;
        float m_miterLimit;
        CanvasDashStyle m_dashStyle;
        float m_dashOffset;
        std::vector<float> m_customDashElements;
        CanvasStrokeTransformBehavior m_transformBehavior;

        //
        // Other interfaces simply check if their contained D2D resource is NULL in order
        // to determine if this object was closed. That isn't sufficient for CanvasStrokeStyle.
        // The contained D2D resource could be NULL at any given time.
        //
        bool m_closed;

    public:
        CanvasStrokeStyle();

        CanvasStrokeStyle(ID2D1StrokeStyle1* d2dStrokeStyle);

        IFACEMETHOD(get_StartCap)(_Out_ CanvasCapStyle* value) override;

        IFACEMETHOD(put_StartCap)(_In_ CanvasCapStyle value) override;

        IFACEMETHOD(get_EndCap)(_Out_ CanvasCapStyle* value) override;

        IFACEMETHOD(put_EndCap)(_In_ CanvasCapStyle value) override;

        IFACEMETHOD(get_DashCap)(_Out_ CanvasCapStyle* value) override;

        IFACEMETHOD(put_DashCap)(_In_ CanvasCapStyle value) override;

        IFACEMETHOD(get_LineJoin)(_Out_ CanvasLineJoin* value) override;

        IFACEMETHOD(put_LineJoin)(_In_ CanvasLineJoin value) override;

        IFACEMETHOD(get_MiterLimit)(_Out_ float* value) override;

        IFACEMETHOD(put_MiterLimit)(_In_ float value) override;

        IFACEMETHOD(get_DashStyle)(_Out_ CanvasDashStyle* value) override;

        IFACEMETHOD(put_DashStyle)(_In_ CanvasDashStyle value) override;

        IFACEMETHOD(get_DashOffset)(_Out_ float* value) override;

        IFACEMETHOD(put_DashOffset)(_In_ float value) override;

        IFACEMETHOD(get_CustomDashStyle)(
            UINT32 *valueCount,
            float **valueElements);

        IFACEMETHOD(put_CustomDashStyle)(
            UINT32 valueCount,
            float *valueElements);

        IFACEMETHOD(get_TransformBehavior)(_Out_ CanvasStrokeTransformBehavior* value) override;

        IFACEMETHOD(put_TransformBehavior)(_In_ CanvasStrokeTransformBehavior value) override;

        // IClosable
        IFACEMETHOD(Close)() override;

        // ICanvasStrokeStyleInternal
        virtual ComPtr<ID2D1StrokeStyle1> GetRealizedD2DStrokeStyle(ID2D1Factory* d2dFactory) override;

        // ICanvasResourceWrapperNative
        IFACEMETHOD(GetNativeResource)(ICanvasDevice* device, float dpi, REFIID iid, void** outResource) override;

    private:
        void ThrowIfClosed();

        Lock GetLock()
        {
            return Lock(m_mutex);
        }
    };
}}}}}

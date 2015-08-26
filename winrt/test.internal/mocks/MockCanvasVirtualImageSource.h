// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class MockCanvasVirtualImageSource
    : public RuntimeClass<ICanvasVirtualImageSource>
{
public:
    CALL_COUNTER_WITH_MOCK(get_SourceMethod, HRESULT(IVirtualSurfaceImageSource**));
    CALL_COUNTER_WITH_MOCK(CreateDrawingSessionMethod, HRESULT(Color,Rect,ICanvasDrawingSession**));
    CALL_COUNTER_WITH_MOCK(SuspendDrawingSessionMethod, HRESULT(ICanvasDrawingSession*));
    CALL_COUNTER_WITH_MOCK(ResumeDrawingSessionMethod, HRESULT(ICanvasDrawingSession*));
    CALL_COUNTER_WITH_MOCK(InvalidateMethod, HRESULT());
    CALL_COUNTER_WITH_MOCK(InvalidateRegionMethod, HRESULT(Rect));
    CALL_COUNTER_WITH_MOCK(RaiseRegionsInvalidatedIfAnyMethod, HRESULT());
    CALL_COUNTER_WITH_MOCK(add_RegionsInvalidatedMethod, HRESULT(ImageSourceRegionsInvalidatedHandler* ,EventRegistrationToken*));
    CALL_COUNTER_WITH_MOCK(remove_RegionsInvalidatedMethod, HRESULT(EventRegistrationToken));
    CALL_COUNTER_WITH_MOCK(ResizeWithSizeMethod, HRESULT(Size));
    CALL_COUNTER_WITH_MOCK(ResizeWithWidthAndHeightMethod, HRESULT(float,float));
    CALL_COUNTER_WITH_MOCK(ResizeWithWidthAndHeightAndDpiMethod, HRESULT(float,float,float));
    CALL_COUNTER_WITH_MOCK(get_DeviceMethod, HRESULT(ICanvasDevice**));
    CALL_COUNTER_WITH_MOCK(RecreateMethod, HRESULT(ICanvasResourceCreator*));
    CALL_COUNTER_WITH_MOCK(get_DpiMethod, HRESULT(float*));
    CALL_COUNTER_WITH_MOCK(ConvertPixelsToDipsMethod, HRESULT(INT32,float*));
    CALL_COUNTER_WITH_MOCK(ConvertDipsToPixelsMethod, HRESULT(float,CanvasDpiRounding,INT32*));
    CALL_COUNTER_WITH_MOCK(get_SizeMethod, HRESULT(Size*));
    CALL_COUNTER_WITH_MOCK(get_SizeInPixelsMethod, HRESULT(BitmapSize*));
    CALL_COUNTER_WITH_MOCK(get_AlphaModeMethod, HRESULT(CanvasAlphaMode*));

    //
    // ICanvasVirtualImageSource
    //
    
    IFACEMETHODIMP get_Source(IVirtualSurfaceImageSource** value) override
    {
        return get_SourceMethod.WasCalled(value);
    }
    
    IFACEMETHODIMP CreateDrawingSession(Color c, Rect r, ICanvasDrawingSession** d) override
    {
        return CreateDrawingSessionMethod.WasCalled(c, r, d);
    }
    
    IFACEMETHODIMP SuspendDrawingSession(ICanvasDrawingSession* d) override
    {
        return SuspendDrawingSessionMethod.WasCalled(d);
    }
    
    IFACEMETHODIMP ResumeDrawingSession(ICanvasDrawingSession* d) override
    {
        return ResumeDrawingSessionMethod.WasCalled(d);
    }

    IFACEMETHODIMP Invalidate() override
    {
        return InvalidateMethod.WasCalled();
    }

    IFACEMETHODIMP InvalidateRegion(Rect r) override
    {
        return InvalidateRegionMethod.WasCalled(r);
    }

    IFACEMETHODIMP RaiseRegionsInvalidatedIfAny() override
    {
        return RaiseRegionsInvalidatedIfAnyMethod.WasCalled();
    }

    IFACEMETHODIMP add_RegionsInvalidated(ImageSourceRegionsInvalidatedHandler* h, EventRegistrationToken* t) override
    {
        return add_RegionsInvalidatedMethod.WasCalled(h, t);
    }

    IFACEMETHODIMP remove_RegionsInvalidated(EventRegistrationToken t) override
    {
        return remove_RegionsInvalidatedMethod.WasCalled(t);
    }

    IFACEMETHODIMP ResizeWithSize(Size s) override
    {
        return ResizeWithSizeMethod.WasCalled(s);
    }

    IFACEMETHODIMP ResizeWithWidthAndHeight(float w, float h) override
    {
        return ResizeWithWidthAndHeightMethod.WasCalled(w, h);
    }

    IFACEMETHODIMP ResizeWithWidthAndHeightAndDpi(float w, float h, float d) override
    {
        return ResizeWithWidthAndHeightAndDpiMethod.WasCalled(w, h, d);
    }

    IFACEMETHODIMP get_Device(ICanvasDevice** value) override
    {
        return get_DeviceMethod.WasCalled(value);
    }

    IFACEMETHODIMP Recreate(ICanvasResourceCreator* d) override
    {
        return RecreateMethod.WasCalled(d);
    }

    IFACEMETHODIMP get_Dpi(float* value) override
    {
        return get_DpiMethod.WasCalled(value);
    }

    IFACEMETHODIMP ConvertPixelsToDips(int p, float* d) override
    {
        return ConvertPixelsToDipsMethod.WasCalled(p, d);
    }

    IFACEMETHODIMP ConvertDipsToPixels(float d, CanvasDpiRounding r, int* p) override
    {
        return ConvertDipsToPixelsMethod.WasCalled(d, r, p);
    }

    IFACEMETHODIMP get_Size(Size* value) override
    {
        return get_SizeMethod.WasCalled(value);
    }

    IFACEMETHODIMP get_SizeInPixels(BitmapSize* value) override
    {
        return get_SizeInPixelsMethod.WasCalled(value);
    }

    IFACEMETHODIMP get_AlphaMode(CanvasAlphaMode* value) override
    {
        return get_AlphaModeMethod.WasCalled(value);
    }
};

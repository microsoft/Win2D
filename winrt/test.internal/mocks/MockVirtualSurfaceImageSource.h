// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

using namespace ABI::Windows::UI::Xaml::Media::Imaging;

class MockVirtualSurfaceImageSource : public RuntimeClass<
    RuntimeClassFlags<WinRtClassicComMix>,
    IVirtualSurfaceImageSource,
    IImageSource,
    IDependencyObject,
    ChainInterfaces<IVirtualSurfaceImageSourceNative, ISurfaceImageSourceNative>,    
    ISurfaceImageSourceNativeWithD2D>
{
public:
    CALL_COUNTER_WITH_MOCK(ISurfaceImageSource_SetDeviceMethod, HRESULT(IDXGIDevice*));
    CALL_COUNTER_WITH_MOCK(ISurfaceImageSource_BeginDrawMethod, HRESULT(RECT, IDXGISurface**, POINT*));

    CALL_COUNTER_WITH_MOCK(InvalidateMethod, HRESULT(RECT));
    CALL_COUNTER_WITH_MOCK(GetUpdateRectCountMethod, HRESULT(DWORD*));    
    CALL_COUNTER_WITH_MOCK(GetUpdateRectsMethod, HRESULT(RECT*, DWORD));
    CALL_COUNTER_WITH_MOCK(GetVisibleBoundsMethod, HRESULT(RECT*));
    CALL_COUNTER_WITH_MOCK(RegisterForUpdatesNeededMethod, HRESULT(IVirtualSurfaceUpdatesCallbackNative*));
    CALL_COUNTER_WITH_MOCK(ResizeMethod, HRESULT(int, int));

    CALL_COUNTER_WITH_MOCK(GetValueMethod, HRESULT(IDependencyProperty*, IInspectable**));
    CALL_COUNTER_WITH_MOCK(SetValueMethod, HRESULT(IDependencyProperty*, IInspectable*));
    CALL_COUNTER_WITH_MOCK(ClearValueMethod, HRESULT(IDependencyProperty*));
    CALL_COUNTER_WITH_MOCK(ReadLocalValueMethod, HRESULT(IDependencyProperty*, IInspectable**));
    CALL_COUNTER_WITH_MOCK(GetAnimationBaseValueMethod, HRESULT(IDependencyProperty*, IInspectable**));
    CALL_COUNTER_WITH_MOCK(get_DispatcherMethod, HRESULT(ICoreDispatcher**));    

    CALL_COUNTER_WITH_MOCK(SetDeviceMethod, HRESULT(IUnknown*));
    CALL_COUNTER_WITH_MOCK(BeginDrawMethod, HRESULT(RECT const&, IID const&, void**, POINT*));
    CALL_COUNTER_WITH_MOCK(EndDrawMethod, HRESULT());
    CALL_COUNTER_WITH_MOCK(SuspendDrawMethod, HRESULT());
    CALL_COUNTER_WITH_MOCK(ResumeDrawMethod, HRESULT());

    //
    // ISurfaceImageSource
    //
    
    IFACEMETHODIMP SetDevice(IDXGIDevice* device) override
    {
        return ISurfaceImageSource_SetDeviceMethod.WasCalled(device);
    }
    
    IFACEMETHODIMP BeginDraw(RECT updateRect, IDXGISurface** surface, POINT* offset) override
    {
        return ISurfaceImageSource_BeginDrawMethod.WasCalled(updateRect, surface, offset);
    }

    //
    // IVirtualSurfaceImageSource
    //
    
    IFACEMETHODIMP Invalidate(RECT updateRect) override
    {
        return InvalidateMethod.WasCalled(updateRect);
    }
    
    IFACEMETHODIMP GetUpdateRectCount(DWORD* count) override
    {
        return GetUpdateRectCountMethod.WasCalled(count);
    }
    
    IFACEMETHODIMP GetUpdateRects(RECT* updates, DWORD count) override
    {
        return GetUpdateRectsMethod.WasCalled(updates, count);
    }
    
    IFACEMETHODIMP GetVisibleBounds(RECT* bounds) override
    {
        return GetVisibleBoundsMethod.WasCalled(bounds);
    }
    
    IFACEMETHODIMP RegisterForUpdatesNeeded(IVirtualSurfaceUpdatesCallbackNative* callback) override
    {
        return RegisterForUpdatesNeededMethod.WasCalled(callback);
    }
    
    IFACEMETHODIMP Resize(int width, int height) override
    {
        return ResizeMethod.WasCalled(width, height);
    }

    //
    // IDependencyObject
    //

    IFACEMETHODIMP GetValue(IDependencyProperty* p, IInspectable** v) override
    {
        return GetValueMethod.WasCalled(p, v);
    }
    
    IFACEMETHODIMP SetValue(IDependencyProperty* p, IInspectable* v) override
    {
        return SetValueMethod.WasCalled(p, v);
    }
    
    IFACEMETHODIMP ClearValue(IDependencyProperty* p) override
    {
        return ClearValueMethod.WasCalled(p);
    }
    
    IFACEMETHODIMP ReadLocalValue(IDependencyProperty* p, IInspectable** v) override
    {
        return ReadLocalValueMethod.WasCalled(p, v);
    }
    
    IFACEMETHODIMP GetAnimationBaseValue(IDependencyProperty* p, IInspectable** v) override
    {
        return GetAnimationBaseValueMethod.WasCalled(p, v);
    }
    
    IFACEMETHODIMP get_Dispatcher(ICoreDispatcher** v) override
    {
        return get_DispatcherMethod.WasCalled(v);
    }    

    //
    // ISurfaceImageSourceNativeWithD2D
    //
    
    IFACEMETHODIMP SetDevice(IUnknown* device) override 
    {
        return SetDeviceMethod.WasCalled(device);
    }
    
    IFACEMETHODIMP BeginDraw(RECT const& updateRect, IID const& iid, void** updateObject, POINT* offset) override 
    {
        return BeginDrawMethod.WasCalled(updateRect, iid, updateObject, offset);
    }
    
    IFACEMETHODIMP EndDraw() override 
    {
        return EndDrawMethod.WasCalled();
    }
    
    IFACEMETHODIMP SuspendDraw() override 
    {
        return SuspendDrawMethod.WasCalled();
    }
    
    IFACEMETHODIMP ResumeDraw() override 
    {
        return ResumeDrawMethod.WasCalled();
    }
};

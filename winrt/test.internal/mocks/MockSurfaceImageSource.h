// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    using namespace ABI::Microsoft::UI::Xaml::Media::Imaging;
    using namespace ABI::Microsoft::UI::Xaml::Media;

    class MockSurfaceImageSource : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ISurfaceImageSource,
        IImageSource,
        ISurfaceImageSourceNativeWithD2D>
    {
    public:
        CALL_COUNTER_WITH_MOCK(SetDeviceMethod, HRESULT(IUnknown*));
        CALL_COUNTER_WITH_MOCK(BeginDrawMethod, HRESULT(RECT const&, IID const&, void**, POINT*));
        CALL_COUNTER(EndDrawMethod);

        // ISurfaceImageSourceNativeWithD2D
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
            EndDrawMethod.WasCalled();
            return S_OK;
        }

        IFACEMETHODIMP SuspendDraw() override 
        {
            Assert::Fail(L"Unexpected call to SuspendDraw");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP ResumeDraw() override 
        {
            Assert::Fail(L"Unexpected call to ResumeDraw");
            return E_NOTIMPL;
        }
    };
}

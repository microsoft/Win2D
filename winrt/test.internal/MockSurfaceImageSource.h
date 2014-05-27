// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;

    class MockSurfaceImageSource : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ISurfaceImageSource,
        ISurfaceImageSourceNativeWithD2D>
    {
    public:
        std::function<void(IUnknown*)> MockSetDevice;
        std::function<void(const RECT&,const IID&,void**,POINT*)> MockBeginDraw;
        std::function<void()> MockEndDraw;

        // ISurfaceImageSourceNativeWithD2D
        IFACEMETHODIMP SetDevice(IUnknown* device) override 
        {
            if (!MockSetDevice)
            {
                Assert::Fail(L"Unexpected call to SetDevice");
                return E_NOTIMPL;
            }

            return ExceptionBoundary(
                [&]()
                {
                    MockSetDevice(device);
                });
        }

        IFACEMETHODIMP BeginDraw(const RECT& updateRect, const IID& iid, void** updateObject, POINT* offset) override 
        {
            if (!MockBeginDraw)
            {
                Assert::Fail(L"Unexpected call to BeginDraw");
                return E_NOTIMPL;
            }

            return ExceptionBoundary(
                [&]()
                {
                    MockBeginDraw(updateRect, iid, updateObject, offset);
                });
        }

        IFACEMETHODIMP EndDraw() override 
        {
            if (!MockEndDraw)
            {
                Assert::Fail(L"Unexpected call to EndDraw");
                return E_NOTIMPL;
            }

            return ExceptionBoundary(
                [&]()
                {
                    MockEndDraw();
                });
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

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

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;
    using namespace ABI::Windows::UI::Xaml::Media;

    class MockSurfaceImageSource : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ISurfaceImageSource,
        IImageSource,
        ISurfaceImageSourceNativeWithD2D>
    {
    public:
        std::function<void(IUnknown*)> MockSetDevice;
        std::function<void(RECT const&,IID const&,void**,POINT*)> MockBeginDraw;
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
                [&]
                {
                    MockSetDevice(device);
                });
        }

        IFACEMETHODIMP BeginDraw(RECT const& updateRect, IID const& iid, void** updateObject, POINT* offset) override 
        {
            if (!MockBeginDraw)
            {
                Assert::Fail(L"Unexpected call to BeginDraw");
                return E_NOTIMPL;
            }

            return ExceptionBoundary(
                [&]
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
                [&]
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

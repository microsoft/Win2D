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
    class MockCanvasDevice : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasDevice,
        CloakedIid<ICanvasDeviceInternal>,
        ICanvasResourceCreator>
    {
    public:        
        std::function<ComPtr<ID2D1Device1>()> MockGetD2DDevice;
        std::function<void(ICanvasDevice**)> Mockget_Device;
        std::function<ComPtr<ID2D1SolidColorBrush>(const D2D1_COLOR_F&)> MockCreateSolidColorBrush;
        std::function<ComPtr<ID2D1ImageBrush>(ID2D1Image* image)> MockCreateImageBrush;
        std::function<ComPtr<ID2D1BitmapBrush1>(ID2D1Bitmap1* bitmap)> MockCreateBitmapBrush;
        std::function<ComPtr<ID2D1Bitmap1>()> MockCreateBitmap;
        
        //
        // ICanvasDevice
        //

        IFACEMETHODIMP get_HardwareAcceleration(CanvasHardwareAcceleration *) override
        {
            Assert::Fail(L"Unexpected call to get_HardwareAcceleration");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_Direct3DDevice(IDirect3DDevice **) override
        {
            Assert::Fail(L"Unexpected call to get_Direct3DDevice");
            return E_NOTIMPL;
        }

        //
        // ICanvasResourceCreator
        //

        IFACEMETHODIMP get_Device(ICanvasDevice** value)
        {
            if (!Mockget_Device)
            {
                Assert::Fail(L"Unexpected call to get_Device");
                return E_NOTIMPL;
            }

            Mockget_Device(value);
            return S_OK;
        }

        //
        // ICanvasDeviceInternal
        //

        virtual ComPtr<ID2D1Device1> GetD2DDevice() override
        {
            if (!MockGetD2DDevice)
            {
                Assert::Fail(L"Unexpected call to GetDevice");
                ThrowHR(E_NOTIMPL);
            }

            return MockGetD2DDevice();
        }

        virtual ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(const D2D1_COLOR_F& color) override
        {
            if (!MockCreateSolidColorBrush)
            {
                Assert::Fail(L"Unexpected call to CreateSolidColorBrush");
                return nullptr;
            }

            return MockCreateSolidColorBrush(color);
        }

        virtual ComPtr<ID2D1Bitmap1> CreateBitmap(IWICFormatConverter* converter) override
        {
            if (!MockCreateBitmap)
            {
                Assert::Fail(L"Unexpected call to CreateBitmap");
                return nullptr;
            }
            return MockCreateBitmap();
        }

        virtual ComPtr<ID2D1BitmapBrush1> CreateBitmapBrush(ID2D1Bitmap1* Bitmap) override
        {
            if (!MockCreateBitmapBrush)
            {
                Assert::Fail(L"Unexpected call to CreateBitmapBrush");
                return nullptr;
            }

            return MockCreateBitmapBrush(Bitmap);
        }

        virtual ComPtr<ID2D1ImageBrush> CreateImageBrush(ID2D1Image* image) override
        {
            if (!MockCreateImageBrush)
            {
                Assert::Fail(L"Unexpected call to CreateImageBrush");
                return nullptr;
            }

            return MockCreateImageBrush(image);
        }

        virtual ComPtr<ID2D1Image> GetD2DImage(ICanvasImage* canvasImage) override
        {
            Assert::Fail(L"Unexpected call to GetD2DImage");
            return nullptr;
        }
    };
}


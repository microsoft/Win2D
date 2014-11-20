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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX::Direct3D11;

    template<typename U>
    inline ComPtr<ICanvasDevice> GetCanvasDevice(U* resourceCreator)
    {
        auto realResourceCreator = As<ICanvasResourceCreator>(resourceCreator);
        
        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(realResourceCreator->get_Device(&device));

        return device;
    }

    template<typename T, typename U>
    ComPtr<T> GetDXGIInterfaceFromResourceCreator(U* resourceCreator)
    {
        auto device = GetCanvasDevice(resourceCreator);
        return GetDXGIInterface<T>(device.Get());
    }

    template<typename T>
    ComPtr<T> GetDXGIInterface(IDXGIInterfaceAccess* dxgiInterfaceAccess)
    {
        ComPtr<T> dxgiInterface;
        ThrowIfFailed(dxgiInterfaceAccess->GetDXGIInterface(IID_PPV_ARGS(&dxgiInterface)));
        return dxgiInterface;
    }

    template<typename T, typename U>
    ComPtr<T> GetDXGIInterface(U* obj)
    {
        return GetDXGIInterface<T>(As<IDXGIInterfaceAccess>(obj).Get());
    }

    template<typename T, typename U>
    ComPtr<T> GetDXGIInterface(ComPtr<U> obj)
    {
        return GetDXGIInterface<T>(As<IDXGIInterfaceAccess>(obj).Get());
    }

    template<typename T>
    float GetDpi(T const& obj)
    {
        float dpiX, dpiY;

        obj->GetDpi(&dpiX, &dpiY);

        return dpiX;
    }

}}}}

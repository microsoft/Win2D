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
#include "CanvasSolidColorBrush.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using ABI::Windows::UI::Color;
    using namespace ABI::Windows::Foundation;
    using namespace ::Microsoft::WRL::Wrappers;

    ComPtr<CanvasSolidColorBrush> CanvasSolidColorBrushManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        Color color)
    {
        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(resourceCreator->get_Device(&device));

        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(device.As(&canvasDeviceInternal));

        auto d2dBrush = canvasDeviceInternal->CreateSolidColorBrush(ToD2DColor(color));

        auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
            shared_from_this(),
            d2dBrush.Get(),
            device.Get());
        CheckMakeResult(canvasSolidColorBrush);
        
        return canvasSolidColorBrush;
    };


    ComPtr<CanvasSolidColorBrush> CanvasSolidColorBrushManager::CreateWrapper(
        ICanvasDevice* device,
        ID2D1SolidColorBrush* brush)
    {
        auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
            shared_from_this(),
            brush,
            device);
        CheckMakeResult(canvasSolidColorBrush);

        return canvasSolidColorBrush;
    }

    IFACEMETHODIMP CanvasSolidColorBrushFactory::Create(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::UI::Color color,
        ICanvasSolidColorBrush** canvasSolidColorBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(canvasSolidColorBrush);

                auto newSolidColorBrush = GetManager()->Create(
                    resourceCreator,
                    color);

                ThrowIfFailed(newSolidColorBrush.CopyTo(canvasSolidColorBrush));
            });
    }


    CanvasSolidColorBrush::CanvasSolidColorBrush(
        std::shared_ptr<CanvasSolidColorBrushManager> manager,
        ID2D1SolidColorBrush* brush,
        ICanvasDevice *device)
        : CanvasBrush(device)
        , ResourceWrapper(manager, brush)
    {
    }

    IFACEMETHODIMP CanvasSolidColorBrush::get_Color(_Out_ Color *value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);
                *value = ToWindowsColor(GetResource()->GetColor());
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::put_Color(_In_ Color value)
    {
        return ExceptionBoundary(
            [&]
            {                
                GetResource()->SetColor(ToD2DColor(value));
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::Close()
    {
        CanvasBrush::Close();
        return ResourceWrapper::Close();
    }

    ComPtr<ID2D1Brush> CanvasSolidColorBrush::GetD2DBrush(ID2D1DeviceContext*, bool)
    {
        return GetResource();
    }

    ActivatableClassWithFactory(CanvasSolidColorBrush, CanvasSolidColorBrushFactory);

} } } }

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
#include "CanvasBrush.h"
#include "CanvasDevice.h"

namespace canvas
{
    using ABI::Windows::UI::Color;

    ComPtr<CanvasSolidColorBrush> CanvasSolidColorBrushManager::CreateNew(
        ICanvasDevice* canvasDevice,
        Color color)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        auto& d2dBrush = canvasDeviceInternal->CreateSolidColorBrush(ToD2DColor(color));

        auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
            shared_from_this(),
            d2dBrush.Get());
        CheckMakeResult(canvasSolidColorBrush);
        
        return canvasSolidColorBrush;
    };


    ComPtr<CanvasSolidColorBrush> CanvasSolidColorBrushManager::CreateWrapper(
        ID2D1SolidColorBrush* brush)
    {
        auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
            shared_from_this(),
            brush);
        CheckMakeResult(canvasSolidColorBrush);

        return canvasSolidColorBrush;
    }

    IFACEMETHODIMP CanvasSolidColorBrushFactory::Create(
        ICanvasDevice* canvasDevice,
        ABI::Windows::UI::Color color,
        ICanvasSolidColorBrush** canvasSolidColorBrush)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(canvasDevice);
                CheckAndClearOutPointer(canvasSolidColorBrush);

                auto newSolidColorBrush = GetManager()->Create(
                    canvasDevice, 
                    color);

                ThrowIfFailed(newSolidColorBrush.CopyTo(canvasSolidColorBrush));
            });
    }


    IFACEMETHODIMP CanvasSolidColorBrushFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<ID2D1SolidColorBrush> d2dBrush;
                ThrowIfFailed(resource->QueryInterface(d2dBrush.GetAddressOf()));

                auto newBrush = GetManager()->GetOrCreate(d2dBrush.Get());

                ThrowIfFailed(newBrush.CopyTo(wrapper));
            });
    }


    CanvasSolidColorBrush::CanvasSolidColorBrush(
        std::shared_ptr<CanvasSolidColorBrushManager> manager,
        ID2D1SolidColorBrush* brush)
        : ResourceWrapper(manager, brush)
    {
    }

    IFACEMETHODIMP CanvasSolidColorBrush::get_Color(_Out_ Color *value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = ToWindowsColor(GetResource()->GetColor());
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::put_Color(_In_ Color value)
    {
        return ExceptionBoundary(
            [&]()
            {                
                GetResource()->SetColor(ToD2DColor(value));
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::get_Opacity(_Out_ float *value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = GetResource()->GetOpacity();
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::put_Opacity(_In_ float value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetResource()->SetOpacity(value);
            });
    }


    IFACEMETHODIMP CanvasSolidColorBrush::get_Transform(_Out_ Numerics::Matrix3x2 *value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);

                GetResource()->GetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F*>(value));
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::put_Transform(_In_ Numerics::Matrix3x2 value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetResource()->SetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F*>(&value));
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::Close()
    {
        return ResourceWrapper::Close();
    }

    ComPtr<ID2D1Brush> CanvasSolidColorBrush::GetD2DBrush()
    {
        return GetResource();
    }

    ComPtr<ID2D1SolidColorBrush> CanvasSolidColorBrush::GetD2DSolidColorBrush()
    {
        return GetResource();
    }

    ActivatableClassWithFactory(CanvasSolidColorBrush, CanvasSolidColorBrushFactory);
}

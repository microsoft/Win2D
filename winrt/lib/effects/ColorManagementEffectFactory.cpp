// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "ColorManagementEffectFactory.h"
#include "generated/ColorManagementEffect.h"

    
IFACEMETHODIMP ColorManagementEffectFactory::ActivateInstance(IInspectable** instance)
{
    return ExceptionBoundary([&]
    {
        auto effect = Make<ColorManagementEffect>();
        CheckMakeResult(effect);
    
        ThrowIfFailed(effect.CopyTo(instance));
    });
}


IFACEMETHODIMP ColorManagementEffectFactory::IsBestQualitySupported(ICanvasDevice* device, boolean* result)
{
    return ExceptionBoundary([&]
    {
        CheckInPointer(device);
        CheckInPointer(result);

        // Best quality mode requires floating point buffer precision.
        boolean isFloatPrecisionSupported;
        ThrowIfFailed(device->IsBufferPrecisionSupported(CanvasBufferPrecision::Precision32Float, &isFloatPrecisionSupported));

        // It also requires at least D3D feature level 10.
        ComPtr<ID3D11Device> d3dDevice;
        ThrowIfFailed(As<IDirect3DDxgiInterfaceAccess>(device)->GetInterface(IID_PPV_ARGS(&d3dDevice)));
        auto isFeatureLevel10 = d3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_0;

        *result = isFloatPrecisionSupported && isFeatureLevel10;
    });
}


ActivatableClassWithFactory(ColorManagementEffect, ColorManagementEffectFactory);

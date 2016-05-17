// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/effects/generated/ColorManagementEffect.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ColorManagementEffectUnitTests)
{
    TEST_METHOD_EX(ColorManagementEffect_IsBestQualitySupported)
    {
        auto factory = Make<ColorManagementEffectFactory>();
        auto d2dDevice = Make<StubD2DDevice>();
        auto d3dDevice = Make<MockD3D11Device>();
        auto canvasDevice = Make<StubCanvasDevice>(d2dDevice, d3dDevice);

        bool isFloatPrecisionSupported;
        D3D_FEATURE_LEVEL featureLevel;

        canvasDevice->IsBufferPrecisionSupportedMethod.AllowAnyCall([&](CanvasBufferPrecision bufferPrecision, boolean* value)
        {
            Assert::AreEqual(CanvasBufferPrecision::Precision32Float, bufferPrecision);
            *value = isFloatPrecisionSupported;
            return S_OK;
        });

        d3dDevice->GetFeatureLevelMethod.AllowAnyCall([&]
        {
            return featureLevel;
        });

        // With float precision and feature level 10 = ok.
        isFloatPrecisionSupported = true;
        featureLevel = D3D_FEATURE_LEVEL_10_0;

        boolean isSupported;
        ThrowIfFailed(factory->IsBestQualitySupported(canvasDevice.Get(), &isSupported));
        Assert::IsTrue(!!isSupported);

        // Without float precision = no.
        isFloatPrecisionSupported = false;
        featureLevel = D3D_FEATURE_LEVEL_10_0;

        ThrowIfFailed(factory->IsBestQualitySupported(canvasDevice.Get(), &isSupported));
        Assert::IsFalse(!!isSupported);

        // Feature level 9x = no.
        isFloatPrecisionSupported = true;
        featureLevel = D3D_FEATURE_LEVEL_9_3;

        ThrowIfFailed(factory->IsBestQualitySupported(canvasDevice.Get(), &isSupported));
        Assert::IsFalse(!!isSupported);

        // Null args.
        Assert::AreEqual(E_INVALIDARG, factory->IsBestQualitySupported(canvasDevice.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, factory->IsBestQualitySupported(nullptr, &isSupported));
    }
};

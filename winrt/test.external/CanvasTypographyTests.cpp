// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Text;

TEST_CLASS(CanvasTextLayoutTests)
{
public:
    TEST_METHOD(CanvasTypography_Construction)
    {
        ref new CanvasTypography();
    }

    TEST_METHOD(CanvasTypography_Interop)
    {
        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));

        ComPtr<IDWriteTypography> dwriteTypography;
        ThrowIfFailed(factory->CreateTypography(&dwriteTypography));

        auto canvasTypography = GetOrCreate<CanvasTypography>(dwriteTypography.Get());

        auto wrappedTypography = GetWrappedResource<IDWriteTypography>(canvasTypography);
        Assert::AreEqual<void*>(dwriteTypography.Get(), wrappedTypography.Get());
    }

    TEST_METHOD(CanvasTypography_GetFeatures_WhenTypographyHasNoFeatures)
    {
        auto typography = ref new CanvasTypography();

        auto features = typography->GetFeatures();

        Assert::IsNotNull(features);
        Assert::AreEqual(0u, features->Length);
    }

    TEST_METHOD(CanvasTypography_Feature_UintMaxParameter)
    {
        auto typography = ref new CanvasTypography();

        typography->AddFeature(CanvasTypographyFeatureName::Default, UINT_MAX);

        auto features = typography->GetFeatures();
        Assert::AreEqual(UINT_MAX, features[0].Parameter);
    }
};

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "StubDWriteFontCollection.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Text;

TEST_CLASS(CanvasTextRenderingParametersTests)
{

public:

    TEST_METHOD(CanvasTextRenderingParameters_Construction)
    {
        auto format = ref new CanvasTextFormat();

        auto renderingParameters = ref new CanvasTextRenderingParameters(CanvasTextRenderingMode::Default, CanvasTextGridFit::Default);
    }

    TEST_METHOD(CanvasTextRenderingParameters_Interop)
    {
        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));

        ComPtr<IDWriteRenderingParams2> dwriteRenderingParams;
        ThrowIfFailed(factory->CreateCustomRenderingParams(
            2.2f,
            1.0,
            1.0,
            1.0f,
            DWRITE_PIXEL_GEOMETRY_BGR,
            DWRITE_RENDERING_MODE_ALIASED,
            DWRITE_GRID_FIT_MODE_DISABLED,
            &dwriteRenderingParams));

        auto canvasTextRenderingParameters = GetOrCreate<CanvasTextRenderingParameters>(dwriteRenderingParams.Get());
        Assert::AreEqual(CanvasTextRenderingMode::Aliased, canvasTextRenderingParameters->RenderingMode);
        Assert::AreEqual(CanvasTextGridFit::Disable, canvasTextRenderingParameters->GridFit);

        auto wrappedTextRenderingParameters = GetWrappedResource<IDWriteRenderingParams>(canvasTextRenderingParameters);

        Assert::AreEqual<void*>(dwriteRenderingParams.Get(), wrappedTextRenderingParameters.Get());
    }
};

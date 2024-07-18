// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasTextRenderingParameters.h"
#include "TextUtilities.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

IFACEMETHODIMP CanvasTextRenderingParametersFactory::Create(
    CanvasTextRenderingMode textRenderingMode,
    CanvasTextGridFit gridFit,
    ICanvasTextRenderingParameters** textRenderingParameters)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(textRenderingParameters);

            auto newTextRenderingParameters = CanvasTextRenderingParameters::CreateNew(
                textRenderingMode,
                gridFit);

            ThrowIfFailed(newTextRenderingParameters.CopyTo(textRenderingParameters));
        });
}

CanvasTextRenderingParameters::CanvasTextRenderingParameters(
    DWriteTextParamsType* renderingParams)
    : ResourceWrapper(renderingParams)
{
}

IFACEMETHODIMP CanvasTextRenderingParameters::get_RenderingMode(
    CanvasTextRenderingMode* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& dwriteRenderingParams = GetResource();
            CheckInPointer(value);

            auto renderingMode = dwriteRenderingParams->GetRenderingMode1();

            *value = ToCanvasTextRenderingMode(renderingMode);
        });
}

IFACEMETHODIMP CanvasTextRenderingParameters::get_GridFit(
    CanvasTextGridFit* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& dwriteRenderingParameters = GetResource();
            CheckInPointer(value);

            *value = ToCanvasTextGridFit(dwriteRenderingParameters->GetGridFitMode());
        });
}

IFACEMETHODIMP CanvasTextRenderingParameters::Close()
{
    return ResourceWrapper::Close();
}

ComPtr<CanvasTextRenderingParameters> CanvasTextRenderingParameters::CreateNew(
    CanvasTextRenderingMode textRenderingMode,
    CanvasTextGridFit textGridFit)
{
    auto customFontManager = CustomFontManager::GetInstance();
    auto dwriteFactoryBase = customFontManager->GetSharedFactory();

    ComPtr<IDWriteRenderingParams> defaultRenderingParamsBase;
    ThrowIfFailed(dwriteFactoryBase->CreateRenderingParams(&defaultRenderingParamsBase));

    auto defaultRenderingParams = As<DWriteTextParamsType>(defaultRenderingParamsBase);

    auto dwriteFactory = As<IDWriteFactory3>(dwriteFactoryBase);

    ComPtr<DWriteTextParamsType> newRenderingParams;

    ThrowIfFailed(dwriteFactory->CreateCustomRenderingParams(
        defaultRenderingParams->GetGamma(),
        defaultRenderingParams->GetEnhancedContrast(),
        defaultRenderingParams->GetGrayscaleEnhancedContrast(),
        defaultRenderingParams->GetClearTypeLevel(),
        defaultRenderingParams->GetPixelGeometry(),
        ToDWriteRenderingMode(textRenderingMode),
        ToDWriteGridFitMode(textGridFit),
        &newRenderingParams));

    auto canvasTextRenderingParameters = Make<CanvasTextRenderingParameters>(newRenderingParams.Get());
    CheckMakeResult(canvasTextRenderingParameters);

    return canvasTextRenderingParameters;
}

ActivatableClassWithFactory(CanvasTextRenderingParameters, CanvasTextRenderingParametersFactory);
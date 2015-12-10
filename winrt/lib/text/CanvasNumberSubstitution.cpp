// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasNumberSubstitution.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

IFACEMETHODIMP CanvasNumberSubstitutionFactory::Create(
    CanvasNumberSubstitutionMethod method,
    ICanvasNumberSubstitution** numberSubstitution)
{
    return CreateWithLocaleAndIgnoreOverrides(method, nullptr, false, numberSubstitution);
}

IFACEMETHODIMP CanvasNumberSubstitutionFactory::CreateWithLocaleAndIgnoreOverrides(
    CanvasNumberSubstitutionMethod method,
    HSTRING localeName,
    boolean ignoreOverrides,
    ICanvasNumberSubstitution** numberSubstitution)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(numberSubstitution);

            auto customFontManager = CustomFontManager::GetInstance();
            auto dwriteFactory = customFontManager->GetSharedFactory();

            ComPtr<IDWriteNumberSubstitution> dwriteNumberSubstitution;

            ThrowIfFailed(dwriteFactory->CreateNumberSubstitution(
                ToDWriteNumberSubstitution(method),
                WindowsGetStringRawBuffer(localeName, nullptr),
                ignoreOverrides,
                &dwriteNumberSubstitution));

            auto newNumberSubstitution = Make<CanvasNumberSubstitution>(dwriteNumberSubstitution.Get());
            CheckMakeResult(newNumberSubstitution);

            ThrowIfFailed(newNumberSubstitution.CopyTo(numberSubstitution));
        });
}

CanvasNumberSubstitution::CanvasNumberSubstitution(IDWriteNumberSubstitution* numberSubstitution)
    : ResourceWrapper(numberSubstitution)
{
}

ActivatableClassWithFactory(CanvasNumberSubstitution, CanvasNumberSubstitutionFactory);
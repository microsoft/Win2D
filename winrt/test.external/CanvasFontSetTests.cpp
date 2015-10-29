// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Text;

TEST_CLASS(CanvasFontSetTests)
{
    TEST_METHOD(CanvasFontSet_GetSystemFontSet)
    {
		CanvasFontSet::GetSystemFontSet();
    }

    TEST_METHOD(CanvasFontSet_GetSystemFontSet_TwoReferences)
    {
        auto ref = CanvasFontSet::GetSystemFontSet();

		//
		// Calling GetSystemFontSet when there's an existing object
		// wrapping the system font set shouldn't fail.
		//
		auto ref2 = CanvasFontSet::GetSystemFontSet();
    }

    TEST_METHOD(CanvasFontSet_Interop)
    {
        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));

#if WINVER > _WIN32_WINNT_WINBLUE
        typedef IDWriteFontSet DWriteFontSetResourceType;
        ComPtr<DWriteFontSetResourceType> fontSetResource;
        ThrowIfFailed(As<IDWriteFactory3>(factory)->GetSystemFontSet(&fontSetResource));
#else
        typedef IDWriteFontCollection DWriteFontSetResourceType;
        ComPtr<DWriteFontSetResourceType> fontSetResource;
        ThrowIfFailed(factory->GetSystemFontCollection(&fontSetResource));
#endif

        auto canvasFontSet = GetOrCreate<CanvasFontSet>(fontSetResource.Get());

        auto wrappedFontCollection = GetWrappedResource<DWriteFontSetResourceType>(canvasFontSet);
        Assert::AreEqual<void*>(fontSetResource.Get(), wrappedFontCollection.Get());
    }
};

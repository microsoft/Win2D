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

        typedef IDWriteFontSet DWriteFontSetResourceType;
        ComPtr<DWriteFontSetResourceType> fontSetResource;
        ThrowIfFailed(As<IDWriteFactory3>(factory)->GetSystemFontSet(&fontSetResource));

        auto canvasFontSet = GetOrCreate<CanvasFontSet>(fontSetResource.Get());

        auto wrappedFontCollection = GetWrappedResource<DWriteFontSetResourceType>(canvasFontSet);
        Assert::AreEqual<void*>(fontSetResource.Get(), wrappedFontCollection.Get());
    }

    TEST_METHOD(CanvasFontSet_CreateFontSetFromUri)
    {
        auto fontSet = ref new CanvasFontSet(ref new Uri("ms-appx:///Symbols.ttf"));
        Assert::IsNotNull(fontSet);

        //
        // We could normally use the IsSymbol property on the font set to sanity check this. 
        // Interestingly enough this font is not actually flagged as a symbol font. So, we 
        // just use the family name.
        //
        auto face = fontSet->Fonts->GetAt(0);

        auto familyNames = face->GetInformationalStrings(CanvasFontInformation::Win32FamilyNames);
        Assert::AreEqual("Symbols", familyNames->Lookup("en-us"));
    }

    TEST_METHOD(CanvasFontSet_CreateFontSetFromUri_InvalidArgs)
    {
        ExpectCOMException(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND),
            [&]
            {
                ref new CanvasFontSet(ref new Uri("ms-appx:///not_a_font"));
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                ref new CanvasFontSet(ref new Uri("http://not_a_valid_application_uri.msdn.com"));
            });
    }
};

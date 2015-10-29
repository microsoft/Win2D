// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

namespace canvas
{
    class MockDWriteFontSet
        : public RuntimeClass<
            RuntimeClassFlags<ClassicCom>,
           IDWriteFontSet>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetFontCountMethod, UINT32());
        CALL_COUNTER_WITH_MOCK(GetFontFaceReferenceMethod, HRESULT(UINT32, IDWriteFontFaceReference**));
        CALL_COUNTER_WITH_MOCK(FindFontFaceReferenceMethod, HRESULT(IDWriteFontFaceReference*, UINT32*, BOOL*));
        CALL_COUNTER_WITH_MOCK(FindFontFaceMethod, HRESULT(IDWriteFontFace*, UINT32*, BOOL*));
        CALL_COUNTER_WITH_MOCK(GetPropertyValuesMethod0, HRESULT(UINT32, DWRITE_FONT_PROPERTY_ID, BOOL*, IDWriteLocalizedStrings**));
        CALL_COUNTER_WITH_MOCK(GetPropertyValuesMethod1, HRESULT(DWRITE_FONT_PROPERTY_ID, WCHAR const*, IDWriteStringList**));
        CALL_COUNTER_WITH_MOCK(GetPropertyValuesMethod2, HRESULT(DWRITE_FONT_PROPERTY_ID, IDWriteStringList**));
        CALL_COUNTER_WITH_MOCK(GetPropertyOccurrenceCountMethod, HRESULT(DWRITE_FONT_PROPERTY const*, UINT32*));
        CALL_COUNTER_WITH_MOCK(GetMatchingFontsMethod0, HRESULT(DWRITE_FONT_PROPERTY const*, UINT32, IDWriteFontSet**));
        CALL_COUNTER_WITH_MOCK(GetMatchingFontsMethod1, HRESULT(WCHAR const*, DWRITE_FONT_WEIGHT, DWRITE_FONT_STRETCH, DWRITE_FONT_STYLE, IDWriteFontSet**));

        IFACEMETHODIMP_(UINT32) GetFontCount()
        {
            return GetFontCountMethod.WasCalled();
        }

        IFACEMETHODIMP GetFontFaceReference(
            UINT32 listIndex,
            IDWriteFontFaceReference** fontFaceReference)
        {
            return GetFontFaceReferenceMethod.WasCalled(listIndex, fontFaceReference);
        }

        IFACEMETHODIMP FindFontFaceReference(
            IDWriteFontFaceReference* fontFaceReference,
            UINT32* listIndex,
            BOOL* exists)
        {
            return FindFontFaceReferenceMethod.WasCalled(fontFaceReference, listIndex, exists);
        }

        IFACEMETHODIMP FindFontFace(
            IDWriteFontFace* fontFace,
            UINT32* listIndex,
            BOOL* exists)
        {
            return FindFontFaceMethod.WasCalled(fontFace, listIndex, exists);
        }

        IFACEMETHODIMP GetPropertyValues(
            UINT32 listIndex,
            DWRITE_FONT_PROPERTY_ID propertyId,
            BOOL* exists,
            IDWriteLocalizedStrings** values)
        {
            return GetPropertyValuesMethod0.WasCalled(listIndex, propertyId, exists, values);
        }

        IFACEMETHODIMP GetPropertyValues(
            DWRITE_FONT_PROPERTY_ID propertyID,
            WCHAR const* preferredLocaleNames,
            IDWriteStringList** values)
        {
            return GetPropertyValuesMethod1.WasCalled(propertyID, preferredLocaleNames, values);
        }

        IFACEMETHODIMP GetPropertyValues(
            DWRITE_FONT_PROPERTY_ID propertyID,
            IDWriteStringList** values)
        {
            return GetPropertyValuesMethod2.WasCalled(propertyID, values);
        }

        IFACEMETHODIMP GetPropertyOccurrenceCount(
            DWRITE_FONT_PROPERTY const* property,
            UINT32* propertyOccurrenceCount)
        {
            return GetPropertyOccurrenceCountMethod.WasCalled(property, propertyOccurrenceCount);
        }

        IFACEMETHODIMP GetMatchingFonts(
            DWRITE_FONT_PROPERTY const* properties,
            UINT32 propertyCount,
            IDWriteFontSet** filteredSet)
        {
            return GetMatchingFontsMethod0.WasCalled(properties, propertyCount, filteredSet);
        }

        IFACEMETHODIMP GetMatchingFonts(
            WCHAR const* familyName,
            DWRITE_FONT_WEIGHT fontWeight,
            DWRITE_FONT_STRETCH fontStretch,
            DWRITE_FONT_STYLE fontStyle,
            IDWriteFontSet** filteredSet)
        {
            return GetMatchingFontsMethod1.WasCalled(familyName, fontWeight, fontStretch, fontStyle, filteredSet);
        }
    };
}

#endif
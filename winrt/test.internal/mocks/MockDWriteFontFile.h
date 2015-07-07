// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteFontFile
        : public RuntimeClass<
            RuntimeClassFlags<ClassicCom>,
            IDWriteFontFile>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetReferenceKeyMethod, HRESULT(void const**, UINT32*));
        CALL_COUNTER_WITH_MOCK(GetLoaderMethod, HRESULT(IDWriteFontFileLoader**));
        CALL_COUNTER_WITH_MOCK(AnalyzeMethod, HRESULT(BOOL*, DWRITE_FONT_FILE_TYPE*, DWRITE_FONT_FACE_TYPE*, UINT32*));

        IFACEMETHODIMP GetReferenceKey(
            void const** fontFileReferenceKey,
            UINT32* fontFileReferenceKeySize) override
        {
            return GetReferenceKeyMethod.WasCalled(fontFileReferenceKey, fontFileReferenceKeySize);
        }

        IFACEMETHODIMP GetLoader(
            IDWriteFontFileLoader** fontFileLoader) override
        {
            return GetLoaderMethod.WasCalled(fontFileLoader);
        }

        IFACEMETHODIMP Analyze(
            BOOL* isSupportedFontType,
            DWRITE_FONT_FILE_TYPE* fontFileType,
            DWRITE_FONT_FACE_TYPE* fontFaceType,
            UINT32* numberOfFaces) override
        {
            return AnalyzeMethod.WasCalled(isSupportedFontType, fontFileType, fontFaceType, numberOfFaces);
        }
    };
}

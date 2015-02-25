// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

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

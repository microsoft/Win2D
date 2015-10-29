// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

#include "stubs/StubDWriteFontSet.h"

namespace canvas
{
    class StubDWriteFontSetBuilder : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IDWriteFontSetBuilder>
    {
        std::vector<ComPtr<IDWriteFontFaceReference>> m_fontFaceReferences;
    public:         
        IFACEMETHODIMP AddFontFaceReference(IDWriteFontFaceReference* fontFaceReference)
        {
            m_fontFaceReferences.push_back(fontFaceReference);

            return S_OK;
        }

        IFACEMETHODIMP AddFontFaceReference(
            IDWriteFontFaceReference*,
            DWRITE_FONT_PROPERTY const*,
            UINT32 propertyCount
            )
        {
            Assert::Fail(L"Unexpected call to AddFontFaceReference");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP AddFontSet(
            IDWriteFontSet*
            )
        {
            Assert::Fail(L"Unexpected call to AddFontSet");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateFontSet(
            IDWriteFontSet** fontSet
            )
        {
            auto stubFontSet = Make<StubDWriteFontSet>(m_fontFaceReferences);

            return stubFontSet.CopyTo(fontSet);
        }
    };
}

#endif
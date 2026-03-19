// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "stubs/StubDWriteFontFaceReference.h"

namespace canvas
{
    class StubDWriteFontSet : public MockDWriteFontSet
    {
        std::vector<ComPtr<IDWriteFontFaceReference>> m_fontFaceReferences;

    public:

        StubDWriteFontSet(std::vector<ComPtr<IDWriteFontFaceReference>> fontFaceReferences)
            : m_fontFaceReferences(fontFaceReferences)
        {
        }

        StubDWriteFontSet()
        {
            //
            // We create 3 font references, as a convention, just to test with.
            //
            for (uint32_t i = 0; i < 3; ++i)
            {
                m_fontFaceReferences.push_back(Make<StubDWriteFontFaceReference>());
            }
        }

        IFACEMETHODIMP_(UINT32) GetFontCount()
        {
            return static_cast<uint32_t>(m_fontFaceReferences.size());
        }

        IFACEMETHODIMP GetFontFaceReference(
            UINT32 listIndex,
            IDWriteFontFaceReference** fontFaceReference)
        {
            Assert::IsTrue(listIndex < m_fontFaceReferences.size());
            m_fontFaceReferences[listIndex].CopyTo(fontFaceReference);
            return S_OK;
        }

        ComPtr<IDWriteFontFaceReference> GetFontFaceReferenceInternal(int index)
        {
            return m_fontFaceReferences[index];
        }
    };
}
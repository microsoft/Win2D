// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "mocks/MockDWriteFontFaceReference.h"

namespace canvas
{
    class StubDWriteFontFaceReference : public MockDWriteFontFaceReference
    {
        DWRITE_LOCALITY m_locality;

    public:

        StubDWriteFontFaceReference()
            : m_locality(DWRITE_LOCALITY_LOCAL)
        {}

        IFACEMETHODIMP_(DWRITE_LOCALITY) GetLocality()
        {
            return m_locality;
        }

        void SetLocality(DWRITE_LOCALITY locality)
        {
            m_locality = locality;
        }
    };
}
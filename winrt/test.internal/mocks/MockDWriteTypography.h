// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteTypography : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IDWriteTypography>
    {
    public: 
        MOCK_METHOD1(AddFontFeature, HRESULT(DWRITE_FONT_FEATURE));
        MOCK_METHOD0(GetFontFeatureCount, UINT32());
        MOCK_METHOD2(GetFontFeature, HRESULT(UINT32, DWRITE_FONT_FEATURE*));
    };
}

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

class MockD2DTransformedImageSource
    : public RuntimeClass<RuntimeClassFlags<ClassicCom>
    , ChainInterfaces<ID2D1TransformedImageSource, ID2D1Image, ID2D1Resource>>
{
public:
    // ID2D1Resource
    MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    // ID2D1TransformedImageSource
    MOCK_METHOD1_CONST(GetProperties, void(D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES*));
    MOCK_METHOD1_CONST(GetSource,     void(ID2D1ImageSource**));
};

#endif
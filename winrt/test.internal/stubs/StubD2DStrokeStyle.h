// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This device derives from MockD2DStrokeStyle, but it allows for retrieval of the factory.

#pragma once

class StubD2DStrokeStyleWithGetFactory : public MockD2DStrokeStyle
{
    ComPtr<StubD2DFactoryWithCreateStrokeStyle> m_factory;

public:
    StubD2DStrokeStyleWithGetFactory(ComPtr<StubD2DFactoryWithCreateStrokeStyle> factory)
        : m_factory(factory)
    {}

    IFACEMETHODIMP_(void) GetFactory(ID2D1Factory** factory) const override
    {
        ThrowIfFailed(m_factory.CopyTo(factory));
    }
};

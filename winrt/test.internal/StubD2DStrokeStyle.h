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

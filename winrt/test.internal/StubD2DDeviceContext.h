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
    class StubD2DDeviceContext : public MockD2DDeviceContext
    {
        ComPtr<ID2D1Device> m_owner;
        ComPtr<ID2D1Image> m_target;

    public:
        StubD2DDeviceContext(ID2D1Device* owner)
            : m_owner(owner)
        {
        }

        IFACEMETHODIMP_(void) GetDevice(ID2D1Device** device) const override
        {
            m_owner.CopyTo(device);
        }

        IFACEMETHODIMP_(void) BeginDraw() override
        {
        }

        IFACEMETHODIMP EndDraw(D2D1_TAG *,D2D1_TAG *) override
        {
            return S_OK;
        }

        IFACEMETHODIMP_(void) SetTarget(ID2D1Image* value) override
        {
            m_target = value;
        }

        IFACEMETHODIMP_(void) GetTarget(ID2D1Image** value) const override
        {
            m_target.CopyTo(value);
        }
    };
}

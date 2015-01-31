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
    class StubD2DBrush : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ID2D1Brush>
    {
    public:
        IFACEMETHODIMP_(void) GetFactory(ID2D1Factory**) const
        {
            Assert::Fail(L"Unexpected call to GetFactory");
        }

        IFACEMETHODIMP_(void) SetOpacity(float)
        {
            // nothing
        }

        IFACEMETHODIMP_(float) GetOpacity() const
        {
            return 1.0f;
        }

        IFACEMETHODIMP_(void) SetTransform(const D2D1_MATRIX_3X2_F*)
        {
            // nothing
        }

        IFACEMETHODIMP_(void) GetTransform(D2D1_MATRIX_3X2_F*) const
        {
            // nothing
        }
    };
}

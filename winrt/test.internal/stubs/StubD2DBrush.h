// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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

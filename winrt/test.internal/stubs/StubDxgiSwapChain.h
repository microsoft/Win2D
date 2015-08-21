// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "mocks/MockDxgiSwapChain.h"

namespace canvas
{
    class StubDxgiSwapChain : public MockDxgiSwapChain
    {
        DXGI_MATRIX_3X2_F m_transform;
        ComPtr<MockDxgiSurface> m_backBufferSurface;

    public:

        StubDxgiSwapChain(
            ComPtr<MockDxgiSurface> const& backBufferSurface = Make<MockDxgiSurface>())
            : m_backBufferSurface(backBufferSurface)
        {
            ResizeBuffersMethod.AllowAnyCall();

            SetMatrixTransformMethod.AllowAnyCall(
                [&](const DXGI_MATRIX_3X2_F* matrix)
                {
                    m_transform = *matrix;
                    return S_OK;
                });
        }

        IFACEMETHODIMP GetMatrixTransform(
            DXGI_MATRIX_3X2_F* matrix) override
        {
            *matrix = m_transform;
            return S_OK;
        }

        IFACEMETHODIMP GetBuffer(
            UINT buffer,
            REFIID riid,
            void** out) override
        {
            Assert::AreEqual(0u, buffer);
            Assert::AreEqual(__uuidof(IDXGISurface2), riid);
            ThrowIfFailed(m_backBufferSurface.CopyTo(reinterpret_cast<IDXGISurface2**>(out)));

            return S_OK;
        }
    };
}
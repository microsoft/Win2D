// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

class StubInkRenderer : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IInkD2DRenderer>
{
public:
    CALL_COUNTER_WITH_MOCK(DrawMethod, HRESULT(IUnknown*, IUnknown*, BOOL));

    IFACEMETHODIMP Draw(
        IUnknown* deviceContext,
        IUnknown* strokeCollection,
        BOOL highContrast)
    {
        return DrawMethod.WasCalled(deviceContext, strokeCollection, highContrast);
    }
};

class StubInkAdapter : public InkAdapter
{
    bool m_highContrast;
    ComPtr<StubInkRenderer> m_inkRenderer;
public:

    StubInkAdapter() : m_highContrast(false), m_inkRenderer(Make<StubInkRenderer>()) {}

    ComPtr<IInkD2DRenderer> CreateInkRenderer() override
    {
        return m_inkRenderer;
    }

    virtual bool IsHighContrastEnabled() override
    {
        return m_highContrast;
    }

    void SetHighContrastEnabled(bool highContrast)
    {
        m_highContrast = highContrast;
    }

    ComPtr<StubInkRenderer> GetInkRenderer()
    {
        return m_inkRenderer;
    }
};

class MockStrokeCollection : public RuntimeClass<IIterable<InkStroke*>>
{
    IFACEMETHODIMP First(IIterator<InkStroke*>**)
    {
        return E_NOTIMPL;
    }
};


// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#define IMPLEMENT_MOCK_PROPERTY(PROPERTY_NAME, TYPE)                                \
        IFACEMETHODIMP TestEffect::get_##PROPERTY_NAME(_Out_ TYPE* value)           \
        {                                                                           \
            Assert::Fail(L"Unexpected call to getter in TestEffect");               \
            return E_NOTIMPL;                                                       \
        }                                                                           \
                                                                                    \
        IFACEMETHODIMP TestEffect::put_##PROPERTY_NAME(_In_ TYPE value)             \
        {                                                                           \
            Assert::Fail(L"Unexpected call to setter in TestEffect");               \
            return E_NOTIMPL;                                                       \
        }

class TestEffect : public RuntimeClass <
    IGaussianBlurEffect,
    MixIn<TestEffect, CanvasEffect> >,
    public CanvasEffect
{
    float m_deviation;
    InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Effects_GaussianBlurEffect, BaseTrust);

public:
    TestEffect(GUID effectId = CLSID_D2D1GaussianBlur, int propertiesSize = 1, int sourcesSize = 1, bool isSourcesSizeFixed = true)
        : CanvasEffect(effectId, propertiesSize, sourcesSize, isSourcesSizeFixed, nullptr, nullptr, nullptr)
    {
    }

    IMPLEMENT_EFFECT_PROPERTY(TestEffect,
        BlurAmount,
        float,
        float,
        0)

    IMPLEMENT_MOCK_PROPERTY(Optimization, EffectOptimization)
    IMPLEMENT_MOCK_PROPERTY(BorderMode, EffectBorderMode)

    IMPLEMENT_EFFECT_SOURCE_PROPERTY(TestEffect, Source, 0)

    std::function<void()> MockGetSource;
    std::function<void()> MockSetSource;

    std::function<void()> MockGetProperty;
    std::function<void()> MockSetProperty;

    //
    // Forwarding base protected methods in order to inspect calls
    //

    STDMETHOD(GetSource)(unsigned int index, IGraphicsEffectSource** source)
    {
        if (MockGetSource)
            MockGetSource();
        return CanvasEffect::GetSource(index, source);
    }

    STDMETHOD(SetSource)(unsigned int index, IGraphicsEffectSource* source)
    {
        if (MockSetSource)
            MockSetSource();
        return CanvasEffect::SetSource(index, source);
    }

    template<typename TBoxed, typename TPublic>
    void GetBoxedProperty(unsigned int index, TPublic* value)
    {
        if (MockGetProperty)
            MockGetProperty();
        CanvasEffect::GetBoxedProperty<TBoxed>(index, value);
    }

    template<typename TBoxed, typename TPublic>
    void SetBoxedProperty(unsigned int index, TPublic const& value)
    {
        if (MockSetProperty)
            MockSetProperty();
        CanvasEffect::SetBoxedProperty<TBoxed>(index, value);
    }
};

inline void CheckEffectTypeAndInput(MockD2DEffectThatCountsCalls* mockEffect, IID const& expectedId, ID2D1Image* expectedInput, float expectedDpi = 0)
{
    Assert::IsTrue(!!IsEqualGUID(expectedId, mockEffect->m_effectId));

    Assert::IsTrue(IsSameInstance(expectedInput, mockEffect->m_inputs[0].Get()));

    // If this is a DPI compensation effect, also validate that its DPI got set to what we expect.
    if (expectedDpi != 0)
    {
        Assert::IsTrue(mockEffect->m_properties.size() > D2D1_DPICOMPENSATION_PROP_INPUT_DPI);
        Assert::AreEqual(sizeof(D2D1_VECTOR_2F), mockEffect->m_properties[D2D1_DPICOMPENSATION_PROP_INPUT_DPI].size());

        D2D1_VECTOR_2F dpi = *reinterpret_cast<D2D1_VECTOR_2F*>(&mockEffect->m_properties[D2D1_DPICOMPENSATION_PROP_INPUT_DPI].front());

        Assert::AreEqual(expectedDpi, dpi.x);
        Assert::AreEqual(expectedDpi, dpi.y);
    }
}

inline void CheckEffectTypeAndInput(
    MockD2DEffectThatCountsCalls* mockEffect,
    IID const& expectedId,
    ICanvasImage* expectedInput,
    ID2D1DeviceContext* deviceContext,
    float expectedDpi = 0)
{
    CheckEffectTypeAndInput(mockEffect, expectedId, As<ICanvasImageInternal>(expectedInput)->GetD2DImage(deviceContext).Get(), expectedDpi);
}

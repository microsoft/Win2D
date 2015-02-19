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
    TestEffect(GUID effectId, int propertiesSize, int inputsSize, bool isInputSizeFixed)
        : CanvasEffect(effectId, propertiesSize, inputsSize, isInputSizeFixed)
    {
    }

    TestEffect()
        : CanvasEffect(CLSID_D2D1GaussianBlur, 1, 1, true)
    {
    }

    IMPLEMENT_EFFECT_PROPERTY(TestEffect,
        BlurAmount,
        float,
        float,
        0)

    IMPLEMENT_MOCK_PROPERTY(Optimization, EffectOptimization)
    IMPLEMENT_MOCK_PROPERTY(BorderMode, EffectBorderMode)

    IMPLEMENT_EFFECT_INPUT_PROPERTY(TestEffect, Source, 0)

    std::function<void()> MockGetInput;
    std::function<void()> MockSetInput;

    std::function<void()> MockGetProperty;
    std::function<void()> MockSetProperty;

    //
    // Forwarding base protected methods in order to inspect calls
    //

    void GetInput(unsigned int index, IEffectInput** input)
    {
        if (MockGetInput)
            MockGetInput();
        CanvasEffect::GetInput(index, input);
    }

    void SetInput(unsigned int index, IEffectInput* input)
    {
        if (MockSetInput)
            MockSetInput();
        CanvasEffect::SetInput(index, input);
    }

    template<typename TBoxed, typename TPublic>
    void GetProperty(unsigned int index, TPublic* value)
    {
        if (MockGetProperty)
            MockGetProperty();
        CanvasEffect::GetProperty<TBoxed>(index, value);
    }

    template<typename TBoxed, typename TPublic>
    void SetProperty(unsigned int index, TPublic const& value)
    {
        if (MockSetProperty)
            MockSetProperty();
        CanvasEffect::SetProperty<TBoxed>(index, value);
    }
};

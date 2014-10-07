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

#include "pch.h"
#include "CanvasEffect.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    CanvasEffect::CanvasEffect(IID effectId, unsigned int propertiesSize, unsigned int inputSize, bool isInputSizeFixed)
        : m_effectId(effectId)
        , m_realizationId(0)
        , m_insideGetImage(false)
    {
        m_properties = Make<Vector<IInspectable*>>(propertiesSize, true);
        CheckMakeResult(m_properties);

        m_inputs = Make<Vector<IEffectInput*>>(inputSize, isInputSizeFixed);
        CheckMakeResult(m_inputs);
        m_inputs->SetChanged(true);

        // Create propertyValueFactory
        HSTRING stringActivableClassId = Wrappers::HStringReference(RuntimeClass_Windows_Foundation_PropertyValue).Get();
        ThrowIfFailed(GetActivationFactory(stringActivableClassId, &m_propertyValueFactory));
    }

    //
    // ICanvasImageInternal
    //

    ComPtr<ID2D1Image> CanvasEffect::GetD2DImage(ID2D1DeviceContext* deviceContext, uint64_t* realizationId)
    {
        CheckInPointer(deviceContext);

        // Check if device is the same as previous device
        ComPtr<ID2D1Device> device;
        deviceContext->GetDevice(&device);
        auto deviceIdentity = As<IUnknown>(device);

        if (deviceIdentity != m_previousDeviceIdentity)
        {
            m_previousDeviceIdentity = deviceIdentity;
            m_resource.Reset();
        }

        // Create resource if not created yet
        // TODO #802: make sure this lazy create (and the following cycle detection) is made properly threadsafe
        bool wasRecreated = false;

        if (!m_resource)
        {
            ThrowIfFailed(deviceContext->CreateEffect(m_effectId, &m_resource));
            wasRecreated = true;
            m_realizationId++;
        }

        // Check for graph cycles
        if (m_insideGetImage)
            ThrowHR(D2DERR_CYCLIC_GRAPH);

        m_insideGetImage = true;
        auto clearFlagWarden = MakeScopeWarden([&] { m_insideGetImage = false; });

        // Update ID2D1Image with the latest property values if a change is detected
        if (wasRecreated || m_properties->IsChanged())
        {
            SetProperties();
            m_properties->SetChanged(false);
        }

        // Update ID2D1Image with the latest inputs, and recurse through 
        // the effect graph to make sure child nodes are properly realized
        bool inputsChanged = wasRecreated || m_inputs->IsChanged();
            
        auto& inputs = m_inputs->InternalVector();
        auto inputsSize = (unsigned int) inputs.size();

        // Resize input array?
        if (inputsChanged)
        {
            m_resource->SetInputCount(inputsSize);
            m_previousInputRealizationIds.resize(inputsSize);
        }

        for (unsigned int i = 0; i < inputsSize; ++i)
        {
            // Look up the WinRT interface representing this input
            if (!inputs[i])
                ThrowHR(E_POINTER);

            ComPtr<ICanvasImageInternal> internalInput;
            HRESULT hr = inputs[i].As(&internalInput);

            if (hr == E_NOINTERFACE)
            {
                WinStringBuilder message;
                message.Format(Strings::EffectWrongInputType, i);
                ThrowHR(hr, message.Get());
            }
            else
            {
                ThrowIfFailed(hr);
            }

            // Get the underlying D2D interface (this call recurses through the effect graph)
            uint64_t inputRealizationId;
            auto realizedInput = internalInput->GetD2DImage(deviceContext, &inputRealizationId);

            // If the input value has changed, update the D2D effect
            if (inputsChanged || inputRealizationId != m_previousInputRealizationIds[i])
            {
                m_resource->SetInput(i, realizedInput.Get());
                m_previousInputRealizationIds[i] = inputRealizationId;
            }
        }
        
        m_inputs->SetChanged(false);

        if (realizationId)
            *realizationId = m_realizationId;

        return As<ID2D1Image>(m_resource);
    }

    //
    // IClosable
    //

    IFACEMETHODIMP CanvasEffect::Close()
    {
        m_resource.Reset();
        m_previousDeviceIdentity.Reset();
        
        auto& inputs = m_inputs->InternalVector();

        if (m_inputs->IsFixedSize())
        {
            inputs.assign(inputs.size(), nullptr);
        }
        else
        {
            inputs.clear();
        }

        return S_OK;
    }

    //
    // IEffect
    //

    IFACEMETHODIMP CanvasEffect::get_EffectId(GUID* effectId)
    {
        CheckInPointer(effectId);

        *effectId = m_effectId;
        return S_OK;
    }

    IFACEMETHODIMP CanvasEffect::get_Inputs(_Out_ IVector<IEffectInput*>** inputs)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(inputs);
                ThrowIfFailed(m_inputs.CopyTo(inputs));
            });
    }

    IFACEMETHODIMP CanvasEffect::get_Properties(_Out_ IVector<IInspectable*>** properties)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(properties);
                ThrowIfFailed(m_properties.CopyTo(properties));
            });
    }

    void CanvasEffect::GetInput(unsigned int index, IEffectInput** input)
    {
        CheckAndClearOutPointer(input);

        ThrowIfFailed(m_inputs->GetAt(index, input));
    }

    void CanvasEffect::SetInput(unsigned int index, IEffectInput* input)
    {
        if (input)
        {
            ComPtr<IEffectInput> effectInput;
            ThrowIfFailed(input->QueryInterface(effectInput.GetAddressOf()));
            ThrowIfFailed(m_inputs->SetAt(index, effectInput.Get()));
        }
        else
        {
            ThrowIfFailed(m_inputs->SetAt(index, nullptr));
        }
    }

    void CanvasEffect::SetProperties()
    {
        auto& properties = m_properties->InternalVector();
        auto propertiesSize = (unsigned int) properties.size();

        for (unsigned int i = 0; i < propertiesSize; ++i)
        {
            if (!properties[i])
                ThrowHR(E_POINTER);

            auto propertyValue = As<IPropertyValue>(properties[i]);

            PropertyType propertyType;
            ThrowIfFailed(propertyValue->get_Type(&propertyType));

            // TODO #2283: detailed exception error if failed due to type mismatch
            switch (propertyType)
            {
            case PropertyType_Boolean:
            {
                boolean value;
                ThrowIfFailed(propertyValue->GetBoolean(&value));
                ThrowIfFailed(m_resource->SetValue(i, static_cast<BOOL>(value)));
                break;
            }
            case PropertyType_Int32:
            {
                INT32 value;
                ThrowIfFailed(propertyValue->GetInt32(&value));
                ThrowIfFailed(m_resource->SetValue(i, value));
                break;
            }
            case PropertyType_UInt32:
            {
                UINT32 value;
                ThrowIfFailed(propertyValue->GetUInt32(&value));
                ThrowIfFailed(m_resource->SetValue(i, value));
                break;
            }
            case PropertyType_Single:
            {
                float value;
                ThrowIfFailed(propertyValue->GetSingle(&value));
                ThrowIfFailed(m_resource->SetValue(i, value));
                break;
            }
            case PropertyType_SingleArray:
            {
                ComArray<float> value;
                ThrowIfFailed(propertyValue->GetSingleArray(value.GetAddressOfSize(), value.GetAddressOfData()));

                // Since d2d effects have input array based types:
                // D2D1_MATRIX_3X2_F, D2D1_MATRIX_4X4_F, D2D1_MATRIX_5X4_F, 
                // D2D1_VECTOR_2F, D2D1_VECTOR_3F, D2D1_VECTOR_4F
                // we can uniquely identify input type based on size
                // size 20 => D2D1_MATRIX_5X4_F
                // size 16 => D2D1_MATRIX_4X4_F
                // size 6  => D2D1_MATRIX_3X2_F
                // size 4  => D2D1_VECTOR_4F
                // size 3  => D2D1_VECTOR_3F
                // size 2  => D2D1_VECTOR_2F
                switch (value.GetSize())
                {
                case 2:
                {
                    ThrowIfFailed(m_resource->SetValue(i, *reinterpret_cast<D2D1_VECTOR_2F*>(value.GetData())));
                    break;
                }
                case 3:
                {
                    ThrowIfFailed(m_resource->SetValue(i, *reinterpret_cast<D2D1_VECTOR_3F*>(value.GetData())));
                    break;
                }
                case 4:
                {
                    ThrowIfFailed(m_resource->SetValue(i, *reinterpret_cast<D2D1_VECTOR_4F*>(value.GetData())));
                    break;
                }
                case 6:
                {
                    ThrowIfFailed(m_resource->SetValue(i, *reinterpret_cast<D2D1_MATRIX_3X2_F*>(value.GetData())));
                    break;
                }
                case 16:
                {
                    ThrowIfFailed(m_resource->SetValue(i, *reinterpret_cast<D2D1_MATRIX_4X4_F*>(value.GetData())));
                    break;
                }
                default:
                    ThrowHR(E_NOTIMPL);
                }
                break;
            }
            default:
                ThrowHR(E_NOTIMPL);
            }
        }
    }
}}}}}

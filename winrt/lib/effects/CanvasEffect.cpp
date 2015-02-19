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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    auto MAGIC_FORCE_DPI_COMPENSATION_VALUE = -1.0f;

    CanvasEffect::CanvasEffect(IID effectId, unsigned int propertiesSize, unsigned int inputSize, bool isInputSizeFixed)
        : m_effectId(effectId)
        , m_realizationId(0)
        , m_insideGetImage(false)
        , m_closed(false)
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

    IFACEMETHODIMP CanvasEffect::GetBounds(
        ICanvasDrawingSession* drawingSession,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, drawingSession, nullptr, bounds);
    }

    IFACEMETHODIMP CanvasEffect::GetBoundsWithTransform(
        ICanvasDrawingSession* drawingSession,
        Numerics::Matrix3x2 transform,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, drawingSession, &transform, bounds);
    }

    //
    // ICanvasImageInternal
    //

    static float GetTargetDpi(ID2D1DeviceContext* deviceContext)
    {
        ComPtr<ID2D1Image> target;
        deviceContext->GetTarget(&target);

        if (MaybeAs<ID2D1CommandList>(target))
        {
            // Command lists are DPI independent, so we always need to insert
            // DPI compensation.
            return MAGIC_FORCE_DPI_COMPENSATION_VALUE;
        }
        else
        {
            return GetDpi(deviceContext);
        }        
    }

    ComPtr<ID2D1Image> CanvasEffect::GetD2DImage(ID2D1DeviceContext* deviceContext)
    {
        ThrowIfClosed();

        float targetDpi = GetTargetDpi(deviceContext);

        return GetRealizedEffectNode(deviceContext, targetDpi).Image;
    }

    ICanvasImageInternal::RealizedEffectNode CanvasEffect::GetRealizedEffectNode(ID2D1DeviceContext* deviceContext, float targetDpi)
    {
        ThrowIfClosed();

        // Check if device is the same as previous device
        ComPtr<ID2D1Device> device;
        deviceContext->GetDevice(&device);
        auto deviceIdentity = As<IUnknown>(device);

        if (deviceIdentity != m_previousDeviceIdentity)
        {
            m_previousDeviceIdentity = deviceIdentity;
            m_resource.Reset();
            m_dpiCompensators.clear();
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

        // If this is a DPI compensation effect, we no longer need to insert
        // any further compensation, so stop tracking the target DPI
        if (IsEqualGUID(m_effectId, CLSID_D2D1DpiCompensation))
        {
            targetDpi = 0;
        }

        // Check for graph cycles
        if (m_insideGetImage)
            ThrowHR(D2DERR_CYCLIC_GRAPH);

        m_insideGetImage = true;
        auto clearFlagWarden = MakeScopeWarden([&] { m_insideGetImage = false; });

        // Update ID2D1Image with the latest property values if a change is detected
        if (wasRecreated || m_properties->IsChanged())
        {
            SetD2DProperties();
        }

        // Update ID2D1Image with the latest inputs, and recurse through 
        // the effect graph to make sure child nodes are properly realized
        SetD2DInputs(deviceContext, targetDpi, wasRecreated);

        return RealizedEffectNode{ As<ID2D1Image>(m_resource), 0, m_realizationId };
    }

    //
    // IClosable
    //

    IFACEMETHODIMP CanvasEffect::Close()
    {
        m_resource.Reset();
        m_previousDeviceIdentity.Reset();
        m_dpiCompensators.clear();
        
        auto& inputs = m_inputs->InternalVector();

        if (m_inputs->IsFixedSize())
        {
            inputs.assign(inputs.size(), nullptr);
        }
        else
        {
            inputs.clear();
        }

        m_closed = true;

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
        ThrowIfFailed(m_inputs->SetAt(index, input));
    }

    static ComPtr<ID2D1Effect> InsertDpiCompensationEffect(ID2D1DeviceContext* deviceContext, ID2D1Image* inputImage, float inputDpi, ID2D1Effect* reuseExistingCompensator)
    {
        ComPtr<ID2D1Effect> dpiCompensator = reuseExistingCompensator;

        // Create the D2D1DpiCompensation effect, if we don't have one already.
        if (!dpiCompensator)
        {
            ThrowIfFailed(deviceContext->CreateEffect(CLSID_D2D1DpiCompensation, &dpiCompensator));

            ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD));
            ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_INTERPOLATION_MODE, D2D1_DPICOMPENSATION_INTERPOLATION_MODE_LINEAR));
        }

        // Set our input image as source for the DPI compensation.
        dpiCompensator->SetInput(0, inputImage);
        
        // Set the DPI.
        ThrowIfFailed(dpiCompensator->SetValue(D2D1_DPICOMPENSATION_PROP_INPUT_DPI, D2D1_VECTOR_2F{ inputDpi, inputDpi }));

        return dpiCompensator;
    }

    void CanvasEffect::SetD2DInputs(ID2D1DeviceContext* deviceContext, float targetDpi, bool wasRecreated)
    {
        auto& inputs = m_inputs->InternalVector();
        auto inputsSize = (unsigned int)inputs.size();

        bool inputsChanged = wasRecreated || m_inputs->IsChanged();

        // Resize input array?
        if (inputsChanged)
        {
            m_resource->SetInputCount(inputsSize);
            m_previousInputRealizationIds.resize(inputsSize);
            m_dpiCompensators.resize(inputsSize);
        }

        for (unsigned int i = 0; i < inputsSize; ++i)
        {
            // Look up the WinRT interface representing this input
            if (!inputs[i])
            {
                WinStringBuilder message;
                message.Format(Strings::EffectNullInput, i);
                ThrowHR(E_POINTER, message.Get());
            }

            ComPtr<ICanvasImageInternal> internalInput;
            HRESULT hr = inputs[i].As(&internalInput);

            if (FAILED(hr))
            {
                if (hr == E_NOINTERFACE)
                {
                    WinStringBuilder message;
                    message.Format(Strings::EffectWrongInputType, i);
                    ThrowHR(hr, message.Get());
                }
                else
                {
                    ThrowHR(hr);
                }
            }

            // Get the underlying D2D interface (this call recurses through the effect graph)
            auto realizedInput = internalInput->GetRealizedEffectNode(deviceContext, targetDpi);

            bool needsDpiCompensation = (realizedInput.Dpi != targetDpi) && (realizedInput.Dpi != 0) && (targetDpi != 0);
            bool hasDpiCompensation = m_dpiCompensators[i] != nullptr;

            // If the input value has changed, update the D2D effect graph
            if (inputsChanged || 
                realizedInput.RealizationId != m_previousInputRealizationIds[i] ||
                needsDpiCompensation != hasDpiCompensation)
            {
                if (needsDpiCompensation)
                {
                    m_dpiCompensators[i] = InsertDpiCompensationEffect(deviceContext, realizedInput.Image.Get(), realizedInput.Dpi, m_dpiCompensators[i].Get());
                    realizedInput.Image = As<ID2D1Image>(m_dpiCompensators[i]);
                }
                else
                {
                    m_dpiCompensators[i].Reset();
                }

                m_resource->SetInput(i, realizedInput.Image.Get());
                m_previousInputRealizationIds[i] = realizedInput.RealizationId;
            }
        }

        m_inputs->SetChanged(false);
    }

    void CanvasEffect::SetD2DProperties()
    {
        auto& properties = m_properties->InternalVector();
        auto propertiesSize = (unsigned int) properties.size();

        for (unsigned int i = 0; i < propertiesSize; ++i)
        {
            if (!properties[i])
            {
                WinStringBuilder message;
                message.Format(Strings::EffectNullProperty, i);
                ThrowHR(E_POINTER, message.Get());
            }

            auto propertyValue = As<IPropertyValue>(properties[i]);

            PropertyType propertyType;
            ThrowIfFailed(propertyValue->get_Type(&propertyType));

            HRESULT hr;

            switch (propertyType)
            {
            case PropertyType_Boolean:
            {
                boolean value;
                ThrowIfFailed(propertyValue->GetBoolean(&value));
                hr = m_resource->SetValue(i, static_cast<BOOL>(value));
                break;
            }
            case PropertyType_Int32:
            {
                INT32 value;
                ThrowIfFailed(propertyValue->GetInt32(&value));
                hr = m_resource->SetValue(i, value);
                break;
            }
            case PropertyType_UInt32:
            {
                UINT32 value;
                ThrowIfFailed(propertyValue->GetUInt32(&value));
                hr = m_resource->SetValue(i, value);
                break;
            }
            case PropertyType_Single:
            {
                float value;
                ThrowIfFailed(propertyValue->GetSingle(&value));
                hr = m_resource->SetValue(i, value);
                break;
            }
            case PropertyType_SingleArray:
            {
                ComArray<float> value;
                ThrowIfFailed(propertyValue->GetSingleArray(value.GetAddressOfSize(), value.GetAddressOfData()));
                hr = m_resource->SetValue(i, reinterpret_cast<BYTE*>(value.GetData()), value.GetSize() * sizeof(float));
                break;
            }
            default:
                hr = E_INVALIDARG;
                break;
            }

            if (FAILED(hr))
            {
                if (hr == E_INVALIDARG)
                {
                    WinStringBuilder message;
                    message.Format(Strings::EffectWrongPropertyType, i);
                    ThrowHR(hr, message.Get());
                }
                else
                {
                    ThrowHR(hr);
                }
            }
        }

        m_properties->SetChanged(false);
    }

    void CanvasEffect::ThrowIfClosed()
    {
        if (m_closed)
        {
            ThrowHR(RO_E_CLOSED);
        }
    }

}}}}}

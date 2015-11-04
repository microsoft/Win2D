// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include "SharedShaderState.h"
#include "utils/HashUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects
{
    CoordinateMappingState::CoordinateMappingState()
        : MaxOffset(0)
    {
        for (int i = 0; i < MaxShaderInputs; i++)
        {
            Mapping[i] = SamplerCoordinateMapping::Unknown;
        }
    }


    SourceInterpolationState::SourceInterpolationState()
    {
        for (int i = 0; i < MaxShaderInputs; i++)
        {
            Filter[i] = D2D1_FILTER_MIN_MAG_MIP_LINEAR;
        }
    }


    SharedShaderState::SharedShaderState(ShaderDescription const& shader, std::vector<BYTE> const& constants, CoordinateMappingState const& coordinateMapping, SourceInterpolationState const& sourceInterpolation)
        : m_shader(shader)
        , m_constants(constants)
        , m_coordinateMapping(coordinateMapping)
        , m_sourceInterpolation(sourceInterpolation)
    { }


    SharedShaderState::SharedShaderState(BYTE* shaderCode, uint32_t shaderCodeSize)
    {
        // Store the shader program code.
        m_shader.Code.assign(shaderCode, shaderCode + shaderCodeSize);

        // Hash it to generate a unique ID.
        static const IID salt{ 0x489257f6, 0x6544, 0x4277, 0x89, 0x82, 0xea, 0xd1, 0x69, 0x39, 0x1f, 0x3d };

        m_shader.Hash = GetVersion5Uuid(salt, shaderCode, shaderCodeSize);

        // Look up shader metadata.
        ReflectOverShader();
    }


    ComPtr<ISharedShaderState> SharedShaderState::Clone()
    {
        auto clone = Make<SharedShaderState>(m_shader, m_constants, m_coordinateMapping, m_sourceInterpolation);
        CheckMakeResult(clone);

        return clone;
    }


    unsigned SharedShaderState::GetPropertyCount()
    {
        return static_cast<unsigned>(m_shader.Variables.size());
    }


    bool SharedShaderState::HasProperty(HSTRING name)
    {
        return std::binary_search(m_shader.Variables.begin(), m_shader.Variables.end(), name, VariableNameComparison());
    }


    ComPtr<IInspectable> SharedShaderState::GetProperty(HSTRING name)
    {
        auto& variable = FindVariable(name);

        return GetProperty(variable);
    }


    ComPtr<IInspectable> SharedShaderState::GetProperty(ShaderVariable const& variable)
    {
        ComPtr<IInspectable> result;

        switch (variable.Type)
        {
        case D3D_SVT_FLOAT:
            {
                // It's a float, or a type comprised of floats (vector, matrix, and/or array).
                std::vector<float> values(variable.ComponentCount());

                CopyConstantData<CopyDirection::Read>(variable, values.data());

                if (variable.IsVector(2))
                {
                    result = Box<Vector2>(variable, values);
                }
                else if (variable.IsVector(3))
                {
                    result = Box<Vector3>(variable, values);
                }
                else if (variable.IsVector(4))
                {
                    result = Box<Vector4>(variable, values);
                }
                else if (variable.IsMatrix(3, 2))
                {
                    result = Box<Matrix3x2>(variable, values);
                }
                else if (variable.IsMatrix(4, 4))
                {
                    result = Box<Matrix4x4>(variable, values);
                }
                else
                {
                    result = Box<float>(variable, values);
                }
            }
            break;

        case D3D_SVT_INT:
            {
                // Integer, or vector/matrix/arrays thereof.
                std::vector<int> values(variable.ComponentCount());

                CopyConstantData<CopyDirection::Read>(variable, values.data());

                result = Box<int>(variable, values);
            }
            break;

        case D3D_SVT_BOOL:
            {
                // Bool, or vector/matrix/arrays thereof.
                std::vector<boolean> values(variable.ComponentCount());

                CopyConstantData<CopyDirection::Read>(variable, values.data());

                result = Box<bool>(variable, values);
            }
            break;

        default:
            assert(false);
        }

        return result;
    }


    void SharedShaderState::SetProperty(HSTRING name, IInspectable* boxedValue)
    {
        auto& variable = FindVariable(name);

        switch (variable.Type)
        {
            case D3D_SVT_FLOAT:
            {
                // It's a float, or a type comprised of floats (vector, matrix, and/or array).
                if (variable.IsVector(2))
                {
                    Unbox<Vector2, float>(variable, boxedValue);
                }
                else if (variable.IsVector(3))
                {
                    Unbox<Vector3, float>(variable, boxedValue);
                }
                else if (variable.IsVector(4))
                {
                    Unbox<Vector4, float>(variable, boxedValue);
                }
                else if (variable.IsMatrix(3, 2))
                {
                    Unbox<Matrix3x2, float>(variable, boxedValue);
                }
                else if (variable.IsMatrix(4, 4))
                {
                    Unbox<Matrix4x4, float>(variable, boxedValue);
                }
                else
                {
                    Unbox<float>(variable, boxedValue);
                }
            }
            break;

        case D3D_SVT_INT:
            {
                // Integer, or vector/matrix/arrays thereof.
                Unbox<int>(variable, boxedValue);
            }
            break;

        case D3D_SVT_BOOL:
            {
                // Bool, or vector/matrix/arrays thereof.
                Unbox<bool, boolean>(variable, boxedValue);
            }
            break;

        default:
            assert(false);
        }
    }


    std::vector<StringObjectPair> SharedShaderState::EnumerateProperties()
    {
        std::vector<StringObjectPair> properties;

        properties.reserve(m_shader.Variables.size());

        for (auto& variable : m_shader.Variables)
        {
            properties.emplace_back(variable.Name, GetProperty(variable));
        }

        return properties;
    }


    ShaderVariable const& SharedShaderState::FindVariable(HSTRING name)
    {
        VariableNameComparison comparison;

        auto it = std::lower_bound(m_shader.Variables.begin(), m_shader.Variables.end(), name, comparison);

        if (it == m_shader.Variables.end() || comparison(name, *it))
        {
            WinStringBuilder message;
            message.Format(Strings::CustomEffectUnknownProperty, WindowsGetStringRawBuffer(name, nullptr));
            ThrowHR(E_INVALIDARG, message.Get());
        }

        return *it;
    }


    // For formatting error message strings.
    template<typename T> wchar_t const* PropertyTypeName() { static_assert(false, "missing specialization"); }

    template<> wchar_t const* PropertyTypeName<float>()     { return L"Single"; }
    template<> wchar_t const* PropertyTypeName<int>()       { return L"Int32"; }
    template<> wchar_t const* PropertyTypeName<bool>()      { return L"Boolean"; }
    template<> wchar_t const* PropertyTypeName<Vector2>()   { return L"Vector2"; }
    template<> wchar_t const* PropertyTypeName<Vector3>()   { return L"Vector3"; }
    template<> wchar_t const* PropertyTypeName<Vector4>()   { return L"Vector4"; }
    template<> wchar_t const* PropertyTypeName<Matrix3x2>() { return L"Matrix3x2"; }
    template<> wchar_t const* PropertyTypeName<Matrix4x4>() { return L"Matrix4x4"; }


    // Types used in the following templates:
    //
    // For floats and ints:
    //      TBoxed, TComponent, and TBoxed_abi = float or int (all three the same)
    //
    // For vectors and matrices:
    //      TBoxed and TBoxed_abi = the vector/matrix type
    //      TComponent = float
    //
    // For booleans:
    //      TBoxed = bool
    //      TComponent and TBoxed_abi = boolean


    // Transfers a property from constant buffer format to a boxed IInspectable.
    template<typename TBoxed, typename TComponent>
    ComPtr<IInspectable> SharedShaderState::Box(ShaderVariable const& variable, std::vector<TComponent> const& values)
    {
        typedef Nullable<TBoxed>::T_abi TBoxed_abi;

        ComPtr<IInspectable> result;

        auto boxedValueCount = values.size() * sizeof(TComponent) / sizeof(TBoxed_abi);
        auto asBoxed = reinterpret_cast<TBoxed_abi const*>(values.data());

        if (boxedValueCount == 1 && !variable.Elements)
        {
            // Box a single value.
            result = Make<Nullable<TBoxed>>(asBoxed[0]);
        }
        else
        {
            // Box an array of values.
            result = Make<ReferenceArray<TBoxed>>(asBoxed, asBoxed + boxedValueCount);
        }

        CheckMakeResult(result);
        return result;
    }


    // Transfers a property from a boxed IInspectable to constant buffer format.
    template<typename TBoxed, typename TComponent>
    void SharedShaderState::Unbox(ShaderVariable const& variable, IInspectable* boxedValue)
    {
        typedef Nullable<TBoxed>::T_abi TBoxed_abi;

        auto boxedValueCount = variable.ComponentCount() * sizeof(TComponent) / sizeof(TBoxed_abi);

        if (boxedValueCount == 1 && !variable.Elements)
        {
            // Unbox a single value.
            TBoxed_abi unboxed;

            auto reference = MaybeAs<IReference<TBoxed>>(boxedValue);

            if (!reference)
            {
                WinStringBuilder message;
                message.Format(Strings::CustomEffectWrongPropertyType, static_cast<wchar_t const*>(variable.Name), PropertyTypeName<TBoxed>());
                ThrowHR(E_INVALIDARG, message.Get());
            }

            ThrowIfFailed(reference->get_Value(&unboxed));

            // Copy into the constant buffer.
            CopyConstantData<CopyDirection::Write>(variable, reinterpret_cast<TComponent*>(&unboxed));
        }
        else
        {
            // Unbox an array of values.
            ComArray<TBoxed_abi> values;

            auto referenceArray = MaybeAs<IReferenceArray<TBoxed>>(boxedValue);

            if (!referenceArray)
            {
                WinStringBuilder message;
                message.Format(Strings::CustomEffectWrongPropertyTypeArray, static_cast<wchar_t const*>(variable.Name), PropertyTypeName<TBoxed>());
                ThrowHR(E_INVALIDARG, message.Get());
            }

            ThrowIfFailed(referenceArray->get_Value(values.GetAddressOfSize(), values.GetAddressOfData()));

            // Validate the size of the array.
            if (values.GetSize() != boxedValueCount)
            {
                WinStringBuilder message;
                message.Format(Strings::CustomEffectWrongPropertyArraySize, static_cast<wchar_t const*>(variable.Name), boxedValueCount);
                ThrowHR(E_INVALIDARG, message.Get());
            }

            // Copy into the constant buffer.
            CopyConstantData<CopyDirection::Write>(variable, reinterpret_cast<TComponent*>(values.GetData()));
        }
    }


    // Templated helper responsible for transfering a single component value to or from the constant buffer.
    template<CopyDirection Direction, typename T>
    struct TransferValue
    { };


    // Reads an int or float from the constant buffer.
    template<typename T>
    struct TransferValue<CopyDirection::Read, T>
    {
        typedef T TConstant;

        void operator() (T constantBuffer, T& value)
        {
            value = constantBuffer;
        }
    };


    // Writes an int or float to the constant buffer.
    template<typename T>
    struct TransferValue<CopyDirection::Write, T>
    {
        typedef T TConstant;

        void operator() (T& constantBuffer, T value)
        {
            constantBuffer = value;
        }
    };


    // Reads a boolean value from the constant buffer.
    template<>
    struct TransferValue<CopyDirection::Read, boolean>
    {
        typedef int TConstant;

        void operator() (int constantBuffer, boolean& value)
        {
            value = !!constantBuffer;
        }
    };


    // Writes a boolean value to the constant buffer.
    template<>
    struct TransferValue<CopyDirection::Write, boolean>
    {
        typedef int TConstant;

        void operator() (int& constantBuffer, boolean value)
        {
            constantBuffer = !!value;
        }
    };


    // Generic helper used by both the property get and set implementations.
    // Transfers a property value in either direction between the constant buffer
    // and an array of individual component values, handling type conversions
    // (boolean <-> int), matrix row vs. column layout, and array element alignment.
    template<CopyDirection Direction, typename TComponent>
    void SharedShaderState::CopyConstantData(ShaderVariable const& variable, TComponent* values)
    {
        // Templated transfer function handles read vs. write and boolean type conversion.
        typedef TransferValue<Direction, TComponent> Transferer;
        
        Transferer transferFunction;

        // Look up our region of the constant buffer.
        auto constantBuffer = reinterpret_cast<Transferer::TConstant*>(m_constants.data() + variable.Offset);

        // Compute the size of a single array element.
        const unsigned componentsPerRegister = 4;

        unsigned elementSize = ((variable.Class == D3D_SVC_MATRIX_COLUMNS) ? variable.Columns : variable.Rows) * componentsPerRegister;

        unsigned valueIndex = 0;

        for (unsigned element = 0; element < std::max(variable.Elements, 1u); element++)
        {
            for (unsigned row = 0; row < variable.Rows; row++)
            {
                for (unsigned column = 0; column < variable.Columns; column++)
                {
                    // Where is this component located in the constant buffer?
                    unsigned x = column;
                    unsigned y = row;

                    if (variable.Class == D3D_SVC_MATRIX_COLUMNS)
                    {
                        std::swap(x, y);
                    }

                    unsigned constantIndex = (element * elementSize) + (y * componentsPerRegister) + x;

                    // Sanity check (can only fail if the shader blob is corrupted or my code is horribly buggy).
                    if (constantIndex >= variable.Size / sizeof(Transferer::TConstant))
                    {
                        ThrowHR(E_UNEXPECTED);
                    }

                    // Read or write this component value.
                    transferFunction(constantBuffer[constantIndex], values[valueIndex++]);
                }
            }
        }
    }


    void SharedShaderState::ReflectOverShader()
    {
        // Create the shader reflection interface.
        ComPtr<ID3D11ShaderReflection> reflector;

        HRESULT hr = D3DReflect(m_shader.Code.data(), m_shader.Code.size(), IID_PPV_ARGS(&reflector));

        if (FAILED(hr))
            ThrowHR(E_INVALIDARG, Strings::CustomEffectBadShader);

        D3D11_SHADER_DESC desc;
        ThrowIfFailed(reflector->GetDesc(&desc));

        // Make sure this is a pixel shader.
        auto shaderType = D3D11_SHVER_GET_TYPE(desc.Version);
        auto shaderModel = D3D11_SHVER_GET_MAJOR(desc.Version);

        if (shaderType != D3D11_SHVER_PIXEL_SHADER || 
            shaderModel != 4)
        {
            ThrowHR(E_INVALIDARG, Strings::CustomEffectBadShader);
        }

        // Examine the input bindings.
        ReflectOverBindings(reflector.Get(), desc);

        // Store the mapping from named constants to buffer locations.
        if (desc.ConstantBuffers)
        {
            ReflectOverConstantBuffer(reflector->GetConstantBufferByIndex(0));
        }

        // Grab some other metadata.
        m_shader.InstructionCount = desc.InstructionCount;

        ThrowIfFailed(reflector->GetMinFeatureLevel(&m_shader.MinFeatureLevel));

        // If this shader was compiled to support shader linking, we can also determine which inputs are simple vs. complex.
        ReflectOverShaderLinkingFunction();
    }


    void SharedShaderState::ReflectOverBindings(ID3D11ShaderReflection* reflector, D3D11_SHADER_DESC const& desc)
    {
        for (unsigned i = 0; i < desc.BoundResources; i++)
        {
            D3D11_SHADER_INPUT_BIND_DESC inputDesc;
            ThrowIfFailed(reflector->GetResourceBindingDesc(i, &inputDesc));

            switch (inputDesc.Type)
            {
            case D3D_SIT_TEXTURE:
                if (inputDesc.BindPoint >= MaxShaderInputs)
                    ThrowHR(E_INVALIDARG, Strings::CustomEffectTooManyTextures);

                // Record how many input textures this shader uses.
                m_shader.InputCount = std::max(m_shader.InputCount, inputDesc.BindPoint + 1);
                break;

            case D3D_SIT_CBUFFER:
                // We want just one constant buffer, thanks all the same.
                if (inputDesc.BindPoint > 0)
                    ThrowHR(E_INVALIDARG, Strings::CustomEffectTooManyConstantBuffers);
                break;
            }
        }
    }


    void SharedShaderState::ReflectOverConstantBuffer(ID3D11ShaderReflectionConstantBuffer* constantBuffer)
    {
        D3D11_SHADER_BUFFER_DESC desc;
        ThrowIfFailed(constantBuffer->GetDesc(&desc));

        // Resize our constant buffer to match the shader.
        m_constants.resize(desc.Size);

        // Look up variable metadata.
        m_shader.Variables.reserve(desc.Variables);

        for (unsigned i = 0; i < desc.Variables; i++)
        {
            ReflectOverVariable(constantBuffer->GetVariableByIndex(i));
        }

        // Sort the variables by name.
        std::sort(m_shader.Variables.begin(), m_shader.Variables.end(), VariableNameComparison());
    }


    static bool IsSupportedVariableClass(D3D_SHADER_VARIABLE_CLASS value)
    {
        switch (value)
        {
        case D3D_SVC_SCALAR:
        case D3D_SVC_VECTOR:
        case D3D_SVC_MATRIX_ROWS:
        case D3D_SVC_MATRIX_COLUMNS:
            return true;

        default:
            return false;
        }
    }


    static bool IsSupportedVariableType(D3D_SHADER_VARIABLE_TYPE value)
    {
        switch (value)
        {
        case D3D_SVT_FLOAT:
        case D3D_SVT_INT:
        case D3D_SVT_BOOL:
            return true;

        default:
            return false;
        }
    }


    static void CopyDefaultValue(void* output, D3D11_SHADER_VARIABLE_DESC const& desc, D3D11_SHADER_TYPE_DESC const& type)
    {
        if (type.Class == D3D_SVC_MATRIX_ROWS || type.Class == D3D_SVC_MATRIX_COLUMNS)
        {
            // The HLSL compiler writes out default matrix values with their rows and columns swapped.
            // This code flips them back into the correct layout.
            auto src = reinterpret_cast<uint32_t*>(desc.DefaultValue);
            auto dest = reinterpret_cast<uint32_t*>(output);

            unsigned elementSize = ((type.Class == D3D_SVC_MATRIX_COLUMNS) ? type.Columns : type.Rows) * 4;

            for (unsigned element = 0; element < std::max(type.Elements, 1u); element++)
            {
                for (unsigned i = 0; i < type.Rows * type.Columns; i++)
                {
                    unsigned srcX = i / type.Rows;
                    unsigned srcY = i % type.Rows;

                    unsigned destX = i % type.Columns;
                    unsigned destY = i / type.Columns;

                    if (type.Class == D3D_SVC_MATRIX_COLUMNS)
                    {
                        std::swap(srcX, srcY);
                        std::swap(destX, destY);
                    }

                    unsigned srcIndex = (element * elementSize) + (srcY * 4) + srcX;
                    unsigned destIndex = (element * elementSize) + (destY * 4) + destX;

                    // Sanity check.
                    if (std::max(srcIndex, destIndex) >= desc.Size / sizeof(uint32_t))
                    {
                        ThrowHR(E_UNEXPECTED);
                    }

                    dest[destIndex] = src[srcIndex];
                }
            }
        }
        else
        {
            // Other types can be copied directly.
            memcpy(output, desc.DefaultValue, desc.Size);
        }
    }


    void SharedShaderState::ReflectOverVariable(ID3D11ShaderReflectionVariable* variable)
    {
        D3D11_SHADER_VARIABLE_DESC desc;
        ThrowIfFailed(variable->GetDesc(&desc));

        D3D11_SHADER_TYPE_DESC type;
        ThrowIfFailed(variable->GetType()->GetDesc(&type));

        // Make sure we support this variable type.
        if (!IsSupportedVariableClass(type.Class) ||
            !IsSupportedVariableType(type.Type) ||
            type.Members != 0 || 
            type.Offset != 0)
        {
            WinStringBuilder message;
            message.Format(Strings::CustomEffectBadPropertyType, desc.Name);
            ThrowHR(E_INVALIDARG, message.Get());
        }

        // Sanity check that the variable lies inside the constant buffer.
        // This can only fail if the shader blob is corrupted.
        auto endOffset = desc.StartOffset + desc.Size;

        if (endOffset > m_constants.size() || endOffset < desc.StartOffset)
        {
            ThrowHR(E_UNEXPECTED);
        }

        // Initialize our constant buffer with the default value of the variable.
        if (desc.DefaultValue)
        {
            CopyDefaultValue(m_constants.data() + desc.StartOffset, desc, type);
        }

        // Store metadata about this variable.
        m_shader.Variables.emplace_back(desc, type);
    }


    void SharedShaderState::ReflectOverShaderLinkingFunction()
    {
        // If this shader was compiled to support shader linking, we can get extra information
        // (telling us which inputs are simple vs. complex) from the shader linking function.
        
        // It's valid to use shaders that don't support linking, so we return on failure rather than throwing.
        ComPtr<ID3DBlob> privateData;

        if (FAILED(D3DGetBlobPart(m_shader.Code.data(), m_shader.Code.size(), D3D_BLOB_PRIVATE_DATA, 0, &privateData)))
            return;

        ComPtr<ID3D11LibraryReflection> reflector;

        if (FAILED(D3DReflectLibrary(privateData->GetBufferPointer(), privateData->GetBufferSize(), IID_PPV_ARGS(&reflector))))
            return;

        D3D11_LIBRARY_DESC desc;
        reflector->GetDesc(&desc);

        // For shader linking there should be a single function entrypoint.
        if (desc.FunctionCount != 1)
            return;

        auto function = reflector->GetFunctionByIndex(0);

        D3D11_FUNCTION_DESC functionDesc;
        ThrowIfFailed(function->GetDesc(&functionDesc));

        int inputCount = 0;

        for (int i = 0; i < functionDesc.FunctionParameterCount; i++)
        {
            D3D11_PARAMETER_DESC parameterDesc;
            ThrowIfFailed(function->GetFunctionParameter(i)->GetDesc(&parameterDesc));

            if (strstr(parameterDesc.SemanticName, "TEXCOORD"))
            {
                // TEXCOORD semantic means this is a complex input, so skip past it.
                inputCount++;
            }
            else if (strstr(parameterDesc.SemanticName, "INPUT"))
            {
                // INPUT semantic means a simple input, so select passthrough coordinate mapping mode.
                m_coordinateMapping.Mapping[inputCount++] = SamplerCoordinateMapping::OneToOne;
            }
        }
    }

}}}}}

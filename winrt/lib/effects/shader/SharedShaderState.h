// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "ShaderDescription.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Effects 
{
    typedef std::pair<WinString, ComPtr<IInspectable>> StringObjectPair;

    enum class CopyDirection { Read, Write };

    const int MaxShaderInputs = 8;


    // Describes how this shader maps between its input images and output locations.
    struct CoordinateMappingState
    {
        CoordinateMappingState();

        SamplerCoordinateMapping Mapping[MaxShaderInputs];
        int MaxOffset;
    };


    // Implementation state shared between PixelShaderEffect and PixelShaderEffectImpl.
    // This stores the compiled shader code, metadata obtained via shader reflection,
    // and app-specified state such as the current constant buffer.
    //
    // This interface is communicated from Win2D to D2D by setting it as a property on
    // the D2D effect. It tightly couples the two sides (compared to the alternative of
    // splitting the state into a larger number of individual D2D effect properties),
    // which greatly simplifies binding PixelShaderEffectImpl to its Win2D wrapper at
    // the cost of limiting ability to use it directly from native D2D.

    [uuid("9B81CB94-C07D-4635-9507-D9857B7B281F")]
    class ISharedShaderState : public IUnknown
    {
    public:
        virtual ComPtr<ISharedShaderState> Clone() = 0;

        virtual ShaderDescription const& Shader() = 0;
        virtual std::vector<BYTE> const& Constants() = 0;
        virtual CoordinateMappingState& CoordinateMapping() = 0;

        // Property accessors.
        virtual unsigned GetPropertyCount() = 0;
        virtual bool HasProperty(HSTRING name) = 0;
        virtual ComPtr<IInspectable> GetProperty(HSTRING name) = 0;
        virtual void SetProperty(HSTRING name, IInspectable* boxedValue) = 0;
        virtual std::vector<StringObjectPair> EnumerateProperties() = 0;
    };
    

    class SharedShaderState : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ISharedShaderState>
                            , private LifespanTracker<SharedShaderState>
    {
        ShaderDescription m_shader;
        std::vector<BYTE> m_constants;
        CoordinateMappingState m_coordinateMapping;

    public:
        SharedShaderState(ShaderDescription const& shader, std::vector<BYTE> const& constants, CoordinateMappingState const& coordinateMapping);
        SharedShaderState(BYTE* shaderCode, uint32_t shaderCodeSize);

        virtual ComPtr<ISharedShaderState> Clone() override;

        virtual ShaderDescription const& Shader() override { return m_shader; }
        virtual std::vector<BYTE> const& Constants() override { return m_constants; }
        virtual CoordinateMappingState& CoordinateMapping() override { return m_coordinateMapping; }

        // Property accessors.
        virtual unsigned GetPropertyCount() override;
        virtual bool HasProperty(HSTRING name) override;
        virtual ComPtr<IInspectable> GetProperty(HSTRING name) override;
        virtual void SetProperty(HSTRING name, IInspectable* boxedValue) override;
        virtual std::vector<StringObjectPair> EnumerateProperties() override;

    private:
        ComPtr<IInspectable> GetProperty(ShaderVariable const& variable);
        ShaderVariable const& FindVariable(HSTRING name);


        // Transfer property values between constant buffer and boxed IInspectable formats.
        template<typename TBoxed, typename TComponent>
        static ComPtr<IInspectable> Box(ShaderVariable const& variable, std::vector<TComponent> const& values);

        template<typename TBoxed, typename TComponent = TBoxed>
        void Unbox(ShaderVariable const& variable, IInspectable* boxedValue);

        template<CopyDirection Direction, typename TComponent>
        void CopyConstantData(ShaderVariable const& variable, TComponent* values);


        // Shader reflection (done at init time).
        void ReflectOverShader();
        void ReflectOverBindings(ID3D11ShaderReflection* reflector, D3D11_SHADER_DESC const& desc);
        void ReflectOverConstantBuffer(ID3D11ShaderReflectionConstantBuffer* constantBuffer);
        void ReflectOverVariable(ID3D11ShaderReflectionVariable* variable);
        void ReflectOverShaderLinkingFunction();
    };

}}}}}

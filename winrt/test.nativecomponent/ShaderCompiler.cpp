// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace NativeComponent
{
    using namespace Microsoft::WRL;

    public ref class ShaderCompiler sealed
    {
    public:
        static Platform::Array<byte>^ CompileShader(Platform::String^ shaderCode, Platform::String^ targetProfile, Platform::String^ entryPoint)
        {
            // Convert Unicode -> ASCII (sloppily, just casting wchar_t to char, but this is good enough for unit tests).
            std::string asciiShader(shaderCode->Begin(), shaderCode->End());
            std::string asciiProfile(targetProfile->Begin(), targetProfile->End());
            std::string asciiEntryPoint(entryPoint->Begin(), entryPoint->End());

            ComPtr<ID3DBlob> result;
            ComPtr<ID3DBlob> errors;

            HRESULT hr = D3DCompile(
                asciiShader.c_str(),
                asciiShader.size(),
                nullptr, // sourceName
                nullptr, // defines
                nullptr, // include
                asciiEntryPoint.empty() ? nullptr : asciiEntryPoint.c_str(),
                asciiProfile.c_str(),
                0, // flags1
                0, // flags2
                &result,
                &errors);

            // Report any shader compile errors.
            if (FAILED(hr))
            {
                auto errorBuffer = reinterpret_cast<char const*>(errors->GetBufferPointer());

                std::wstring errorString(errorBuffer, errorBuffer + errors->GetBufferSize());

                throw ref new Platform::Exception(hr, ref new Platform::String(errorString.c_str()));
            }

            return BlobToArray(result.Get());
        }


        static Platform::Array<byte>^ CompileShader(Platform::String^ shaderCode, Platform::String^ targetProfile)
        {
            return CompileShader(shaderCode, targetProfile, L"main");
        }


#if WINVER > _WIN32_WINNT_WINBLUE

        static Platform::Array<byte>^ CompileShaderAndEmbedLinkingFunction(Platform::String^ shaderCode)
        {
            auto bin = CompileShader(shaderCode, L"ps_4_0_level_9_3", L"main");
            auto lib = CompileShader(shaderCode, L"lib_4_0_level_9_3_ps_only", nullptr);

            ComPtr<ID3DBlob> mergedBlob;

            ThrowIfFailed(D3DSetBlobPart(bin->Data, bin->Length, D3D_BLOB_PRIVATE_DATA, 0, lib->Data, lib->Length, &mergedBlob));

            return BlobToArray(mergedBlob.Get());
        }

#endif


    private:
        static Platform::Array<byte>^ BlobToArray(ID3DBlob* blob)
        {
            auto buffer = reinterpret_cast<byte*>(blob->GetBufferPointer());

            return ref new Platform::Array<byte>(buffer, static_cast<unsigned>(blob->GetBufferSize()));
        }
    };
}

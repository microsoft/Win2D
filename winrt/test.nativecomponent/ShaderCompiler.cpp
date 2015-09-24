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
        static Platform::Array<byte>^ CompileShader(Platform::String^ shaderCode, Platform::String^ targetProfile)
        {
            // Convert Unicode -> ASCII (sloppily, just casting wchar_t to char, but this is good enough for unit tests).
            std::string asciiShader(shaderCode->Begin(), shaderCode->End());
            std::string asciiProfile(targetProfile->Begin(), targetProfile->End());

            ComPtr<ID3DBlob> result;
            ComPtr<ID3DBlob> errors;

            HRESULT hr = D3DCompile(
                asciiShader.c_str(),
                asciiShader.size(),
                nullptr, // sourceName
                nullptr, // defines
                nullptr, // include
                "main",
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

            // Convert the output blob to a WinRT array.
            auto resultBuffer = reinterpret_cast<byte*>(result->GetBufferPointer());

            return ref new Platform::Array<byte>(resultBuffer, static_cast<unsigned>(result->GetBufferSize()));
        }
    };
}

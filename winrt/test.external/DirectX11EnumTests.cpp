// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"

using namespace Microsoft::DirectX;

TEST_CLASS(DirectX11EnumTests)
{
public:
    TEST_METHOD(DirectX11UsageMatchesD3D11_USAGE)
    {
#define CHECK(WINRT, D3D11) Assert::AreEqual(static_cast<int>(DirectX11Usage::WINRT), static_cast<int>(D3D11))

        CHECK(Default   , D3D11_USAGE_DEFAULT);
        CHECK(Immutable , D3D11_USAGE_IMMUTABLE);
        CHECK(Dynamic   , D3D11_USAGE_DYNAMIC);
        CHECK(Staging   , D3D11_USAGE_STAGING);

#undef CHECK
    }

    TEST_METHOD(DirectX11BindingsMatchesD3D11_BIND_FLAG)
    {
#define CHECK(WINRT, D3D11) Assert::AreEqual(static_cast<uint32>(DirectX11Bindings::WINRT), static_cast<uint32>(D3D11))

        CHECK(VertexBuffer    , D3D11_BIND_VERTEX_BUFFER);
        CHECK(IndexBuffer     , D3D11_BIND_INDEX_BUFFER);
        CHECK(ConstantBuffer  , D3D11_BIND_CONSTANT_BUFFER);
        CHECK(ShaderResource  , D3D11_BIND_SHADER_RESOURCE);
        CHECK(StreamOutput    , D3D11_BIND_STREAM_OUTPUT);
        CHECK(RenderTarget    , D3D11_BIND_RENDER_TARGET);
        CHECK(DepthStencil    , D3D11_BIND_DEPTH_STENCIL);
        CHECK(UnorderedAccess , D3D11_BIND_UNORDERED_ACCESS);
        CHECK(Decoder         , D3D11_BIND_DECODER);
        CHECK(VideoEncoder    , D3D11_BIND_VIDEO_ENCODER);

#undef CHECK
    }
};


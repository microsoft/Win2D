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
#include "TeapotRenderer.h"

using namespace ExampleGallery::Direct3DInterop;
using namespace Windows::Foundation::Numerics;


TeapotRenderer::TeapotRenderer(ICanvasResourceCreator^ resourceCreator)
    : m_depthStencilWidth(0)
    , m_depthStencilHeight(0)
{
    // Look up the Direct3D device and context corresponding to the specified Win2D resource creator.
    __abi_ThrowIfFailed(GetDXGIInterface(resourceCreator->Device, m_device.GetAddressOf()));

    m_device->GetImmediateContext(&m_deviceContext);

    // Create the teapot model (thank you DirectX Tool Kit for handling all the messy D3D calls this entails!)
    m_teapot = GeometricPrimitive::CreateTeapot(m_deviceContext.Get());

    // Create and configure our shaders (thanks again, DirectX Tool Kit!)
    m_basicEffect = std::make_unique<BasicEffect>(m_device.Get());

    m_basicEffect->EnableDefaultLighting();
    m_basicEffect->SetPerPixelLighting(true);
    m_basicEffect->SetTextureEnabled(true);

    // Create a D3D input layout descriptor.
    m_teapot->CreateInputLayout(m_basicEffect.get(), &m_inputLayout);
}


void TeapotRenderer::SetWorld(Matrix4x4 value)
{
    float4x4 temp(value);

    m_basicEffect->SetWorld(XMLoadFloat4x4(&temp));
}


void TeapotRenderer::SetView(Matrix4x4 value)
{
    float4x4 temp(value);

    m_basicEffect->SetView(XMLoadFloat4x4(&temp));
}


void TeapotRenderer::SetProjection(Matrix4x4 value)
{
    float4x4 temp(value);

    m_basicEffect->SetProjection(XMLoadFloat4x4(&temp));
}


void TeapotRenderer::SetTexture(IDirect3DSurface^ texture)
{
    // Look up the Direct3D texture corresponding to the specified IDirect3DSurface.
    ComPtr<ID3D11Texture2D> d3dTexture;
    __abi_ThrowIfFailed(GetDXGIInterface(texture, d3dTexture.GetAddressOf()));

    // Create a shader resource view.
    ComPtr<ID3D11ShaderResourceView> textureView;
    CD3D11_SHADER_RESOURCE_VIEW_DESC viewDesc(d3dTexture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D);
    __abi_ThrowIfFailed(m_device->CreateShaderResourceView(d3dTexture.Get(), &viewDesc, &textureView));

    // Tell our BasicEffect (provided by the DirectX Tool Kit) to use this new texture.
    m_basicEffect->SetTexture(textureView.Get());
}


void TeapotRenderer::Draw(CanvasDrawingSession^ drawingSession)
{
    // Look up the Direct2D device context corresponding to the specified CanvasDrawingSession.
    ComPtr<ID2D1DeviceContext> d2dContext = GetWrappedResource<ID2D1DeviceContext>(drawingSession);;

    // Flush any in-progress Direct2D rendering.
    d2dContext->Flush();

    // Tell Direct3D to render into the same surface as Direct2D.
    SetRenderTarget(d2dContext.Get());

    // Clear the depth buffer.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Draw the teapot (via the DirectX Tool Kit).
    m_teapot->Draw(m_basicEffect.get(), m_inputLayout.Get());
}


void TeapotRenderer::SetRenderTarget(ID2D1DeviceContext* d2dContext)
{
    // Look up what target image this Direct2D device context is drawing onto.
    ComPtr<ID2D1Image> d2dTarget;
    d2dContext->GetTarget(&d2dTarget);

    // The target must be a bitmap.
    ComPtr<ID2D1Bitmap1> d2dBitmap;
    __abi_ThrowIfFailed(d2dTarget.As(&d2dBitmap));

    // Direct2D bitmap -> DXGI surface.
    ComPtr<IDXGISurface> dxgiSurface;
    __abi_ThrowIfFailed(d2dBitmap->GetSurface(&dxgiSurface));

    // DXGI surface -> Direct3D resource.
    ComPtr<ID3D11Resource> d3dResource;
    __abi_ThrowIfFailed(dxgiSurface.As(&d3dResource));

    // Create a rendertarget view.
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    __abi_ThrowIfFailed(m_device->CreateRenderTargetView(d3dResource.Get(), nullptr, &renderTargetView));

    // How big is the surface?
    DXGI_SURFACE_DESC surfaceDesc;
    __abi_ThrowIfFailed(dxgiSurface->GetDesc(&surfaceDesc));

    // If we don't already have a depth buffer of the right size, create one now.
    if (!m_depthStencilView || 
        m_depthStencilWidth != surfaceDesc.Width || 
        m_depthStencilHeight != surfaceDesc.Height)
    {
        ComPtr<ID3D11Texture2D> depthTexture;
        CD3D11_TEXTURE2D_DESC depthDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, surfaceDesc.Width, surfaceDesc.Height, 1, 1, D3D11_BIND_DEPTH_STENCIL);
        __abi_ThrowIfFailed(m_device->CreateTexture2D(&depthDesc, nullptr, &depthTexture));

        CD3D11_DEPTH_STENCIL_VIEW_DESC viewDesc(depthTexture.Get(), D3D11_DSV_DIMENSION_TEXTURE2D);
        __abi_ThrowIfFailed(m_device->CreateDepthStencilView(depthTexture.Get(), &viewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

        m_depthStencilWidth = surfaceDesc.Width;
        m_depthStencilHeight = surfaceDesc.Height;
    }

    // Set the rendertarget and depth buffer.
    m_deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    D3D11_VIEWPORT viewport = { 0, 0, (float)surfaceDesc.Width, (float)surfaceDesc.Height, 0, 1 };

    m_deviceContext->RSSetViewports(1, &viewport);
}

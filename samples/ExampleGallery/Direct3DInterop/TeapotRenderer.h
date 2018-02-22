// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ExampleGallery
{
    namespace Direct3DInterop
    {
        using namespace ::DirectX;
        using namespace Microsoft::Graphics::Canvas;
        using namespace Microsoft::WRL;

#if WINVER <= _WIN32_WINNT_WINBLUE
        // On Windows 8.1, we must use the Microsoft::Graphics::Canvas::Numerics types
        // when exchanging matrices between different WinRT components.
        using namespace Microsoft::Graphics::Canvas::Numerics;
        using namespace Microsoft::Graphics::Canvas::DirectX::Direct3D11;
#else
        // On Windows 10, we can directly use the Windows::Foundation::Numerics::float4x4 type.
        // See http://blogs.msdn.com/b/win2d/archive/2015/06/02/winrt-vector-and-matrix-types-in-windows-10.aspx
        using Matrix4x4 = Windows::Foundation::Numerics::float4x4;
        using namespace Windows::Graphics::DirectX::Direct3D11;
#endif


        // Interop helper shows how to mix Win2D with Direct3D rendering.
        // Implemented in C++/CX, this class serves as a bridge between the C# Win2D code
        // in Direct3DInteropExample.xaml.cs, and Direct3D teapot rendering functionality
        // provided by the DirectX Tool Kit (https://github.com/Microsoft/DirectXTK).
        public ref class TeapotRenderer sealed
        {
        public:
            TeapotRenderer(ICanvasResourceCreator^ resourceCreator);

            void SetWorld(Matrix4x4 value);
            void SetView(Matrix4x4 value);
            void SetProjection(Matrix4x4 value);
            
            void SetTexture(IDirect3DSurface^ texture);

            void Draw(CanvasDrawingSession^ drawingSession);

        private:
            void SetRenderTarget(ID2D1DeviceContext* d2dContext);

            // DirectX Tool Kit objects.
            std::unique_ptr<GeometricPrimitive> m_teapot;
            std::unique_ptr<BasicEffect> m_basicEffect;

            // Direct3D objects.
            ComPtr<ID3D11Device> m_device;
            ComPtr<ID3D11DeviceContext> m_deviceContext;

            ComPtr<ID3D11InputLayout> m_inputLayout;

            ComPtr<ID3D11DepthStencilView> m_depthStencilView;
            UINT m_depthStencilWidth;
            UINT m_depthStencilHeight;
        };
    }
}

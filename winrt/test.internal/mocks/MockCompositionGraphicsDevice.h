// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

class MockCompositionGraphicsDevice : public RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>,
    ICompositionGraphicsDevice,
    ICompositionGraphicsDeviceInterop>
{
public:
    // ICompositionGraphicsDevice
    MOCK_METHOD4(CreateDrawingSurface, HRESULT(Size sizePixels, ABI::Microsoft::Graphics::DirectX::DirectXPixelFormat pixelFormat, ABI::Microsoft::Graphics::DirectX::DirectXAlphaMode alphaMode, ICompositionDrawingSurface** result));
    MOCK_METHOD2(add_RenderingDeviceReplaced, HRESULT(ITypedEventHandler<CompositionGraphicsDevice*, RenderingDeviceReplacedEventArgs*>* handler, EventRegistrationToken *token));
    MOCK_METHOD1(remove_RenderingDeviceReplaced, HRESULT(EventRegistrationToken token));

    // ICompositionGraphicsDeviceInterop
    MOCK_METHOD1(GetRenderingDevice, HRESULT(IUnknown** ppBackingDevice));
    MOCK_METHOD1(SetRenderingDevice, HRESULT(IUnknown* pBackingDevice));
};

#endif

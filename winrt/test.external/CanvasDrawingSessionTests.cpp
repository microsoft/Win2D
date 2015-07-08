// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;

TEST_CLASS(CanvasDrawingSessionTests)
{
    //
    // CanvasDrawingSession interop is tested more directly in test.internal.
    // This exercises the published API.
    //
    TEST_METHOD(CanvasDrawingSession_Interop)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

        auto drawingSession = GetOrCreate<CanvasDrawingSession>(context.Get());
        auto actualContext = GetWrappedResource<ID2D1DeviceContext1>(drawingSession);

        Assert::AreEqual(context.Get(), actualContext.Get());
    }

    //
    // Tests the projections of the state properties, and that they actually 
    // call through to D2D.
    //
    TEST_METHOD(CanvasDrawingSession_StateProperties)
    {
        ComPtr<ID2D1DeviceContext1> context = CreateTestD2DDeviceContext();

        float3x2 someTransform = { 1, 2, 3, 4, 5, 6 };
        float3x2 identity = { 1, 0, 0, 1, 0, 0};

        context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        context->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_COPY);
        context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
        context->SetTransform(reinterpret_cast<const D2D1_MATRIX_3X2_F*>(&someTransform));
        context->SetUnitMode(D2D1_UNIT_MODE_PIXELS);

        auto drawingSession = GetOrCreate<CanvasDrawingSession>(context.Get());

        Assert::AreEqual(CanvasAntialiasing::Aliased, drawingSession->Antialiasing);
        Assert::AreEqual(CanvasBlend::Copy, drawingSession->Blend);
        Assert::AreEqual(CanvasTextAntialiasing::Aliased, drawingSession->TextAntialiasing);
        Assert::AreEqual<float3x2>(someTransform, drawingSession->Transform);
        Assert::AreEqual(CanvasUnits::Pixels, drawingSession->Units);

        drawingSession->Antialiasing = CanvasAntialiasing::Antialiased;
        drawingSession->Blend = CanvasBlend::SourceOver;
        drawingSession->TextAntialiasing = CanvasTextAntialiasing::Auto;
        drawingSession->Transform = identity;
        drawingSession->Units = CanvasUnits::Dips;

        Assert::AreEqual(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, context->GetAntialiasMode());
        Assert::AreEqual(D2D1_PRIMITIVE_BLEND_SOURCE_OVER, context->GetPrimitiveBlend());
        Assert::AreEqual(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT, context->GetTextAntialiasMode());
        Assert::AreEqual(D2D1_UNIT_MODE_DIPS, context->GetUnitMode());

        float3x2 verifyTransform;
        context->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(&verifyTransform));
        Assert::AreEqual(identity, verifyTransform);
    }

    TEST_METHOD(CanvasDrawingSession_Interop_get_Device)
    {
        //
        // This verifes that the device retrieved from get_Device returns 
        // something which is actually compatible with the drawing session. 
        //
        CanvasDevice^ canvasDeviceA = ref new CanvasDevice();
        auto d2dDeviceA = GetWrappedResource<ID2D1Device1>(canvasDeviceA);
        ComPtr<ID2D1DeviceContext1> d2dDeviceContextA;
        ThrowIfFailed(d2dDeviceA->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &d2dDeviceContextA));

        auto drawingSessionA = GetOrCreate<CanvasDrawingSession>(d2dDeviceContextA.Get());

        CanvasDevice^ deviceB = drawingSessionA->Device;
        auto d2dDeviceB = GetWrappedResource<ID2D1Device1>(deviceB);

        Assert::AreEqual(d2dDeviceA.Get(), d2dDeviceB.Get());
    }
};


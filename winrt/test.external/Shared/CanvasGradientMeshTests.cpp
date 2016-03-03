// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

using namespace Microsoft::Graphics::Canvas::Geometry;

TEST_CLASS(CanvasGradientMeshTests)
{
    CanvasDevice^ m_device;

public:
    CanvasGradientMeshTests()
        : m_device(ref new CanvasDevice())
    {
    }

    TEST_METHOD(CanvasGradientMesh_InteropFailAfterClosure)
    {
        auto patches = ref new Platform::Array<CanvasGradientMeshPatch>(1);
        auto canvasGradientMesh = ref new CanvasGradientMesh(m_device, patches);

        delete canvasGradientMesh;

        ExpectObjectClosed([&]{ GetWrappedResource<ID2D1GradientMesh>(canvasGradientMesh); });
    }

    TEST_METHOD(CanvasGradientMesh_NativeInterop)
    {
        auto d2dDeviceContext = As<ID2D1DeviceContext2>(CreateTestD2DDeviceContext(m_device));
        D2D1_GRADIENT_MESH_PATCH patch = {};

        ComPtr<ID2D1GradientMesh> originalGradientMesh;
        ThrowIfFailed(d2dDeviceContext->CreateGradientMesh(&patch, 1, &originalGradientMesh));

        auto canvasGradientMesh = GetOrCreate<CanvasGradientMesh>(m_device, originalGradientMesh.Get());

        auto retrievedGradientMesh = GetWrappedResource<ID2D1GradientMesh>(canvasGradientMesh);

        Assert::IsTrue(IsSameInstance(originalGradientMesh.Get(), retrievedGradientMesh.Get()));
    }

    TEST_METHOD(CanvasGradientMesh_ZeroArray_Ok)
    {
        auto canvasGradientMesh = ref new CanvasGradientMesh(m_device, nullptr);
    }
};

#endif

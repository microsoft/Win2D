// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "CanvasGradientMesh.h"

using namespace ABI::Microsoft::Graphics::Canvas::Geometry;
using namespace ABI::Microsoft::Graphics::Canvas;

CanvasGradientMeshPatch CanvasGradientMeshFactory::PatchFromD2DPatch(D2D1_GRADIENT_MESH_PATCH const& d2dPatch)
{
    CanvasGradientMeshPatch out = {};

    out.Point00 = FromD2DPoint(d2dPatch.point00);
    out.Point01 = FromD2DPoint(d2dPatch.point01);
    out.Point02 = FromD2DPoint(d2dPatch.point02);
    out.Point03 = FromD2DPoint(d2dPatch.point03);

    out.Point10 = FromD2DPoint(d2dPatch.point10);
    out.Point11 = FromD2DPoint(d2dPatch.point11);
    out.Point12 = FromD2DPoint(d2dPatch.point12);
    out.Point13 = FromD2DPoint(d2dPatch.point13);

    out.Point20 = FromD2DPoint(d2dPatch.point20);
    out.Point21 = FromD2DPoint(d2dPatch.point21);
    out.Point22 = FromD2DPoint(d2dPatch.point22);
    out.Point23 = FromD2DPoint(d2dPatch.point23);

    out.Point30 = FromD2DPoint(d2dPatch.point30);
    out.Point31 = FromD2DPoint(d2dPatch.point31);
    out.Point32 = FromD2DPoint(d2dPatch.point32);
    out.Point33 = FromD2DPoint(d2dPatch.point33);

    out.Color00 = *ReinterpretAs<Vector4 const*>(&d2dPatch.color00);
    out.Color03 = *ReinterpretAs<Vector4 const*>(&d2dPatch.color03);
    out.Color30 = *ReinterpretAs<Vector4 const*>(&d2dPatch.color30);
    out.Color33 = *ReinterpretAs<Vector4 const*>(&d2dPatch.color33);

    out.Edge00To03 = FromD2DPatchEdgeMode(d2dPatch.topEdgeMode);
    out.Edge03To33 = FromD2DPatchEdgeMode(d2dPatch.rightEdgeMode);
    out.Edge33To30 = FromD2DPatchEdgeMode(d2dPatch.bottomEdgeMode);
    out.Edge30To00 = FromD2DPatchEdgeMode(d2dPatch.leftEdgeMode);

    return out;
}

D2D1_GRADIENT_MESH_PATCH CanvasGradientMeshFactory::PatchToD2DPatch(CanvasGradientMeshPatch const& patch)
{
    D2D1_GRADIENT_MESH_PATCH out = {};

    out.point00 = ToD2DPoint(patch.Point00);
    out.point01 = ToD2DPoint(patch.Point01);
    out.point02 = ToD2DPoint(patch.Point02);
    out.point03 = ToD2DPoint(patch.Point03);

    out.point10 = ToD2DPoint(patch.Point10);
    out.point11 = ToD2DPoint(patch.Point11);
    out.point12 = ToD2DPoint(patch.Point12);
    out.point13 = ToD2DPoint(patch.Point13);

    out.point20 = ToD2DPoint(patch.Point20);
    out.point21 = ToD2DPoint(patch.Point21);
    out.point22 = ToD2DPoint(patch.Point22);
    out.point23 = ToD2DPoint(patch.Point23);

    out.point30 = ToD2DPoint(patch.Point30);
    out.point31 = ToD2DPoint(patch.Point31);
    out.point32 = ToD2DPoint(patch.Point32);
    out.point33 = ToD2DPoint(patch.Point33);

    out.color00 = ToD2DColor(patch.Color00);
    out.color03 = ToD2DColor(patch.Color03);
    out.color30 = ToD2DColor(patch.Color30);
    out.color33 = ToD2DColor(patch.Color33);

    out.topEdgeMode = ToD2DPatchEdgeMode(patch.Edge00To03);
    out.rightEdgeMode = ToD2DPatchEdgeMode(patch.Edge03To33);
    out.bottomEdgeMode = ToD2DPatchEdgeMode(patch.Edge33To30);
    out.leftEdgeMode = ToD2DPatchEdgeMode(patch.Edge30To00);

    return out;
}

void ValidateArraySize(wchar_t const* name, uint32_t expected, uint32_t actual)
{
    if (actual != expected)
    {
        WinStringBuilder message;
        message.Format(Strings::WrongNamedArrayLength, name, expected, actual);
        ThrowHR(E_INVALIDARG, message.Get());
    }
}

//
// CanvasGradientMeshFactory
//

IFACEMETHODIMP CanvasGradientMeshFactory::Create(
    ICanvasResourceCreator* resourceCreator,
    uint32_t patchCount,
    CanvasGradientMeshPatch* patchElements,
    ICanvasGradientMesh** canvasGradientMesh)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(canvasGradientMesh);

            auto newGradientMesh = CanvasGradientMesh::CreateNew(resourceCreator, patchCount, patchElements);

            ThrowIfFailed(newGradientMesh.CopyTo(canvasGradientMesh));
        });
}
        
IFACEMETHODIMP CanvasGradientMeshFactory::CreateCoonsPatch(
    uint32_t pointCount,
    Vector2* pointElements,
    uint32_t colorCount,
    Vector4* colorElements,
    uint32_t edgeCount,
    CanvasGradientMeshPatchEdge* edgeElements,
    CanvasGradientMeshPatch* out)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(pointElements);
            CheckInPointer(colorElements);
            CheckInPointer(edgeElements);
            CheckInPointer(out);

            ValidateArraySize(L"points", 12, pointCount);
            ValidateArraySize(L"colors", 4, colorCount);
            ValidateArraySize(L"edges", 4, edgeCount);

            auto d2dPatch = D2D1::GradientMeshPatchFromCoonsPatch(
                ToD2DPoint(pointElements[0]),
                ToD2DPoint(pointElements[1]),
                ToD2DPoint(pointElements[2]),
                ToD2DPoint(pointElements[3]),

                ToD2DPoint(pointElements[4]),
                ToD2DPoint(pointElements[5]),
                ToD2DPoint(pointElements[6]),
                ToD2DPoint(pointElements[7]), 

                ToD2DPoint(pointElements[8]),
                ToD2DPoint(pointElements[9]),
                ToD2DPoint(pointElements[10]),
                ToD2DPoint(pointElements[11]),
        
                ToD2DColor(colorElements[0]),
                ToD2DColor(colorElements[1]),
                ToD2DColor(colorElements[2]),
                ToD2DColor(colorElements[3]),

                ToD2DPatchEdgeMode(edgeElements[0]), // Top
                ToD2DPatchEdgeMode(edgeElements[3]), // Left
                ToD2DPatchEdgeMode(edgeElements[2]), // Bottom
                ToD2DPatchEdgeMode(edgeElements[1])); // Right

            *out = PatchFromD2DPatch(d2dPatch);
        });
}

IFACEMETHODIMP CanvasGradientMeshFactory::CreateTensorPatch(
    uint32_t pointCount,
    Vector2* pointElements,
    uint32_t colorCount,
    Vector4* colorElements,
    uint32_t edgeCount,
    CanvasGradientMeshPatchEdge* edgeElements,
    CanvasGradientMeshPatch* out)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(pointElements);
            CheckInPointer(colorElements);
            CheckInPointer(edgeElements);
            CheckInPointer(out);

            ValidateArraySize(L"points", 16, pointCount);
            ValidateArraySize(L"colors", 4, colorCount);
            ValidateArraySize(L"edges", 4, edgeCount);

            out->Point00 = pointElements[0];
            out->Point01 = pointElements[1];
            out->Point02 = pointElements[2];
            out->Point03 = pointElements[3];

            out->Point10 = pointElements[4];
            out->Point11 = pointElements[5];
            out->Point12 = pointElements[6];
            out->Point13 = pointElements[7];

            out->Point20 = pointElements[8];
            out->Point21 = pointElements[9];
            out->Point22 = pointElements[10];
            out->Point23 = pointElements[11];

            out->Point30 = pointElements[12];
            out->Point31 = pointElements[13];
            out->Point32 = pointElements[14];
            out->Point33 = pointElements[15];

            out->Color00 = colorElements[0];
            out->Color03 = colorElements[1];
            out->Color30 = colorElements[2];
            out->Color33 = colorElements[3];

            out->Edge00To03 = edgeElements[0];
            out->Edge03To33 = edgeElements[1];
            out->Edge33To30 = edgeElements[2];
            out->Edge30To00 = edgeElements[3];
        });
}

//
// CanvasGradientMesh
//

CanvasGradientMesh::CanvasGradientMesh(
    ICanvasDevice* canvasDevice,
    ID2D1GradientMesh* d2dGradientMesh)
        : ResourceWrapper(d2dGradientMesh)
        , m_canvasDevice(canvasDevice)
{
}
        
IFACEMETHODIMP CanvasGradientMesh::get_Patches(
    uint32_t* valueCount,
    CanvasGradientMeshPatch** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            auto& resource = GetResource();

            uint32_t patchCount = resource->GetPatchCount();

            ComArray<CanvasGradientMeshPatch> patches(patchCount);

            if (patchCount > 0)
            {
                std::vector<D2D1_GRADIENT_MESH_PATCH> d2dPatches;
                d2dPatches.resize(patchCount);

                ThrowIfFailed(resource->GetPatches(0, &d2dPatches[0], patchCount));

                for (uint32_t i = 0; i < patchCount; ++i)
                {
                    patches[i] = CanvasGradientMeshFactory::PatchFromD2DPatch(d2dPatches[i]);
                }
            }
            patches.Detach(valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasGradientMesh::GetBounds(
    ICanvasDrawingSession* drawingSession,
    Rect* bounds)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(drawingSession);
            CheckInPointer(bounds);

            auto& resource = GetResource();

            auto deviceContext = GetWrappedResource<ID2D1DeviceContext2>(drawingSession);

            D2D1_RECT_F d2dRect;
            ThrowIfFailed(deviceContext->GetGradientMeshWorldBounds(resource.Get(), &d2dRect));
            *bounds = FromD2DRect(d2dRect);
        });
}

IFACEMETHODIMP CanvasGradientMesh::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasGradientMesh::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}

ComPtr<CanvasGradientMesh> CanvasGradientMesh::CreateNew(
    ICanvasResourceCreator* resourceCreator,
    uint32_t patchCount,
    CanvasGradientMeshPatch* patchElements)
{
    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    auto deviceInternal = As<ICanvasDeviceInternal>(device);

    ComPtr<ID2D1GradientMesh> d2dGradientMesh;
    if (patchCount > 0)
    {
        CheckInPointer(patchElements);

        std::vector<D2D1_GRADIENT_MESH_PATCH> d2dPatches;
        d2dPatches.resize(patchCount);
        for (uint32_t i = 0; i < patchCount; ++i)
        {
            d2dPatches[i] = CanvasGradientMeshFactory::PatchToD2DPatch(patchElements[i]);
        }

        d2dGradientMesh = deviceInternal->CreateGradientMesh(&d2dPatches[0], patchCount);
    }
    else
    {
        d2dGradientMesh = deviceInternal->CreateGradientMesh(nullptr, 0);
    }

    auto canvasGradientMesh = Make<CanvasGradientMesh>(
        device.Get(),
        d2dGradientMesh.Get());
    CheckMakeResult(canvasGradientMesh);

    return canvasGradientMesh;
}

ActivatableClassWithFactory(CanvasGradientMesh, CanvasGradientMeshFactory);

#endif

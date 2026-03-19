// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/effects/EffectTransferTable3D.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(EffectTransferTable3DUnitTests)
{
    template<typename TValue, typename TCreator, typename TInitializeData, typename TValidateData>
    static void TestCreate(D2D1_BUFFER_PRECISION expectedPrecision, int bytesPerValue, int valuesPerPixel, TCreator&& createTable, TInitializeData&& initializeData, TValidateData&& validateData)
    {
        auto canvasDevice = Make<StubCanvasDevice>();
        auto d2dContext = Make<StubD2DDeviceContext>();

        canvasDevice->GetResourceCreationDeviceContextMethod.AllowAnyCall([&] { return DeviceContextLease(d2dContext); });

        const int sizeB = 2;
        const int sizeG = 3;
        const int sizeR = 7;

        const int valueCount = sizeB * sizeG * sizeR * valuesPerPixel;

        std::vector<TValue> values(valueCount);
        
        initializeData(values.data());

        d2dContext->CreateLookupTable3DMethod.SetExpectedCalls(1, [&](D2D1_BUFFER_PRECISION precision, UINT32 const* sizes, BYTE const* data, UINT32 dataSize, UINT32 const* strides, ID2D1LookupTable3D** result)
        {
            Assert::AreEqual(expectedPrecision, precision);
           
            Assert::AreEqual(sizeB, static_cast<int>(sizes[0]));
            Assert::AreEqual(sizeG, static_cast<int>(sizes[1]));
            Assert::AreEqual(sizeR, static_cast<int>(sizes[2]));

            Assert::AreEqual(valueCount * bytesPerValue, static_cast<int>(dataSize));

            validateData(values.data(), data);

            Assert::AreEqual(sizeB * valuesPerPixel * bytesPerValue, static_cast<int>(strides[0]));
            Assert::AreEqual(sizeB * sizeG * valuesPerPixel * bytesPerValue, static_cast<int>(strides[1]));

            *result = nullptr;
            return S_OK;
        });
        
        auto table = createTable(canvasDevice.Get(), valueCount, values.data(), sizeB, sizeG, sizeR);

        ComPtr<ICanvasDevice> actualDevice;
        ThrowIfFailed(table->get_Device(&actualDevice));
        Assert::IsTrue(IsSameInstance(canvasDevice.Get(), actualDevice.Get()));

        // Providing too small a buffer should fail.
        ExpectHResultException(E_INVALIDARG, [&] { createTable(canvasDevice.Get(), valueCount - 1, values.data(), sizeB, sizeG, sizeR); });
    }


    TEST_METHOD_EX(EffectTransferTable3D_CreateFromColors)
    {
        auto createTable = [](MockCanvasDevice* device, int dataSize, Color* data, int sizeB, int sizeG, int sizeR)
        {
            return EffectTransferTable3D::CreateNew(device, dataSize, data, sizeB, sizeG, sizeR);
        };

        auto initializeData = [](Color* data)
        {
            data[0].R = 1;
            data[0].G = 2;
            data[0].B = 3;
            data[0].A = 4;

            data[1].R = 5;
            data[1].G = 6;
            data[1].B = 7;
            data[1].A = 8;
        };

        auto validateData = [](Color const* expected, uint8_t const* actual)
        {
            Assert::AreEqual(actual[0], expected[0].R);
            Assert::AreEqual(actual[1], expected[0].G);
            Assert::AreEqual(actual[2], expected[0].B);
            Assert::AreEqual(actual[3], expected[0].A);

            Assert::AreEqual(actual[4], expected[1].R);
            Assert::AreEqual(actual[5], expected[1].G);
            Assert::AreEqual(actual[6], expected[1].B);
            Assert::AreEqual(actual[7], expected[1].A);
        };

        TestCreate<Color>(D2D1_BUFFER_PRECISION_8BPC_UNORM, sizeof(Color), 1, createTable, initializeData, validateData);
    }


    static void TestCreateFromBytes(DirectXPixelFormat pixelFormat, D2D1_BUFFER_PRECISION expectedPrecision, int bytesPerPixel)
    {
        auto createTable = [&](MockCanvasDevice* device, int dataSize, uint8_t* data, int sizeB, int sizeG, int sizeR)
        {
            return EffectTransferTable3D::CreateNew(device, dataSize, data, sizeB, sizeG, sizeR, pixelFormat);
        };

        auto initializeData = [](uint8_t*)
        {
        };

        auto validateData = [](uint8_t const* expected, uint8_t const* actual)
        {
            Assert::AreEqual(expected, actual);
        };

        TestCreate<uint8_t>(expectedPrecision, 1, bytesPerPixel, createTable, initializeData, validateData);
    }


    TEST_METHOD_EX(EffectTransferTable3D_CreateFromBytes)
    {
        TestCreateFromBytes(PIXEL_FORMAT(R8G8B8A8UIntNormalized),     D2D1_BUFFER_PRECISION_8BPC_UNORM,      4);
        TestCreateFromBytes(PIXEL_FORMAT(R8G8B8A8UIntNormalizedSrgb), D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB, 4);
        TestCreateFromBytes(PIXEL_FORMAT(R16G16B16A16UIntNormalized), D2D1_BUFFER_PRECISION_16BPC_UNORM,     8);
        TestCreateFromBytes(PIXEL_FORMAT(R16G16B16A16Float),          D2D1_BUFFER_PRECISION_16BPC_FLOAT,     8);
        TestCreateFromBytes(PIXEL_FORMAT(R32G32B32A32Float),          D2D1_BUFFER_PRECISION_32BPC_FLOAT,     16);
    }


    TEST_METHOD_EX(EffectTransferTable3D_CreateFromColors_InvalidArgs)
    {
        auto device = Make<StubCanvasDevice>();

        Color colors[8] = { 0 };

        // Null resource creator.
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(nullptr, 8, colors, 2, 2, 2); });

        // Null data array.
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), 8, nullptr, 2, 2, 2); });

        // Too-small size.
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), 8, colors, 1, 2, 2); });
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), 8, colors, 2, 1, 2); });
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), 8, colors, 2, 2, 1); });
    }


    TEST_METHOD_EX(EffectTransferTable3D_CreateFromBytes_InvalidArgs)
    {
        auto device = Make<StubCanvasDevice>();

        uint8_t bytes[32] = { 0 };

        // Null resource creator.
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(nullptr, 32, bytes, 2, 2, 2, PIXEL_FORMAT(R8G8B8A8UIntNormalized)); });

        // Null data array.
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), 8, nullptr, 2, 2, 2, PIXEL_FORMAT(R8G8B8A8UIntNormalized)); });

        // Too-small size.
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), 32, bytes, 1, 2, 2, PIXEL_FORMAT(R8G8B8A8UIntNormalized)); });
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), 32, bytes, 2, 1, 2, PIXEL_FORMAT(R8G8B8A8UIntNormalized)); });
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), 32, bytes, 2, 2, 1, PIXEL_FORMAT(R8G8B8A8UIntNormalized)); });

        // Too-big size.
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), INT_MAX, bytes, 257, 2, 2, PIXEL_FORMAT(R8G8B8A8UIntNormalized)); });
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), INT_MAX, bytes, 2, 257, 2, PIXEL_FORMAT(R8G8B8A8UIntNormalized)); });
        ExpectHResultException(E_INVALIDARG, [&] { EffectTransferTable3D::CreateNew(device.Get(), INT_MAX, bytes, 2, 2, 257, PIXEL_FORMAT(R8G8B8A8UIntNormalized)); });

        // Invalid pixel format.
        ExpectHResultException(WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT, [&] { EffectTransferTable3D::CreateNew(device.Get(), 32, bytes, 2, 2, 2, PIXEL_FORMAT(B8G8R8A8UIntNormalized)); });
    }
};

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

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::DirectX;
using namespace Microsoft::Graphics::Canvas::Numerics;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Graphics::Imaging;
using namespace Windows::UI;
using namespace Windows::Storage::Streams;
using Platform::String;

TEST_CLASS(CanvasGeometryTests)
{
    CanvasDevice^ m_device;

public:
    CanvasGeometryTests()
        : m_device(ref new CanvasDevice())
    {
    }

    TEST_METHOD(CanvasGeometry_InteropFailAfterClosure)
    {
        auto canvasGeometry = CanvasGeometry::CreateRectangle(m_device, Rect{});

        delete canvasGeometry;

        ExpectObjectClosed([&]{ GetWrappedResource<ID2D1RectangleGeometry>(canvasGeometry); });
    }

    TEST_METHOD(CanvasGeometry_NativeInterop_Rectangle)
    {
        ComPtr<ID2D1RectangleGeometry> originalRectangleGeometry;
        ThrowIfFailed(GetD2DFactory()->CreateRectangleGeometry(D2D1::RectF(1, 2, 3, 4), &originalRectangleGeometry));

        auto canvasGeometry = GetOrCreate<CanvasGeometry>(m_device, originalRectangleGeometry.Get());

        auto retrievedRectangleGeometry = GetWrappedResource<ID2D1RectangleGeometry>(canvasGeometry);

        Assert::AreEqual(originalRectangleGeometry.Get(), retrievedRectangleGeometry.Get());

    }

    TEST_METHOD(CanvasGeometry_NativeInterop_Ellipse)
    {
        ComPtr<ID2D1EllipseGeometry> originalEllipseGeometry;
        ThrowIfFailed(GetD2DFactory()->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(1, 2), 3, 4), &originalEllipseGeometry));

        auto canvasGeometry = GetOrCreate<CanvasGeometry>(m_device, originalEllipseGeometry.Get());

        auto retrievedEllipseGeometry = GetWrappedResource<ID2D1EllipseGeometry>(canvasGeometry);

        Assert::AreEqual(originalEllipseGeometry.Get(), retrievedEllipseGeometry.Get());
    }

    TEST_METHOD(CanvasGeometry_NativeInterop_RoundedRectangle)
    {
        ComPtr<ID2D1RoundedRectangleGeometry> originalRoundedRectangleGeometry;
        ThrowIfFailed(GetD2DFactory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(1, 2, 3, 4), 5, 6), &originalRoundedRectangleGeometry));

        auto canvasGeometry = GetOrCreate<CanvasGeometry>(m_device, originalRoundedRectangleGeometry.Get());

        auto retrievedRoundedRectangleGeometry = GetWrappedResource<ID2D1RoundedRectangleGeometry>(canvasGeometry);

        Assert::AreEqual(originalRoundedRectangleGeometry.Get(), retrievedRoundedRectangleGeometry.Get());
    }

    TEST_METHOD(CanvasGeometry_NativeInterop_Path)
    {
        ComPtr<ID2D1PathGeometry> originalPathGeometry;

        GetD2DFactory()->CreatePathGeometry(&originalPathGeometry);

        auto canvasGeometry = GetOrCreate<CanvasGeometry>(m_device, originalPathGeometry.Get());

        auto retrievedPathGeometry = GetWrappedResource<ID2D1PathGeometry>(canvasGeometry);

        Assert::AreEqual(originalPathGeometry.Get(), retrievedPathGeometry.Get());
    }

    TEST_METHOD(CanvasPathBuilder_NullDevice)
    {        
        Assert::ExpectException< Platform::InvalidArgumentException^>(
            [=]
            {
                ref new CanvasPathBuilder(nullptr);
            });
    }

    TEST_METHOD(CanvasPathBuilder_NoInterop)
    {
        // 
        // PathBuilder wraps an ID2D1GeometrySink, and ID2D1PathGeometry,
        // but interop for these types should not exist.
        //
        auto canvasPathBuilder = ref new CanvasPathBuilder(m_device);
        
        Assert::ExpectException< Platform::InvalidCastException^>(
            [=]
            {
                GetWrappedResource<ID2D1GeometrySink>(canvasPathBuilder);
            });
        
        Assert::ExpectException< Platform::InvalidCastException^>(
            [=]
            {
                GetWrappedResource<ID2D1PathGeometry1>(canvasPathBuilder);
            });
    }

private:
    ComPtr<ID2D1Factory> GetD2DFactory()
    {
        auto d2dDevice = GetWrappedResource<ID2D1Device1>(m_device);

        ComPtr<ID2D1Factory> d2dFactory;
        d2dDevice->GetFactory(&d2dFactory);

        return d2dFactory;
    }
};

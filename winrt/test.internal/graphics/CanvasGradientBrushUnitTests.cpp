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
#include <CanvasLinearGradientBrush.h>
#include <CanvasRadialGradientBrush.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI;

TEST_CLASS(CanvasGradientBrushTests)
{
    struct Fixture
    {
        ComPtr<StubCanvasDevice> m_canvasDevice;
        std::shared_ptr<CanvasLinearGradientBrushManager> m_linearGradientBrushManager;
        std::shared_ptr<CanvasRadialGradientBrushManager> m_radialGradientBrushManager;

        Fixture()
        {
            m_canvasDevice = Make<StubCanvasDevice>();
            m_linearGradientBrushManager = std::make_shared<CanvasLinearGradientBrushManager>();
            m_radialGradientBrushManager = std::make_shared<CanvasRadialGradientBrushManager>();

            m_canvasDevice->MockCreateLinearGradientBrush =
                [&](ID2D1GradientStopCollection1* stopCollection)
                {
                    return Make<MockD2DLinearGradientBrush>();
                };

            m_canvasDevice->MockCreateRadialGradientBrush =
                [&](ID2D1GradientStopCollection1* stopCollection)
                {
                    return Make<MockD2DRadialGradientBrush>();
                };
        }
    };

    TEST_METHOD_EX(CanvasGradientBrush_Implements_Expected_Interfaces)
    {
        Fixture f;

        auto stopCollection = Make<MockD2DGradientStopCollection>();

        auto linearGradientBrush = f.m_linearGradientBrushManager->Create(f.m_canvasDevice.Get(), stopCollection.Get());
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ICanvasLinearGradientBrush);
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ICanvasBrush);
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ICanvasBrushInternal);
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ICanvasResourceWrapperNative);

        auto radialGradientBrush = f.m_radialGradientBrushManager->Create(f.m_canvasDevice.Get(), stopCollection.Get());
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ICanvasRadialGradientBrush);
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ICanvasBrush);
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ICanvasBrushInternal);
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ICanvasResourceWrapperNative);
    }

    template<class MOCK_TYPE, class BRUSH_MANAGER_TYPE, typename MOCK_CREATE_BRUSH_FN_TYPE>
    void TestCommonProperties(
        std::shared_ptr<BRUSH_MANAGER_TYPE> manager,
        ComPtr<StubCanvasDevice> const testDevice,
        MOCK_CREATE_BRUSH_FN_TYPE* mockCreateBrushFunction)
    {
        bool getOpacityCalled = false;
        bool setOpacityCalled = false;
        bool getTransformCalled = false;
        bool setTransformCalled = false;

        bool getGradientStopCollectionCalled = false;

        bool getStopsCalled = false;
        bool getStopCountCalled = false;
        bool getEdgeBehaviorCalled = false;
        bool getAlphaModeCalled = false;
        bool getPreInterpolationSpaceCalled = false;
        bool getPostInterpolationSpaceCalled = false;
        bool getBufferPrecisionCalled = false;

        auto mockStopCollection = Make<MockD2DGradientStopCollection>();
        
        mockStopCollection->MockGetGradientStops1 =
            [&](D2D1_GRADIENT_STOP* gradientStops, UINT32 gradientStopsCount)
            {
                Assert::IsFalse(getStopsCalled);
                getStopsCalled = true;
                Assert::AreEqual(3u, gradientStopsCount);
                gradientStops[0].color = D2D1::ColorF(1, 0, 0);
                gradientStops[0].position = 0.4f;
                gradientStops[1].color = D2D1::ColorF(0, 1, 0);
                gradientStops[1].position = 0.5f;
                gradientStops[2].color = D2D1::ColorF(0, 0, 1);
                gradientStops[2].position = 0.6f;
            };
        mockStopCollection->MockGetGradientStopCount =
            [&]()
            {
                Assert::IsFalse(getStopCountCalled);
                getStopCountCalled = true;
                return 3;
            };
        mockStopCollection->MockGetExtendMode =
            [&]()
            {
                Assert::IsFalse(getEdgeBehaviorCalled);
                getEdgeBehaviorCalled = true;
                return D2D1_EXTEND_MODE_WRAP;
            };
        mockStopCollection->MockGetColorInterpolationMode =
            [&]()
            {
                Assert::IsFalse(getAlphaModeCalled);
                getAlphaModeCalled = true;
                return D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT;
            };
        mockStopCollection->MockGetPreInterpolationSpace =
            [&]()
            {
                Assert::IsFalse(getPreInterpolationSpaceCalled);
                getPreInterpolationSpaceCalled = true;
                return D2D1_COLOR_SPACE_SCRGB;
            };
        mockStopCollection->MockGetPostInterpolationSpace =
            [&]()
            {
                Assert::IsFalse(getPostInterpolationSpaceCalled);
                getPostInterpolationSpaceCalled = true;
                return D2D1_COLOR_SPACE_SCRGB;
            };
        mockStopCollection->MockGetBufferPrecision =
            [&]()
            {
                Assert::IsFalse(getBufferPrecisionCalled);
                getBufferPrecisionCalled = true;
                return D2D1_BUFFER_PRECISION_32BPC_FLOAT;
            };
        
        (*mockCreateBrushFunction) =
            [&](ID2D1GradientStopCollection1* stopCollection)
            {
                auto brush = Make<MOCK_TYPE>();

                brush->MockGetGradientStopCollection =
                    [&](ID2D1GradientStopCollection** stopCollection)
                    {
                        Assert::IsFalse(getGradientStopCollectionCalled);
                        getGradientStopCollectionCalled = true;
                        mockStopCollection.CopyTo(stopCollection);
                    };
                brush->MockGetOpacity =
                    [&]()
                    {
                        Assert::IsFalse(getOpacityCalled);
                        getOpacityCalled = true;
                        return 13.0f;
                    };
                brush->MockSetOpacity =
                    [&](float o)
                    {
                        Assert::IsFalse(setOpacityCalled);
                        setOpacityCalled = true;
                        Assert::AreEqual(14.0f, o);
                    };
                brush->MockGetTransform =
                    [&](D2D1_MATRIX_3X2_F* matrix)
                    {
                        Assert::IsFalse(getTransformCalled);
                        getTransformCalled = true;
                        *matrix = D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6);
                    };
                brush->MockSetTransform =
                    [&](D2D1_MATRIX_3X2_F const* matrix)
                    {
                        Assert::IsFalse(setTransformCalled);
                        setTransformCalled = true;
                        D2D1_MATRIX_3X2_F expected = D2D1::Matrix3x2F(12, 34, 56, 78, 90, 12);
                        Assert::AreEqual(expected, *matrix);
                    };

                return brush;
            };

        uint32_t i;
        float f;
        Numerics::Matrix3x2 m;
        CanvasGradientStop* stops;
        CanvasEdgeBehavior edgeBehavior;
        CanvasAlphaMode alpha;
        CanvasColorSpace colorSpace;
        CanvasBufferPrecision bufferPrecision;

        auto brush = manager->Create(testDevice.Get(), mockStopCollection.Get());

        ThrowIfFailed(brush->get_EdgeBehavior(&edgeBehavior));
        Assert::AreEqual(CanvasEdgeBehavior::Wrap, edgeBehavior);
        Assert::IsTrue(getGradientStopCollectionCalled);
        Assert::IsTrue(getEdgeBehaviorCalled);
        getGradientStopCollectionCalled = false;

        ThrowIfFailed(brush->get_AlphaMode(&alpha));
        Assert::AreEqual(CanvasAlphaMode::Straight, alpha);
        Assert::IsTrue(getGradientStopCollectionCalled);
        Assert::IsTrue(getAlphaModeCalled);
        getGradientStopCollectionCalled = false;

        ThrowIfFailed(brush->get_PreInterpolationSpace(&colorSpace));
        Assert::AreEqual(CanvasColorSpace::ScRgb, colorSpace);
        Assert::IsTrue(getGradientStopCollectionCalled);
        Assert::IsTrue(getPreInterpolationSpaceCalled);
        getGradientStopCollectionCalled = false;

        ThrowIfFailed(brush->get_PostInterpolationSpace(&colorSpace));
        Assert::AreEqual(CanvasColorSpace::ScRgb, colorSpace);
        Assert::IsTrue(getGradientStopCollectionCalled);
        Assert::IsTrue(getPostInterpolationSpaceCalled);
        getGradientStopCollectionCalled = false;

        ThrowIfFailed(brush->get_BufferPrecision(&bufferPrecision));
        Assert::AreEqual(CanvasBufferPrecision::Precision32Float, bufferPrecision);
        Assert::IsTrue(getGradientStopCollectionCalled);
        Assert::IsTrue(getBufferPrecisionCalled);
        getGradientStopCollectionCalled = false;

        ThrowIfFailed(brush->get_Stops(&i, &stops));
        Assert::AreEqual(3u, i);
        Assert::AreEqual(Color{ 255, 255, 0, 0 }, stops[0].Color);
        Assert::AreEqual(0.4f, stops[0].Position);
        Assert::AreEqual(Color{ 255, 0, 255, 0 }, stops[1].Color);
        Assert::AreEqual(0.5f, stops[1].Position);
        Assert::AreEqual(Color{ 255, 0, 0, 255 }, stops[2].Color);
        Assert::AreEqual(0.6f, stops[2].Position);
        Assert::IsTrue(getGradientStopCollectionCalled);
        Assert::IsTrue(getStopsCalled);

        ThrowIfFailed(brush->get_Opacity(&f));
        Assert::AreEqual(13.0f, f);
        Assert::IsTrue(getOpacityCalled);

        ThrowIfFailed(brush->put_Opacity(14.0f));
        Assert::IsTrue(setOpacityCalled);

        ThrowIfFailed(brush->get_Transform(&m));
        Numerics::Matrix3x2 expected = Numerics::Matrix3x2{ 1, 2, 3, 4, 5, 6 };
        Assert::AreEqual(expected, m);
        Assert::IsTrue(getTransformCalled);

        m = Numerics::Matrix3x2{ 12, 34, 56, 78, 90, 12 };
        ThrowIfFailed(brush->put_Transform(m));
        Assert::IsTrue(setTransformCalled);

        ComPtr<ICanvasDevice> actualDevice;
        brush->get_Device(&actualDevice);
        Assert::AreEqual<ICanvasDevice*>(testDevice.Get(), actualDevice.Get());
    }

    TEST_METHOD_EX(CanvasGradientBrush_TestCommonProperties)
    {
        Fixture f;

        auto testDevice = Make<StubCanvasDevice>();

        TestCommonProperties<MockD2DLinearGradientBrush>(
            f.m_linearGradientBrushManager,
            testDevice,
            &testDevice->MockCreateLinearGradientBrush);

        TestCommonProperties<MockD2DRadialGradientBrush>(
            f.m_radialGradientBrushManager,
            testDevice,
            &testDevice->MockCreateRadialGradientBrush);
    }

    TEST_METHOD_EX(CanvasGradientBrush_LinearGradientBrush_Properties)
    {
        Fixture f;

        auto testDevice = Make<StubCanvasDevice>();

        bool getStartPointCalled = false;
        bool setStartPointCalled = false;
        bool getEndPointCalled = false;
        bool setEndPointCalled = false;

        testDevice->MockCreateLinearGradientBrush =
            [&](ID2D1GradientStopCollection1* stopCollection)
            {
                auto brush = Make<MockD2DLinearGradientBrush>();
                brush->MockGetStartPoint =
                    [&]()
                    {
                        Assert::IsFalse(getStartPointCalled);
                        getStartPointCalled = true;
                        return D2D1::Point2F(1, 2);
                    };
                brush->MockSetStartPoint =
                    [&](D2D1_POINT_2F pt)
                    {
                        Assert::IsFalse(setStartPointCalled);
                        setStartPointCalled = true;
                        Assert::AreEqual(3.0f, pt.x);
                        Assert::AreEqual(4.0f, pt.y);
                    };
                brush->MockGetEndPoint =
                    [&]()
                    {
                        Assert::IsFalse(getEndPointCalled);
                        getEndPointCalled = true;
                        return D2D1::Point2F(5, 6);
                    };
                brush->MockSetEndPoint =
                    [&](D2D1_POINT_2F pt)
                    {
                        Assert::IsFalse(setEndPointCalled);
                        setEndPointCalled = true;
                        Assert::AreEqual(7.0f, pt.x);
                        Assert::AreEqual(8.0f, pt.y);
                    };

                return brush;
            };

        auto stopCollection = Make<MockD2DGradientStopCollection>();
        auto linearGradientBrush = f.m_linearGradientBrushManager->Create(testDevice.Get(), stopCollection.Get());

        Numerics::Vector2 v;

        ThrowIfFailed(linearGradientBrush->get_StartPoint(&v));
        Assert::AreEqual(1.0f, v.X);
        Assert::AreEqual(2.0f, v.Y);
        Assert::IsTrue(getStartPointCalled);

        v = Numerics::Vector2{3.0f, 4.0f};
        ThrowIfFailed(linearGradientBrush->put_StartPoint(v));
        Assert::IsTrue(setStartPointCalled);

        ThrowIfFailed(linearGradientBrush->get_EndPoint(&v));
        Assert::AreEqual(5.0f, v.X);
        Assert::AreEqual(6.0f, v.Y);
        Assert::IsTrue(getEndPointCalled);

        v = Numerics::Vector2{ 7.0f, 8.0f };
        ThrowIfFailed(linearGradientBrush->put_EndPoint(v));
        Assert::IsTrue(setEndPointCalled);
    }

    TEST_METHOD_EX(CanvasGradientBrush_RadialGradientBrush_Properties)
    {
        Fixture f;

        auto testDevice = Make<StubCanvasDevice>();

        bool getCenterCalled = false;
        bool setCenterCalled = false;
        bool getOriginOffsetCalled = false;
        bool setOriginOffsetCalled = false;
        bool getRadiusXCalled = false;
        bool setRadiusXCalled = false;
        bool getRadiusYCalled = false;
        bool setRadiusYCalled = false;

        testDevice->MockCreateRadialGradientBrush =
            [&](ID2D1GradientStopCollection1* stopCollection)
        {
            auto brush = Make<MockD2DRadialGradientBrush>();
            brush->MockGetCenter =
                [&]()
                {
                    Assert::IsFalse(getCenterCalled);
                    getCenterCalled = true;
                    return D2D1::Point2F(1, 2);
                };
            brush->MockSetCenter =
                [&](D2D1_POINT_2F pt)
                {
                    Assert::IsFalse(setCenterCalled);
                    setCenterCalled = true;
                    Assert::AreEqual(3.0f, pt.x);
                    Assert::AreEqual(4.0f, pt.y);
                };
            brush->MockGetGradientOriginOffset =
                [&]()
                {
                    Assert::IsFalse(getOriginOffsetCalled);
                    getOriginOffsetCalled = true;
                    return D2D1::Point2F(5, 6);
                };
            brush->MockSetGradientOriginOffset =
                [&](D2D1_POINT_2F pt)
                {
                    Assert::IsFalse(setOriginOffsetCalled);
                    setOriginOffsetCalled = true;
                    Assert::AreEqual(7.0f, pt.x);
                    Assert::AreEqual(8.0f, pt.y);
                };
            brush->MockGetRadiusX =
                [&]()
                {
                    Assert::IsFalse(getRadiusXCalled);
                    getRadiusXCalled = true;
                    return 9.0f;
                };
            brush->MockSetRadiusX =
                [&](float r)
                {
                    Assert::IsFalse(setRadiusXCalled);
                    setRadiusXCalled = true;
                    Assert::AreEqual(10.0f, r);
                };
            brush->MockGetRadiusY =
                [&]()
                {
                    Assert::IsFalse(getRadiusYCalled);
                    getRadiusYCalled = true;
                    return 11.0f;
                };
            brush->MockSetRadiusY =
                [&](float r)
                {
                    Assert::IsFalse(setRadiusYCalled);
                    setRadiusYCalled = true;
                    Assert::AreEqual(12.0f, r);
                };

            return brush;
        };

        auto stopCollection = Make<MockD2DGradientStopCollection>();
        auto radialGradientBrush = f.m_radialGradientBrushManager->Create(testDevice.Get(), stopCollection.Get());

        Numerics::Vector2 v;

        ThrowIfFailed(radialGradientBrush->get_Center(&v));
        Assert::AreEqual(1.0f, v.X);
        Assert::AreEqual(2.0f, v.Y);
        Assert::IsTrue(getCenterCalled);

        v = Numerics::Vector2{ 3.0f, 4.0f };
        ThrowIfFailed(radialGradientBrush->put_Center(v));
        Assert::IsTrue(setCenterCalled);

        ThrowIfFailed(radialGradientBrush->get_OriginOffset(&v));
        Assert::AreEqual(5.0f, v.X);
        Assert::AreEqual(6.0f, v.Y);
        Assert::IsTrue(getOriginOffsetCalled);

        v = Numerics::Vector2{ 7.0f, 8.0f };
        ThrowIfFailed(radialGradientBrush->put_OriginOffset(v));
        Assert::IsTrue(setOriginOffsetCalled);

        float fv;

        ThrowIfFailed(radialGradientBrush->get_RadiusX(&fv));
        Assert::AreEqual(9.0f, fv);
        Assert::IsTrue(getRadiusXCalled);

        ThrowIfFailed(radialGradientBrush->put_RadiusX(10.0f));
        Assert::IsTrue(setRadiusXCalled);

        ThrowIfFailed(radialGradientBrush->get_RadiusY(&fv));
        Assert::AreEqual(11.0f, fv);
        Assert::IsTrue(getRadiusYCalled);

        ThrowIfFailed(radialGradientBrush->put_RadiusY(12.0f));
        Assert::IsTrue(setRadiusYCalled);
    }

    TEST_METHOD_EX(CanvasGradientBrush_NullArgs)
    {
        Fixture f;

        UINT i;
        CanvasGradientStop* stop;

        auto stopCollection = Make<MockD2DGradientStopCollection>();

        auto linearGradientBrush = f.m_linearGradientBrushManager->Create(f.m_canvasDevice.Get(), stopCollection.Get());

        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_StartPoint(nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_EndPoint(nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_Stops(&i, nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_Stops(nullptr, &stop));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_Stops(nullptr, nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_EdgeBehavior(nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_AlphaMode(nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_PreInterpolationSpace(nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_PostInterpolationSpace(nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_BufferPrecision(nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_Opacity(nullptr));
        Assert::AreEqual(E_INVALIDARG, linearGradientBrush->get_Transform(nullptr));

        auto radialGradientBrush = f.m_radialGradientBrushManager->Create(f.m_canvasDevice.Get(), stopCollection.Get());

        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_Center(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_OriginOffset(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_RadiusX(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_RadiusY(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_Stops(&i, nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_Stops(nullptr, &stop));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_Stops(nullptr, nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_EdgeBehavior(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_AlphaMode(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_PreInterpolationSpace(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_PostInterpolationSpace(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_BufferPrecision(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_Opacity(nullptr));
        Assert::AreEqual(E_INVALIDARG, radialGradientBrush->get_Transform(nullptr));
    }

    TEST_METHOD_EX(CanvasGradientBrush_Closed)
    {
        Fixture f;

        auto stopCollection = Make<MockD2DGradientStopCollection>();
        Numerics::Vector2 v;
        float fv;
        Numerics::Matrix3x2 m;
        UINT i;
        CanvasGradientStop* stops;
        CanvasEdgeBehavior edgeBehavior;
        CanvasAlphaMode alpha;
        CanvasColorSpace colorSpace;
        CanvasBufferPrecision bufferPrecision;
        ComPtr<ICanvasDevice> actualDevice;

        auto linearGradientBrush = f.m_linearGradientBrushManager->Create(f.m_canvasDevice.Get(), stopCollection.Get());
        ThrowIfFailed(linearGradientBrush->Close());

        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_StartPoint(&v));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->put_StartPoint(v));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_EndPoint(&v));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->put_EndPoint(v));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_Stops(&i, &stops));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_EdgeBehavior(&edgeBehavior));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_AlphaMode(&alpha));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_PreInterpolationSpace(&colorSpace));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_PostInterpolationSpace(&colorSpace));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_BufferPrecision(&bufferPrecision));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_Opacity(&fv));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->put_Opacity(fv));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_Transform(&m));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->put_Transform(m));
        Assert::AreEqual(RO_E_CLOSED, linearGradientBrush->get_Device(&actualDevice));

        auto radialGradientBrush = f.m_radialGradientBrushManager->Create(f.m_canvasDevice.Get(), stopCollection.Get());
        ThrowIfFailed(radialGradientBrush->Close());

        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_Center(&v));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->put_Center(v));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_OriginOffset(&v));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->put_OriginOffset(v));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_RadiusX(&fv));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->put_RadiusX(fv));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_RadiusY(&fv));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->put_RadiusY(fv));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_Stops(&i, &stops));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_EdgeBehavior(&edgeBehavior));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_AlphaMode(&alpha));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_PreInterpolationSpace(&colorSpace));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_PostInterpolationSpace(&colorSpace));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_BufferPrecision(&bufferPrecision));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_Opacity(&fv));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->put_Opacity(fv));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_Transform(&m));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->put_Transform(m));
        Assert::AreEqual(RO_E_CLOSED, radialGradientBrush->get_Device(&actualDevice));
    }

    TEST_METHOD_EX(CanvasGradientBrush_CreateStops_Overload)
    {
        auto device = Make<StubCanvasDevice>();

        device->MockCreateGradientStopCollection =
            [&](UINT gradientStopCount,
            CanvasGradientStop const* gradientStops,
            CanvasEdgeBehavior edgeBehavior,
            CanvasColorSpace preInterpolationSpace,
            CanvasColorSpace postInterpolationSpace,
            CanvasBufferPrecision bufferPrecision,
            CanvasAlphaMode alphaMode)
            {
                Assert::AreEqual(CanvasEdgeBehavior::Clamp, edgeBehavior);
                Assert::AreEqual(CanvasAlphaMode::Premultiplied, alphaMode);

                auto stopCollection = Make<MockD2DGradientStopCollection>();
                return stopCollection;
            };
        
        device->MockCreateLinearGradientBrush =
            [&](ID2D1GradientStopCollection1* stopCollection)
            {
                return Make<MockD2DLinearGradientBrush>();
            };
        
        device->MockCreateRadialGradientBrush =
            [&](ID2D1GradientStopCollection1* stopCollection)
            {
                return Make<MockD2DRadialGradientBrush>();
            };

        CanvasGradientStop stop{};

        auto linearGradientBrushFactory = Make<CanvasLinearGradientBrushFactory>();
        ComPtr<ICanvasLinearGradientBrush> linearGradientBrush;
        Assert::AreEqual(S_OK, linearGradientBrushFactory->CreateWithStops(device.Get(), 1, &stop, &linearGradientBrush));

        auto radialGradientBrushFactory = Make<CanvasRadialGradientBrushFactory>();
        ComPtr<ICanvasRadialGradientBrush> radialGradientBrush;
        Assert::AreEqual(S_OK, radialGradientBrushFactory->CreateWithStops(device.Get(), 1, &stop, &radialGradientBrush));
    }
};

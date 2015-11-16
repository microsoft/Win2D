// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/brushes/CanvasLinearGradientBrush.h>
#include <lib/brushes/CanvasRadialGradientBrush.h>
#include <lib/brushes/Gradients.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI;

TEST_CLASS(CanvasGradientBrushTests)
{
    struct Fixture
    {
        ComPtr<StubCanvasDevice> m_canvasDevice;

        Fixture()
        {
            m_canvasDevice = Make<StubCanvasDevice>();

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

        auto linearGradientBrush = CanvasLinearGradientBrush::CreateNew(f.m_canvasDevice.Get(), stopCollection.Get());
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ICanvasLinearGradientBrush);
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ICanvasBrush);
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ICanvasBrushInternal);
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(linearGradientBrush, ICanvasResourceWrapperNative);

        auto radialGradientBrush = CanvasRadialGradientBrush::CreateNew(f.m_canvasDevice.Get(), stopCollection.Get());
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ICanvasRadialGradientBrush);
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ICanvasBrush);
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ICanvasBrushInternal);
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(radialGradientBrush, ICanvasResourceWrapperNative);
    }

    template<class BRUSH_TYPE, class MOCK_TYPE, typename MOCK_CREATE_BRUSH_FN_TYPE>
    void TestCommonProperties(
        ComPtr<StubCanvasDevice> const testDevice,
        MOCK_CREATE_BRUSH_FN_TYPE* mockCreateBrushFunction)
    {
        auto mockStopCollection = Make<MockD2DGradientStopCollection>();
        
        mockStopCollection->GetGradientStops1Method.SetExpectedCalls(1,
            [] (D2D1_GRADIENT_STOP* gradientStops, UINT32 gradientStopsCount)
            {
                Assert::AreEqual(3u, gradientStopsCount);
                gradientStops[0].color = D2D1::ColorF(1, 0, 0);
                gradientStops[0].position = 0.4f;
                gradientStops[1].color = D2D1::ColorF(0, 1, 0);
                gradientStops[1].position = 0.5f;
                gradientStops[2].color = D2D1::ColorF(0, 0, 1);
                gradientStops[2].position = 0.6f;
            });
        mockStopCollection->GetGradientStopCountMethod.SetExpectedCalls(1,      [] { return 3; });
        mockStopCollection->GetExtendModeMethod.SetExpectedCalls(1,             [] { return D2D1_EXTEND_MODE_WRAP; });
        mockStopCollection->GetColorInterpolationModeMethod.SetExpectedCalls(1, [] { return D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT; });
        mockStopCollection->GetPreInterpolationSpaceMethod.SetExpectedCalls(1,  [] { return D2D1_COLOR_SPACE_SCRGB; });
        mockStopCollection->GetPostInterpolationSpaceMethod.SetExpectedCalls(1, [] { return D2D1_COLOR_SPACE_SCRGB; });
        mockStopCollection->GetBufferPrecisionMethod.SetExpectedCalls(1,        [] { return D2D1_BUFFER_PRECISION_32BPC_FLOAT; });
        
        (*mockCreateBrushFunction) =
            [&](ID2D1GradientStopCollection1* stopCollection)
            {
                auto brush = Make<MOCK_TYPE>();

                brush->GetGradientStopCollectionMethod.SetExpectedCalls(6,
                    [=] (ID2D1GradientStopCollection** stopCollection)
                    {
                        mockStopCollection.CopyTo(stopCollection);
                    });
                
                brush->GetOpacityMethod.SetExpectedCalls(1, [] { return 13.0f; });
                brush->SetOpacityMethod.SetExpectedCalls(1,
                    [] (float o)
                    {
                        Assert::AreEqual(14.0f, o);
                    });
                
                brush->GetTransformMethod.SetExpectedCalls(1,
                    [] (D2D1_MATRIX_3X2_F* matrix)
                    {
                        *matrix = D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6);
                    });
                brush->SetTransformMethod.SetExpectedCalls(1,
                    [] (D2D1_MATRIX_3X2_F const* matrix)
                    {
                        D2D1_MATRIX_3X2_F expected = D2D1::Matrix3x2F(12, 34, 56, 78, 90, 12);
                        Assert::AreEqual(expected, *matrix);
                    });

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

        auto brush = BRUSH_TYPE::CreateNew(testDevice.Get(), mockStopCollection.Get());

        ThrowIfFailed(brush->get_EdgeBehavior(&edgeBehavior));
        Assert::AreEqual(CanvasEdgeBehavior::Wrap, edgeBehavior);

        ThrowIfFailed(brush->get_AlphaMode(&alpha));
        Assert::AreEqual(CanvasAlphaMode::Straight, alpha);

        ThrowIfFailed(brush->get_PreInterpolationSpace(&colorSpace));
        Assert::AreEqual(CanvasColorSpace::ScRgb, colorSpace);

        ThrowIfFailed(brush->get_PostInterpolationSpace(&colorSpace));
        Assert::AreEqual(CanvasColorSpace::ScRgb, colorSpace);

        ThrowIfFailed(brush->get_BufferPrecision(&bufferPrecision));
        Assert::AreEqual(CanvasBufferPrecision::Precision32Float, bufferPrecision);

        ThrowIfFailed(brush->get_Stops(&i, &stops));
        Assert::AreEqual(3u, i);
        Assert::AreEqual(Color{ 255, 255, 0, 0 }, stops[0].Color);
        Assert::AreEqual(0.4f, stops[0].Position);
        Assert::AreEqual(Color{ 255, 0, 255, 0 }, stops[1].Color);
        Assert::AreEqual(0.5f, stops[1].Position);
        Assert::AreEqual(Color{ 255, 0, 0, 255 }, stops[2].Color);
        Assert::AreEqual(0.6f, stops[2].Position);

        ThrowIfFailed(brush->get_Opacity(&f));
        Assert::AreEqual(13.0f, f);

        ThrowIfFailed(brush->put_Opacity(14.0f));

        ThrowIfFailed(brush->get_Transform(&m));
        Numerics::Matrix3x2 expected = Numerics::Matrix3x2{ 1, 2, 3, 4, 5, 6 };
        Assert::AreEqual(expected, m);

        m = Numerics::Matrix3x2{ 12, 34, 56, 78, 90, 12 };
        ThrowIfFailed(brush->put_Transform(m));

        ComPtr<ICanvasDevice> actualDevice;
        brush->get_Device(&actualDevice);
        Assert::AreEqual<ICanvasDevice*>(testDevice.Get(), actualDevice.Get());
    }

    TEST_METHOD_EX(CanvasGradientBrush_TestCommonProperties)
    {
        Fixture f;

        auto testDevice = Make<StubCanvasDevice>();

        TestCommonProperties<CanvasLinearGradientBrush, MockD2DLinearGradientBrush>(
            testDevice,
            &testDevice->MockCreateLinearGradientBrush);

        TestCommonProperties<CanvasRadialGradientBrush, MockD2DRadialGradientBrush>(
            testDevice,
            &testDevice->MockCreateRadialGradientBrush);
    }

    TEST_METHOD_EX(CanvasGradientBrush_LinearGradientBrush_Properties)
    {
        Fixture f;

        auto testDevice = Make<StubCanvasDevice>();

        testDevice->MockCreateLinearGradientBrush =
            [&](ID2D1GradientStopCollection1* stopCollection)
            {
                auto brush = Make<MockD2DLinearGradientBrush>();
                brush->GetStartPointMethod.SetExpectedCalls(1, [] { return D2D1::Point2F(1, 2); });
                brush->SetStartPointMethod.SetExpectedCalls(1,
                    [] (D2D1_POINT_2F pt)
                    {
                        Assert::AreEqual(3.0f, pt.x);
                        Assert::AreEqual(4.0f, pt.y);
                    });

                brush->GetEndPointMethod.SetExpectedCalls(1, [] { return D2D1::Point2F(5, 6); });
                brush->SetEndPointMethod.SetExpectedCalls(1,
                    [] (D2D1_POINT_2F pt)
                    {
                        Assert::AreEqual(7.0f, pt.x);
                        Assert::AreEqual(8.0f, pt.y);
                    });

                return brush;
            };

        auto stopCollection = Make<MockD2DGradientStopCollection>();
        auto linearGradientBrush = CanvasLinearGradientBrush::CreateNew(testDevice.Get(), stopCollection.Get());

        Numerics::Vector2 v;

        ThrowIfFailed(linearGradientBrush->get_StartPoint(&v));
        Assert::AreEqual(1.0f, v.X);
        Assert::AreEqual(2.0f, v.Y);

        v = Numerics::Vector2{3.0f, 4.0f};
        ThrowIfFailed(linearGradientBrush->put_StartPoint(v));

        ThrowIfFailed(linearGradientBrush->get_EndPoint(&v));
        Assert::AreEqual(5.0f, v.X);
        Assert::AreEqual(6.0f, v.Y);

        v = Numerics::Vector2{ 7.0f, 8.0f };
        ThrowIfFailed(linearGradientBrush->put_EndPoint(v));
    }

    TEST_METHOD_EX(CanvasGradientBrush_RadialGradientBrush_Properties)
    {
        Fixture f;

        auto testDevice = Make<StubCanvasDevice>();

        testDevice->MockCreateRadialGradientBrush = [&] (ID2D1GradientStopCollection1* stopCollection)
        {
            auto brush = Make<MockD2DRadialGradientBrush>();
            brush->GetCenterMethod.SetExpectedCalls(1, [] { return D2D1::Point2F(1, 2); });
            brush->SetCenterMethod.SetExpectedCalls(1,
                [] (D2D1_POINT_2F pt)
                {
                    Assert::AreEqual(3.0f, pt.x);
                    Assert::AreEqual(4.0f, pt.y);
                });

            brush->GetGradientOriginOffsetMethod.SetExpectedCalls(1, [] { return D2D1::Point2F(5, 6); });
            brush->SetGradientOriginOffsetMethod.SetExpectedCalls(1,
                [] (D2D1_POINT_2F pt)
                {
                    Assert::AreEqual(7.0f, pt.x);
                    Assert::AreEqual(8.0f, pt.y);
                });

            brush->GetRadiusXMethod.SetExpectedCalls(1, [] { return 9.0f; });
            brush->SetRadiusXMethod.SetExpectedCalls(1,
                [] (float r) 
                {
                    Assert::AreEqual(10.0f, r);
                });
            
            brush->GetRadiusYMethod.SetExpectedCalls(1, [] { return 11.0f; });
            brush->SetRadiusYMethod.SetExpectedCalls(1,
                [] (float r)
                {
                    Assert::AreEqual(12.0f, r);
                });

            return brush;
        };

        auto stopCollection = Make<MockD2DGradientStopCollection>();
        auto radialGradientBrush = CanvasRadialGradientBrush::CreateNew(testDevice.Get(), stopCollection.Get());

        Numerics::Vector2 v;

        ThrowIfFailed(radialGradientBrush->get_Center(&v));
        Assert::AreEqual(1.0f, v.X);
        Assert::AreEqual(2.0f, v.Y);

        v = Numerics::Vector2{ 3.0f, 4.0f };
        ThrowIfFailed(radialGradientBrush->put_Center(v));

        ThrowIfFailed(radialGradientBrush->get_OriginOffset(&v));
        Assert::AreEqual(5.0f, v.X);
        Assert::AreEqual(6.0f, v.Y);

        v = Numerics::Vector2{ 7.0f, 8.0f };
        ThrowIfFailed(radialGradientBrush->put_OriginOffset(v));

        float fv;

        ThrowIfFailed(radialGradientBrush->get_RadiusX(&fv));
        Assert::AreEqual(9.0f, fv);

        ThrowIfFailed(radialGradientBrush->put_RadiusX(10.0f));

        ThrowIfFailed(radialGradientBrush->get_RadiusY(&fv));
        Assert::AreEqual(11.0f, fv);

        ThrowIfFailed(radialGradientBrush->put_RadiusY(12.0f));
    }

    TEST_METHOD_EX(CanvasGradientBrush_NullArgs)
    {
        Fixture f;

        UINT i;
        CanvasGradientStop* stop;

        auto stopCollection = Make<MockD2DGradientStopCollection>();

        auto linearGradientBrush = CanvasLinearGradientBrush::CreateNew(f.m_canvasDevice.Get(), stopCollection.Get());

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

        auto radialGradientBrush = CanvasRadialGradientBrush::CreateNew(f.m_canvasDevice.Get(), stopCollection.Get());

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

        auto linearGradientBrush = CanvasLinearGradientBrush::CreateNew(f.m_canvasDevice.Get(), stopCollection.Get());
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

        auto radialGradientBrush = CanvasRadialGradientBrush::CreateNew(f.m_canvasDevice.Get(), stopCollection.Get());
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
            [] (std::vector<D2D1_GRADIENT_STOP>&&,
                D2D1_COLOR_SPACE,
                D2D1_COLOR_SPACE,
                D2D1_BUFFER_PRECISION,
                D2D1_EXTEND_MODE extendMode,
                D2D1_COLOR_INTERPOLATION_MODE colorInterpolationMode)
            {
                Assert::AreEqual(D2D1_EXTEND_MODE_CLAMP, extendMode);
                Assert::AreEqual<uint32_t>(D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED, colorInterpolationMode);
                
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

    struct Linear
    {
        typedef CanvasLinearGradientBrushFactory factory_t;
        typedef CanvasLinearGradientBrush brush_t;
        typedef ICanvasLinearGradientBrush ibrush_t;
        typedef ID2D1LinearGradientBrush d2dBrush_t;
        typedef MockD2DLinearGradientBrush mockD2DBrush_t;

        static ComPtr<MockD2DLinearGradientBrush> ExpectCreateBrush(
            ComPtr<MockD2DDeviceContext> const& d2dDeviceContext, 
            ComPtr<MockD2DGradientStopCollection> const& expectedCollection)
        {
            auto brush = Make<MockD2DLinearGradientBrush>();

            d2dDeviceContext->CreateLinearGradientBrushMethod.SetExpectedCalls(1,
                [=] (D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES const* gradientProperties, 
                     const D2D1_BRUSH_PROPERTIES* properties, ID2D1GradientStopCollection* collection, 
                     ID2D1LinearGradientBrush** value)
                {
                    Assert::AreEqual(D2D1_POINT_2F{}, gradientProperties->startPoint);
                    Assert::AreEqual(D2D1_POINT_2F{}, gradientProperties->endPoint);
                    Assert::IsNull(properties);
                    Assert::IsTrue(IsSameInstance(expectedCollection.Get(), collection));
                    return brush.CopyTo(value);
                });

            return brush;
        }
    };

    struct Radial
    {
        typedef CanvasRadialGradientBrushFactory factory_t;
        typedef CanvasRadialGradientBrush brush_t;
        typedef ICanvasRadialGradientBrush ibrush_t;
        typedef ID2D1RadialGradientBrush d2dBrush_t;
        typedef MockD2DRadialGradientBrush mockD2DBrush_t;

        static ComPtr<MockD2DRadialGradientBrush> ExpectCreateBrush(
            ComPtr<MockD2DDeviceContext> const& d2dDeviceContext, 
            ComPtr<MockD2DGradientStopCollection> const& expectedCollection)
        {
            auto brush = Make<MockD2DRadialGradientBrush>();

            d2dDeviceContext->CreateRadialGradientBrushMethod.SetExpectedCalls(1,
                [=] (D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES const* gradientProperties, 
                     const D2D1_BRUSH_PROPERTIES* properties, ID2D1GradientStopCollection* collection, 
                     ID2D1RadialGradientBrush** value)
                {
                    Assert::AreEqual(D2D1_POINT_2F{}, gradientProperties->center);
                    Assert::AreEqual(D2D1_POINT_2F{}, gradientProperties->gradientOriginOffset);
                    Assert::AreEqual(0.0f, gradientProperties->radiusX);
                    Assert::AreEqual(0.0f, gradientProperties->radiusY);
                    Assert::IsNull(properties);
                    Assert::IsTrue(IsSameInstance(expectedCollection.Get(), collection));
                    return brush.CopyTo(value);
                });

            return brush;
        }

    };

    template<typename T>
    struct FactoryFixture
    {
        ComPtr<MockD2DDeviceContext> D2DDeviceContext;
        ComPtr<MockD2DDevice> D2DDevice;
        ComPtr<StubD3D11Device> D3DDevice;
        ComPtr<CanvasDevice> Device;

        ComPtr<typename T::factory_t> Factory;
        ComPtr<typename T::ibrush_t> Brush;

        ComPtr<typename T::d2dBrush_t> ExpectedD2DBrush;

        FactoryFixture()
            : D2DDeviceContext(Make<MockD2DDeviceContext>())
            , D2DDevice(Make<MockD2DDevice>())
            , D3DDevice(Make<StubD3D11Device>())
            , Device(Make<CanvasDevice>(D2DDevice.Get(), D3DDevice.Get()))
            , Factory(Make<T::factory_t>())
        {
            D2DDevice->MockCreateDeviceContext =
                [=] (D2D1_DEVICE_CONTEXT_OPTIONS, ID2D1DeviceContext1** deviceContext)
                {
                    ThrowIfFailed(D2DDeviceContext.CopyTo(deviceContext));
                };
        }

        void ExpectCreateBrush(
            std::initializer_list<D2D1_GRADIENT_STOP> expectedStopsIl,
            D2D1_COLOR_SPACE expectedPreCS,
            D2D1_COLOR_SPACE expectedPostCS,
            D2D1_BUFFER_PRECISION expectedPrecision,
            D2D1_EXTEND_MODE expectedExtendMode,
            D2D1_COLOR_INTERPOLATION_MODE expectedInterpMode)
        {
            auto collection = Make<MockD2DGradientStopCollection>();

            std::vector<D2D1_GRADIENT_STOP> expectedStops(expectedStopsIl);

            D2DDeviceContext->CreateGradientStopCollectionMethod.SetExpectedCalls(1,
                [=] (D2D1_GRADIENT_STOP const* stops, uint32_t stopCount, 
                     D2D1_COLOR_SPACE preCS, D2D1_COLOR_SPACE postCS, 
                     D2D1_BUFFER_PRECISION precision, D2D1_EXTEND_MODE extendMode, 
                     D2D1_COLOR_INTERPOLATION_MODE interpMode, ID2D1GradientStopCollection1** result)
                {
                    Assert::AreEqual((uint32_t)std::distance(expectedStops.begin(), expectedStops.end()), stopCount);

                    auto stop = stops;
                    for (auto it = expectedStops.begin(); it != expectedStops.end(); ++it, ++stop)
                    {
                        Assert::AreEqual(it->position, stop->position);
                        Assert::AreEqual(it->color.r, stop->color.r);
                        Assert::AreEqual(it->color.g, stop->color.g);
                        Assert::AreEqual(it->color.b, stop->color.b);
                        Assert::AreEqual(it->color.a, stop->color.a);
                    }

                    Assert::AreEqual<uint32_t>(expectedPreCS, preCS);
                    Assert::AreEqual<uint32_t>(expectedPostCS, postCS);
                    Assert::AreEqual<uint32_t>(expectedPrecision, precision);
                    Assert::AreEqual<uint32_t>(expectedExtendMode, extendMode);
                    Assert::AreEqual<uint32_t>(expectedInterpMode, interpMode);

                    return collection.CopyTo(result);
                });

            ExpectedD2DBrush = T::ExpectCreateBrush(D2DDeviceContext, collection);
        }

        void Validate()
        {
            Assert::IsNotNull(Brush.Get());
            auto wrappedBrush = GetWrappedResource<T::d2dBrush_t>(Brush);
            Assert::IsTrue(IsSameInstance(ExpectedD2DBrush.Get(), wrappedBrush.Get()));
        }
    };

    template<typename T>
    static void TestCreateBadParams()
    {
        FactoryFixture<T> f;        
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateSimple(nullptr,        Color{}, Color{}, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateSimple(f.Device.Get(), Color{}, Color{}, nullptr));

        CanvasGradientStop stops[3];
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithStops(nullptr,        _countof(stops),   stops, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithStops(f.Device.Get(), _countof(stops), nullptr, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithStops(f.Device.Get(), _countof(stops),   stops,  nullptr));

        auto extend = CanvasEdgeBehavior::Clamp;
        auto alphaMode = CanvasAlphaMode::Premultiplied;
        
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithEdgeBehaviorAndAlphaMode(nullptr,        _countof(stops),   stops, extend, alphaMode, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithEdgeBehaviorAndAlphaMode(f.Device.Get(), _countof(stops), nullptr, extend, alphaMode, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithEdgeBehaviorAndAlphaMode(f.Device.Get(), _countof(stops),   stops, extend, alphaMode, nullptr));

        auto cs = CanvasColorSpace::Srgb;
        auto precision = CanvasBufferPrecision::Precision8UIntNormalized;

        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithEdgeBehaviorAndInterpolationOptions(nullptr,        _countof(stops),   stops, extend, alphaMode, cs, cs, precision, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithEdgeBehaviorAndInterpolationOptions(f.Device.Get(), _countof(stops), nullptr, extend, alphaMode, cs, cs, precision, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithEdgeBehaviorAndInterpolationOptions(f.Device.Get(), _countof(stops),   stops, extend, alphaMode, cs, cs, precision, nullptr));
    }

    template<typename T>
    static void TestCreateHdrBadParams()
    {
        FactoryFixture<T> f;
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrSimple(nullptr,        Vector4{}, Vector4{}, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrSimple(f.Device.Get(), Vector4{}, Vector4{}, nullptr));

        CanvasGradientStopHdr stops[3];
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithStops(nullptr,        _countof(stops),   stops, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithStops(f.Device.Get(), _countof(stops), nullptr, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithStops(f.Device.Get(), _countof(stops),   stops,  nullptr));

        auto extend = CanvasEdgeBehavior::Clamp;
        auto alphaMode = CanvasAlphaMode::Premultiplied;
        
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithEdgeBehaviorAndAlphaMode(nullptr,        _countof(stops),   stops, extend, alphaMode, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithEdgeBehaviorAndAlphaMode(f.Device.Get(), _countof(stops), nullptr, extend, alphaMode, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithEdgeBehaviorAndAlphaMode(f.Device.Get(), _countof(stops),   stops, extend, alphaMode, nullptr));

        auto cs = CanvasColorSpace::Srgb;
        auto precision = CanvasBufferPrecision::Precision8UIntNormalized;

        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithEdgeBehaviorAndInterpolationOptions(nullptr,        _countof(stops),   stops, extend, alphaMode, cs, cs, precision, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithEdgeBehaviorAndInterpolationOptions(f.Device.Get(), _countof(stops), nullptr, extend, alphaMode, cs, cs, precision, &f.Brush));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateHdrWithEdgeBehaviorAndInterpolationOptions(f.Device.Get(), _countof(stops),   stops, extend, alphaMode, cs, cs, precision, nullptr));
    }

    template<typename T, typename COLOR, typename FN>
    static void TestCreateSimple(FN fn)
    {
        FactoryFixture<T> f;

        auto startColor = COLOR{ 1, 2, 3, 4 };
        auto endColor   = COLOR{ 5, 6, 7, 8 };

        auto d2dStartColor = ToD2DColor(startColor);
        auto d2dEndColor   = ToD2DColor(endColor);

        f.ExpectCreateBrush(
            { 
                D2D1_GRADIENT_STOP{ 0.0f, d2dStartColor },
                D2D1_GRADIENT_STOP{ 1.0f, d2dEndColor } 
            },
            D2D1_COLOR_SPACE_SRGB,
            D2D1_COLOR_SPACE_SRGB,
            D2D1_BUFFER_PRECISION_8BPC_UNORM,
            D2D1_EXTEND_MODE_CLAMP,
            D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED);

        ThrowIfFailed((f.Factory.Get()->*fn)(f.Device.Get(), startColor, endColor, &f.Brush));

        f.Validate();
    }
    
    template<typename T>
    static void TestCreateSimple()
    {
        TestCreateSimple<T, Color>(&T::factory_t::CreateSimple);
    }

    template<typename T>
    static void TestCreateHdrSimple()
    {
        TestCreateSimple<T, Vector4>(&T::factory_t::CreateHdrSimple);
    }


    template<typename T, typename STOP>
    struct FactoryFixtureWithStops : FactoryFixture<T>
    {
        STOP Stops[3];

        FactoryFixtureWithStops(
            D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE_CLAMP,
            D2D1_COLOR_INTERPOLATION_MODE interpolationMode = D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED,
            D2D1_COLOR_SPACE preColorSpace = D2D1_COLOR_SPACE_SRGB,
            D2D1_COLOR_SPACE postColorSpace = D2D1_COLOR_SPACE_SRGB,
            D2D1_BUFFER_PRECISION precision = D2D1_BUFFER_PRECISION_8BPC_UNORM)
        {
            Stops[0] = STOP{ 0.0f, { 1,  2,  3,  4 } };
            Stops[1] = STOP{ 0.5f, { 5,  6,  7,  8 } };
            Stops[2] = STOP{ 0.9f, { 9, 10, 11, 12 } };

            ExpectCreateBrush(
                {
                    D2D1_GRADIENT_STOP{ Stops[0].Position, ToD2DColor(Stops[0].Color) },
                    D2D1_GRADIENT_STOP{ Stops[1].Position, ToD2DColor(Stops[1].Color) },
                    D2D1_GRADIENT_STOP{ Stops[2].Position, ToD2DColor(Stops[2].Color) }
                },
                preColorSpace,
                postColorSpace,
                precision,
                extendMode,
                interpolationMode);
        }
    };


    template<typename T, typename STOP, typename FN>
    static void TestCreateWithStops(FN fn)
    {
        FactoryFixtureWithStops<T, STOP> f;

        ThrowIfFailed((f.Factory.Get()->*fn)(
            f.Device.Get(),
            _countof(f.Stops),
            f.Stops,
            &f.Brush));

        f.Validate();
    }

    template<typename T>
    static void TestCreateWithStops()
    {
        TestCreateWithStops<T, CanvasGradientStop>(&T::factory_t::CreateWithStops);
    }

    template<typename T>
    static void TestCreateHdrWithStops()
    {
        TestCreateWithStops<T, CanvasGradientStopHdr>(&T::factory_t::CreateHdrWithStops);
    }

    template<typename T, typename STOP, typename FN>
    static void TestCreateWithEdgeBehaviorAndAlphaMode(FN fn)
    {
        FactoryFixtureWithStops<T, STOP> f(D2D1_EXTEND_MODE_WRAP, D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT);

        ThrowIfFailed((f.Factory.Get()->*fn)(
            f.Device.Get(),
            _countof(f.Stops),
            f.Stops,
            CanvasEdgeBehavior::Wrap,
            CanvasAlphaMode::Straight,
            &f.Brush));

        f.Validate();
    }

    template<typename T>
    static void TestCreateWithEdgeBehaviorAndAlphaMode()
    {
        TestCreateWithEdgeBehaviorAndAlphaMode<T, CanvasGradientStop>(&T::factory_t::CreateWithEdgeBehaviorAndAlphaMode);
    }

    template<typename T>
    static void TestCreateHdrWithEdgeBehaviorAndAlphaMode()
    {
        TestCreateWithEdgeBehaviorAndAlphaMode<T, CanvasGradientStopHdr>(&T::factory_t::CreateHdrWithEdgeBehaviorAndAlphaMode);
    }

    template<typename T, typename STOP, typename FN>
    static void TestCreateWithEdgeBehaviorAndInterpolationOptions(FN fn)
    {
        FactoryFixtureWithStops<T, STOP> f(D2D1_EXTEND_MODE_WRAP, D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT, D2D1_COLOR_SPACE_SCRGB, D2D1_COLOR_SPACE_CUSTOM, D2D1_BUFFER_PRECISION_32BPC_FLOAT);

        ThrowIfFailed((f.Factory.Get()->*fn)(
            f.Device.Get(),
            _countof(f.Stops),
            f.Stops,
            CanvasEdgeBehavior::Wrap,
            CanvasAlphaMode::Straight,
            CanvasColorSpace::ScRgb,
            CanvasColorSpace::Custom,
            CanvasBufferPrecision::Precision32Float,
            &f.Brush));

        f.Validate();
    }

    template<typename T>
    static void TestCreateWithEdgeBehaviorAndInterpolationOptions()
    {
        TestCreateWithEdgeBehaviorAndInterpolationOptions<T, CanvasGradientStop>(&T::factory_t::CreateWithEdgeBehaviorAndInterpolationOptions);
    }

    template<typename T>
    static void TestCreateHdrWithEdgeBehaviorAndInterpolationOptions()
    {
        TestCreateWithEdgeBehaviorAndInterpolationOptions<T, CanvasGradientStopHdr>(&T::factory_t::CreateHdrWithEdgeBehaviorAndInterpolationOptions);
    }


    template<typename T, typename STOP, typename FN>
    static void TestGetStops(FN fn)
    {
        auto stopCollection = Make<MockD2DGradientStopCollection>();                
        auto d2dBrush = Make<typename T::mockD2DBrush_t>();
        auto brush = Make<typename T::brush_t>(Make<MockCanvasDevice>().Get(), d2dBrush.Get());

        d2dBrush->GetGradientStopCollectionMethod.SetExpectedCalls(1,
            [=] (ID2D1GradientStopCollection** v)
            {
                stopCollection.CopyTo(v);
            });

        D2D1_GRADIENT_STOP d2dStops[] = {
            { 0.0f, {  1.0f,  2.0f,  3.0f,  4.0f } },
            { 0.2f, {  5.0f,  6.0f,  7.0f,  8.0f } },
            { 0.4f, {  9.0f, 10.0f, 11.0f, 12.0f } },
            { 0.8f, { 13.0f, 14.0f, 15.0f, 16.0f } } };

        stopCollection->GetGradientStopCountMethod.SetExpectedCalls(1,
            [&]
            {
                return (uint32_t)_countof(d2dStops);
            });

        stopCollection->GetGradientStops1Method.SetExpectedCalls(1,
            [&] (D2D1_GRADIENT_STOP* s, uint32_t c)
            {
                Assert::AreEqual<uint32_t>(_countof(d2dStops), c);
                std::copy(
                    std::begin(d2dStops),
                    std::end(d2dStops),
                    stdext::checked_array_iterator<D2D1_GRADIENT_STOP*>(s, c));
            });

        ComArray<STOP> stops;
        ThrowIfFailed((brush.Get()->*fn)(stops.GetAddressOfSize(), stops.GetAddressOfData()));

        Assert::AreEqual<uint32_t>(_countof(d2dStops), stops.GetSize());
        for (uint32_t i = 0; i < stops.GetSize(); ++i)
        {
            auto& d2dStop = d2dStops[i];
            auto& stop = stops[i];

            Assert::AreEqual(d2dStop.position, stop.Position);
            Assert::AreEqual(ToWin2DColor<STOP>::Convert(d2dStop.color), stop.Color);
        }
    }


    template<typename T>
    static void TestGetStops()
    {
        TestGetStops<T, CanvasGradientStop>(&T::ibrush_t::get_Stops);
    }

    template<typename T>
    static void TestGetStopsHdr()
    {
        TestGetStops<T, CanvasGradientStopHdr>(&T::ibrush_t::get_StopsHdr);
    }

#define TEST_BRUSHES(name)                              \
    TEST_METHOD_EX(CanvasLinearGradientBrush_##name)    \
    {                                                   \
        Test##name<Linear>();                           \
    }                                                   \
    TEST_METHOD_EX(CanvasRadialGradientBrush_##name)    \
    {                                                   \
        Test##name<Radial>();                           \
    }

    TEST_BRUSHES(CreateBadParams);
    TEST_BRUSHES(CreateHdrBadParams);

    TEST_BRUSHES(CreateSimple);
    TEST_BRUSHES(CreateHdrSimple);

    TEST_BRUSHES(CreateWithStops);
    TEST_BRUSHES(CreateHdrWithStops);

    TEST_BRUSHES(CreateWithEdgeBehaviorAndAlphaMode);
    TEST_BRUSHES(CreateHdrWithEdgeBehaviorAndAlphaMode);

    TEST_BRUSHES(CreateWithEdgeBehaviorAndInterpolationOptions);
    TEST_BRUSHES(CreateHdrWithEdgeBehaviorAndInterpolationOptions);

    TEST_BRUSHES(GetStops);
    TEST_BRUSHES(GetStopsHdr);
};

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include <lib/geometry/CanvasCachedGeometry.h>
#include "mocks/MockD2DGeometryRealization.h"
#include "mocks/MockD2DRectangleGeometry.h"

TEST_CLASS(CanvasCachedGeometryTests)
{
    struct SetupFixture
    {
        ComPtr<StubCanvasDevice> Device;
        std::shared_ptr<CanvasCachedGeometryManager> Manager;

        SetupFixture()
            : Device(Make<StubCanvasDevice>())
            , Manager(std::make_shared<CanvasCachedGeometryManager>())
        {
        }

        ComPtr<CanvasCachedGeometry> CreateCachedGeometry()
        {
            auto mockD2DGeometryRealization = Make<MockD2DGeometryRealization>();
            auto cachedGeometry = Manager->GetOrCreate(Device.Get(), mockD2DGeometryRealization.Get());
            return cachedGeometry;
        }
    };

    TEST_METHOD_EX(CanvasCachedGeometry_get_Device)
    {
        SetupFixture f;
        auto cachedGeometry = f.CreateCachedGeometry();

        ComPtr<ICanvasDevice> device;
        Assert::AreEqual(S_OK, cachedGeometry->get_Device(&device));

        Assert::AreEqual(static_cast<ICanvasDevice*>(f.Device.Get()), device.Get());
    }

    TEST_METHOD_EX(CanvasCachedGeometry_get_Device_Null)
    {
        SetupFixture f;
        auto cachedGeometry = f.CreateCachedGeometry();

        Assert::AreEqual(E_INVALIDARG, cachedGeometry->get_Device(nullptr));
    }

    TEST_METHOD_EX(CanvasCachedGeometry_get_Device_Closed)
    {
        SetupFixture f;
        auto cachedGeometry = f.CreateCachedGeometry();

        Assert::AreEqual(S_OK, cachedGeometry->Close());

        ComPtr<ICanvasDevice> device;
        Assert::AreEqual(RO_E_CLOSED, cachedGeometry->get_Device(&device));
    }

    class GeometryObjectAccess_Fixture
    {
        ComPtr<StubD2DFactoryWithCreateStrokeStyle> m_factory;
        std::shared_ptr<CanvasGeometryManager> m_geometryManager;
    public:
        ComPtr<StubCanvasDevice> Device;
        std::shared_ptr<CanvasCachedGeometryManager> CachedGeometryManager;
        ComPtr<MockD2DRectangleGeometry> D2DRectangleGeometry;
        ComPtr<ICanvasGeometry> CanvasRectangleGeometry;
        ComPtr<ICanvasStrokeStyle> StrokeStyle;

        GeometryObjectAccess_Fixture()
            : Device(Make<StubCanvasDevice>())
            , m_geometryManager(std::make_shared<CanvasGeometryManager>())
            , CachedGeometryManager(std::make_shared<CanvasCachedGeometryManager>())
            , D2DRectangleGeometry(Make<MockD2DRectangleGeometry>())
            , StrokeStyle(Make<CanvasStrokeStyle>())
            , m_factory(Make<StubD2DFactoryWithCreateStrokeStyle>())
        {
            CanvasRectangleGeometry = m_geometryManager->GetOrCreate(Device.Get(), D2DRectangleGeometry.Get()); 

            StrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);

            D2DRectangleGeometry->GetFactoryMethod.AllowAnyCall(
                [this](ID2D1Factory** out)
                {
                    m_factory.CopyTo(out);
                });
        }

        void VerifyStrokeStyle()
        {
            Assert::AreEqual(m_factory->m_lineJoin, D2D1_LINE_JOIN_MITER_OR_BEVEL);
        }
    };

    TEST_METHOD_EX(CanvasCachedGeometry_CreateFill)
    {
        GeometryObjectAccess_Fixture f;
        
        f.Device->CreateFilledGeometryRealizationMethod.AllowAnyCall(
            [&](ID2D1Geometry* geometry, FLOAT flatteningTolerance)
            {
                Assert::AreEqual(static_cast<ID2D1Geometry*>(f.D2DRectangleGeometry.Get()), geometry);
                Assert::AreEqual(5.0f, flatteningTolerance);

                return Make<MockD2DGeometryRealization>();
            });

        f.Device->CreateStrokedGeometryRealizationMethod.SetExpectedCalls(0);

        f.CachedGeometryManager->Create(f.Device.Get(), f.CanvasRectangleGeometry.Get(), 5.0f);
    }

    TEST_METHOD_EX(CanvasCachedGeometry_CreateFill_NullArgs)
    {
        GeometryObjectAccess_Fixture f;

        ExpectHResultException(E_INVALIDARG, [&]{ f.CachedGeometryManager->Create(nullptr, f.CanvasRectangleGeometry.Get(), 5.0f); });
        ExpectHResultException(E_INVALIDARG, [&]{ f.CachedGeometryManager->Create(f.Device.Get(), nullptr, 5.0f); });
    }

    TEST_METHOD_EX(CanvasCachedGeometry_CreateStroke)
    {
        GeometryObjectAccess_Fixture f;

        f.Device->CreateStrokedGeometryRealizationMethod.AllowAnyCall(
            [&](ID2D1Geometry* geometry, FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle, FLOAT flatteningTolerance)
            {
                Assert::AreEqual(static_cast<ID2D1Geometry*>(f.D2DRectangleGeometry.Get()), geometry);
                Assert::AreEqual(99.0f, strokeWidth);
                Assert::IsNotNull(strokeStyle);
                Assert::AreEqual(5.0f, flatteningTolerance);

                return Make<MockD2DGeometryRealization>();
            });

        f.Device->CreateFilledGeometryRealizationMethod.SetExpectedCalls(0);

        f.CachedGeometryManager->Create(f.Device.Get(), f.CanvasRectangleGeometry.Get(), 99.0f, f.StrokeStyle.Get(), 5.0f);
        f.VerifyStrokeStyle();
    }

    TEST_METHOD_EX(CanvasCachedGeometry_CreateStroke_NullArgs)
    {
        GeometryObjectAccess_Fixture f;

        ExpectHResultException(E_INVALIDARG, [&]{ f.CachedGeometryManager->Create(nullptr, f.CanvasRectangleGeometry.Get(), 0.0f, f.StrokeStyle.Get(), 0.0f); });
        ExpectHResultException(E_INVALIDARG, [&]{ f.CachedGeometryManager->Create(f.Device.Get(), nullptr, 0.0f, f.StrokeStyle.Get(), 0.0f); });
    }
};

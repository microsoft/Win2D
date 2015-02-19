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
#include "MockD2DRectangleGeometry.h"
#include "MockD2DEllipseGeometry.h"
#include "MockD2DRoundedRectangleGeometry.h"
#include "MockD2DPathGeometry.h"
#include "MockD2DGeometrySink.h"

static const D2D1_MATRIX_3X2_F sc_someD2DTransform = { 1, 2, 3, 4, 5, 6 };
static const D2D1_MATRIX_3X2_F sc_identityD2DTransform = { 1, 0, 0, 1, 0, 0 };
static const Matrix3x2 sc_someTransform = { 1, 2, 3, 4, 5, 6 };

TEST_CLASS(CanvasGeometryTests)
{
public: 

    struct Fixture
    {
        ComPtr<StubCanvasDevice> Device;
        std::shared_ptr<CanvasGeometryManager> Manager;

        Fixture()
            : Device(Make<StubCanvasDevice>())
            , Manager(std::make_shared<CanvasGeometryManager>())
        {
            Device->CreateRectangleGeometryMethod.AllowAnyCall(
                [](D2D1_RECT_F const&)
                {
                    return Make<MockD2DRectangleGeometry>();
                });

        }
    };

    TEST_METHOD_EX(CanvasGeometry_ImplementsExpectedInterfaces)
    {
        Fixture f;

        auto canvasGeometry = f.Manager->Create(f.Device.Get(), Rect{});

        ASSERT_IMPLEMENTS_INTERFACE(canvasGeometry, ICanvasGeometry);
        ASSERT_IMPLEMENTS_INTERFACE(canvasGeometry, ABI::Windows::Foundation::IClosable);
    }

    TEST_METHOD_EX(CanvasGeometry_CreatedWithCorrectD2DRectangleResource)
    {
        Fixture f;

        Rect expectedRect{ 1, 2, 3, 4 };
        const D2D1_RECT_F expectedD2DRect = 
            D2D1::RectF(expectedRect.X, expectedRect.Y, expectedRect.X + expectedRect.Width, expectedRect.Y + expectedRect.Height);
        
        f.Device->CreateRectangleGeometryMethod.SetExpectedCalls(1, 
            [expectedD2DRect](D2D1_RECT_F const& rect)
            {
                Assert::AreEqual(expectedD2DRect, rect);
                return Make<MockD2DRectangleGeometry>();
            });

        f.Manager->Create(f.Device.Get(), expectedRect);
    }

    TEST_METHOD_EX(CanvasGeometry_CreatedWithCorrectD2DEllipseResource)
    {
        Fixture f;

        D2D1_ELLIPSE testEllipse = D2D1::Ellipse(D2D1::Point2F(1, 2), 3, 4);

        f.Device->CreateEllipseGeometryMethod.SetExpectedCalls(1,
            [testEllipse](D2D1_ELLIPSE const& ellipse)
            {
                Assert::AreEqual(testEllipse, ellipse);
                return Make<MockD2DEllipseGeometry>();
            });
            
        f.Manager->Create(f.Device.Get(), Vector2{ testEllipse.point.x, testEllipse.point.y }, testEllipse.radiusX, testEllipse.radiusY);
    }

    TEST_METHOD_EX(CanvasGeometry_CreatedWithCorrectD2DRoundedRectangleResource)
    {
        Fixture f;

        Rect expectedRect{ 1, 2, 3, 4 };
        const D2D1_RECT_F expectedD2DRect =
            D2D1::RectF(expectedRect.X, expectedRect.Y, expectedRect.X + expectedRect.Width, expectedRect.Y + expectedRect.Height);
        float expectedRadiusX = 5;
        float expectedRadiusY = 6;

        f.Device->CreateRoundedRectangleGeometryMethod.SetExpectedCalls(1,
            [expectedD2DRect, expectedRadiusX, expectedRadiusY](D2D1_ROUNDED_RECT const& roundedRect)
            {
                Assert::AreEqual(D2D1::RoundedRect(expectedD2DRect, expectedRadiusX, expectedRadiusY), roundedRect);
                return Make<MockD2DRoundedRectangleGeometry>();
            });

        f.Manager->Create(f.Device.Get(), expectedRect, expectedRadiusX, expectedRadiusY);
    }

    class GeometryOperationsFixture_DoesNotOutputGeometry : public Fixture
    {
        ComPtr<StubD2DFactoryWithCreateStrokeStyle> m_factory;

    public:
        ComPtr<MockD2DRectangleGeometry> D2DRectangleGeometry;
        ComPtr<ICanvasGeometry> RectangleGeometry;

        ComPtr<MockD2DEllipseGeometry> D2DEllipseGeometry;
        ComPtr<ICanvasGeometry> EllipseGeometry;

        ComPtr<ID2D1StrokeStyle1> D2DStrokeStyle;
        ComPtr<ICanvasStrokeStyle> StrokeStyle;

        GeometryOperationsFixture_DoesNotOutputGeometry()
            : D2DRectangleGeometry(Make<MockD2DRectangleGeometry>())
            , D2DEllipseGeometry(Make<MockD2DEllipseGeometry>())
            , StrokeStyle(Make<CanvasStrokeStyle>())
            , m_factory(Make<StubD2DFactoryWithCreateStrokeStyle>())
        {
            RectangleGeometry = Manager->GetOrCreate(Device.Get(), D2DRectangleGeometry.Get());

            EllipseGeometry = Manager->GetOrCreate(Device.Get(), D2DEllipseGeometry.Get());

            StrokeStyle->put_LineJoin(CanvasLineJoin::MiterOrBevel);            

            D2DRectangleGeometry->GetFactoryMethod.AllowAnyCall(
                [this](ID2D1Factory** out)
                {
                    m_factory.CopyTo(out);
                });
            
            Device->CreatePathGeometryMethod.AllowAnyCall(
                []()
                {
                    auto pathGeometry = Make<MockD2DPathGeometry>();

                    pathGeometry->OpenMethod.AllowAnyCall(
                        [](ID2D1GeometrySink** out)
                        {
                            auto geometrySink = Make<MockD2DGeometrySink>();
                            return geometrySink.CopyTo(out);
                        });

                    return pathGeometry;
                });
        }

        void VerifyStrokeStyle()
        {
            Assert::AreEqual(m_factory->m_lineJoin, D2D1_LINE_JOIN_MITER_OR_BEVEL);
        }
    };

    class GeometryOperationsFixture_OutputsGeometry : public GeometryOperationsFixture_DoesNotOutputGeometry
    {
    public:
        ComPtr<MockD2DPathGeometry> TemporaryPathGeometry;
        ComPtr<MockD2DGeometrySink> SinkForTemporaryPath;

        GeometryOperationsFixture_OutputsGeometry()
            : TemporaryPathGeometry(Make<MockD2DPathGeometry>())
            , SinkForTemporaryPath(Make<MockD2DGeometrySink>())
        {
            SinkForTemporaryPath->CloseMethod.AllowAnyCall();

            Device->CreatePathGeometryMethod.SetExpectedCalls(1,
                [this]()
                {
                    TemporaryPathGeometry->OpenMethod.SetExpectedCalls(1,
                        [this](ID2D1GeometrySink** out)
                        {
                            return SinkForTemporaryPath.CopyTo(out);
                        });

                    return TemporaryPathGeometry;
                });
        }

        void VerifyResult(ComPtr<ICanvasGeometry> const& resultCanvasGeometry, bool expectStrokeStyle = false)
        {
            auto d2dResource = GetWrappedResource<ID2D1Geometry>(resultCanvasGeometry);

            Assert::AreEqual<ID2D1Geometry*>(d2dResource.Get(), TemporaryPathGeometry.Get());

            if (expectStrokeStyle)
                VerifyStrokeStyle();
        }
    };

    TEST_METHOD_EX(CanvasGeometry_Combine)
    {
        GeometryOperationsFixture_OutputsGeometry f;

        f.D2DRectangleGeometry->CombineWithGeometryMethod.SetExpectedCalls(1,
            [=](ID2D1Geometry* geometry, D2D1_COMBINE_MODE combineMode, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* sink)
            {
                Assert::AreEqual(static_cast<ID2D1Geometry*>(f.D2DEllipseGeometry.Get()), geometry);
                Assert::AreEqual(D2D1_COMBINE_MODE_INTERSECT, combineMode);
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                Assert::AreEqual<ID2D1SimplifiedGeometrySink*>(f.SinkForTemporaryPath.Get(), sink);
                return S_OK;
            });

        ComPtr<ICanvasGeometry> g;
        Assert::AreEqual(S_OK, f.RectangleGeometry->CombineWith(f.EllipseGeometry.Get(), sc_someTransform, CanvasGeometryCombine::Intersect, &g));
        
        f.VerifyResult(g);
    }

    TEST_METHOD_EX(CanvasGeometry_CombineWithUsingFlatteningTolerance)
    {
        GeometryOperationsFixture_OutputsGeometry f;

        f.D2DRectangleGeometry->CombineWithGeometryMethod.SetExpectedCalls(1,
            [=](ID2D1Geometry* geometry, D2D1_COMBINE_MODE combineMode, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* sink)
        {
            Assert::AreEqual(static_cast<ID2D1Geometry*>(f.D2DEllipseGeometry.Get()), geometry);
            Assert::AreEqual(D2D1_COMBINE_MODE_INTERSECT, combineMode);
            Assert::AreEqual(sc_someD2DTransform, *transform);
            Assert::AreEqual(2.0f, tol);
            Assert::AreEqual<ID2D1SimplifiedGeometrySink*>(f.SinkForTemporaryPath.Get(), sink);
            return S_OK;
        });

        ComPtr<ICanvasGeometry> g;
        Assert::AreEqual(S_OK, f.RectangleGeometry->CombineWithUsingFlatteningTolerance(f.EllipseGeometry.Get(), sc_someTransform, CanvasGeometryCombine::Intersect, 2.0f, &g));

        f.VerifyResult(g);
    }

    TEST_METHOD_EX(CanvasGeometry_Combine_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;
        ComPtr<ICanvasGeometry> g;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->CombineWith(nullptr, Matrix3x2{}, CanvasGeometryCombine::Union, &g));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->CombineWith(f.RectangleGeometry.Get(), Matrix3x2{}, CanvasGeometryCombine::Union, nullptr));

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->CombineWithUsingFlatteningTolerance(nullptr, Matrix3x2{}, CanvasGeometryCombine::Union, 0, &g));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->CombineWithUsingFlatteningTolerance(f.RectangleGeometry.Get(), Matrix3x2{}, CanvasGeometryCombine::Union, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_Stroke)
    {
        GeometryOperationsFixture_OutputsGeometry f;

        f.D2DRectangleGeometry->WidenMethod.SetExpectedCalls(1,
            [=](FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* sink)
            {
                Assert::AreEqual(5.0f, strokeWidth);
                Assert::IsNull(transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                Assert::IsNull(strokeStyle);
                Assert::AreEqual<ID2D1SimplifiedGeometrySink*>(f.SinkForTemporaryPath.Get(), sink);
                return S_OK;
            });

        ComPtr<ICanvasGeometry> g;
        Assert::AreEqual(S_OK, f.RectangleGeometry->Stroke(5.0f, &g));

        f.VerifyResult(g);
    }

    TEST_METHOD_EX(CanvasGeometry_StrokeWithAllOptions)
    {
        GeometryOperationsFixture_OutputsGeometry f;

        f.D2DRectangleGeometry->WidenMethod.SetExpectedCalls(1,
            [=](FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* sink)
            {
                Assert::AreEqual(5.0f, strokeWidth);
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);
                Assert::IsNotNull(strokeStyle);
                Assert::AreEqual<ID2D1SimplifiedGeometrySink*>(f.SinkForTemporaryPath.Get(), sink);
                return S_OK;
            });

        ComPtr<ICanvasGeometry> g;
        Assert::AreEqual(S_OK, f.RectangleGeometry->StrokeWithAllOptions(5.0f, f.StrokeStyle.Get(), sc_someTransform, 2.0f, &g));

        f.VerifyResult(g, true);
    }

    TEST_METHOD_EX(CanvasGeometry_Stroke_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;
        ComPtr<ICanvasGeometry> g;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->Stroke(0, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->StrokeWithAllOptions(0, nullptr, Matrix3x2{}, 0, &g));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->StrokeWithAllOptions(0, f.StrokeStyle.Get(), Matrix3x2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_Outline)
    {
        GeometryOperationsFixture_OutputsGeometry f;

        f.D2DRectangleGeometry->OutlineMethod.SetExpectedCalls(1,
            [=](CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* sink)
            {
                Assert::AreEqual(sc_identityD2DTransform, *transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                Assert::AreEqual<ID2D1SimplifiedGeometrySink*>(f.SinkForTemporaryPath.Get(), sink);
                return S_OK;
            });

        ComPtr<ICanvasGeometry> g;
        Assert::AreEqual(S_OK, f.RectangleGeometry->Outline(&g));

        f.VerifyResult(g);
    }

    TEST_METHOD_EX(CanvasGeometry_OutlineWithTransformAndFlatteningTolerance)
    {
        GeometryOperationsFixture_OutputsGeometry f;

        f.D2DRectangleGeometry->OutlineMethod.SetExpectedCalls(1,
            [=](CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* sink)
            {
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);
                Assert::AreEqual<ID2D1SimplifiedGeometrySink*>(f.SinkForTemporaryPath.Get(), sink);
                return S_OK;
            });

        ComPtr<ICanvasGeometry> g;
        Assert::AreEqual(S_OK, f.RectangleGeometry->OutlineWithTransformAndFlatteningTolerance(sc_someTransform, 2.0f, &g));

        f.VerifyResult(g);
    }

    TEST_METHOD_EX(CanvasGeometry_Outline_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;
        ComPtr<ICanvasGeometry> g;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->Outline(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->OutlineWithTransformAndFlatteningTolerance(Matrix3x2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_Simplify)
    {
        GeometryOperationsFixture_OutputsGeometry f;

        f.D2DRectangleGeometry->SimplifyMethod.SetExpectedCalls(1,
            [=](D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplification, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* sink)
            {
                Assert::AreEqual(D2D1_GEOMETRY_SIMPLIFICATION_OPTION_LINES, simplification);
                Assert::AreEqual(sc_identityD2DTransform, *transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                Assert::AreEqual<ID2D1SimplifiedGeometrySink*>(f.SinkForTemporaryPath.Get(), sink);
                return S_OK;
            });

        ComPtr<ICanvasGeometry> g;
        Assert::AreEqual(S_OK, f.RectangleGeometry->Simplify(CanvasGeometrySimplification::Lines, &g));

        f.VerifyResult(g);
    }

    TEST_METHOD_EX(CanvasGeometry_SimplifyWithTransformAndFlatteningTolerance)
    {
        GeometryOperationsFixture_OutputsGeometry f;

        f.D2DRectangleGeometry->SimplifyMethod.SetExpectedCalls(1,
            [=](D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplification, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* sink)
            {
                Assert::AreEqual(D2D1_GEOMETRY_SIMPLIFICATION_OPTION_LINES, simplification);
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);
                Assert::AreEqual<ID2D1SimplifiedGeometrySink*>(f.SinkForTemporaryPath.Get(), sink);
                return S_OK;
            });

        ComPtr<ICanvasGeometry> g;
        Assert::AreEqual(S_OK, f.RectangleGeometry->SimplifyWithTransformAndFlatteningTolerance(CanvasGeometrySimplification::Lines, sc_someTransform, 2.0f, &g));

        f.VerifyResult(g);
    }

    TEST_METHOD_EX(CanvasGeometry_Simplify_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;
        ComPtr<ICanvasGeometry> g;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->Simplify(CanvasGeometrySimplification::Lines, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->SimplifyWithTransformAndFlatteningTolerance(CanvasGeometrySimplification::Lines, Matrix3x2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_CompareWith)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->CompareWithGeometryMethod.SetExpectedCalls(1,
            [=](ID2D1Geometry* otherGeometry, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, D2D1_GEOMETRY_RELATION* relation)
        {
            Assert::AreEqual(static_cast<ID2D1Geometry*>(f.D2DEllipseGeometry.Get()), otherGeometry);
            Assert::AreEqual(sc_identityD2DTransform, *transform);
            Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);

            *relation = D2D1_GEOMETRY_RELATION_DISJOINT;

            return S_OK;
        });

        CanvasGeometryRelation result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->CompareWith(f.EllipseGeometry.Get(), &result));
        Assert::AreEqual(CanvasGeometryRelation::Disjoint, result);
    }

    TEST_METHOD_EX(CanvasGeometry_CompareWithUsingTransformAndFlatteningTolerance)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->CompareWithGeometryMethod.SetExpectedCalls(1,
            [=](ID2D1Geometry* otherGeometry, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, D2D1_GEOMETRY_RELATION* relation)
            {
                Assert::AreEqual(static_cast<ID2D1Geometry*>(f.D2DEllipseGeometry.Get()), otherGeometry);
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);

                *relation = D2D1_GEOMETRY_RELATION_DISJOINT;

                return S_OK;
            });

        CanvasGeometryRelation result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->CompareWithUsingTransformAndFlatteningTolerance(f.EllipseGeometry.Get(), sc_someTransform, 2.0f, &result));
        Assert::AreEqual(CanvasGeometryRelation::Disjoint, result);
    }

    TEST_METHOD_EX(CanvasGeometry_CompareWith_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;
        ComPtr<ICanvasGeometry> g;

        CanvasGeometryRelation r;
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->CompareWith(nullptr, &r));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->CompareWith(f.RectangleGeometry.Get(), nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->CompareWithUsingTransformAndFlatteningTolerance(nullptr, Matrix3x2{}, 0, &r));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->CompareWithUsingTransformAndFlatteningTolerance(f.RectangleGeometry.Get(), Matrix3x2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeArea)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->ComputeAreaMethod.SetExpectedCalls(1,
            [=](CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, float* area)
            {
                Assert::AreEqual(sc_identityD2DTransform, *transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                *area = 123.0f;
                return S_OK;
            });

        float result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputeArea(&result));
        Assert::AreEqual(123.0f, result);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeAreaWithTransformAndFlatteningTolerance)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->ComputeAreaMethod.SetExpectedCalls(1,
            [=](CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, float* area)
            {
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);
                *area = 123.0f;
                return S_OK;
            });

        float result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputeAreaWithTransformAndFlatteningTolerance(sc_someTransform, 2.0f, &result));
        Assert::AreEqual(123.0f, result);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeArea_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputeArea(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputeAreaWithTransformAndFlatteningTolerance(Matrix3x2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_ComputePathLength)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->ComputeLengthMethod.SetExpectedCalls(1,
            [=](CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, float* area)
            {
                Assert::AreEqual(sc_identityD2DTransform, *transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                *area = 123.0f;
                return S_OK;
            });

        float result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputePathLength(&result));
        Assert::AreEqual(123.0f, result);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputePathLengthWithTransformAndFlatteningTolerance)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->ComputeLengthMethod.SetExpectedCalls(1,
            [=](CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, float* length)
            {
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);
                *length = 123.0f;
                return S_OK;
            });

        float result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputePathLengthWithTransformAndFlatteningTolerance(sc_someTransform, 2.0f, &result));
        Assert::AreEqual(123.0f, result);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputePathLength_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputePathLength(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputePathLengthWithTransformAndFlatteningTolerance(Matrix3x2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_ComputePointOnPath)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->ComputePointAtLengthMethod.SetExpectedCalls(1,
            [=](FLOAT length, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, D2D1_POINT_2F* position, D2D1_POINT_2F* tangent)
            {
                Assert::AreEqual(99.0f, length);
                Assert::IsNull(transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                *position = D2D1_POINT_2F{ 12, 34 };
                Assert::IsNotNull(tangent);
                return S_OK;
            });

        Vector2 point;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputePointOnPath(99.0f, &point));
        Assert::AreEqual(Vector2{ 12, 34 }, point);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputePointOnPathWithTransformAndFlatteningToleranceAndTangent)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->ComputePointAtLengthMethod.SetExpectedCalls(1,
            [=](FLOAT length, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, D2D1_POINT_2F* position, D2D1_POINT_2F* tangent)
            {
                Assert::AreEqual(99.0f, length);
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);
                *position = D2D1_POINT_2F{ 12, 34 };
                *tangent = D2D1_POINT_2F{ 56, 78 };
                return S_OK;
            });

        Vector2 tangent, point;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputePointOnPathWithTransformAndFlatteningToleranceAndTangent(99.0f, sc_someTransform, 2.0f, &tangent, &point));
        Assert::AreEqual(Vector2{ 12, 34 }, point);
        Assert::AreEqual(Vector2{ 56, 78 }, tangent);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputePointOnPath_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;
        Vector2 pt;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputePointOnPath(0, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputePointOnPathWithTransformAndFlatteningToleranceAndTangent(0, Matrix3x2{}, 0, &pt, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputePointOnPathWithTransformAndFlatteningToleranceAndTangent(0, Matrix3x2{}, 0, nullptr, &pt));
    }

    TEST_METHOD_EX(CanvasGeometry_FillContainsPoint)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->FillContainsPointMethod.SetExpectedCalls(1,
            [=](D2D1_POINT_2F point, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, BOOL* contains)
            {
                Assert::AreEqual(D2D1_POINT_2F{ 123, 456 }, point);
                Assert::AreEqual(sc_identityD2DTransform, *transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                *contains = true;
                return S_OK;
            });

        boolean result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->FillContainsPoint(Vector2{ 123, 456 }, &result));
        Assert::IsTrue(!!result);
    }

    TEST_METHOD_EX(CanvasGeometry_FillContainsPointWithTransformAndFlatteningTolerance)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->FillContainsPointMethod.SetExpectedCalls(1,
            [=](D2D1_POINT_2F point, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, BOOL* contains)
            {
                Assert::AreEqual(D2D1_POINT_2F{ 123, 456 }, point);
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);
                *contains = true;
                return S_OK;
            });

        boolean result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->FillContainsPointWithTransformAndFlatteningTolerance(Vector2{ 123, 456 }, sc_someTransform, 2.0f, &result));
        Assert::IsTrue(!!result);
    }

    TEST_METHOD_EX(CanvasGeometry_FillContainsPoint_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->FillContainsPoint(Vector2{}, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->FillContainsPointWithTransformAndFlatteningTolerance(Vector2{}, Matrix3x2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeBounds)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->GetBoundsMethod.SetExpectedCalls(1,
            [=](CONST D2D1_MATRIX_3X2_F* transform, D2D1_RECT_F* bounds)
            {
                Assert::AreEqual(sc_identityD2DTransform, *transform);
                *bounds = D2D1_RECT_F{ 1, 2, 1 + 3, 2 + 4 };
                return S_OK;
            });

        Rect result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputeBounds(&result));
        Assert::AreEqual(Rect{ 1, 2, 3, 4 }, result);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeBoundsWithTransform)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->GetBoundsMethod.SetExpectedCalls(1,
            [=](CONST D2D1_MATRIX_3X2_F* transform, D2D1_RECT_F* bounds)
            {
                Assert::AreEqual(sc_someD2DTransform, *transform);
                *bounds = D2D1_RECT_F{ 1, 2, 1 + 3, 2 + 4 };
                return S_OK;
            });

        Rect result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputeBoundsWithTransform(sc_someTransform, &result));
        Assert::AreEqual(Rect{ 1, 2, 3, 4 }, result);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeBounds_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputeBounds(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputeBoundsWithTransform(Matrix3x2{}, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeStrokeBounds)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->GetWidenedBoundsMethod.SetExpectedCalls(1,
            [=](FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle, CONST D2D1_MATRIX_3X2_F* transform, FLOAT flatteningTolerance, D2D1_RECT_F* bounds)
            {
                Assert::AreEqual(strokeWidth, 5.0f);
                Assert::IsNull(strokeStyle);
                Assert::IsNull(transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, flatteningTolerance);
                *bounds = D2D1_RECT_F{ 1, 2, 1 + 3, 2 + 4 };
                return S_OK;
            });

        Rect result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputeStrokeBounds(5.0f, &result));
        Assert::AreEqual(Rect{ 1, 2, 3, 4 }, result);
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeStrokeBoundsWithAllOptions)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->GetWidenedBoundsMethod.SetExpectedCalls(1,
            [=](FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle, CONST D2D1_MATRIX_3X2_F* transform, FLOAT flatteningTolerance, D2D1_RECT_F* bounds)
            {
                Assert::AreEqual(strokeWidth, 5.0f);
                Assert::IsNotNull(strokeStyle);
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, flatteningTolerance);
                *bounds = D2D1_RECT_F{ 1, 2, 1 + 3, 2 + 4 };
                return S_OK;
            });

        Rect result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->ComputeStrokeBoundsWithAllOptions(5.0f, f.StrokeStyle.Get(), sc_someTransform, 2.0f, &result));
        Assert::AreEqual(Rect{ 1, 2, 3, 4 }, result);
        f.VerifyStrokeStyle();
    }

    TEST_METHOD_EX(CanvasGeometry_ComputeStrokeBounds_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        Rect rect;
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputeStrokeBounds(0, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputeStrokeBoundsWithAllOptions(0, nullptr, Matrix3x2{}, 0, &rect));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->ComputeStrokeBoundsWithAllOptions(0, f.StrokeStyle.Get(), Matrix3x2{}, 0, nullptr));
    }


    TEST_METHOD_EX(CanvasGeometry_StrokeContainsPoint)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->StrokeContainsPointMethod.SetExpectedCalls(1,
            [=](D2D1_POINT_2F point, float strokeWidth, ID2D1StrokeStyle* strokeStyle, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, BOOL* contains)
            {
                Assert::AreEqual(D2D1_POINT_2F{ 123, 456 }, point);
                Assert::AreEqual(5.0f, strokeWidth);
                Assert::IsNull(strokeStyle);
                Assert::IsNull(transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                *contains = true;
                return S_OK;
            });

        boolean result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->StrokeContainsPoint(Vector2{ 123, 456 }, 5.0f, &result));
        Assert::IsTrue(!!result);
    }

    TEST_METHOD_EX(CanvasGeometry_StrokeContainsPointWithTransformAndFlatteningTolerance)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        f.D2DRectangleGeometry->StrokeContainsPointMethod.SetExpectedCalls(1,
            [=](D2D1_POINT_2F point, float strokeWidth, ID2D1StrokeStyle* strokeStyle, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, BOOL* contains)
            {
                Assert::AreEqual(D2D1_POINT_2F{ 123, 456 }, point);
                Assert::AreEqual(5.0f, strokeWidth);
                Assert::IsNotNull(strokeStyle);
                Assert::AreEqual(sc_someD2DTransform, *transform);
                Assert::AreEqual(2.0f, tol);
                *contains = true;
                return S_OK;
            });

        boolean result;
        Assert::AreEqual(S_OK, f.RectangleGeometry->StrokeContainsPointWithAllOptions(Vector2{ 123, 456 }, 5.0f, f.StrokeStyle.Get(), sc_someTransform, 2.0f, &result));
        Assert::IsTrue(!!result);
        f.VerifyStrokeStyle();
    }

    TEST_METHOD_EX(CanvasGeometry_StrokeContainsPoint_NullArgs)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        boolean b;
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->StrokeContainsPoint(Vector2{}, 0, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->StrokeContainsPointWithAllOptions(Vector2{}, 0, nullptr, Matrix3x2{}, 0, &b));
        Assert::AreEqual(E_INVALIDARG, f.RectangleGeometry->StrokeContainsPointWithAllOptions(Vector2{}, 0, f.StrokeStyle.Get(), Matrix3x2{}, 0, nullptr));
    }

    TEST_METHOD_EX(CanvasGeometry_Closure)
    {
        GeometryOperationsFixture_DoesNotOutputGeometry f;

        auto canvasGeometry = f.Manager->Create(f.Device.Get(), Rect{});
        auto otherCanvasGeometry = f.Manager->Create(f.Device.Get(), Rect{});
        auto pb = Make<CanvasPathBuilder>(f.Device.Get());

        auto strokeStyle = Make<CanvasStrokeStyle>();
        Matrix3x2 m{};
        ComPtr<ICanvasGeometry> g;
        CanvasGeometryRelation r;
        float fl;
        boolean b;
        Rect rect;
        Vector2 pt;

        Assert::AreEqual(S_OK, canvasGeometry->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->CombineWith(otherCanvasGeometry.Get(), m, CanvasGeometryCombine::Union, &g));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->CombineWithUsingFlatteningTolerance(otherCanvasGeometry.Get(), m, CanvasGeometryCombine::Union, 0, &g));        

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->Stroke(0, &g));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->StrokeWithAllOptions(0, strokeStyle.Get(), m, 0, &g));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->Outline(&g));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->OutlineWithTransformAndFlatteningTolerance(m, 0, &g));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->Simplify(CanvasGeometrySimplification::Lines, &g));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->SimplifyWithTransformAndFlatteningTolerance(CanvasGeometrySimplification::Lines, m, 0, &g));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->CompareWith(otherCanvasGeometry.Get(), &r));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->CompareWithUsingTransformAndFlatteningTolerance(otherCanvasGeometry.Get(), m, 0, &r));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputeArea(&fl));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputeAreaWithTransformAndFlatteningTolerance(m, 0, &fl));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputePathLength(&fl));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputePathLengthWithTransformAndFlatteningTolerance(m, 0, &fl));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputePointOnPath(0, &pt));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputePointOnPathWithTransformAndFlatteningToleranceAndTangent(0, m, 0, &pt, &pt));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->FillContainsPoint(Vector2{}, &b));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->FillContainsPointWithTransformAndFlatteningTolerance(Vector2{}, m, 0, &b));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputeBounds(&rect));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputeBoundsWithTransform(m, &rect));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputeStrokeBounds(0, &rect));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->ComputeStrokeBoundsWithAllOptions(0, strokeStyle.Get(), m, 0, &rect));

        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->StrokeContainsPoint(Vector2{}, 0, &b));
        Assert::AreEqual(RO_E_CLOSED, canvasGeometry->StrokeContainsPointWithAllOptions(Vector2{}, 0, strokeStyle.Get(), m, 0, &b));
    }

};

TEST_CLASS(CanvasPathBuilderUnitTests)
{
    struct SetupFixture
    {
        ComPtr<StubCanvasDevice> Device;
        std::shared_ptr<CanvasGeometryManager> Manager;
        ComPtr<ICanvasGeometry> SomeTestGeometry;

        SetupFixture()
            : Device(Make<StubCanvasDevice>())
            , Manager(std::make_shared<CanvasGeometryManager>())
        {
            Device->CreatePathGeometryMethod.AllowAnyCall(
                []()
                {
                    auto pathGeometry = Make<MockD2DPathGeometry>();

                    pathGeometry->OpenMethod.AllowAnyCall(
                        [](ID2D1GeometrySink** out)
                        {
                            auto geometrySink = Make<MockD2DGeometrySink>();                            

                            geometrySink->BeginFigureMethod.AllowAnyCall();
                            geometrySink->EndFigureMethod.AllowAnyCall();
                            geometrySink->CloseMethod.AllowAnyCall();

                            return geometrySink.CopyTo(out);
                        });

                    return pathGeometry;
                });

            SomeTestGeometry = Manager->GetOrCreate(Device.Get(), Make<MockD2DRectangleGeometry>().Get());
        }
    };

    TEST_METHOD_EX(CanvasPathBuilder_Closure)
    {
        SetupFixture f;

        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        Assert::AreEqual(S_OK, canvasPathBuilder->Close());

        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->BeginFigure(Vector2{}));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->BeginFigureWithFigureFill(Vector2{}, CanvasFigureFill::DoesNotAffectFills));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->AddArc(Vector2{}, 0, 0, 0, CanvasSweepDirection::Clockwise, CanvasArcSize::Small));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->AddCubicBezier(Vector2{}, Vector2{}, Vector2{}));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->AddLine(Vector2{}));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->AddLineWithCoords(0, 0));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->AddQuadraticBezier(Vector2{}, Vector2{}));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->SetSegmentOptions(CanvasFigureSegmentOptions::None));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->SetFilledRegionDetermination(CanvasFilledRegionDetermination::Alternate));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->EndFigure(CanvasFigureLoop::Closed));
        Assert::AreEqual(RO_E_CLOSED, canvasPathBuilder->AddGeometry(f.SomeTestGeometry.Get()));

        // Verify that path builder's device was closed, as well.
        auto pathBuilderInternal = As<ICanvasPathBuilderInternal>(canvasPathBuilder);
        ExpectHResultException(RO_E_CLOSED, [&]{ pathBuilderInternal->GetDevice(); });
    }

    TEST_METHOD_EX(CanvasPathBuilder_CreationOpensGeometrySink)
    {
        SetupFixture f;
        
        f.Device->CreatePathGeometryMethod.SetExpectedCalls(1,
            []()
            {
                auto pathGeometry = Make<MockD2DPathGeometry>();

                pathGeometry->OpenMethod.SetExpectedCalls(1,
                    [](ID2D1GeometrySink** out)
                    {
                        auto geometrySink = Make<MockD2DGeometrySink>();
                        return geometrySink.CopyTo(out);
                    });

                return pathGeometry;
            });

        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());
    }

    TEST_METHOD_EX(CanvasPathBuilder_CanOnlyCreateOneGeometry)
    {
        SetupFixture f;

        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        f.Manager->Create(canvasPathBuilder.Get());

        ExpectHResultException(RO_E_CLOSED, [&]{ f.Manager->Create(canvasPathBuilder.Get()); });
    }

    TEST_METHOD_EX(CanvasPathBuilder_CreatePathClosesSink)
    {
        SetupFixture f;

        auto geometrySink = Make<MockD2DGeometrySink>();
        geometrySink->CloseMethod.SetExpectedCalls(1);

        auto pathGeometry = Make<MockD2DPathGeometry>();
        pathGeometry->OpenMethod.AllowAnyCall(
            [=](ID2D1GeometrySink** out)
            {
                return geometrySink.CopyTo(out);
            });
        
        f.Device->CreatePathGeometryMethod.AllowAnyCall(
            [=]()
            {
                return pathGeometry;
            });

        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        f.Manager->Create(canvasPathBuilder.Get());
    }

    struct SinkAccessFixture : SetupFixture
    {
        ComPtr<MockD2DGeometrySink> GeometrySink;
        ComPtr<CanvasPathBuilder> PathBuilder;

        SinkAccessFixture()
        {
            auto pathGeometry = Make<MockD2DPathGeometry>();
            GeometrySink = Make<MockD2DGeometrySink>();

            Device->CreatePathGeometryMethod.AllowAnyCall([=]() { return pathGeometry; });
            pathGeometry->OpenMethod.AllowAnyCall([=](ID2D1GeometrySink** out) { return GeometrySink.CopyTo(out); });

            PathBuilder = Make<CanvasPathBuilder>(Device.Get());

            GeometrySink->BeginFigureMethod.AllowAnyCall();
        }

    };

    TEST_METHOD_EX(CanvasPathBuilder_BeginFigure)
    {
        SinkAccessFixture f;
        
        f.GeometrySink->BeginFigureMethod.SetExpectedCalls(1, 
            [](D2D1_POINT_2F point, D2D1_FIGURE_BEGIN figureBegin)
            {
                Assert::AreEqual(D2D1::Point2F(1, 2), point);
                Assert::AreEqual(D2D1_FIGURE_BEGIN_FILLED, figureBegin);
            });
        ThrowIfFailed(f.PathBuilder->BeginFigure(Vector2{ 1, 2 }));
    }

    TEST_METHOD_EX(CanvasPathBuilder_BeginFigureWithFigureFill)
    {
        SinkAccessFixture f;

        f.GeometrySink->BeginFigureMethod.SetExpectedCalls(1,
            [](D2D1_POINT_2F point, D2D1_FIGURE_BEGIN figureBegin)
            {
                Assert::AreEqual(D2D1::Point2F(1, 2), point);
                Assert::AreEqual(D2D1_FIGURE_BEGIN_HOLLOW, figureBegin);
            });
        ThrowIfFailed(f.PathBuilder->BeginFigureWithFigureFill(Vector2{ 1, 2 }, CanvasFigureFill::DoesNotAffectFills));
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddArc)
    {
        SinkAccessFixture f;

        f.PathBuilder->BeginFigure(Vector2{});

        f.GeometrySink->AddArcMethod.SetExpectedCalls(1,
            [](CONST D2D1_ARC_SEGMENT* arc)
            {
                Assert::AreEqual(D2D1::Point2F(1, 2), arc->point);
                Assert::AreEqual(D2D1::SizeF(3, 4), arc->size);
                Assert::AreEqual(0.0f, arc->rotationAngle);
                Assert::AreEqual(D2D1_SWEEP_DIRECTION_CLOCKWISE, arc->sweepDirection);
                Assert::AreEqual(D2D1_ARC_SIZE_LARGE, arc->arcSize);
            });
        ThrowIfFailed(f.PathBuilder->AddArc(Vector2{ 1, 2 }, 3.0f, 4.0f, 0.0f, CanvasSweepDirection::Clockwise, CanvasArcSize::Large));
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddArc_ExpectsRadians)
    {
        const float pi = 3.14159f;
        float angles[] = { 0, pi / 2, pi, 3 * pi / 2, 2 * pi };

        for (float angle : angles)
        {
            SinkAccessFixture f;

            f.PathBuilder->BeginFigure(Vector2{});

            f.GeometrySink->AddArcMethod.SetExpectedCalls(1,
                [=](CONST D2D1_ARC_SEGMENT* arc)
                {
                    const float d2dDegrees = arc->rotationAngle;
                    const float radians = d2dDegrees  * pi / 180.0f;
                    Assert::AreEqual(angle, radians, 0.0001f);
                });
            ThrowIfFailed(f.PathBuilder->AddArc(Vector2{}, 0, 0, angle, CanvasSweepDirection::Clockwise, CanvasArcSize::Large));
        }
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddArc_InvalidState)
    {
        SinkAccessFixture f;

        Assert::AreEqual(E_INVALIDARG, f.PathBuilder->AddArc(Vector2{}, 0, 0, 0, CanvasSweepDirection::Clockwise, CanvasArcSize::Large));

        ValidateStoredErrorState(E_INVALIDARG, Strings::CanOnlyAddPathDataWhileInFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddCubicBezier)
    {
        SinkAccessFixture f;

        f.PathBuilder->BeginFigure(Vector2{});

        f.GeometrySink->AddBezierMethod.SetExpectedCalls(1,
            [](const D2D1_BEZIER_SEGMENT* segment)
            {
                Assert::AreEqual(D2D1::Point2F(1, 2), segment->point1);
                Assert::AreEqual(D2D1::Point2F(3, 4), segment->point2);
                Assert::AreEqual(D2D1::Point2F(5, 6), segment->point3);
            });
        ThrowIfFailed(f.PathBuilder->AddCubicBezier(Vector2{ 1, 2 }, Vector2{ 3, 4 }, Vector2{ 5, 6 }));
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddCubicBezier_InvalidState)
    {
        SinkAccessFixture f;

        Assert::AreEqual(E_INVALIDARG, f.PathBuilder->AddCubicBezier(Vector2{}, Vector2{}, Vector2{}));

        ValidateStoredErrorState(E_INVALIDARG, Strings::CanOnlyAddPathDataWhileInFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddLine)
    {
        SinkAccessFixture f;

        f.PathBuilder->BeginFigure(Vector2{});
        
        f.GeometrySink->AddLineMethod.SetExpectedCalls(1,
            [](D2D1_POINT_2F point)
            {
                Assert::AreEqual(D2D1::Point2F(1, 2), point);
            });
        ThrowIfFailed(f.PathBuilder->AddLine(Vector2{ 1, 2 }));
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddLine_InvalidState)
    {
        SinkAccessFixture f;

        Assert::AreEqual(E_INVALIDARG, f.PathBuilder->AddLine(Vector2{}));

        ValidateStoredErrorState(E_INVALIDARG, Strings::CanOnlyAddPathDataWhileInFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddLineWithCoords)
    {
        SinkAccessFixture f;

        f.PathBuilder->BeginFigure(Vector2{});
        
        f.GeometrySink->AddLineMethod.SetExpectedCalls(1,
            [](D2D1_POINT_2F point)
            {
                Assert::AreEqual(D2D1::Point2F(1, 2), point);
            });
        ThrowIfFailed(f.PathBuilder->AddLineWithCoords(1, 2));
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddLineWithCoords_InvalidState)
    {
        SinkAccessFixture f;

        Assert::AreEqual(E_INVALIDARG, f.PathBuilder->AddLineWithCoords(0, 0));

        ValidateStoredErrorState(E_INVALIDARG, Strings::CanOnlyAddPathDataWhileInFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddQuadraticBezier)
    {
        SinkAccessFixture f;

        f.PathBuilder->BeginFigure(Vector2{});
        
        f.GeometrySink->AddQuadraticBezierMethod.SetExpectedCalls(1,
            [](const D2D1_QUADRATIC_BEZIER_SEGMENT* segment)
            {
                Assert::AreEqual(D2D1::Point2F(1, 2), segment->point1);
                Assert::AreEqual(D2D1::Point2F(3, 4), segment->point2);
            });
        ThrowIfFailed(f.PathBuilder->AddQuadraticBezier(Vector2{ 1, 2 }, Vector2{ 3, 4 }));
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddQuadraticBezier_InvalidState)
    {
        SinkAccessFixture f;

        Assert::AreEqual(E_INVALIDARG, f.PathBuilder->AddQuadraticBezier(Vector2{}, Vector2{}));

        ValidateStoredErrorState(E_INVALIDARG, Strings::CanOnlyAddPathDataWhileInFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_SetSegmentOptions)
    {
        SinkAccessFixture f;
        
        f.GeometrySink->SetSegmentFlagsMethod.SetExpectedCalls(1,
            [](D2D1_PATH_SEGMENT pathSegment)
            {
                Assert::AreEqual(D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN, pathSegment);
            });
        ThrowIfFailed(f.PathBuilder->SetSegmentOptions(CanvasFigureSegmentOptions::ForceRoundLineJoin));
    }

    TEST_METHOD_EX(CanvasPathBuilder_SetFilledRegionDetermination)
    {
        SinkAccessFixture f;
        
        f.GeometrySink->SetFillModeMethod.SetExpectedCalls(1,
            [](D2D1_FILL_MODE fillMode)
            {
                Assert::AreEqual(D2D1_FILL_MODE_WINDING, fillMode);
            });
        ThrowIfFailed(f.PathBuilder->SetFilledRegionDetermination(CanvasFilledRegionDetermination::Winding));
    }

    TEST_METHOD_EX(CanvasPathBuilder_SetFilledRegionDetermination_InvalidAfterBeginFigure)
    {
        SinkAccessFixture f;

        f.PathBuilder->BeginFigure(Vector2{});

        f.GeometrySink->SetFillModeMethod.AllowAnyCall();

        Assert::AreEqual(E_INVALIDARG, f.PathBuilder->SetFilledRegionDetermination(CanvasFilledRegionDetermination::Winding));

        ValidateStoredErrorState(E_INVALIDARG, Strings::SetFilledRegionDeterminationAfterBeginFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_SetFilledRegionDetermination_InvalidAfterFirstCompleteFigure)
    {
        SinkAccessFixture f;

        f.GeometrySink->EndFigureMethod.AllowAnyCall();

        f.PathBuilder->BeginFigure(Vector2{});
        f.PathBuilder->EndFigure(CanvasFigureLoop::Closed);

        f.GeometrySink->SetFillModeMethod.AllowAnyCall();

        Assert::AreEqual(E_INVALIDARG, f.PathBuilder->SetFilledRegionDetermination(CanvasFilledRegionDetermination::Winding));

        ValidateStoredErrorState(E_INVALIDARG, Strings::SetFilledRegionDeterminationAfterBeginFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_EndFigure)
    {
        SinkAccessFixture f;   

        f.GeometrySink->BeginFigureMethod.AllowAnyCall();

        f.GeometrySink->EndFigureMethod.SetExpectedCalls(1, 
            [](D2D1_FIGURE_END figureEnd)
            {
                Assert::AreEqual(D2D1_FIGURE_END_CLOSED, figureEnd);
        });
        ThrowIfFailed(f.PathBuilder->BeginFigure(Vector2{}));
        ThrowIfFailed(f.PathBuilder->EndFigure(CanvasFigureLoop::Closed));
    }

    TEST_METHOD_EX(CanvasPathBuilder_DoubleClose_NothingBadHappens)
    {
        SetupFixture f;

        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        Assert::AreEqual(S_OK, canvasPathBuilder->Close());
        Assert::AreEqual(S_OK, canvasPathBuilder->Close());
    }

    TEST_METHOD_EX(CanvasPathBuilder_TwoBeginFigures_ReturnsErrorAndOutputsMessage)
    {
        SetupFixture f;
        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        Assert::AreEqual(S_OK, canvasPathBuilder->BeginFigure(Vector2{}));
        Assert::AreEqual(E_INVALIDARG, canvasPathBuilder->BeginFigure(Vector2{}));
        ValidateStoredErrorState(E_INVALIDARG, Strings::TwoBeginFigures);
    }

    TEST_METHOD_EX(CanvasPathBuilder_StrayEndFigureOnNewSink_ReturnsErrorAndOutputsMessage)
    {
        SetupFixture f;
        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        Assert::AreEqual(E_INVALIDARG, canvasPathBuilder->EndFigure(CanvasFigureLoop::Closed));
        ValidateStoredErrorState(E_INVALIDARG, Strings::EndFigureWithoutBeginFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_StrayEndFigureAfterFirstFigure_ReturnsErrorAndOutputsMessage)
    {
        SetupFixture f;
        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        Assert::AreEqual(S_OK, canvasPathBuilder->BeginFigure(Vector2{}));
        Assert::AreEqual(S_OK, canvasPathBuilder->EndFigure(CanvasFigureLoop::Closed));
        Assert::AreEqual(E_INVALIDARG, canvasPathBuilder->EndFigure(CanvasFigureLoop::Closed));
        ValidateStoredErrorState(E_INVALIDARG, Strings::EndFigureWithoutBeginFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_ClosedMidFigure_ReturnsErrorAndOutputsMessage)
    {
        SetupFixture f;
        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        Assert::AreEqual(S_OK, canvasPathBuilder->BeginFigure(Vector2{}));

        ExpectHResultException(E_INVALIDARG, [&]{ f.Manager->Create(canvasPathBuilder.Get()); });
        ValidateStoredErrorState(E_INVALIDARG, Strings::PathBuilderClosedMidFigure);

    }

    TEST_METHOD_EX(CanvasPathBuilder_AddGeometry_NullArg)
    {
        SetupFixture f;
        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        Assert::AreEqual(E_INVALIDARG, canvasPathBuilder->AddGeometry(nullptr));
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddGeometry_ShouldNotBeInFigure)
    {
        SetupFixture f;
        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        Assert::AreEqual(S_OK, canvasPathBuilder->BeginFigure(Vector2{}));
        Assert::AreEqual(E_INVALIDARG, canvasPathBuilder->AddGeometry(f.SomeTestGeometry.Get()));
        ValidateStoredErrorState(E_INVALIDARG, Strings::PathBuilderAddGeometryMidFigure);
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddGeometryWithPath_CausesStream)
    {
        SinkAccessFixture f;

        auto mockD2DPathGeometry = Make<MockD2DPathGeometry>();
        auto pathGeometry = f.Manager->GetOrCreate(f.Device.Get(), mockD2DPathGeometry.Get());

        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        mockD2DPathGeometry->StreamMethod.SetExpectedCalls(1,
            [f](ID2D1GeometrySink* geometrySink)
            {
                Assert::AreEqual(static_cast<ID2D1GeometrySink*>(f.GeometrySink.Get()), geometrySink);
                return S_OK;
            });

        Assert::AreEqual(S_OK, canvasPathBuilder->AddGeometry(pathGeometry.Get()));
    }

    TEST_METHOD_EX(CanvasPathBuilder_AddGeometryWithNonPath_CausesSimplify)
    {
        SinkAccessFixture f;

        auto mockD2DRectangleGeometry = Make<MockD2DRectangleGeometry>();
        auto rectangleGeometry = f.Manager->GetOrCreate(f.Device.Get(), mockD2DRectangleGeometry.Get());

        auto canvasPathBuilder = Make<CanvasPathBuilder>(f.Device.Get());

        mockD2DRectangleGeometry->SimplifyMethod.SetExpectedCalls(1,
            [f](D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplification, CONST D2D1_MATRIX_3X2_F* transform, FLOAT tol, ID2D1SimplifiedGeometrySink* geometrySink)
            {
                Assert::AreEqual(D2D1_GEOMETRY_SIMPLIFICATION_OPTION_CUBICS_AND_LINES, simplification);
                Assert::IsNull(transform);
                Assert::AreEqual(D2D1_DEFAULT_FLATTENING_TOLERANCE, tol);
                Assert::AreEqual(static_cast<ID2D1SimplifiedGeometrySink*>(f.GeometrySink.Get()), geometrySink);
                return S_OK;
            });

        Assert::AreEqual(S_OK, canvasPathBuilder->AddGeometry(rectangleGeometry.Get()));
    }

};
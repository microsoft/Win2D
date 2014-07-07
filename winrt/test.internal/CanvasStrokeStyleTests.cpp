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
#include "TestDeviceResourceCreationAdapter.h"
#include "StubD2DFactoryWithCreateStrokeStyle.h"

TEST_CLASS(CanvasStrokeStyleTests)
{
public: 
    TEST_METHOD(CanvasStrokeStyle_Implements_Expected_Interfaces)
    {
        using canvas::CanvasStrokeStyle;
        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();

        ASSERT_IMPLEMENTS_INTERFACE(canvasStrokeStyle, ICanvasStrokeStyle);
        ASSERT_IMPLEMENTS_INTERFACE(canvasStrokeStyle, ABI::Windows::Foundation::IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(canvasStrokeStyle, ICanvasStrokeStyleInternal);
    }

    TEST_METHOD(CanvasStrokeStyle_NullOnGetters)
    {
        // Test that getters return the expected error for nullptr in pointer.
        using canvas::CanvasStrokeStyle;
        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        UINT32 placeholderDashCount;
        float* placeholderDashElements;
        
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_StartCap(nullptr));        
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_EndCap(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_DashCap(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_LineJoin(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_MiterLimit(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_DashStyle(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_DashOffset(nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_CustomDashStyle(&placeholderDashCount, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_CustomDashStyle(nullptr, &placeholderDashElements));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_CustomDashStyle(nullptr, nullptr));
        Assert::AreEqual(E_INVALIDARG, canvasStrokeStyle->get_TransformBehavior(nullptr));

        // Note: D2D validates invalid enum values. For now, Canvas relies on this behavior to verify invalid
        // stroke style properties values.
    }

    TEST_METHOD(CanvasStrokeStyle_Properties)
    {
        using canvas::CanvasDevice;
        using canvas::CanvasStrokeStyle;
        using ABI::Windows::UI::Color;

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        
        // Use the getters to ensure that all the defaults are set correctly.
        CanvasCapStyle capStyle;
        canvasStrokeStyle->get_StartCap(&capStyle);
        Assert::AreEqual(CanvasCapStyle::Flat, capStyle);

        canvasStrokeStyle->get_EndCap(&capStyle);
        Assert::AreEqual(CanvasCapStyle::Flat, capStyle);

        canvasStrokeStyle->get_DashCap(&capStyle);
        Assert::AreEqual(CanvasCapStyle::Flat, capStyle);

        CanvasLineJoin lineJoin;
        canvasStrokeStyle->get_LineJoin(&lineJoin);
        Assert::AreEqual(CanvasLineJoin::Miter, lineJoin);

        float miterLimit;
        canvasStrokeStyle->get_MiterLimit(&miterLimit);
        Assert::AreEqual(10.0f, miterLimit);

        CanvasDashStyle dashStyle;
        canvasStrokeStyle->get_DashStyle(&dashStyle);
        Assert::AreEqual(CanvasDashStyle::Solid, dashStyle);

        float dashOffset;
        canvasStrokeStyle->get_DashOffset(&dashOffset);
        Assert::AreEqual(0.0f, dashOffset);

        UINT32 customDashElementCount;
        float* customDashElements;
        canvasStrokeStyle->get_CustomDashStyle(&customDashElementCount, &customDashElements);
        Assert::AreEqual(0u, customDashElementCount);
        Assert::IsNotNull(customDashElements);

        CanvasStrokeTransformBehavior transformBehavior;
        canvasStrokeStyle->get_TransformBehavior(&transformBehavior);
        Assert::AreEqual(CanvasStrokeTransformBehavior::Normal, transformBehavior);
        
        // Modify all the setters to a non-default, and force realization.
        canvasStrokeStyle->put_StartCap(CanvasCapStyle::Square);
        canvasStrokeStyle->put_EndCap(CanvasCapStyle::Round);
        canvasStrokeStyle->put_DashCap(CanvasCapStyle::Triangle);
        canvasStrokeStyle->put_LineJoin(CanvasLineJoin::Round);
        canvasStrokeStyle->put_MiterLimit(50.0f);
        canvasStrokeStyle->put_DashStyle(CanvasDashStyle::Dot);
        canvasStrokeStyle->put_DashOffset(123.0f);
        float customDashPattern[4] = {1, 2, 3, 4};
        canvasStrokeStyle->put_CustomDashStyle(4, customDashPattern);
        canvasStrokeStyle->put_TransformBehavior(CanvasStrokeTransformBehavior::Fixed);

        auto testFactory = Make<StubD2DFactoryWithCreateStrokeStyle>();
        auto realizedD2DStrokeStyle = canvasStrokeStyle->GetRealizedD2DStrokeStyle(testFactory.Get());

        // Verify a resource was realized.
        Assert::IsNotNull(realizedD2DStrokeStyle.Get());
        Assert::AreEqual(1, testFactory->m_numCallsToCreateStrokeStyle);

        // Test all the getters again to ensure no unexpected state problems.
        canvasStrokeStyle->get_StartCap(&capStyle);
        Assert::AreEqual(CanvasCapStyle::Square, capStyle);
        Assert::AreEqual(D2D1_CAP_STYLE_SQUARE, testFactory->m_startCap);

        canvasStrokeStyle->get_EndCap(&capStyle);
        Assert::AreEqual(CanvasCapStyle::Round, capStyle);
        Assert::AreEqual(D2D1_CAP_STYLE_ROUND, testFactory->m_endCap);

        canvasStrokeStyle->get_DashCap(&capStyle);
        Assert::AreEqual(CanvasCapStyle::Triangle, capStyle);
        Assert::AreEqual(D2D1_CAP_STYLE_TRIANGLE, testFactory->m_dashCap);

        canvasStrokeStyle->get_LineJoin(&lineJoin);
        Assert::AreEqual(CanvasLineJoin::Round, lineJoin);
        Assert::AreEqual(D2D1_LINE_JOIN_ROUND, testFactory->m_lineJoin);

        canvasStrokeStyle->get_MiterLimit(&miterLimit);
        Assert::AreEqual(50.0f, miterLimit);
        Assert::AreEqual(50.0f, testFactory->m_miterLimit);

        //
        // This returns the dash style from before (Dot), although the
        // dash style actually seen by D2D is CUSTOM, because this stroke style
        // has a dash array.
        //
        canvasStrokeStyle->get_DashStyle(&dashStyle);
        Assert::AreEqual(CanvasDashStyle::Dot, dashStyle);
        Assert::AreEqual(D2D1_DASH_STYLE_CUSTOM, testFactory->m_dashStyle);

        canvasStrokeStyle->get_DashOffset(&dashOffset);
        Assert::AreEqual(123.0f, dashOffset);
        Assert::AreEqual(123.0f, testFactory->m_dashOffset);

        canvasStrokeStyle->get_CustomDashStyle(&customDashElementCount, &customDashElements);
        Assert::AreEqual(4u, customDashElementCount);
        Assert::IsNotNull(customDashElements);
        Assert::AreEqual(1.0f, customDashElements[0]);
        Assert::AreEqual(2.0f, customDashElements[1]);
        Assert::AreEqual(3.0f, customDashElements[2]);
        Assert::AreEqual(4.0f, customDashElements[3]);
        Assert::AreEqual(4u, static_cast<UINT32>(testFactory->m_customDashElements.size()));
        Assert::AreEqual(1.0f, testFactory->m_customDashElements[0]);
        Assert::AreEqual(2.0f, testFactory->m_customDashElements[1]);
        Assert::AreEqual(3.0f, testFactory->m_customDashElements[2]);
        Assert::AreEqual(4.0f, testFactory->m_customDashElements[3]);

        canvasStrokeStyle->get_TransformBehavior(&transformBehavior);
        Assert::AreEqual(CanvasStrokeTransformBehavior::Fixed, transformBehavior);
        Assert::AreEqual(D2D1_STROKE_TRANSFORM_TYPE_FIXED, testFactory->m_transformBehavior);

        // Some extra testing to verify the behavior of custom vs. non-custom dash styles.
        canvasStrokeStyle->put_CustomDashStyle(0, NULL);

        // Re-realize
        realizedD2DStrokeStyle = canvasStrokeStyle->GetRealizedD2DStrokeStyle(testFactory.Get());
        Assert::IsNotNull(realizedD2DStrokeStyle.Get());
        Assert::AreEqual(2, testFactory->m_numCallsToCreateStrokeStyle);

        Assert::AreEqual(CanvasDashStyle::Dot, dashStyle);
        Assert::AreEqual(D2D1_DASH_STYLE_DOT, testFactory->m_dashStyle);
    }

    class RealizationBehaviorVerifier
    {
    public:
        RealizationBehaviorVerifier(
            ComPtr<canvas::CanvasStrokeStyle> canvasStrokeStyle,
            ComPtr<StubD2DFactoryWithCreateStrokeStyle> testFactory)
            : m_realizationCounter(0)
            , m_canvasStrokeStyle(canvasStrokeStyle)
            , m_testFactory(testFactory)
        {}

        template<typename STROKE_STYLE_PROPERTY>
        void VerifyProperty(
            STROKE_STYLE_PROPERTY&& fn)
        {
            auto realizedD2DStrokeStyle = m_canvasStrokeStyle->GetRealizedD2DStrokeStyle(m_testFactory.Get());
            m_realizationCounter++;
            Assert::IsNotNull(realizedD2DStrokeStyle.Get());
            Assert::AreEqual(m_realizationCounter, m_testFactory->m_numCallsToCreateStrokeStyle);

            fn();

            Assert::AreEqual(m_realizationCounter, m_testFactory->m_numCallsToCreateStrokeStyle);
        }
    private:

        int m_realizationCounter;
        ComPtr<canvas::CanvasStrokeStyle> m_canvasStrokeStyle;
        ComPtr<StubD2DFactoryWithCreateStrokeStyle> m_testFactory;
    };

    TEST_METHOD(CanvasStrokeStyle_Setters_Invalidate_Realization)
    {
        using canvas::CanvasStrokeStyle;

        auto canvasStrokeStyle = Make<CanvasStrokeStyle>();
        auto testFactory = Make<StubD2DFactoryWithCreateStrokeStyle>();

        RealizationBehaviorVerifier verifier(canvasStrokeStyle, testFactory);

        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_StartCap(CanvasCapStyle::Triangle); });

        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_EndCap(CanvasCapStyle::Triangle); });

        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_DashCap(CanvasCapStyle::Triangle); });

        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_LineJoin(CanvasLineJoin::Round); });

        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_MiterLimit(123.0f); });

        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_DashStyle(CanvasDashStyle::Solid); });

        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_DashOffset(1.0f); });

        float customDashPattern[6] = { 0, 2, 0, 1, 44, 55};
        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_CustomDashStyle(6, customDashPattern); });

        verifier.VerifyProperty(
            [&](){canvasStrokeStyle->put_TransformBehavior(CanvasStrokeTransformBehavior::Fixed); });
    }

    TEST_METHOD(CanvasStrokeStyle_Closed)
    {
        using canvas::CanvasStrokeStyle;

        auto testFactory = Make<StubD2DFactoryWithCreateStrokeStyle>();

        CanvasCapStyle capStyle;
        CanvasLineJoin lineJoin;
        float miterLimit;
        CanvasDashStyle dashStyle;
        float dashOffset;
        UINT32 customDashElementCount = 0;
        float* customDashElements = nullptr;
        CanvasStrokeTransformBehavior transformBehavior;

        // Test for each of the realized, vs. non-realized cases.
        for (int i = 0; i < 2; ++i)
        {
            auto canvasStrokeStyle = Make<CanvasStrokeStyle>();

            Assert::IsNotNull(canvasStrokeStyle.Get());

            if (i == 1) canvasStrokeStyle->GetRealizedD2DStrokeStyle(testFactory.Get());

            Assert::AreEqual(S_OK, canvasStrokeStyle->Close());

            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_StartCap(&capStyle));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_EndCap(&capStyle));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_DashCap(&capStyle));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_LineJoin(&lineJoin));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_MiterLimit(&miterLimit));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_DashStyle(&dashStyle));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_DashOffset(&dashOffset));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_CustomDashStyle(&customDashElementCount, &customDashElements));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->get_TransformBehavior(&transformBehavior));

            Assert::AreEqual(0u, customDashElementCount);
            Assert::IsNull(customDashElements);

            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_StartCap(CanvasCapStyle::Flat));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_EndCap(CanvasCapStyle::Flat));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_DashCap(CanvasCapStyle::Flat));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_LineJoin(CanvasLineJoin::Miter));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_MiterLimit(10.0f));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_DashStyle(CanvasDashStyle::Solid));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_DashOffset(0.0f));
            float customDashPattern[4] = { 1, 2, 3, 4 };
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_CustomDashStyle(4, customDashPattern));
            Assert::AreEqual(RO_E_CLOSED, canvasStrokeStyle->put_TransformBehavior(CanvasStrokeTransformBehavior::Normal));
        }
    }

};

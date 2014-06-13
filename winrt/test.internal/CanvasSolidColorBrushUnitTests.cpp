// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "TestDeviceResourceCreationAdapter.h"

struct SolidColorBrushCounters
{
    SolidColorBrushCounters()
        : NumSetColorCalls(0)
        , NumGetColorCalls(0)
        , NumSetOpacityCalls(0)
        , NumGetOpacityCalls(0)
        , NumSetTransformCalls(0)
        , NumGetTransformCalls(0)
    {}

    int NumSetColorCalls;
    int NumGetColorCalls;

    int NumSetOpacityCalls;
    int NumGetOpacityCalls;

    int NumSetTransformCalls;
    int NumGetTransformCalls;
};

class TestD2DSolidColorBrush : public MockD2DSolidColorBrush
{
public:
    TestD2DSolidColorBrush(const D2D1_COLOR_F& color, SolidColorBrushCounters* counters)
        : m_color(color)
        , m_opacity(1.0f)
        , m_counters(counters)
    {
        m_transform = D2D1::Matrix3x2F::Identity();
    }

    //
    // ID2D1SolidColorBrush
    //
    STDMETHOD_(void, SetColor)(_In_ CONST D2D1_COLOR_F *color) override
    {
        m_counters->NumSetColorCalls++;
        m_color = *color;
    }

    STDMETHOD_(D2D1_COLOR_F, GetColor)() CONST override
    {
        m_counters->NumGetColorCalls++;
        return m_color;
    }

    //
    // ID2D1Brush
    //
    STDMETHOD_(void, SetOpacity)(FLOAT opacity) override
    {
        m_counters->NumSetOpacityCalls++;
        m_opacity = opacity;
    }

    STDMETHOD_(FLOAT, GetOpacity)() CONST override
    {
        m_counters->NumGetOpacityCalls++;
        return m_opacity;
    }

    STDMETHOD_(void, SetTransform)(_In_ CONST D2D1_MATRIX_3X2_F *transform) override
    {
        m_counters->NumSetTransformCalls++;
        m_transform = *transform;
    }

    STDMETHOD_(void, GetTransform)(_Out_ D2D1_MATRIX_3X2_F *transform) CONST override
    {
        m_counters->NumGetTransformCalls++;
        *transform = m_transform;
    }

    D2D1_COLOR_F m_color;
    D2D1_MATRIX_3X2_F m_transform;
    float m_opacity;

    SolidColorBrushCounters* m_counters;
};

class TestCanvasSolidColorBrushResourceCreationAdapter : public ICanvasSolidColorBrushResourceCreationAdapter
{
public:
    ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(
        ID2D1DeviceContext* deviceContext, 
        ABI::Windows::UI::Color color
        ) override
    {
        m_d2dSolidColorBrush = Make<TestD2DSolidColorBrush>(ToD2DColor(color), &m_counters);
        return m_d2dSolidColorBrush;
    }

    ComPtr<TestD2DSolidColorBrush> m_d2dSolidColorBrush;
    SolidColorBrushCounters m_counters;
};

TEST_CLASS(CanvasSolidColorBrushTests)
{
public:

    TEST_METHOD(CanvasSolidColorBrush_Properties)
    {
        using canvas::CanvasSolidColorBrush;
        using canvas::CanvasDevice;
        using ABI::Windows::UI::Color;
        
        auto deviceResourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
        auto deviceManager = std::make_shared<CanvasDeviceManager>(deviceResourceCreationAdapter);

        auto canvasDevice = deviceManager->Create(
            CanvasDebugLevel::Information,
            CanvasHardwareAcceleration::Auto);

        Color red = { 255, 255, 0, 0 };
        Color cyan = { 255, 0, 255, 255 };
        D2D1_COLOR_F d2dRed = D2D1::ColorF(1, 0, 0);
        D2D1_COLOR_F d2dCyan = D2D1::ColorF(0, 1, 1);
        D2D1_MATRIX_3X2_F d2dIdentityMatrix = D2D1::Matrix3x2F::Identity();

        std::shared_ptr<TestCanvasSolidColorBrushResourceCreationAdapter> brushResourceCreationAdapter = 
            std::make_shared<TestCanvasSolidColorBrushResourceCreationAdapter>();

        auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
            canvasDevice.Get(),
            red,
            brushResourceCreationAdapter
            );

        ComPtr<TestD2DSolidColorBrush> testD2DSolidColorBrush = brushResourceCreationAdapter->m_d2dSolidColorBrush;

        // Verify the brush resource got initialized correctly.
        Assert::AreEqual(d2dRed, brushResourceCreationAdapter->m_d2dSolidColorBrush->m_color);
        Assert::AreEqual(1.0f, brushResourceCreationAdapter->m_d2dSolidColorBrush->m_opacity);
        Assert::AreEqual(d2dIdentityMatrix, brushResourceCreationAdapter->m_d2dSolidColorBrush->m_transform);
        Assert::AreEqual(0, brushResourceCreationAdapter->m_counters.NumGetColorCalls);
        Assert::AreEqual(0, brushResourceCreationAdapter->m_counters.NumSetColorCalls);
        Assert::AreEqual(0, brushResourceCreationAdapter->m_counters.NumGetOpacityCalls);
        Assert::AreEqual(0, brushResourceCreationAdapter->m_counters.NumSetOpacityCalls);
        Assert::AreEqual(0, brushResourceCreationAdapter->m_counters.NumGetTransformCalls);
        Assert::AreEqual(0, brushResourceCreationAdapter->m_counters.NumSetTransformCalls);

        // Verify the Color getter returns the right thing.
        Color colorActual;
        canvasSolidColorBrush->get_Color(&colorActual);
        Assert::AreEqual(red, colorActual);
        Assert::AreEqual(1, brushResourceCreationAdapter->m_counters.NumGetColorCalls);

        // Set the Color to a new value, and verify it is correct.
        canvasSolidColorBrush->put_Color(cyan);
        Assert::AreEqual(1, brushResourceCreationAdapter->m_counters.NumSetColorCalls);
        Assert::AreEqual(d2dCyan, brushResourceCreationAdapter->m_d2dSolidColorBrush->m_color);

        // Verify the getter works once more.
        canvasSolidColorBrush->get_Color(&colorActual);
        Assert::AreEqual(cyan, colorActual);
        Assert::AreEqual(2, brushResourceCreationAdapter->m_counters.NumGetColorCalls);

        // put_Opacity
        canvasSolidColorBrush->put_Opacity(0.75f);
        Assert::AreEqual(0.75f, brushResourceCreationAdapter->m_d2dSolidColorBrush->m_opacity);
        Assert::AreEqual(1, brushResourceCreationAdapter->m_counters.NumSetOpacityCalls);

        // get_Opacity
        float opacityActual = 0;
        canvasSolidColorBrush->get_Opacity(&opacityActual);
        Assert::AreEqual(0.75f, opacityActual);
        Assert::AreEqual(1, brushResourceCreationAdapter->m_counters.NumGetOpacityCalls);
        
        // put_Transform
        Math::Matrix3x2 scaleAndTranslate = { 2, 0, 0, 2, 5, 10 };
        D2D1_MATRIX_3X2_F d2dScaleAndTranslate = D2D1::Matrix3x2F(2, 0, 0, 2, 5, 10);
        canvasSolidColorBrush->put_Transform(scaleAndTranslate);
        Assert::AreEqual(d2dScaleAndTranslate, brushResourceCreationAdapter->m_d2dSolidColorBrush->m_transform);
        Assert::AreEqual(1, brushResourceCreationAdapter->m_counters.NumSetTransformCalls);

        // get_Transform
        Math::Matrix3x2 transformActual;
        canvasSolidColorBrush->get_Transform(&transformActual);
        Assert::AreEqual(scaleAndTranslate, transformActual);
        Assert::AreEqual(1, brushResourceCreationAdapter->m_counters.NumGetTransformCalls);
    }

    TEST_METHOD(CanvasSolidColorBrush_Closed)
    {
        using canvas::CanvasSolidColorBrush;
        using canvas::CanvasDevice;
        using ABI::Windows::UI::Color;

        auto deviceResourceCreationAdapter = std::make_shared<TestDeviceResourceCreationAdapter>();
        auto deviceManager = std::make_shared<CanvasDeviceManager>(deviceResourceCreationAdapter);

        auto canvasDevice = deviceManager->Create(
            CanvasDebugLevel::Information,
            CanvasHardwareAcceleration::Auto);

        Color color = { 255, 127, 127, 127 };

        std::shared_ptr<TestCanvasSolidColorBrushResourceCreationAdapter> brushResourceCreationAdapter =
            std::make_shared<TestCanvasSolidColorBrushResourceCreationAdapter>();

        auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
            canvasDevice.Get(),
            color,
            brushResourceCreationAdapter
            );

        Assert::IsNotNull(canvasSolidColorBrush.Get());
        Assert::AreEqual(S_OK, canvasSolidColorBrush->Close());

        Color colorActual;
        Assert::AreEqual(RO_E_CLOSED, canvasSolidColorBrush->get_Color(&colorActual));

        Assert::AreEqual(RO_E_CLOSED, canvasSolidColorBrush->put_Color(color));

        float opacityActual;
        Assert::AreEqual(RO_E_CLOSED, canvasSolidColorBrush->get_Opacity(&opacityActual));

        Assert::AreEqual(RO_E_CLOSED, canvasSolidColorBrush->put_Opacity(1));

        Math::Matrix3x2 transformActual;
        Assert::AreEqual(RO_E_CLOSED, canvasSolidColorBrush->get_Transform(&transformActual));

        Assert::AreEqual(RO_E_CLOSED, canvasSolidColorBrush->put_Transform(transformActual));
    }

};

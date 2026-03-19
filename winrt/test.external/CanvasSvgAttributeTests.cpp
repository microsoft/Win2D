// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas::Svg;
using namespace Windows::Storage::Streams;
using namespace Microsoft::UI;

TEST_CLASS(CanvasSvgAttributeTests)
{
    CanvasDevice^ m_device;
    bool m_isSupported;

    CanvasSvgDocument^ m_document;
    ComPtr<ID2D1SvgDocument> m_nativeDocument;


public:
    CanvasSvgAttributeTests()
    {
        m_device = ref new CanvasDevice();
        m_isSupported = CanvasSvgDocument::IsSupported(m_device);

        if (m_isSupported)
        {
            m_document = CanvasSvgDocument::LoadFromXml(m_device, "<svg/>");
            m_nativeDocument = GetWrappedResource<ID2D1SvgDocument>(m_document);
        }
    }

    TEST_METHOD(CanvasSvgAttribute_Paint_NativeInterop)
    {
        if (!m_isSupported)
            return;

        ComPtr<ID2D1SvgPaint> d2dAttribute;
        ThrowIfFailed(m_nativeDocument->CreatePaint(D2D1_SVG_PAINT_TYPE_NONE, nullptr, nullptr, &d2dAttribute));
        
        auto canvasAttribute = GetOrCreate<CanvasSvgPaintAttribute>(m_device, d2dAttribute.Get());

        auto retrievedNativeAttribute = GetWrappedResource<ID2D1SvgPaint>(canvasAttribute);

        Assert::IsTrue(IsSameInstance(d2dAttribute.Get(), retrievedNativeAttribute.Get()));
    }

    TEST_METHOD(CanvasSvgAttribute_CreateAll)
    {
        if (!m_isSupported)
            return;
        
        CanvasSvgPaintAttribute^ paint = m_document->CreatePaintAttribute();
        CanvasSvgPaintAttribute^ paint2 = m_document->CreatePaintAttribute(CanvasSvgPaintType::None, Windows::UI::Color{}, L"");

        CanvasSvgPathAttribute^ path = m_document->CreatePathAttribute();
        CanvasSvgPathAttribute^ path2 = m_document->CreatePathAttribute(ref new Platform::Array<float>(0), ref new Platform::Array<CanvasSvgPathCommand>(0));
        CanvasSvgPathAttribute^ path3 = m_document->CreatePathAttribute(ref new Platform::Array<float>(2), ref new Platform::Array<CanvasSvgPathCommand>(1) { CanvasSvgPathCommand::MoveAbsolute});

        CanvasSvgPointsAttribute^ points = m_document->CreatePointsAttribute();
        CanvasSvgPointsAttribute^ points2 = m_document->CreatePointsAttribute(ref new Platform::Array<float2>(0));
        CanvasSvgPointsAttribute^ points3 = m_document->CreatePointsAttribute(ref new Platform::Array<float2>(1));

        CanvasSvgStrokeDashArrayAttribute^ dashes = m_document->CreateStrokeDashArrayAttribute();
        CanvasSvgStrokeDashArrayAttribute^ dashes2 = m_document->CreateStrokeDashArrayAttribute(ref new Platform::Array<float>(0), ref new Platform::Array<CanvasSvgLengthUnits>(0));
        CanvasSvgStrokeDashArrayAttribute^ dashes3 = m_document->CreateStrokeDashArrayAttribute(ref new Platform::Array<float>(1), ref new Platform::Array<CanvasSvgLengthUnits>(1));
    }

    TEST_METHOD(CanvasSvgAttribute_Path_NativeInterop)
    {
        if (!m_isSupported)
            return;

        ComPtr<ID2D1SvgPathData> d2dAttribute;
        ThrowIfFailed(m_nativeDocument->CreatePathData(nullptr, 0, nullptr, 0, &d2dAttribute));

        auto canvasAttribute = GetOrCreate<CanvasSvgPathAttribute>(m_device, d2dAttribute.Get());

        auto retrievedNativeAttribute = GetWrappedResource<ID2D1SvgPathData>(canvasAttribute);

        Assert::IsTrue(IsSameInstance(d2dAttribute.Get(), retrievedNativeAttribute.Get()));
    }

    TEST_METHOD(CanvasSvgAttribute_Points_NativeInterop)
    {
        if (!m_isSupported)
            return;

        ComPtr<ID2D1SvgPointCollection> d2dAttribute;
        ThrowIfFailed(m_nativeDocument->CreatePointCollection(nullptr, 0, &d2dAttribute));

        auto canvasAttribute = GetOrCreate<CanvasSvgPointsAttribute>(m_device, d2dAttribute.Get());

        auto retrievedNativeAttribute = GetWrappedResource<ID2D1SvgPointCollection>(canvasAttribute);

        Assert::IsTrue(IsSameInstance(d2dAttribute.Get(), retrievedNativeAttribute.Get()));
    }

    TEST_METHOD(CanvasSvgAttribute_StrokeDashArray_NativeInterop)
    {
        if (!m_isSupported)
            return;

        ComPtr<ID2D1SvgStrokeDashArray> d2dAttribute;
        ThrowIfFailed(m_nativeDocument->CreateStrokeDashArray(nullptr, 0, &d2dAttribute));

        auto canvasAttribute = GetOrCreate<CanvasSvgStrokeDashArrayAttribute>(m_device, d2dAttribute.Get());

        auto retrievedNativeAttribute = GetWrappedResource<ID2D1SvgStrokeDashArray>(canvasAttribute);

        Assert::IsTrue(IsSameInstance(d2dAttribute.Get(), retrievedNativeAttribute.Get()));
    }

    TEST_METHOD(CanvasSvgAttribute_SetArraysToBeLarger)
    {
        if (!m_isSupported)
            return;

        // Verifies that arrays can be resized, and that the correct elements are written.
        
        {
            auto pathAttribute = m_document->CreatePathAttribute();
            pathAttribute->SetCommands(0, ref new Platform::Array<CanvasSvgPathCommand>(1) { CanvasSvgPathCommand::MoveAbsolute });
            pathAttribute->SetCommands(0, ref new Platform::Array<CanvasSvgPathCommand>(2) { CanvasSvgPathCommand::MoveRelative, CanvasSvgPathCommand::ClosePath });

            auto commands = pathAttribute->GetCommands(0, 1);
            Assert::AreEqual(1u, commands->Length);
            Assert::AreEqual(CanvasSvgPathCommand::MoveRelative, commands->Data[0]);

            commands = pathAttribute->GetCommands(1, 1);
            Assert::AreEqual(1u, commands->Length);
            Assert::AreEqual(CanvasSvgPathCommand::ClosePath, commands->Data[0]);
        }
        {
            auto pointsAttribute = m_document->CreatePointsAttribute();
            pointsAttribute->SetPoints(0, ref new Platform::Array<float2>(1) { float2{ 1, 2 } });
            pointsAttribute->SetPoints(0, ref new Platform::Array<float2>(2) { float2{ 3, 4 }, float2{ 5, 6 } });

            auto points = pointsAttribute->GetPoints(0, 1);
            Assert::AreEqual(1u, points->Length);
            Assert::AreEqual(float2{ 3, 4 }, points->Data[0]);

            points = pointsAttribute->GetPoints(1, 1);
            Assert::AreEqual(1u, points->Length);
            Assert::AreEqual(float2{ 5, 6 }, points->Data[0]);
        }
        {
            auto strokeDashArrayAttribute = m_document->CreateStrokeDashArrayAttribute();
            strokeDashArrayAttribute->SetDashes(0, ref new Platform::Array<float>(1) { 1.0f });

            auto strokeDashArray = strokeDashArrayAttribute->GetDashes();
            Assert::AreEqual(1u, strokeDashArray->Length);
            Assert::AreEqual(1.0f, strokeDashArray->Data[0]);

            strokeDashArrayAttribute->SetDashes(0, ref new Platform::Array<float>(1) { 2.0f }, ref new Platform::Array<CanvasSvgLengthUnits>(1) { CanvasSvgLengthUnits::Percentage });
            Platform::Array<CanvasSvgLengthUnits>^ units;
            strokeDashArray = strokeDashArrayAttribute->GetDashes(0, 1, &units);
            Assert::AreEqual(1u, strokeDashArray->Length);
            Assert::AreEqual(2.0f, strokeDashArray->Data[0]);
            Assert::AreEqual(1u, units->Length);
            Assert::AreEqual(CanvasSvgLengthUnits::Percentage, units->Data[0]);
        }
    }

    TEST_METHOD(CanvasSvgAttribute_StrokeDashArrayValidatesMismatchingSizes)
    {
        if (!m_isSupported)
            return;

        auto strokeDashArrayAttribute = m_document->CreateStrokeDashArrayAttribute();        

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                strokeDashArrayAttribute->SetDashes(0, 
                    ref new Platform::Array<float>(1) { 2.0f },
                    ref new Platform::Array<CanvasSvgLengthUnits>(2) { CanvasSvgLengthUnits::Percentage, CanvasSvgLengthUnits::Percentage});
            });
    }
};
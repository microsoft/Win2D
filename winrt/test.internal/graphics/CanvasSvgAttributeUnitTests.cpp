// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#if WINVER > _WIN32_WINNT_WINBLUE

#include <lib/svg/CanvasSvgPaintAttribute.h>
#include <lib/svg/CanvasSvgPointsAttribute.h>
#include <lib/svg/CanvasSvgPathAttribute.h>
#include <lib/svg/CanvasSvgStrokeDashArrayAttribute.h>
#include <lib/svg/CanvasSvgElement.h>
#include <lib/svg/CanvasSvgDocument.h>
#include "mocks/MockD2DSvgPaint.h"
#include "mocks/MockD2DSvgPathData.h"
#include "mocks/MockD2DSvgPointCollection.h"
#include "mocks/MockD2DSvgStrokeDashArray.h"
#include "mocks/MockD2DSvgDocument.h"
#include "mocks/MockD2DSvgElement.h"
#include "mocks/MockD2DPathGeometry.h"

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas::Svg;
    
    TEST_CLASS(CanvasSvgAttributeTests)
    {
        struct Fixture
        {
            ComPtr<StubCanvasDevice> m_canvasDevice;
            ComPtr<ICanvasSvgDocument> m_svgDocument;

            ComPtr<MockD2DSvgPaint> m_mockD2DPaint;
            ComPtr<MockD2DSvgPathData> m_mockD2DPathData;
            ComPtr<MockD2DSvgPointCollection> m_mockD2DPointCollection;
            ComPtr<MockD2DSvgStrokeDashArray> m_mockD2DStrokeDashArray;

            Fixture()
            {
                m_canvasDevice = Make<StubCanvasDevice>();
                auto mockD2DDocument = Make<MockD2DSvgDocument>();
                
                m_mockD2DPaint = Make<MockD2DSvgPaint>();
                m_mockD2DPointCollection = Make<MockD2DSvgPointCollection>();
                m_mockD2DPathData = Make<MockD2DSvgPathData>();
                m_mockD2DStrokeDashArray = Make<MockD2DSvgStrokeDashArray>();

                mockD2DDocument->CreatePaintMethod.AllowAnyCall(
                    [=](D2D1_SVG_PAINT_TYPE, CONST D3DCOLORVALUE*, PCWSTR, ID2D1SvgPaint** result)
                    {
                        return m_mockD2DPaint.CopyTo(result);
                    });

                mockD2DDocument->CreatePathDataMethod.AllowAnyCall(
                    [=](CONST FLOAT*, UINT32, CONST D2D1_SVG_PATH_COMMAND*, UINT32, ID2D1SvgPathData** result)
                    {
                        return m_mockD2DPathData.CopyTo(result);
                    });

                mockD2DDocument->CreatePointCollectionMethod.AllowAnyCall(
                    [=](CONST D2D1_POINT_2F*, UINT32, ID2D1SvgPointCollection** result)
                    {
                        return m_mockD2DPointCollection.CopyTo(result);
                    });

                mockD2DDocument->CreateStrokeDashArrayMethod.AllowAnyCall(
                    [=](CONST D2D1_SVG_LENGTH*, UINT32, ID2D1SvgStrokeDashArray** result)
                    {
                        return m_mockD2DStrokeDashArray.CopyTo(result);
                    });

                m_svgDocument = ResourceManager::GetOrCreate<ICanvasSvgDocument>(m_canvasDevice.Get(), mockD2DDocument.Get());
            }

            ComPtr<CanvasSvgPaintAttribute> CreateSvgPaintAttribute()
            {
                ComPtr<ICanvasSvgPaintAttribute> attribute;
                Assert::AreEqual(S_OK, m_svgDocument->CreatePaintAttributeWithDefaults(&attribute));

                ComPtr<CanvasSvgPaintAttribute> implementation = static_cast<CanvasSvgPaintAttribute*>(attribute.Get());
                return implementation;
            }

            ComPtr<CanvasSvgPathAttribute> CreateSvgPathAttribute()
            {
                ComPtr<ICanvasSvgPathAttribute> attribute;
                Assert::AreEqual(S_OK, m_svgDocument->CreatePathAttributeWithDefaults(&attribute));

                ComPtr<CanvasSvgPathAttribute> implementation = static_cast<CanvasSvgPathAttribute*>(attribute.Get());
                return implementation;
            }

            ComPtr<CanvasSvgPointsAttribute> CreateSvgPointsAttribute()
            {
                ComPtr<ICanvasSvgPointsAttribute> attribute;
                Assert::AreEqual(S_OK, m_svgDocument->CreatePointsAttributeWithDefaults(&attribute));

                ComPtr<CanvasSvgPointsAttribute> implementation = static_cast<CanvasSvgPointsAttribute*>(attribute.Get());
                return implementation;
            }

            ComPtr<CanvasSvgStrokeDashArrayAttribute> CreateSvgStrokeDashArrayAttribute()
            {
                ComPtr<ICanvasSvgStrokeDashArrayAttribute> attribute;
                Assert::AreEqual(S_OK, m_svgDocument->CreateStrokeDashArrayAttributeWithDefaults(&attribute));

                ComPtr<CanvasSvgStrokeDashArrayAttribute> implementation = static_cast<CanvasSvgStrokeDashArrayAttribute*>(attribute.Get());
                return implementation;
            }

            static ComPtr<MockD2DSvgElement> CreateMockD2DElement()
            {
                ComPtr<MockD2DSvgElement> mockD2DSvgElement = Make<MockD2DSvgElement>();
                mockD2DSvgElement->IsTextContentMethod.AllowAnyCall(
                    [=]
                    {
                        return FALSE;
                    });

                return mockD2DSvgElement;
            }
        };

        TEST_METHOD_EX(CanvasSvgAttributeTests_Implements_Expected_Interfaces)
        {
            Fixture f;

            {
                auto paint = f.CreateSvgPaintAttribute();

                ASSERT_IMPLEMENTS_INTERFACE(paint, ICanvasSvgPaintAttribute);
                ASSERT_IMPLEMENTS_INTERFACE(paint, ICanvasSvgAttribute);
                ASSERT_IMPLEMENTS_INTERFACE(paint, ABI::Windows::Foundation::IClosable);
                ASSERT_IMPLEMENTS_INTERFACE(paint, ICanvasResourceWrapperNative);
            }
        }

        void VerifyCloseWithCommonMembers(ICanvasSvgAttribute* attribute)
        {
            ComPtr<ICanvasSvgAttribute> clone;
            Assert::AreEqual(RO_E_CLOSED, attribute->Clone(&clone));

            ComPtr<ICanvasSvgNamedElement> element;
            Assert::AreEqual(RO_E_CLOSED, attribute->GetElement(&element));

            ComPtr<ICanvasDevice> device;
            Assert::AreEqual(RO_E_CLOSED, attribute->get_Device(&device));
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Closure)
        {
            Fixture f;

            {
                auto paint = f.CreateSvgPaintAttribute();

                Assert::AreEqual(S_OK, paint->Close());

                CanvasSvgPaintType paintType;
                Assert::AreEqual(RO_E_CLOSED, paint->get_PaintType(&paintType));
                Assert::AreEqual(RO_E_CLOSED, paint->put_PaintType(paintType));

                Color color;
                Assert::AreEqual(RO_E_CLOSED, paint->get_Color(&color));
                Assert::AreEqual(RO_E_CLOSED, paint->put_Color(color));

                HSTRING string;
                Assert::AreEqual(RO_E_CLOSED, paint->get_Id(&string));
                Assert::AreEqual(RO_E_CLOSED, paint->put_Id(WinString(L"id")));

                VerifyCloseWithCommonMembers(paint.Get());
            }
            {
                auto path = f.CreateSvgPathAttribute();

                Assert::AreEqual(S_OK, path->Close());

                ComPtr<ICanvasGeometry> geometry;
                Assert::AreEqual(RO_E_CLOSED, path->CreatePathGeometry(&geometry));
                Assert::AreEqual(RO_E_CLOSED, path->CreatePathGeometryWithFill(CanvasFilledRegionDetermination{}, &geometry));

                uint32_t u;
                CanvasSvgPathCommand* commandArray;
                Assert::AreEqual(RO_E_CLOSED, path->get_Commands(&u, &commandArray));
                Assert::AreEqual(RO_E_CLOSED, path->GetCommands(1, 2, &u, &commandArray));

                float* segmentArray;
                Assert::AreEqual(RO_E_CLOSED, path->get_SegmentData(&u, &segmentArray));
                Assert::AreEqual(RO_E_CLOSED, path->GetSegmentData(1, 2, &u, &segmentArray));

                Assert::AreEqual(RO_E_CLOSED, path->RemoveCommandsAtEnd(1));
                Assert::AreEqual(RO_E_CLOSED, path->RemoveSegmentDataAtEnd(1));

                commandArray = reinterpret_cast<CanvasSvgPathCommand*>(0x1234);
                segmentArray = reinterpret_cast<float*>(0x1234);
                Assert::AreEqual(RO_E_CLOSED, path->SetCommands(1, 2, commandArray));
                Assert::AreEqual(RO_E_CLOSED, path->SetSegmentData(1, 2, segmentArray));

                VerifyCloseWithCommonMembers(path.Get());
            }
            {
                auto points = f.CreateSvgPointsAttribute();

                Assert::AreEqual(S_OK, points->Close());

                uint32_t u;
                Vector2* pointArray;
                Assert::AreEqual(RO_E_CLOSED, points->get_Points(&u, &pointArray));
                Assert::AreEqual(RO_E_CLOSED, points->GetPoints(1, 2, &u, &pointArray));

                Assert::AreEqual(RO_E_CLOSED, points->RemovePointsAtEnd(1));

                pointArray = reinterpret_cast<Vector2*>(0x1234);
                Assert::AreEqual(RO_E_CLOSED, points->SetPoints(1, 2, pointArray));

                VerifyCloseWithCommonMembers(points.Get());
            }
            {
                auto dashes = f.CreateSvgStrokeDashArrayAttribute();

                Assert::AreEqual(S_OK, dashes->Close());

                uint32_t u;
                float* dashArray;
                CanvasSvgLengthUnits* unitsArray;
                Assert::AreEqual(RO_E_CLOSED, dashes->GetDashes(&u, &dashArray));
                Assert::AreEqual(RO_E_CLOSED, dashes->GetDashesWithUnits(1, 2, &u, &unitsArray, &u, &dashArray));

                dashArray = reinterpret_cast<float*>(0x1234);
                unitsArray = reinterpret_cast<CanvasSvgLengthUnits*>(0x1234);
                Assert::AreEqual(RO_E_CLOSED, dashes->SetDashes(1, 2, dashArray));
                Assert::AreEqual(RO_E_CLOSED, dashes->SetDashesWithUnit(1, 2, dashArray, CanvasSvgLengthUnits::Number));
                Assert::AreEqual(RO_E_CLOSED, dashes->SetDashesWithUnits(1, 2, dashArray, 1, unitsArray));

                VerifyCloseWithCommonMembers(dashes.Get());
            }
        }

        void VerifyNullArgsWithCommonMembers(ICanvasSvgAttribute* attribute)
        {
            Assert::AreEqual(E_INVALIDARG, attribute->Clone(nullptr));

            Assert::AreEqual(E_INVALIDARG, attribute->GetElement(nullptr));

            Assert::AreEqual(E_INVALIDARG, attribute->get_Device(nullptr));
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_NullArgs)
        {
            Fixture f;
            {
                auto paint = f.CreateSvgPaintAttribute();

                Assert::AreEqual(E_INVALIDARG, paint->get_PaintType(nullptr));

                Assert::AreEqual(E_INVALIDARG, paint->get_Color(nullptr));

                Assert::AreEqual(E_INVALIDARG, paint->get_Id(nullptr));

                VerifyNullArgsWithCommonMembers(paint.Get());
            }
            {
                auto path = f.CreateSvgPathAttribute();

                Assert::AreEqual(E_INVALIDARG, path->CreatePathGeometry(nullptr));
                Assert::AreEqual(E_INVALIDARG, path->CreatePathGeometryWithFill(CanvasFilledRegionDetermination{}, nullptr));

                uint32_t u;
                CanvasSvgPathCommand* commandArray;
                Assert::AreEqual(E_INVALIDARG, path->get_Commands(nullptr, &commandArray));
                Assert::AreEqual(E_INVALIDARG, path->get_Commands(&u, nullptr));
                Assert::AreEqual(E_INVALIDARG, path->GetCommands(1, 2, nullptr, &commandArray));
                Assert::AreEqual(E_INVALIDARG, path->GetCommands(1, 2, &u, nullptr));

                float* segmentArray;
                Assert::AreEqual(E_INVALIDARG, path->get_SegmentData(nullptr, &segmentArray));
                Assert::AreEqual(E_INVALIDARG, path->get_SegmentData(&u, nullptr));
                Assert::AreEqual(E_INVALIDARG, path->GetSegmentData(1, 2, nullptr, &segmentArray));
                Assert::AreEqual(E_INVALIDARG, path->GetSegmentData(1, 2, &u, nullptr));

                Assert::AreEqual(E_INVALIDARG, path->SetCommands(1, 2, nullptr));
                Assert::AreEqual(E_INVALIDARG, path->SetSegmentData(1, 2, nullptr));

                VerifyNullArgsWithCommonMembers(path.Get());
            }
            {
                auto points = f.CreateSvgPointsAttribute();

                uint32_t u;
                Vector2* pointArray;
                Assert::AreEqual(E_INVALIDARG, points->get_Points(nullptr, &pointArray));
                Assert::AreEqual(E_INVALIDARG, points->get_Points(&u, nullptr));
                Assert::AreEqual(E_INVALIDARG, points->GetPoints(1, 2, nullptr, &pointArray));
                Assert::AreEqual(E_INVALIDARG, points->GetPoints(1, 2, &u, nullptr));

                Assert::AreEqual(E_INVALIDARG, points->SetPoints(1, 2, nullptr));

                VerifyNullArgsWithCommonMembers(points.Get());
            }
            {
                auto dashes = f.CreateSvgStrokeDashArrayAttribute();

                uint32_t u;
                float* dashArray;
                CanvasSvgLengthUnits* unitsArray;
                Assert::AreEqual(E_INVALIDARG, dashes->GetDashes(nullptr, &dashArray));
                Assert::AreEqual(E_INVALIDARG, dashes->GetDashes(&u, nullptr));
                Assert::AreEqual(E_INVALIDARG, dashes->GetDashesWithUnits(1, 2, nullptr, &unitsArray, &u, &dashArray));
                Assert::AreEqual(E_INVALIDARG, dashes->GetDashesWithUnits(1, 2, &u, nullptr, &u, &dashArray));
                Assert::AreEqual(E_INVALIDARG, dashes->GetDashesWithUnits(1, 2, &u, &unitsArray, nullptr, &dashArray));
                Assert::AreEqual(E_INVALIDARG, dashes->GetDashesWithUnits(1, 2, &u, &unitsArray, &u, nullptr));

                dashArray = reinterpret_cast<float*>(0x1234);
                unitsArray = reinterpret_cast<CanvasSvgLengthUnits*>(0x1234);
                Assert::AreEqual(E_INVALIDARG, dashes->SetDashes(1, 2, nullptr));
                Assert::AreEqual(E_INVALIDARG, dashes->SetDashesWithUnit(1, 2, nullptr, CanvasSvgLengthUnits::Number));
                Assert::AreEqual(E_INVALIDARG, dashes->SetDashesWithUnits(1, 2, nullptr, 1, unitsArray));
                Assert::AreEqual(E_INVALIDARG, dashes->SetDashesWithUnits(1, 2, dashArray, 1, nullptr));

                VerifyNullArgsWithCommonMembers(dashes.Get());
            }
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Paint_put_PaintType)
        {
            Fixture f;
            auto paint = f.CreateSvgPaintAttribute();

            f.m_mockD2DPaint->SetPaintTypeMethod.SetExpectedCalls(1,
                [=](D2D1_SVG_PAINT_TYPE paintType)
                {
                    Assert::AreEqual(D2D1_SVG_PAINT_TYPE_URI_CURRENT_COLOR, paintType);
                    return S_OK;
                });

            Assert::AreEqual(S_OK, paint->put_PaintType(CanvasSvgPaintType::UriThenCurrentColor));
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Paint_get_PaintType)
        {
            Fixture f;
            auto paint = f.CreateSvgPaintAttribute();

            f.m_mockD2DPaint->GetPaintTypeMethod.SetExpectedCalls(1,
                [=]
                {
                    return D2D1_SVG_PAINT_TYPE_URI_CURRENT_COLOR;
                });

            CanvasSvgPaintType retrieved;
            Assert::AreEqual(S_OK, paint->get_PaintType(&retrieved));

            Assert::AreEqual(CanvasSvgPaintType::UriThenCurrentColor, retrieved);
        }


        TEST_METHOD_EX(CanvasSvgAttributeTests_Paint_put_Color)
        {
            Fixture f;
            auto paint = f.CreateSvgPaintAttribute();

            f.m_mockD2DPaint->SetColorMethod.SetExpectedCalls(1,
                [=](D2D1_COLOR_F const* color)
                {
                    Assert::AreEqual<D2D1_COLOR_F>(D2D1::ColorF(D2D1::ColorF::Blue), *color);
                    return S_OK;
                });

            Color blue = {0xFF, 0x0, 0x0, 0xFF};
            Assert::AreEqual(S_OK, paint->put_Color(blue));
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Paint_get_Color)
        {
            Fixture f;
            auto paint = f.CreateSvgPaintAttribute();
            
            f.m_mockD2DPaint->GetColorMethod.SetExpectedCalls(1,
                [=](D2D1_COLOR_F* color)
                {
                    *color = D2D1::ColorF(D2D1::ColorF::Blue);
                });

            Color retrieved;
            Assert::AreEqual(S_OK, paint->get_Color(&retrieved));

            Color blue = { 0xFF, 0x0, 0x0, 0xFF };
            Assert::AreEqual(blue, retrieved);
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Paint_put_Id)
        {
            Fixture f;
            auto paint = f.CreateSvgPaintAttribute();

            f.m_mockD2DPaint->SetIdMethod.SetExpectedCalls(1,
                [=](wchar_t const* str)
                {
                    Assert::AreEqual(L"Something", str);
                    return S_OK;
                });

            Assert::AreEqual(S_OK, paint->put_Id(WinString(L"Something")));
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Paint_get_Id)
        {
            Fixture f;
            auto paint = f.CreateSvgPaintAttribute();

            f.m_mockD2DPaint->GetIdLengthMethod.SetExpectedCalls(1,
                [=]
                {
                    return 9u;
                });

            f.m_mockD2DPaint->GetIdMethod.SetExpectedCalls(1,
                [=](wchar_t* buffer, uint32_t bufferLength)
                {
                    Assert::AreEqual(10u, bufferLength);
                    wcscpy_s(buffer, bufferLength, L"Something");
                    return S_OK;
                });

            HSTRING retrieved;
            Assert::AreEqual(S_OK, paint->get_Id(&retrieved));

            Assert::AreEqual(L"Something", WindowsGetStringRawBuffer(retrieved, nullptr));
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Path_CreatePathGeometry)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;
                auto path = f.CreateSvgPathAttribute();

                auto d2dPath = Make<MockD2DPathGeometry>();

                f.m_mockD2DPathData->CreatePathGeometryMethod.SetExpectedCalls(1,
                    [=](D2D1_FILL_MODE fillMode, ID2D1PathGeometry1** d2dPathGeometry)
                    {
                        Assert::AreEqual(i==0? D2D1_FILL_MODE_ALTERNATE : D2D1_FILL_MODE_WINDING, fillMode);
                        return d2dPath.CopyTo(d2dPathGeometry);
                    });

                ComPtr<ICanvasGeometry> result;

                if (i==0)
                    Assert::AreEqual(S_OK, path->CreatePathGeometry(&result));
                else
                    Assert::AreEqual(S_OK, path->CreatePathGeometryWithFill(CanvasFilledRegionDetermination::Winding, &result));

                Assert::IsTrue(IsSameInstance(
                    d2dPath.Get(),
                    static_cast<CanvasGeometry*>(result.Get())->GetResource().Get()));
            }
        }

        template<typename D2D_ELEMENT, typename WRAPPED_ELEMENT, typename GET_COUNT_METHOD, typename GET_DATA_METHOD>
        void VerifyRetrievalOfArray(
            GET_COUNT_METHOD&& getCountMethod,
            GET_DATA_METHOD&& getDataMethod,
            uint32_t collectionSize,
            uint32_t requestedStartIndex,
            uint32_t requestedElementCount,
            D2D_ELEMENT d2dTestValues[3],
            WRAPPED_ELEMENT wrappedTestValues[3],
            std::function<HRESULT(UINT32*, WRAPPED_ELEMENT**)> memberToTest)
        {
            getCountMethod.SetExpectedCalls(1,
                [=]
                {
                    return collectionSize;
                });

            getDataMethod.SetExpectedCalls(1,
                [=](D2D_ELEMENT* buffer, UINT32 elementCount, UINT32 startIndex)
                {
                    Assert::AreEqual(requestedStartIndex, startIndex);
                    Assert::AreEqual(requestedElementCount, elementCount);

                    for (uint32_t i = 0; i < elementCount; ++i)
                    {
                        buffer[i] = d2dTestValues[i];
                    }
                    return S_OK;
                });

            WRAPPED_ELEMENT* resultBuffer{};

            Assert::AreEqual(S_OK, memberToTest(&requestedElementCount, &resultBuffer));
            for (uint32_t i = 0; i < requestedElementCount; ++i)
            {
                Assert::AreEqual(wrappedTestValues[i], resultBuffer[i]);
            }
        }
        
        template<typename WRAPPER_INTERFACE, typename D2D_REMOVE_METHOD, typename WRAPPED_REMOVE_METHOD>
        void VerifyRemoveAtEnd(
            WRAPPER_INTERFACE* attribute,
            WRAPPED_REMOVE_METHOD&& wrappedRemoveMethod,
            D2D_REMOVE_METHOD&& d2dRemoveMethod)
        {
            uint32_t amountToRemove = 3;

            d2dRemoveMethod.SetExpectedCalls(1,
                [=](uint32_t n)
                {
                    Assert::AreEqual(amountToRemove, n);
                    return S_OK;
                });
            
            Assert::AreEqual(S_OK, (*attribute.*wrappedRemoveMethod)(amountToRemove));
        }

        template<typename WRAPPER_INTERFACE, typename D2D_ELEMENT, typename D2D_SET_METHOD, typename D2D_GET_COUNT_METHOD>
        void VerifySetElements(
            WRAPPER_INTERFACE* attribute,
            D2D_GET_COUNT_METHOD&& getCountMethod,
            D2D_SET_METHOD&& setElementsMethod,
            D2D_ELEMENT d2dTestValues[3],
            std::function<HRESULT(uint32_t, uint32_t)> memberToTest)
        {
            uint32_t requestedStartIndex = 0;
            uint32_t requestedElementCount = 3;

            getCountMethod.AllowAnyCall(
                [=]
                {
                    return 3;
                });
            
            setElementsMethod.SetExpectedCalls(1,
                [=](D2D_ELEMENT const* buffer, uint32_t elementCount, uint32_t startIndex)
                {
                    for (uint32_t i = 0; i < elementCount; ++i)
                    {
                        Assert::AreEqual(d2dTestValues[i], buffer[i]);
                    }
                    return S_OK;
                });

            Assert::AreEqual(S_OK, memberToTest(requestedStartIndex, requestedElementCount));
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Path_Commands)
        {
            Fixture f;
            auto path = f.CreateSvgPathAttribute();

            D2D1_SVG_PATH_COMMAND d2dTestValues[3] = { 
                D2D1_SVG_PATH_COMMAND_ARC_ABSOLUTE,
                D2D1_SVG_PATH_COMMAND_ARC_RELATIVE,
                D2D1_SVG_PATH_COMMAND_CLOSE_PATH, 
            };

            CanvasSvgPathCommand wrappedTestValues[3]{
                CanvasSvgPathCommand::ArcAbsolute,
                CanvasSvgPathCommand::ArcRelative,
                CanvasSvgPathCommand::ClosePath
            };

            VerifyRetrievalOfArray<D2D1_SVG_PATH_COMMAND, CanvasSvgPathCommand>(
                f.m_mockD2DPathData->GetCommandsCountMethod,
                f.m_mockD2DPathData->GetCommandsMethod,
                3,
                0,
                3,
                d2dTestValues,
                wrappedTestValues,
                [=](UINT32* resultCount, CanvasSvgPathCommand** resultBuffer)
                {
                    return path->get_Commands(resultCount, resultBuffer);
                });

            VerifyRetrievalOfArray<D2D1_SVG_PATH_COMMAND, CanvasSvgPathCommand>(
                f.m_mockD2DPathData->GetCommandsCountMethod,
                f.m_mockD2DPathData->GetCommandsMethod,
                3,
                1,
                2,
                d2dTestValues,
                wrappedTestValues,
                [=](UINT32* resultCount, CanvasSvgPathCommand** resultBuffer)
                {
                    return path->GetCommands(1, 2, resultCount, resultBuffer);
                });

            VerifyRemoveAtEnd<ICanvasSvgPathAttribute>(
                path.Get(),
                &ICanvasSvgPathAttribute::RemoveCommandsAtEnd,
                f.m_mockD2DPathData->RemoveCommandsAtEndMethod);

            VerifySetElements<ICanvasSvgPathAttribute, D2D1_SVG_PATH_COMMAND>(
                path.Get(),
                f.m_mockD2DPathData->GetCommandsCountMethod,
                f.m_mockD2DPathData->UpdateCommandsMethod,
                d2dTestValues,
                [=, &wrappedTestValues](uint32_t startIndex, uint32_t commandCount)
                {
                    return path->SetCommands(startIndex, commandCount, wrappedTestValues);
                });
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Path_SegmentData)
        {
            Fixture f;
            auto path = f.CreateSvgPathAttribute();

            float testValues[3] = {1.0f, 2.0f, 3.0f};

            VerifyRetrievalOfArray<float, float>(
                f.m_mockD2DPathData->GetSegmentDataCountMethod,
                f.m_mockD2DPathData->GetSegmentDataMethod,
                3,
                0,
                3,
                testValues,
                testValues,
                [=](UINT32* resultCount, float** resultBuffer)
                {
                    return path->get_SegmentData(resultCount, resultBuffer);
                });

            VerifyRetrievalOfArray<float, float>(
                f.m_mockD2DPathData->GetSegmentDataCountMethod,
                f.m_mockD2DPathData->GetSegmentDataMethod,
                3,
                1,
                2,
                testValues,
                testValues,
                [=](UINT32* resultCount, float** resultBuffer)
                {
                    return path->GetSegmentData(1, 2, resultCount, resultBuffer);
                });

            VerifyRemoveAtEnd<ICanvasSvgPathAttribute>(
                path.Get(),
                &ICanvasSvgPathAttribute::RemoveSegmentDataAtEnd,
                f.m_mockD2DPathData->RemoveSegmentDataAtEndMethod);

            VerifySetElements<ICanvasSvgPathAttribute, float>(
                path.Get(),
                f.m_mockD2DPathData->GetSegmentDataCountMethod,
                f.m_mockD2DPathData->UpdateSegmentDataMethod,
                testValues,
                [=, &testValues](uint32_t startIndex, uint32_t commandCount)
                {
                    return path->SetSegmentData(startIndex, commandCount, testValues);
                });
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Points)
        {
            Fixture f;
            auto points = f.CreateSvgPointsAttribute();

            D2D1_POINT_2F d2dTestValues[3] = { { 1.0f, 2.0f },{ 3.0f, 4.0f },{ 5.0f, 6.0f } };
            Vector2 wrappedTestValues[3] = { {1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f} };

            VerifyRetrievalOfArray<D2D1_POINT_2F, Vector2>(
                f.m_mockD2DPointCollection->GetPointsCountMethod,
                f.m_mockD2DPointCollection->GetPointsMethod,
                3,
                0,
                3,
                d2dTestValues,
                wrappedTestValues,
                [=](UINT32* resultCount, Vector2** resultBuffer)
                {
                    return points->get_Points(resultCount, resultBuffer);
                });

            VerifyRetrievalOfArray<D2D1_POINT_2F, Vector2>(
                f.m_mockD2DPointCollection->GetPointsCountMethod,
                f.m_mockD2DPointCollection->GetPointsMethod,
                3,
                1,
                2,
                d2dTestValues,
                wrappedTestValues,
                [=](UINT32* resultCount, Vector2** resultBuffer)
                {
                    return points->GetPoints(1, 2, resultCount, resultBuffer);
                });

            VerifyRemoveAtEnd<ICanvasSvgPointsAttribute>(
                points.Get(),
                &ICanvasSvgPointsAttribute::RemovePointsAtEnd,
                f.m_mockD2DPointCollection->RemovePointsAtEndMethod);

            VerifySetElements<ICanvasSvgPointsAttribute, D2D1_POINT_2F>(
                points.Get(),
                f.m_mockD2DPointCollection->GetPointsCountMethod,
                f.m_mockD2DPointCollection->UpdatePointsMethod,
                d2dTestValues,
                [=, &wrappedTestValues](uint32_t startIndex, uint32_t commandCount)
                {
                    return points->SetPoints(startIndex, commandCount, wrappedTestValues);
                });
        }
        
        TEST_METHOD_EX(CanvasSvgAttributeTests_Dashes_ValuesOnly)
        {
            Fixture f;
            auto dashes = f.CreateSvgStrokeDashArrayAttribute();

            float testValues[3] = { 1.0f, 2.0f, 3.0f };

            VerifyRetrievalOfArray<float, float>(
                f.m_mockD2DStrokeDashArray->GetDashesCountMethod,
                f.m_mockD2DStrokeDashArray->GetDashesMethod1,
                3,
                0,
                3,
                testValues,
                testValues,
                [=](UINT32* resultCount, float** resultBuffer)
                {
                    return dashes->GetDashes(resultCount, resultBuffer);
                });

            VerifyRemoveAtEnd<ICanvasSvgStrokeDashArrayAttribute>(
                dashes.Get(),
                &ICanvasSvgStrokeDashArrayAttribute::RemoveDashesAtEnd,
                f.m_mockD2DStrokeDashArray->RemoveDashesAtEndMethod);
            
            VerifySetElements<ICanvasSvgStrokeDashArrayAttribute, float>(
                dashes.Get(),
                f.m_mockD2DStrokeDashArray->GetDashesCountMethod,
                f.m_mockD2DStrokeDashArray->UpdateDashesMethod1,
                testValues,
                [=, &testValues](uint32_t startIndex, uint32_t commandCount)
                {
                    return dashes->SetDashes(startIndex, commandCount, testValues);
                });
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_Dashes_ValuesAndUnits)
        {
            float testValues[3] = { 1.0f, 2.0f, 3.0f };
            D2D1_SVG_LENGTH_UNITS d2dTestUnits[3] = { D2D1_SVG_LENGTH_UNITS_PERCENTAGE, D2D1_SVG_LENGTH_UNITS_NUMBER, D2D1_SVG_LENGTH_UNITS_PERCENTAGE };
            CanvasSvgLengthUnits wrappedTestUnits[3] = { CanvasSvgLengthUnits::Percentage, CanvasSvgLengthUnits::Number, CanvasSvgLengthUnits::Percentage };

            const uint32_t collectionSize = 3;
            const uint32_t requestedStartIndex = 1;
            const uint32_t requestedElementCount = 2;

            {
                Fixture f;
                auto dashes = f.CreateSvgStrokeDashArrayAttribute();

                f.m_mockD2DStrokeDashArray->GetDashesCountMethod.SetExpectedCalls(1,
                    [=]
                    {
                        return collectionSize;
                    });

                f.m_mockD2DStrokeDashArray->GetDashesMethod2.SetExpectedCalls(1,
                    [=](D2D1_SVG_LENGTH* lengths, UINT32 elementCount, UINT32 startIndex)
                    {
                        Assert::AreEqual(requestedStartIndex, startIndex);
                        Assert::AreEqual(requestedElementCount, elementCount);

                        for (uint32_t i = 0; i < elementCount; ++i)
                        {
                            lengths[i].value = testValues[i];
                            lengths[i].units = d2dTestUnits[i];
                        }
                        return S_OK;
                    });

                uint32_t retrievedUnitsCount, retrievedValueCount;
                float* retrievedValues;
                CanvasSvgLengthUnits* retrievedUnits;

                Assert::AreEqual(S_OK, dashes->GetDashesWithUnits(
                    requestedStartIndex,
                    requestedElementCount,
                    &retrievedUnitsCount,
                    &retrievedUnits,
                    &retrievedValueCount,
                    &retrievedValues));

                Assert::AreEqual(retrievedUnitsCount, requestedElementCount);
                Assert::AreEqual(retrievedValueCount, requestedElementCount);

                for (uint32_t i = 0; i < requestedElementCount; ++i)
                {
                    Assert::AreEqual(wrappedTestUnits[i], retrievedUnits[i]);
                    Assert::AreEqual(testValues[i], retrievedValues[i]);
                }

            }
            {
                Fixture f;
                auto dashes = f.CreateSvgStrokeDashArrayAttribute();

                f.m_mockD2DStrokeDashArray->UpdateDashesMethod2.SetExpectedCalls(1,
                    [=](D2D1_SVG_LENGTH const* lengths, UINT32 elementCount, UINT32 startIndex)
                    {
                        Assert::AreEqual(requestedStartIndex, startIndex);
                        Assert::AreEqual(requestedElementCount, elementCount);

                        for (uint32_t i = 0; i < elementCount; ++i)
                        {
                            Assert::AreEqual(testValues[i], lengths[i].value);
                            Assert::AreEqual(D2D1_SVG_LENGTH_UNITS_PERCENTAGE, lengths[i].units);
                        }
                        return S_OK;
                    });

                Assert::AreEqual(S_OK, dashes->SetDashesWithUnit(requestedStartIndex, requestedElementCount, testValues, CanvasSvgLengthUnits::Percentage));
            }
        }
        
        template<typename D2D_MOCK_TYPE, typename WRAPPED_IMPLEMENTATION>
        void Verify_Clone(
            Fixture& f,
            ICanvasSvgAttribute* attribute,
            ComPtr<D2D_MOCK_TYPE> fixtureMockObject)
        {
            auto m_mockD2DObject = Make<D2D_MOCK_TYPE>();

            fixtureMockObject->CloneMethod.SetExpectedCalls(1,
                [=](ID2D1SvgAttribute** result)
                {
                    return m_mockD2DObject.CopyTo(result);
                });

            ComPtr<ICanvasSvgAttribute> retrieved;
            Assert::AreEqual(S_OK, attribute->Clone(&retrieved));

            Assert::IsTrue(IsSameInstance(
                m_mockD2DObject.Get(),
                static_cast<WRAPPED_IMPLEMENTATION*>(retrieved.Get())->GetResource().Get()));
        }

        template<typename D2D_MOCK_TYPE>
        void Verify_GetElement_Null(
            Fixture& f,
            ICanvasSvgAttribute* attribute,
            ComPtr<D2D_MOCK_TYPE> fixtureMockObject)
        {            
            fixtureMockObject->GetElementMethod.SetExpectedCalls(1,
                [=](ID2D1SvgElement** root)
                {
                    *root = nullptr;
                    return S_OK;
                });

            ComPtr<ICanvasSvgNamedElement> retrieved;
            Assert::AreEqual(S_OK, attribute->GetElement(&retrieved));

            Assert::IsNull(retrieved.Get());
        }

        template<typename D2D_MOCK_TYPE>
        void Verify_GetElement_NonNull(
            Fixture& f,
            ICanvasSvgAttribute* attribute,
            ComPtr<D2D_MOCK_TYPE> fixtureMockObject)
        {
            ComPtr<MockD2DSvgElement> mockD2DSvgElement = f.CreateMockD2DElement();

            fixtureMockObject->GetElementMethod.SetExpectedCalls(1,
                [=](ID2D1SvgElement** root)
                {
                    return mockD2DSvgElement.CopyTo(root);
                });

            ComPtr<ICanvasSvgNamedElement> retrieved;
            Assert::AreEqual(S_OK, attribute->GetElement(&retrieved));

            Assert::IsTrue(IsSameInstance(
                mockD2DSvgElement.Get(),
                static_cast<CanvasSvgNamedElement*>(retrieved.Get())->GetResource().Get()));
        }

        void Verify_get_Device(
            Fixture& f,
            ICanvasSvgAttribute* attribute)
        {
            ComPtr<ICanvasDevice> device;
            Assert::AreEqual(S_OK, attribute->get_Device(&device));
            Assert::AreEqual<ICanvasDevice*>(f.m_canvasDevice.Get(), device.Get());
        }

        template<typename D2D_MOCK_TYPE, typename WRAPPED_IMPLEMENTATION>
        void VerifyCommonMembers(
            Fixture& f,
            ICanvasSvgAttribute* attribute,
            ComPtr<D2D_MOCK_TYPE> fixtureMockObject)
        {
            Verify_Clone<D2D_MOCK_TYPE, WRAPPED_IMPLEMENTATION>(f, attribute, fixtureMockObject);
            Verify_GetElement_NonNull<D2D_MOCK_TYPE>(f, attribute, fixtureMockObject);
            Verify_GetElement_Null<D2D_MOCK_TYPE>(f, attribute, fixtureMockObject);
            Verify_get_Device(f, attribute);
        }

        TEST_METHOD_EX(CanvasSvgAttributeTests_CommonMembers)
        {
            {
                Fixture f;
                auto attribute = f.CreateSvgPaintAttribute();
                VerifyCommonMembers<MockD2DSvgPaint, CanvasSvgPaintAttribute>(f, attribute.Get(), f.m_mockD2DPaint);
            }
            {
                Fixture f;
                auto attribute = f.CreateSvgPathAttribute();
                VerifyCommonMembers<MockD2DSvgPathData, CanvasSvgPathAttribute>(f, attribute.Get(), f.m_mockD2DPathData);
            }
            {
                Fixture f;
                auto attribute = f.CreateSvgPointsAttribute();
                VerifyCommonMembers<MockD2DSvgPointCollection, CanvasSvgPointsAttribute>(f, attribute.Get(), f.m_mockD2DPointCollection);
            }
            {
                Fixture f;
                auto attribute = f.CreateSvgStrokeDashArrayAttribute();
                VerifyCommonMembers<MockD2DSvgStrokeDashArray, CanvasSvgStrokeDashArrayAttribute>(f, attribute.Get(), f.m_mockD2DStrokeDashArray);
            }
        }
    };
}


#endif
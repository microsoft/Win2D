// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include <lib/svg/CanvasSvgDocument.h>
#include <lib/svg/CanvasSvgElement.h>
#include <lib/svg/CanvasSvgPaintAttribute.h>
#include <lib/svg/CanvasSvgPointsAttribute.h>
#include <lib/svg/CanvasSvgPathAttribute.h>
#include <lib/svg/CanvasSvgStrokeDashArrayAttribute.h>
#include <AsyncOperation.h>
#include <LifespanTracker.h>
#include "mocks/MockD2DSvgDocument.h"
#include "mocks/MockD2DSvgElement.h"
#include "mocks/MockD2DSvgPaint.h"
#include "mocks/MockD2DSvgPathData.h"
#include "mocks/MockD2DSvgPointCollection.h"
#include "mocks/MockD2DSvgStrokeDashArray.h"
#include "mocks/MockStream.h"
#include "mocks/MockRandomAccessStream.h"

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas::Svg;

    struct Fixture
    {
        ComPtr<StubCanvasDevice> m_canvasDevice;
        ComPtr<IStream> m_mockStream;
        ComPtr<MockD2DSvgDocument> m_createdDocument;

        Fixture()
        {
            m_canvasDevice = Make<StubCanvasDevice>();
            m_mockStream = Make<MockStream>();
            m_createdDocument = Make<MockD2DSvgDocument>();

            m_canvasDevice->CreateSvgDocumentMethod.AllowAnyCall(
                [=](IStream* stream)
                {
                    return m_createdDocument;
                });
        }

        ComPtr<CanvasSvgDocument> CreateSvgDocument()
        {
            return CanvasSvgDocument::CreateNew(m_canvasDevice.Get(), m_mockStream.Get());
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

    TEST_CLASS(CanvasSvgDocumentTests)
    {
        TEST_METHOD_EX(CanvasSvgDocumentTests_Implements_Expected_Interfaces)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ASSERT_IMPLEMENTS_INTERFACE(svgDocument, ICanvasSvgDocument);
            ASSERT_IMPLEMENTS_INTERFACE(svgDocument, ABI::Windows::Foundation::IClosable);
            ASSERT_IMPLEMENTS_INTERFACE(svgDocument, ICanvasResourceWrapperNative);
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_Closure)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            Assert::AreEqual(S_OK, svgDocument->Close());

            ComPtr<ICanvasDevice> device;
            Assert::AreEqual(RO_E_CLOSED, svgDocument->get_Device(&device));
            
            HSTRING str;
            Assert::AreEqual(RO_E_CLOSED, svgDocument->GetXml(&str));

            IRandomAccessStream* fakeStream = reinterpret_cast<IRandomAccessStream*>(0x1234);
            IAsyncAction* action;
            Assert::AreEqual(RO_E_CLOSED, svgDocument->SaveAsync(fakeStream, &action));

            ICanvasSvgNamedElement* fakeElement = reinterpret_cast<ICanvasSvgNamedElement*>(0x1234);
            Assert::AreEqual(RO_E_CLOSED, svgDocument->put_Root(fakeElement));
            Assert::AreEqual(RO_E_CLOSED, svgDocument->get_Root(&fakeElement));

            ComPtr<ICanvasSvgNamedElement> element;
            Assert::AreEqual(RO_E_CLOSED, svgDocument->LoadElementFromXml(WinString(L"<svg/>"), &element));

            IAsyncOperation<CanvasSvgNamedElement*>* operation;
            Assert::AreEqual(RO_E_CLOSED, svgDocument->LoadElementAsync(fakeStream, &operation));

            Assert::AreEqual(RO_E_CLOSED, svgDocument->FindElementById(WinString(L"id"), &fakeElement));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_NullArgs)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            Assert::AreEqual(E_INVALIDARG, svgDocument->get_Device(nullptr));

            Assert::AreEqual(E_INVALIDARG, svgDocument->GetXml(nullptr));

            IRandomAccessStream* fakeStream = reinterpret_cast<IRandomAccessStream*>(0x1234);
            IAsyncAction* action;
            Assert::AreEqual(E_INVALIDARG, svgDocument->SaveAsync(nullptr, &action));
            Assert::AreEqual(E_INVALIDARG, svgDocument->SaveAsync(fakeStream, nullptr));
            
            Assert::AreEqual(E_INVALIDARG, svgDocument->put_Root(nullptr));

            Assert::AreEqual(E_INVALIDARG, svgDocument->get_Root(nullptr));

            ComPtr<ICanvasSvgNamedElement> element;
            Assert::AreEqual(E_INVALIDARG, svgDocument->LoadElementFromXml(nullptr, &element));
            Assert::AreEqual(E_INVALIDARG, svgDocument->LoadElementFromXml(WinString(L""), nullptr));

            IAsyncOperation<CanvasSvgNamedElement*>* operation;
            Assert::AreEqual(E_INVALIDARG, svgDocument->LoadElementAsync(nullptr, &operation));
            Assert::AreEqual(E_INVALIDARG, svgDocument->LoadElementAsync(fakeStream, nullptr));

            Assert::AreEqual(E_INVALIDARG, svgDocument->FindElementById(WinString(L""), nullptr));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_Device)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<ICanvasDevice> device;
            Assert::AreEqual(S_OK, svgDocument->get_Device(&device));
            Assert::AreEqual<ICanvasDevice*>(f.m_canvasDevice.Get(), device.Get());
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_GetXml)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            f.m_createdDocument->SerializeMethod.SetExpectedCalls(1,
                [=](IStream* stream, ID2D1SvgElement* subtree)
                {
                    // To implement this method, Win2D uses a stream that it creates internally.
                    Assert::IsNotNull(stream);
                    Assert::IsNull(subtree);

                    // Write some placeholder data to the stream, since a non-empty write is expected. Even for
                    // an empty document, D2D will implicitly have a root element.
                    char const documentText[] = "something";
                    HRESULT writeResult = stream->Write(documentText, _countof(documentText), nullptr);

                    if (FAILED(writeResult))
                        return writeResult;
                    else
                        return S_OK;
                });
            
            WinString returnedXml;
            Assert::AreEqual(S_OK, svgDocument->GetXml(returnedXml.GetAddressOf()));
            Assert::AreEqual(L"something", static_cast<wchar_t const*>(returnedXml));
        }

        static ComPtr<CanvasSvgDocumentStatics> GetSvgDocumentStatics()
        {
            ComPtr<CanvasSvgDocumentStatics> svgDocumentStatics;
            ThrowIfFailed(MakeAndInitialize<CanvasSvgDocumentStatics>(&svgDocumentStatics));
            return svgDocumentStatics;
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_CreateEmpty)
        {
            auto device = Make<StubCanvasDevice>();
            ComPtr<MockD2DSvgDocument> mockD2DSvgDocument = Make<MockD2DSvgDocument>();
            device->CreateSvgDocumentMethod.ExpectAtLeastOneCall(
                [=](IStream* stream)
                {
                    Assert::IsNull(stream);
                    return mockD2DSvgDocument;
                });
            
            auto svgDocumentStatics = GetSvgDocumentStatics();
            ComPtr<ICanvasSvgDocument> emptyDocument;
            Assert::AreEqual(S_OK, svgDocumentStatics->CreateEmpty(As<ICanvasResourceCreator>(device).Get(), &emptyDocument));

            auto nativeResource = static_cast<CanvasSvgDocument*>(emptyDocument.Get())->GetResource();
            Assert::AreEqual<ID2D1SvgDocument*>(nativeResource.Get(), mockD2DSvgDocument.Get());
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_SetRoot)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgElement> mockD2DSvgElement = f.CreateMockD2DElement();

            f.m_createdDocument->SetRootMethod.SetExpectedCalls(1,
                [=](ID2D1SvgElement* newRoot)
                {
                    Assert::IsNotNull(newRoot);
                    Assert::AreEqual<ID2D1SvgElement*>(mockD2DSvgElement.Get(), newRoot);

                    return S_OK;
                });

            auto canvasSvgElement = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(f.m_canvasDevice.Get(), mockD2DSvgElement.Get());
            Assert::AreEqual(S_OK, svgDocument->put_Root(canvasSvgElement.Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_GetRoot)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgElement> mockD2DSvgElement = f.CreateMockD2DElement();

            f.m_createdDocument->GetRootMethod.SetExpectedCalls(1,
                [=](ID2D1SvgElement** root)
                {
                    return mockD2DSvgElement.CopyTo(root);
                });

            ComPtr<ICanvasSvgNamedElement> retrievedRoot;
            Assert::AreEqual(S_OK, svgDocument->get_Root(&retrievedRoot));

            Assert::IsTrue(IsSameInstance(
                mockD2DSvgElement.Get(), 
                static_cast<CanvasSvgNamedElement*>(retrievedRoot.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_FindElementById)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgElement> mockD2DSvgElement = f.CreateMockD2DElement();
            
            f.m_createdDocument->FindElementByIdMethod.SetExpectedCalls(1,
                [=](wchar_t const* name, ID2D1SvgElement** result)
                {
                    Assert::AreEqual(L"Something", name);
                    return mockD2DSvgElement.CopyTo(result);
                });

            ComPtr<ICanvasSvgNamedElement> retrievedElement;
            Assert::AreEqual(S_OK, svgDocument->FindElementById(WinString(L"Something"), &retrievedElement));

            Assert::IsTrue(IsSameInstance(
                mockD2DSvgElement.Get(), 
                static_cast<CanvasSvgNamedElement*>(retrievedElement.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_CreatePaintAttributeWithDefaults)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgPaint> mockD2DAttribute = Make<MockD2DSvgPaint>();
            
            f.m_createdDocument->CreatePaintMethod.SetExpectedCalls(1,
                [=](D2D1_SVG_PAINT_TYPE paintType, CONST D3DCOLORVALUE* color, PCWSTR id, ID2D1SvgPaint** d2dResult)
                {
                    Assert::AreEqual(D2D1_SVG_PAINT_TYPE_NONE, paintType);
                    Assert::AreEqual<D2D1_COLOR_F>(D2D1::ColorF(D2D1::ColorF::Black), *color);
                    Assert::AreEqual(L"", id);
                    return mockD2DAttribute.CopyTo(d2dResult);
                });

            ComPtr<ICanvasSvgPaintAttribute> result;
            Assert::AreEqual(S_OK, svgDocument->CreatePaintAttributeWithDefaults(&result));

            Assert::IsTrue(IsSameInstance(
                mockD2DAttribute.Get(), 
                static_cast<CanvasSvgPaintAttribute*>(result.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_CreatePaintAttribute)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgPaint> mockD2DAttribute = Make<MockD2DSvgPaint>();
            
            f.m_createdDocument->CreatePaintMethod.SetExpectedCalls(1,
                [=](D2D1_SVG_PAINT_TYPE paintType, CONST D3DCOLORVALUE* color, PCWSTR id, ID2D1SvgPaint** d2dResult)
                {
                    Assert::AreEqual(D2D1_SVG_PAINT_TYPE_URI, paintType);
                    Assert::AreEqual<D2D1_COLOR_F>(D2D1::ColorF(D2D1::ColorF::Blue), *color);
                    Assert::AreEqual(L"ABC", id);
                    return mockD2DAttribute.CopyTo(d2dResult);
                });

            Color blue = { 0xFF, 0, 0, 0xFF };
            ComPtr<ICanvasSvgPaintAttribute> result;
            Assert::AreEqual(S_OK, svgDocument->CreatePaintAttribute(CanvasSvgPaintType::Uri, blue, WinString(L"ABC"), &result));

            Assert::IsTrue(IsSameInstance(
                mockD2DAttribute.Get(), 
                static_cast<CanvasSvgPaintAttribute*>(result.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_CreatePathAttributeWithDefaults)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgPathData> mockD2DAttribute = Make<MockD2DSvgPathData>();
            
            f.m_createdDocument->CreatePathDataMethod.SetExpectedCalls(1,
                [=](CONST FLOAT* segmentData, UINT32 segmentCount, CONST D2D1_SVG_PATH_COMMAND* commandData, UINT32 commandCount, ID2D1SvgPathData** d2dResult)
                {                    
                    Assert::IsNull(segmentData);
                    Assert::AreEqual(0u, segmentCount);

                    Assert::IsNull(commandData);
                    Assert::AreEqual(0u, commandCount);

                    return mockD2DAttribute.CopyTo(d2dResult);
                });

            ComPtr<ICanvasSvgPathAttribute> result;
            Assert::AreEqual(S_OK, svgDocument->CreatePathAttributeWithDefaults(&result));

            Assert::IsTrue(IsSameInstance(
                mockD2DAttribute.Get(), 
                static_cast<CanvasSvgPathAttribute*>(result.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_CreatePathAttribute)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgPathData> mockD2DAttribute = Make<MockD2DSvgPathData>();
            
            f.m_createdDocument->CreatePathDataMethod.SetExpectedCalls(1,
                [=](CONST FLOAT* segmentData, UINT32 segmentCount, CONST D2D1_SVG_PATH_COMMAND* commandData, UINT32 commandCount, ID2D1SvgPathData** d2dResult)
                {
                    Assert::AreEqual(3u, segmentCount);
                    Assert::AreEqual(1.0f, segmentData[0]);
                    Assert::AreEqual(2.0f, segmentData[1]);
                    Assert::AreEqual(3.0f, segmentData[2]);

                    Assert::AreEqual(2u, commandCount);
                    Assert::AreEqual(D2D1_SVG_PATH_COMMAND_ARC_ABSOLUTE, commandData[0]);
                    Assert::AreEqual(D2D1_SVG_PATH_COMMAND_ARC_RELATIVE, commandData[1]);

                    return mockD2DAttribute.CopyTo(d2dResult);
                });

            float segments[] = { 1, 2, 3 };
            CanvasSvgPathCommand commands[] = { CanvasSvgPathCommand::ArcAbsolute, CanvasSvgPathCommand::ArcRelative };

            ComPtr<ICanvasSvgPathAttribute> result;
            Assert::AreEqual(S_OK, svgDocument->CreatePathAttribute(_countof(segments), segments, _countof(commands), commands, &result));

            Assert::IsTrue(IsSameInstance(
                mockD2DAttribute.Get(), 
                static_cast<CanvasSvgPathAttribute*>(result.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_CreatePointsAttributeWithDefaults)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgPointCollection> mockD2DAttribute = Make<MockD2DSvgPointCollection>();

            f.m_createdDocument->CreatePointCollectionMethod.SetExpectedCalls(1,
                [=](CONST D2D1_POINT_2F* points, UINT32 pointCount, ID2D1SvgPointCollection** d2dResult)
                {
                    Assert::IsNull(points);
                    Assert::AreEqual(0u, pointCount);

                    return mockD2DAttribute.CopyTo(d2dResult);
                });

            ComPtr<ICanvasSvgPointsAttribute> result;
            Assert::AreEqual(S_OK, svgDocument->CreatePointsAttributeWithDefaults(&result));

            Assert::IsTrue(IsSameInstance(
                mockD2DAttribute.Get(),
                static_cast<CanvasSvgPointsAttribute*>(result.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_CreatePointsAttribute)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgPointCollection> mockD2DAttribute = Make<MockD2DSvgPointCollection>();

            f.m_createdDocument->CreatePointCollectionMethod.SetExpectedCalls(1,
                [=](CONST D2D1_POINT_2F* points, UINT32 pointCount, ID2D1SvgPointCollection** d2dResult)
                {
                    Assert::AreEqual(2u, pointCount);
                    Assert::AreEqual(D2D1::Point2F(1, 2), points[0]);
                    Assert::AreEqual(D2D1::Point2F(3, 4), points[1]);

                    return mockD2DAttribute.CopyTo(d2dResult);
                });

            Vector2 points[2] = { {1, 2}, {3, 4} };

            ComPtr<ICanvasSvgPointsAttribute> result;
            Assert::AreEqual(S_OK, svgDocument->CreatePointsAttribute(_countof(points), points, &result));

            Assert::IsTrue(IsSameInstance(
                mockD2DAttribute.Get(),
                static_cast<CanvasSvgPointsAttribute*>(result.Get())->GetResource().Get()));
        }


        
        TEST_METHOD_EX(CanvasSvgDocumentTests_CreateStrokeDashArrayAttributeWithDefaults)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgStrokeDashArray> mockD2DAttribute = Make<MockD2DSvgStrokeDashArray>();
            
            f.m_createdDocument->CreateStrokeDashArrayMethod.SetExpectedCalls(1,
                [=](CONST D2D1_SVG_LENGTH* svgLengths, UINT32 svgLengthCount, ID2D1SvgStrokeDashArray** d2dResult)
                {                    
                    Assert::IsNull(svgLengths);
                    Assert::AreEqual(0u, svgLengthCount);

                    return mockD2DAttribute.CopyTo(d2dResult);
                });

            ComPtr<ICanvasSvgStrokeDashArrayAttribute> result;
            Assert::AreEqual(S_OK, svgDocument->CreateStrokeDashArrayAttributeWithDefaults(&result));

            Assert::IsTrue(IsSameInstance(
                mockD2DAttribute.Get(), 
                static_cast<CanvasSvgStrokeDashArrayAttribute*>(result.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgDocumentTests_CreateStrokeDashArrayAttribute)
        {
            Fixture f;
            auto svgDocument = f.CreateSvgDocument();

            ComPtr<MockD2DSvgStrokeDashArray> mockD2DAttribute = Make<MockD2DSvgStrokeDashArray>();
            
            f.m_createdDocument->CreateStrokeDashArrayMethod.SetExpectedCalls(1,
                [=](CONST D2D1_SVG_LENGTH* svgLengths, UINT32 svgLengthCount, ID2D1SvgStrokeDashArray** d2dResult)
                {
                    Assert::AreEqual(2u, svgLengthCount);
                    Assert::AreEqual(1.0f, svgLengths[0].value);
                    Assert::AreEqual(D2D1_SVG_LENGTH_UNITS_PERCENTAGE, svgLengths[0].units);

                    Assert::AreEqual(2.0f, svgLengths[1].value);
                    Assert::AreEqual(D2D1_SVG_LENGTH_UNITS_NUMBER, svgLengths[1].units);

                    return mockD2DAttribute.CopyTo(d2dResult);
                });

            float values[] = { 1, 2 };
            CanvasSvgLengthUnits units[] = { CanvasSvgLengthUnits::Percentage, CanvasSvgLengthUnits::Number };

            ComPtr<ICanvasSvgStrokeDashArrayAttribute> result;
            Assert::AreEqual(S_OK, svgDocument->CreateStrokeDashArrayAttribute(_countof(values), values, _countof(units), units, &result));

            Assert::IsTrue(IsSameInstance(
                mockD2DAttribute.Get(), 
                static_cast<CanvasSvgStrokeDashArrayAttribute*>(result.Get())->GetResource().Get()));
        }
    };
}

#endif
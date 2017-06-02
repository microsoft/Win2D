// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include <lib/svg/CanvasSvgDocument.h>
#include <AsyncOperation.h>
#include <LifespanTracker.h>
#include "mocks/MockD2DSvgDocument.h"
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
    };
}

#endif
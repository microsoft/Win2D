// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#if WINVER > _WIN32_WINNT_WINBLUE

#include <lib/svg/CanvasSvgElement.h>
#include <lib/svg/CanvasSvgDocument.h>
#include "mocks/MockD2DSvgDocument.h"
#include "mocks/MockD2DSvgElement.h"

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas::Svg;
    
    TEST_CLASS(CanvasSvgElementTests)
    {
        struct Fixture
        {
            ComPtr<StubCanvasDevice> m_canvasDevice;
            ComPtr<ICanvasSvgDocument> m_svgDocument;

            // All deserialized elements will be backed by this native element, by default
            ComPtr<MockD2DSvgElement> m_mockD2DElement;

            Fixture()
            {
                m_canvasDevice = Make<StubCanvasDevice>();
                auto mockD2DDocument = Make<MockD2DSvgDocument>();
                m_mockD2DElement = Make<MockD2DSvgElement>();
                mockD2DDocument->DeserializeMethod.AllowAnyCall(
                    [=](IStream*, ID2D1SvgElement** result)
                    {
                        return m_mockD2DElement.CopyTo(result);
                    });

                m_svgDocument = ResourceManager::GetOrCreate<ICanvasSvgDocument>(m_canvasDevice.Get(), mockD2DDocument.Get());
            }

            ComPtr<CanvasSvgElement> CreateSvgElement()
            {
                ComPtr<ICanvasSvgElement> svgElement;
                Assert::AreEqual(S_OK, m_svgDocument->LoadElementFromXml(WinString(L"<svg/>"), &svgElement));

                ComPtr<CanvasSvgElement> implementation = static_cast<CanvasSvgElement*>(svgElement.Get());
                return implementation;
            }
        };

        TEST_METHOD_EX(CanvasSvgElementTests_Implements_Expected_Interfaces)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ASSERT_IMPLEMENTS_INTERFACE(svgElement, ICanvasSvgElement);
            ASSERT_IMPLEMENTS_INTERFACE(svgElement, ABI::Windows::Foundation::IClosable);
            ASSERT_IMPLEMENTS_INTERFACE(svgElement, ICanvasResourceWrapperNative);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_Closure)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            Assert::AreEqual(S_OK, svgElement->Close());

            ComPtr<ICanvasDevice> device;
            Assert::AreEqual(RO_E_CLOSED, svgElement->get_Device(&device));

            ICanvasSvgElement* fakeChild = reinterpret_cast<ICanvasSvgElement*>(0x1234);
            Assert::AreEqual(RO_E_CLOSED, svgElement->AppendChild(fakeChild));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_NullArgs)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            Assert::AreEqual(E_INVALIDARG, svgElement->get_Device(nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->AppendChild(nullptr));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_Device)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ComPtr<ICanvasDevice> device;
            Assert::AreEqual(S_OK, svgElement->get_Device(&device));
            Assert::AreEqual<ICanvasDevice*>(f.m_canvasDevice.Get(), device.Get());
        }

        TEST_METHOD_EX(CanvasSvgElementTests_AppendChild)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ComPtr<MockD2DSvgElement> mockD2DChildElement = Make<MockD2DSvgElement>();

            f.m_mockD2DElement->AppendChildMethod.SetExpectedCalls(1,
                [=](ID2D1SvgElement* child)
                {
                    Assert::IsTrue(IsSameInstance(mockD2DChildElement.Get(), child));
                    return S_OK;
                });

            auto childElement = ResourceManager::GetOrCreate<ICanvasSvgElement>(f.m_canvasDevice.Get(), mockD2DChildElement.Get());
            Assert::AreEqual(S_OK, svgElement->AppendChild(childElement.Get()));
        }
    };
}

#endif
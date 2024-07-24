// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/svg/CanvasSvgElement.h>
#include <lib/svg/CanvasSvgDocument.h>
#include <lib/svg/CanvasSvgPaintAttribute.h>
#include "mocks/MockD2DSvgDocument.h"
#include "mocks/MockD2DSvgElement.h"
#include "mocks/MockD2DSvgPaint.h"
#include <WindowsNumerics.h>

using namespace Windows::Foundation::Numerics;

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
                m_mockD2DElement = CreateMockD2DElement();
                mockD2DDocument->DeserializeMethod.AllowAnyCall(
                    [=](IStream*, ID2D1SvgElement** result)
                    {
                        return m_mockD2DElement.CopyTo(result);
                    });

                m_svgDocument = ResourceManager::GetOrCreate<ICanvasSvgDocument>(m_canvasDevice.Get(), mockD2DDocument.Get());
            }

            ComPtr<CanvasSvgNamedElement> CreateSvgElement()
            {
                ComPtr<ICanvasSvgNamedElement> svgElement;
                Assert::AreEqual(S_OK, m_svgDocument->LoadElementFromXml(WinString(L"<svg/>"), &svgElement));

                ComPtr<CanvasSvgNamedElement> implementation = static_cast<CanvasSvgNamedElement*>(svgElement.Get());
                return implementation;
            }

            static ComPtr<MockD2DSvgElement> CreateMockD2DElement(bool isTextContent = false)
            {
                ComPtr<MockD2DSvgElement> mockD2DSvgElement = Make<MockD2DSvgElement>();
                mockD2DSvgElement->IsTextContentMethod.AllowAnyCall(
                    [=]
                    {
                        return isTextContent;
                    });

                return mockD2DSvgElement;
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

            ICanvasSvgElement* fakeInputChild = reinterpret_cast<ICanvasSvgElement*>(0x1234);
            ICanvasSvgElement* fakeOutputChild;
            ICanvasSvgAttribute* fakeAttribute = reinterpret_cast<ICanvasSvgAttribute*>(0x1234);
            ICanvasSvgNamedElement* namedElement;
            ICanvasSvgTextElement* textElement;
            ICanvasSvgDocument* document;
            uint32_t u;
            boolean b;
            WinString string;
            HSTRING* stringArray;
            float fl{};
            Color c{};
            CanvasFilledRegionDetermination filledRegion{};
            CanvasSvgDisplay display{};
            CanvasSvgOverflow overflow{};
            CanvasCapStyle cap{};
            CanvasLineJoin lineJoin{};
            CanvasSvgVisibility visibility{};
            Matrix3x2 mat{};
            CanvasSvgUnits units{};
            CanvasSvgLengthUnits lengthUnits{};
            CanvasEdgeBehavior edge{};
            Rect r{};
            CanvasSvgAspectScaling scaling{};
            CanvasSvgAspectAlignment alignment{};

            Assert::AreEqual(RO_E_CLOSED, svgElement->AppendChild(fakeInputChild));

            Assert::AreEqual(RO_E_CLOSED, svgElement->CreateAndAppendNamedChildElement(string, &namedElement));

            Assert::AreEqual(RO_E_CLOSED, svgElement->CreateAndAppendTextChildElement(string, &textElement));

            Assert::AreEqual(RO_E_CLOSED, svgElement->get_FirstChild(&fakeOutputChild));
            Assert::AreEqual(RO_E_CLOSED, svgElement->get_LastChild(&fakeOutputChild));

            Assert::AreEqual(RO_E_CLOSED, svgElement->GetPreviousSibling(fakeInputChild, &fakeOutputChild));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetNextSibling(fakeInputChild, &fakeOutputChild));

            Assert::AreEqual(RO_E_CLOSED, svgElement->get_SpecifiedAttributes(&u, &stringArray));
            Assert::AreEqual(RO_E_CLOSED, svgElement->get_Tag(string.GetAddressOf()));
            Assert::AreEqual(RO_E_CLOSED, svgElement->get_HasChildren(&b));
            Assert::AreEqual(RO_E_CLOSED, svgElement->InsertChildBefore(fakeInputChild, fakeInputChild));

            Assert::AreEqual(RO_E_CLOSED, svgElement->IsAttributeSpecified(string, &b));
            Assert::AreEqual(RO_E_CLOSED, svgElement->IsAttributeSpecifiedWithInherhited(string, false, &b));

            Assert::AreEqual(RO_E_CLOSED, svgElement->RemoveAttribute(string));
            Assert::AreEqual(RO_E_CLOSED, svgElement->RemoveChild(fakeInputChild));
            Assert::AreEqual(RO_E_CLOSED, svgElement->ReplaceChild(fakeInputChild, fakeInputChild));
            Assert::AreEqual(RO_E_CLOSED, svgElement->get_ContainingDocument(&document));
            Assert::AreEqual(RO_E_CLOSED, svgElement->get_Parent(&namedElement));

            Assert::AreEqual(RO_E_CLOSED, svgElement->SetAttribute(string, fakeAttribute));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetAttribute(string, &fakeAttribute));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetIdAttribute(string, string));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetIdAttribute(string, string.GetAddressOf()));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetStringAttribute(string, string));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetStringAttribute(string, string.GetAddressOf()));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetFloatAttribute(string, 5));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetFloatAttribute(string, &fl));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetColorAttribute(string, c));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetColorAttribute(string, &c));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetFilledRegionDeterminationAttribute(string, filledRegion));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetFilledRegionDeterminationAttribute(string, &filledRegion));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetDisplayAttribute(string, display));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetDisplayAttribute(string, &display));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetOverflowAttribute(string, overflow));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetOverflowAttribute(string, &overflow));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetCapStyleAttribute(string, cap));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetCapStyleAttribute(string, &cap));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetLineJoinAttribute(string, lineJoin));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetLineJoinAttribute(string, &lineJoin));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetVisibilityAttribute(string, visibility));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetVisibilityAttribute(string, &visibility));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetTransformAttribute(string, mat));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetTransformAttribute(string, &mat));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetUnitsAttribute(string, units));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetUnitsAttribute(string, &units));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetEdgeBehaviorAttribute(string, edge));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetEdgeBehaviorAttribute(string, &edge));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetRectangleAttribute(string, r));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetRectangleAttribute(string, &r));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetLengthAttribute(string, 0, lengthUnits));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetLengthAttribute(string, &lengthUnits, &fl));
            Assert::AreEqual(RO_E_CLOSED, svgElement->SetAspectRatioAttribute(string, alignment, scaling));
            Assert::AreEqual(RO_E_CLOSED, svgElement->GetAspectRatioAttribute(string, &scaling, &alignment));

            ComPtr<ICanvasDevice> device;
            Assert::AreEqual(RO_E_CLOSED, svgElement->get_Device(&device));

        }

        TEST_METHOD_EX(CanvasSvgElementTests_NullArgs)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ICanvasSvgElement* fakeChild = reinterpret_cast<ICanvasSvgElement*>(0x1234);
            uint32_t u;
            WinString string;
            HSTRING* stringArray;
            float fl{};
            CanvasSvgLengthUnits lengthUnits{};
            CanvasSvgAspectScaling scaling{};
            CanvasSvgAspectAlignment alignment{};

            Assert::AreEqual(E_INVALIDARG, svgElement->AppendChild(nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->CreateAndAppendNamedChildElement(string, nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->CreateAndAppendTextChildElement(string, nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->get_FirstChild(nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->get_LastChild(nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->GetPreviousSibling(nullptr, &fakeChild));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetPreviousSibling(fakeChild, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetNextSibling(nullptr, &fakeChild));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetNextSibling(fakeChild, nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->get_SpecifiedAttributes(nullptr, &stringArray));
            Assert::AreEqual(E_INVALIDARG, svgElement->get_SpecifiedAttributes(&u, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->get_Tag(nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->get_HasChildren(nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->InsertChildBefore(nullptr, fakeChild));
            Assert::AreEqual(E_INVALIDARG, svgElement->InsertChildBefore(fakeChild, nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->IsAttributeSpecified(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->IsAttributeSpecifiedWithInherhited(string, false, nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->RemoveChild(nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->ReplaceChild(nullptr, fakeChild));
            Assert::AreEqual(E_INVALIDARG, svgElement->ReplaceChild(fakeChild, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->get_ContainingDocument(nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->get_Parent(nullptr));

            Assert::AreEqual(E_INVALIDARG, svgElement->SetAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetIdAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetStringAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetFloatAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetColorAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetFilledRegionDeterminationAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetDisplayAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetOverflowAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetCapStyleAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetLineJoinAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetVisibilityAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetTransformAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetUnitsAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetEdgeBehaviorAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetRectangleAttribute(string, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetLengthAttribute(string, nullptr, &fl));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetLengthAttribute(string, &lengthUnits, nullptr));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetAspectRatioAttribute(string, nullptr, &alignment));
            Assert::AreEqual(E_INVALIDARG, svgElement->GetAspectRatioAttribute(string, &scaling, nullptr));
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

            ComPtr<MockD2DSvgElement> mockD2DChildElement = f.CreateMockD2DElement();

            f.m_mockD2DElement->AppendChildMethod.SetExpectedCalls(1,
                [=](ID2D1SvgElement* child)
                {
                    Assert::IsTrue(IsSameInstance(mockD2DChildElement.Get(), child));
                    return S_OK;
                });

            auto childElement = ResourceManager::GetOrCreate<ICanvasSvgElement>(f.m_canvasDevice.Get(), mockD2DChildElement.Get());
            Assert::AreEqual(S_OK, svgElement->AppendChild(childElement.Get()));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_CreateAndAppendNamedChildElement)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ComPtr<MockD2DSvgElement> mockD2DElement = f.CreateMockD2DElement();

            f.m_mockD2DElement->CreateChildMethod.SetExpectedCalls(1,
                [=](wchar_t const* name, ID2D1SvgElement** result)
                {
                    Assert::AreEqual(L"Something", name);
                    return mockD2DElement.CopyTo(result);
                });

            ComPtr<ICanvasSvgNamedElement> resultElement;
            Assert::AreEqual(S_OK, svgElement->CreateAndAppendNamedChildElement(WinString(L"Something"), &resultElement));

            Assert::IsTrue(IsSameInstance(
                mockD2DElement.Get(),
                static_cast<CanvasSvgNamedElement*>(resultElement.Get())->GetResource().Get()));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_CreateAndAppendTextChildElement)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ComPtr<MockD2DSvgElement> mockD2DElement = f.CreateMockD2DElement(true);

            mockD2DElement->SetTextValueMethod.SetExpectedCalls(1,
                [=](wchar_t const* text, uint32_t textLength)
                {
                    Assert::AreEqual(9u, textLength);
                    Assert::AreEqual(L"Something", text);
                    return S_OK;
                });

            f.m_mockD2DElement->CreateChildMethod.SetExpectedCalls(1,
                [=](wchar_t const* name, ID2D1SvgElement** result)
                {
                    Assert::AreEqual(L"", name);
                    return mockD2DElement.CopyTo(result);
                });

            ComPtr<ICanvasSvgTextElement> resultElement;
            Assert::AreEqual(S_OK, svgElement->CreateAndAppendTextChildElement(WinString(L"Something"), &resultElement));

            Assert::IsTrue(IsSameInstance(
                mockD2DElement.Get(),
                static_cast<CanvasSvgTextElement*>(resultElement.Get())->GetResource().Get()));
        }

        struct ObjectPropertyFixture : public Fixture
        {
            template<typename RESULT_WRAPPED_TYPE, typename WRAPPED_GETTER, typename D2D_MOCK_METHOD>
            void Verify_ObjectPropertyGetter_NonNull(
                WRAPPED_GETTER&& wrappedGetter,
                D2D_MOCK_METHOD* d2dMockMethod)
            {
                auto svgElement = CreateSvgElement();
                ComPtr<MockD2DSvgElement> d2dElementToReturn = CreateMockD2DElement();

                d2dMockMethod->SetExpectedCalls(1,
                    [=](ID2D1SvgElement** result)
                    {
                        return d2dElementToReturn.CopyTo(result);
                    });

                ComPtr<RESULT_WRAPPED_TYPE> resultElement;
                Assert::AreEqual(S_OK, (*(svgElement.Get()).*wrappedGetter)(&resultElement));

                Assert::IsTrue(IsSameInstance(
                    d2dElementToReturn.Get(),
                    static_cast<CanvasSvgNamedElement*>(resultElement.Get())->GetResource().Get()));
            }

            template<typename RESULT_WRAPPED_TYPE, typename WRAPPED_GETTER, typename D2D_MOCK_METHOD>
            void Verify_ObjectPropertyGetter_Null(
                WRAPPED_GETTER&& wrappedGetter,
                D2D_MOCK_METHOD d2dMockMethod)
            {
                auto svgElement = CreateSvgElement();

                d2dMockMethod->SetExpectedCalls(1,
                    [=](ID2D1SvgElement** result)
                    {
                        return S_OK;
                    });

                ComPtr<RESULT_WRAPPED_TYPE> resultElement;
                Assert::AreEqual(S_OK, (*(svgElement.Get()).*wrappedGetter)(&resultElement));
                Assert::IsNull(resultElement.Get());
            }
        };

        TEST_METHOD_EX(CanvasSvgElementTests_get_FirstChild)
        {
            {
                ObjectPropertyFixture f;

                f.Verify_ObjectPropertyGetter_Null<ICanvasSvgElement>(
                    &CanvasSvgNamedElement::get_FirstChild,
                    &f.m_mockD2DElement->GetFirstChildMethod);

            }
            {
                ObjectPropertyFixture f;

                f.Verify_ObjectPropertyGetter_NonNull<ICanvasSvgElement>(
                    &CanvasSvgNamedElement::get_FirstChild,
                    &f.m_mockD2DElement->GetFirstChildMethod);

            }
        }

        TEST_METHOD_EX(CanvasSvgElementTests_get_LastChild)
        {
            {
                ObjectPropertyFixture f;

                f.Verify_ObjectPropertyGetter_Null<ICanvasSvgElement>(
                    &CanvasSvgNamedElement::get_LastChild,
                    &f.m_mockD2DElement->GetLastChildMethod);
            }
            {
                ObjectPropertyFixture f;

                f.Verify_ObjectPropertyGetter_NonNull<ICanvasSvgElement>(
                    &CanvasSvgNamedElement::get_LastChild,
                    &f.m_mockD2DElement->GetLastChildMethod);
            }
        }

        struct GetSiblingFixture : public Fixture
        {
            template<typename WRAPPED_GETTER, typename D2D_MOCK_METHOD>
            void VerifyObjectPropertyGetter(
                WRAPPED_GETTER&& wrappedGetter,
                D2D_MOCK_METHOD* d2dMockMethod)
            {
                auto svgElement = CreateSvgElement();

                ComPtr<MockD2DSvgElement> d2dReference = CreateMockD2DElement();
                auto reference = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(m_canvasDevice.Get(), d2dReference.Get());

                ComPtr<MockD2DSvgElement> d2dElementToReturn = CreateMockD2DElement();

                d2dMockMethod->SetExpectedCalls(1,
                    [=](ID2D1SvgElement* reference, ID2D1SvgElement** result)
                    {
                        Assert::IsTrue(IsSameInstance(d2dReference.Get(), reference));
                        return d2dElementToReturn.CopyTo(result);
                    });

                ComPtr<ICanvasSvgElement> resultElement;
                Assert::AreEqual(S_OK, (*(svgElement.Get()).*wrappedGetter)(
                    As<ICanvasSvgElement>(reference).Get(),
                    &resultElement));

                Assert::IsTrue(IsSameInstance(
                    d2dElementToReturn.Get(),
                    static_cast<CanvasSvgNamedElement*>(resultElement.Get())->GetResource().Get()));
            }
        };

        TEST_METHOD_EX(CanvasSvgElementTests_get_PreviousSibling)
        {
            GetSiblingFixture f;

            f.VerifyObjectPropertyGetter(
                &CanvasSvgNamedElement::GetPreviousSibling,
                &f.m_mockD2DElement->GetPreviousChildMethod);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_get_NextSibling)
        {
            GetSiblingFixture f;

            f.VerifyObjectPropertyGetter(
                &CanvasSvgNamedElement::GetNextSibling,
                &f.m_mockD2DElement->GetNextChildMethod);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_get_SpecifiedAttributes)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            f.m_mockD2DElement->GetSpecifiedAttributeCountMethod.SetExpectedCalls(1,
                [=]
                {
                    return 3;
                });

            static const uint32_t lengths[] = { 1, 2, 3 };
            static const wchar_t* names[] = { L"A", L"BC", L"DEF" };

            f.m_mockD2DElement->GetSpecifiedAttributeNameLengthMethod.SetExpectedCalls(3,
                [=](uint32_t attributeIndex, uint32_t* nameLength, BOOL* isInherited)
                {
                    Assert::IsTrue(attributeIndex < 3);
                    Assert::IsNull(isInherited);

                    *nameLength = lengths[attributeIndex];

                    return S_OK;
                });

            f.m_mockD2DElement->GetSpecifiedAttributeNameMethod.SetExpectedCalls(3,
                [=](uint32_t attributeIndex, wchar_t* name, uint32_t nameLength, BOOL* isInherited)
                {
                    Assert::IsTrue(attributeIndex < 3);
                    Assert::AreEqual(lengths[attributeIndex], nameLength - 1); // Account for null
                    Assert::IsNull(isInherited);

                    wcscpy_s(name, nameLength, names[attributeIndex]);

                    return S_OK;
                });

            uint32_t resultCount;
            HSTRING* resultArray;
            Assert::AreEqual(S_OK, svgElement->get_SpecifiedAttributes(&resultCount, &resultArray));

            Assert::AreEqual(3u, resultCount);
            for (int i = 0; i < 3; ++i)
            {
                Assert::AreEqual(names[i], WindowsGetStringRawBuffer(resultArray[i], nullptr));
            }
        }

        TEST_METHOD_EX(CanvasSvgElementTests_get_Tag)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            f.m_mockD2DElement->GetTagNameLengthMethod.SetExpectedCalls(1,
                [=]
                {
                    return 3;
                });

            f.m_mockD2DElement->GetTagNameMethod.SetExpectedCalls(1,
                [=](wchar_t* buffer, uint32_t bufferLength)
                {
                    Assert::AreEqual(3u, bufferLength - 1); // Account for null
                    wcscpy_s(buffer, bufferLength, L"ABC");
                    return S_OK;
                });

            HSTRING resultString;
            Assert::AreEqual(S_OK, svgElement->get_Tag(&resultString));

            Assert::AreEqual(L"ABC", WindowsGetStringRawBuffer(resultString, nullptr));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_get_HasChildren)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            f.m_mockD2DElement->HasChildrenMethod.SetExpectedCalls(1,
                [=]
                {
                    return TRUE;
                });

            boolean result;
            Assert::AreEqual(S_OK, svgElement->get_HasChildren(&result));
            Assert::IsTrue(result);
        }

        struct InsertRemoveFixture : public Fixture
        {
            template<typename WRAPPED_GETTER, typename D2D_MOCK_METHOD>
            void VerifyMethod(
                WRAPPED_GETTER&& wrappedGetter,
                D2D_MOCK_METHOD* d2dMockMethod)
            {
                auto svgElement = CreateSvgElement();

                ComPtr<MockD2DSvgElement> d2dReference1 = CreateMockD2DElement();
                ComPtr<MockD2DSvgElement> d2dReference2 = CreateMockD2DElement();

                auto reference1 = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(m_canvasDevice.Get(), d2dReference1.Get());
                auto reference2 = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(m_canvasDevice.Get(), d2dReference2.Get());

                d2dMockMethod->SetExpectedCalls(1,
                    [=](ID2D1SvgElement* arg1, ID2D1SvgElement* arg2)
                    {
                        Assert::IsTrue(IsSameInstance(d2dReference1.Get(), arg1));
                        Assert::IsTrue(IsSameInstance(d2dReference2.Get(), arg2));

                        return S_OK;
                    });

                ComPtr<ICanvasSvgElement> resultElement;
                Assert::AreEqual(S_OK, (*(svgElement.Get()).*wrappedGetter)(
                    As<ICanvasSvgElement>(reference1).Get(),
                    As<ICanvasSvgElement>(reference2).Get()));
            }
        };

        TEST_METHOD_EX(CanvasSvgElementTests_InsertChildBefore)
        {
            InsertRemoveFixture f;

            f.VerifyMethod(
                &CanvasSvgNamedElement::InsertChildBefore,
                &f.m_mockD2DElement->InsertChildBeforeMethod);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_IsAttributeSpecified)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;
                auto svgElement = f.CreateSvgElement();

                f.m_mockD2DElement->IsAttributeSpecifiedMethod.SetExpectedCalls(1,
                    [=](wchar_t const* name, BOOL* isInherited)
                    {
                        Assert::AreEqual(L"ABC", name);

                        if (i == 0)
                            Assert::IsFalse(*isInherited);
                        else
                            Assert::IsTrue(*isInherited);

                            return TRUE;
                    });

                boolean result;

                if (i==0)
                    Assert::AreEqual(S_OK, svgElement->IsAttributeSpecified(WinString(L"ABC"), &result));
                else
                    Assert::AreEqual(S_OK, svgElement->IsAttributeSpecifiedWithInherhited(WinString(L"ABC"), true, &result));

                Assert::IsTrue(result);
            }
        }

        TEST_METHOD_EX(CanvasSvgElementTests_RemoveAttribute)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            f.m_mockD2DElement->RemoveAttributeMethod.SetExpectedCalls(1,
                [=](wchar_t const* name)
                {
                    Assert::AreEqual(L"ABC", name);
                    return S_OK;
                });

            Assert::AreEqual(S_OK, svgElement->RemoveAttribute(WinString(L"ABC")));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_RemoveChild)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ComPtr<MockD2DSvgElement> d2dReference = f.CreateMockD2DElement();
            auto reference = ResourceManager::GetOrCreate<ICanvasSvgNamedElement>(f.m_canvasDevice.Get(), d2dReference.Get());

            f.m_mockD2DElement->RemoveChildMethod.SetExpectedCalls(1,
                [=](ID2D1SvgElement* arg)
                {
                    Assert::IsTrue(IsSameInstance(d2dReference.Get(), arg));

                    return S_OK;
                });

            Assert::AreEqual(S_OK, svgElement->RemoveChild(As<ICanvasSvgElement>(reference).Get()));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_ReplaceChild)
        {
            InsertRemoveFixture f;

            f.VerifyMethod(
                &CanvasSvgNamedElement::ReplaceChild,
                &f.m_mockD2DElement->ReplaceChildMethod);
        }
                
        struct get_DocumentFixture : public Fixture
        {
            void Verify_NonNull()
            {
                auto svgElement = CreateSvgElement();
                ComPtr<MockD2DSvgDocument> d2dObjectToReturn = Make<MockD2DSvgDocument>();

                m_mockD2DElement->GetDocumentMethod.SetExpectedCalls(1,
                    [=](ID2D1SvgDocument** result)
                    {
                        Assert::AreEqual(S_OK, d2dObjectToReturn.CopyTo(result));
                    });

                ComPtr<ICanvasSvgDocument> result;
                Assert::AreEqual(S_OK, svgElement->get_ContainingDocument(&result));

                Assert::IsTrue(IsSameInstance(
                    d2dObjectToReturn.Get(),
                    static_cast<CanvasSvgDocument*>(result.Get())->GetResource().Get()));
            }

            void Verify_Null()
            {
                auto svgElement = CreateSvgElement();

                m_mockD2DElement->GetDocumentMethod.SetExpectedCalls(1,
                    [=](ID2D1SvgDocument** result)
                    {
                        return S_OK;
                    });

                ComPtr<ICanvasSvgDocument> result;
                Assert::AreEqual(S_OK, svgElement->get_ContainingDocument(&result));
                Assert::IsNull(result.Get());
            }
        };

        TEST_METHOD_EX(CanvasSvgElementTests_get_ContainingDocument)
        {
            {
                get_DocumentFixture f;

                f.Verify_Null();
            }
            {
                get_DocumentFixture f;

                f.Verify_NonNull();
            }
        }


        TEST_METHOD_EX(CanvasSvgElementTests_get_Parent)
        {
            {
                ObjectPropertyFixture f;

                f.Verify_ObjectPropertyGetter_Null<ICanvasSvgNamedElement>(
                    &CanvasSvgNamedElement::get_Parent,
                    &f.m_mockD2DElement->GetParentMethod);

            }
            {
                ObjectPropertyFixture f;

                f.Verify_ObjectPropertyGetter_NonNull<ICanvasSvgNamedElement>(
                    &CanvasSvgNamedElement::get_Parent,
                    &f.m_mockD2DElement->GetParentMethod);

            }
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetAttribute)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ComPtr<MockD2DSvgPaint> d2dAttribute = Make<MockD2DSvgPaint>();
            auto attribute = ResourceManager::GetOrCreate<ICanvasSvgPaintAttribute>(f.m_canvasDevice.Get(), d2dAttribute.Get());

            f.m_mockD2DElement->SetAttributeValue_Object_Method.SetExpectedCalls(1,
                [=](PCWSTR name, ID2D1SvgAttribute* object)
                {
                    Assert::AreEqual(L"ABC", name);
                    Assert::IsTrue(IsSameInstance(d2dAttribute.Get(), object));

                    return S_OK;
                });

            Assert::AreEqual(S_OK, svgElement->SetAttribute(WinString(L"ABC"), As<ICanvasSvgAttribute>(attribute.Get()).Get()));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetAttribute)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            ComPtr<MockD2DSvgPaint> d2dAttribute = Make<MockD2DSvgPaint>();

            f.m_mockD2DElement->GetAttributeValue_Object_Method.SetExpectedCalls(1,
                [=](PCWSTR name, REFIID iid, void** object)
                {
                    Assert::AreEqual(L"ABC", name);
                    Assert::IsTrue(iid == __uuidof(ID2D1SvgAttribute));
                    return d2dAttribute.CopyTo(reinterpret_cast<ID2D1SvgAttribute**>(object));
                });

            ComPtr<ICanvasSvgAttribute> result;
            Assert::AreEqual(S_OK, svgElement->GetAttribute(WinString(L"ABC"), &result));

            auto resultDerived = As<ICanvasSvgPaintAttribute>(result.Get());
            auto resultImplementation = static_cast<CanvasSvgPaintAttribute*>(resultDerived.Get());

            Assert::IsTrue(IsSameInstance(d2dAttribute.Get(), resultImplementation->GetResource().Get()));
        }

        struct AttributeFixture : public Fixture
        {
            D2D1_SVG_ATTRIBUTE_STRING_TYPE ExpectedStringType;
            D2D1_SVG_ATTRIBUTE_POD_TYPE ExpectedPODType;

            AttributeFixture()
                : ExpectedStringType(D2D1_SVG_ATTRIBUTE_STRING_TYPE_ID)
                , ExpectedPODType(D2D1_SVG_ATTRIBUTE_POD_TYPE_FLOAT)
            {
            }

            template<typename WRAPPED_GETTER>
            void VerifyGetStringAttribute(
                WRAPPED_GETTER&& wrappedGetter)
            {
                auto svgElement = CreateSvgElement();

                m_mockD2DElement->GetAttributeValueLengthMethod.SetExpectedCalls(1,
                    [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_STRING_TYPE stringType, uint32_t* valueCount)
                    {
                        Assert::AreEqual(L"ABC", name);
                        Assert::AreEqual(ExpectedStringType, stringType);

                        *valueCount = 3;

                        return S_OK;
                    });

                m_mockD2DElement->GetAttributeValue_String_Method.SetExpectedCalls(1,
                    [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_STRING_TYPE stringType, PWSTR value, uint32_t valueCount)
                    {
                        Assert::AreEqual(L"ABC", name);
                        Assert::AreEqual(ExpectedStringType, stringType);
                        Assert::AreEqual(3u + 1u, valueCount);

                        wcscpy_s(value, valueCount, L"DEF");

                        return S_OK;
                    });

                HSTRING result;
                Assert::AreEqual(S_OK, (*(svgElement.Get()).*wrappedGetter)(WinString(L"ABC"), &result));
                Assert::AreEqual(L"DEF", WindowsGetStringRawBuffer(result, nullptr));
            }
            
            template<typename WRAPPED_SETTER>
            void VerifySetStringAttribute(
                WRAPPED_SETTER&& wrappedSetter)
            {
                auto svgElement = CreateSvgElement();

                m_mockD2DElement->SetAttributeValue_String_Method.SetExpectedCalls(1,
                    [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_STRING_TYPE stringType, PCWSTR value)
                    {
                        Assert::AreEqual(L"ABC", name);
                        Assert::AreEqual(ExpectedStringType, stringType);
                        Assert::AreEqual(L"DEF", value);

                        return S_OK;
                    });

                Assert::AreEqual(S_OK, (*(svgElement.Get()).*wrappedSetter)(WinString(L"ABC"), WinString(L"DEF")));
            }

            template<typename WRAPPED_GETTER, typename NATIVE_TYPE, typename WRAPPED_VALUE>
            void VerifyGetPODAttribute(
                WRAPPED_GETTER&& wrappedGetter,
                NATIVE_TYPE nativeValue,
                WRAPPED_VALUE wrappedValue)
            {
                auto svgElement = CreateSvgElement();

                m_mockD2DElement->GetAttributeValue_POD_Method.SetExpectedCalls(1,
                    [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_POD_TYPE podType, void* data, uint32_t dataSize)
                    {
                        Assert::AreEqual(L"ABC", name);
                        Assert::AreEqual(ExpectedPODType, podType);

                        Assert::AreEqual(sizeof(NATIVE_TYPE), static_cast<size_t>(dataSize));
                        *(static_cast<NATIVE_TYPE*>(data)) = nativeValue;

                        return S_OK;
                    });

                WRAPPED_VALUE result;
                Assert::AreEqual(S_OK, (*(svgElement.Get()).*wrappedGetter)(WinString(L"ABC"), &result));
                Assert::AreEqual(wrappedValue, result);
            }
            
            template<typename WRAPPED_SETTER, typename NATIVE_TYPE, typename WRAPPED_VALUE>
            void VerifySetPODAttribute(
                WRAPPED_SETTER&& wrappedSetter,
                NATIVE_TYPE nativeValue,
                WRAPPED_VALUE wrappedValue)
            {
                auto svgElement = CreateSvgElement();

                m_mockD2DElement->SetAttributeValue_POD_Method.SetExpectedCalls(1,
                    [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_POD_TYPE podType, const void* data, uint32_t dataSize)
                    {
                        Assert::AreEqual(L"ABC", name);
                        Assert::AreEqual(ExpectedPODType, podType);
                        Assert::AreEqual(sizeof(NATIVE_TYPE), static_cast<size_t>(dataSize));
                        Assert::AreEqual(nativeValue, *(static_cast<const NATIVE_TYPE*>(data)));

                        return S_OK;
                    });

                Assert::AreEqual(S_OK, (*(svgElement.Get()).*wrappedSetter)(WinString(L"ABC"), wrappedValue));
            }
        };

        TEST_METHOD_EX(CanvasSvgElementTests_SetIdAttribute)
        {
            AttributeFixture f;
            f.VerifySetStringAttribute(&CanvasSvgNamedElement::SetIdAttribute);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetIdAttribute)
        {
            AttributeFixture f;
            f.VerifyGetStringAttribute(&CanvasSvgNamedElement::GetIdAttribute);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetStringAttribute)
        {
            AttributeFixture f;
            f.ExpectedStringType = D2D1_SVG_ATTRIBUTE_STRING_TYPE_SVG;
            f.VerifySetStringAttribute(&CanvasSvgNamedElement::SetStringAttribute);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetStringAttribute)
        {
            AttributeFixture f;
            f.ExpectedStringType = D2D1_SVG_ATTRIBUTE_STRING_TYPE_SVG;
            f.VerifyGetStringAttribute(&CanvasSvgNamedElement::GetStringAttribute);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetFloatAttribute)
        {
            AttributeFixture f;
            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetFloatAttribute, 5.0f, 5.0f);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetFloatAttribute)
        {
            AttributeFixture f;
            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetFloatAttribute, 5.0f, 5.0f);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetColorAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_COLOR;

            D2D1_COLOR_F d2dColor = D2D1::ColorF(0, 0, 1.0f, 1.0f);
            Color wrappedColor = {0xFF, 0, 0, 0xFF};
            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetColorAttribute, d2dColor, wrappedColor);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetColorAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_COLOR;

            D2D1_COLOR_F d2dColor = D2D1::ColorF(0, 0, 1.0f, 1.0f);
            Color wrappedColor = { 0xFF, 0, 0, 0xFF };
            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetColorAttribute, d2dColor, wrappedColor);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetFilledRegionDeterminationAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_FILL_MODE;

            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetFilledRegionDeterminationAttribute, D2D1_FILL_MODE_WINDING, CanvasFilledRegionDetermination::Winding);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetFilledRegionDeterminationAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_FILL_MODE;

            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetFilledRegionDeterminationAttribute, D2D1_FILL_MODE_WINDING, CanvasFilledRegionDetermination::Winding);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetDisplayAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_DISPLAY;

            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetDisplayAttribute, D2D1_SVG_DISPLAY_NONE, CanvasSvgDisplay::None);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetDisplayAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_DISPLAY;

            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetDisplayAttribute, D2D1_SVG_DISPLAY_NONE, CanvasSvgDisplay::None);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetOverflowAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_OVERFLOW;

            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetOverflowAttribute, D2D1_SVG_OVERFLOW_HIDDEN, CanvasSvgOverflow::ClipToViewport);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetOverflowAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_OVERFLOW;

            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetOverflowAttribute, D2D1_SVG_OVERFLOW_HIDDEN, CanvasSvgOverflow::ClipToViewport);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetCapStyleAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_LINE_CAP;

            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetCapStyleAttribute, D2D1_SVG_LINE_CAP_ROUND, CanvasCapStyle::Round);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetCapStyleAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_LINE_CAP;

            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetCapStyleAttribute, D2D1_SVG_LINE_CAP_ROUND, CanvasCapStyle::Round);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetLineJoinAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_LINE_JOIN;

            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetLineJoinAttribute, D2D1_SVG_LINE_JOIN_ROUND, CanvasLineJoin::Round);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetLineJoinAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_LINE_JOIN;

            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetLineJoinAttribute, D2D1_SVG_LINE_JOIN_ROUND, CanvasLineJoin::Round);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetVisibilityAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_VISIBILITY;

            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetVisibilityAttribute, D2D1_SVG_VISIBILITY_HIDDEN, CanvasSvgVisibility::Hidden);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetVisibilityAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_VISIBILITY;

            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetVisibilityAttribute, D2D1_SVG_VISIBILITY_HIDDEN, CanvasSvgVisibility::Hidden);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetTransformAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_MATRIX;

            Matrix3x2 wrapped = float3x2::identity();
            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetTransformAttribute, D2D1::Matrix3x2F::Identity(), wrapped);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetTransformAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_MATRIX;

            Matrix3x2 wrapped = float3x2::identity();
            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetTransformAttribute, D2D1::Matrix3x2F::Identity(), wrapped);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetUnitsAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_UNIT_TYPE;

            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetUnitsAttribute, D2D1_SVG_UNIT_TYPE_OBJECT_BOUNDING_BOX, CanvasSvgUnits::ObjectBoundingBox);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetUnitsAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_UNIT_TYPE;

            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetUnitsAttribute, D2D1_SVG_UNIT_TYPE_OBJECT_BOUNDING_BOX, CanvasSvgUnits::ObjectBoundingBox);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetEdgeBehaviorAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_EXTEND_MODE;

            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetEdgeBehaviorAttribute, D2D1_EXTEND_MODE_MIRROR, CanvasEdgeBehavior::Mirror);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetEdgeBehaviorAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_EXTEND_MODE;

            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetEdgeBehaviorAttribute, D2D1_EXTEND_MODE_MIRROR, CanvasEdgeBehavior::Mirror);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetRectangleAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_VIEWBOX;

            D2D1_SVG_VIEWBOX nativeValue;
            nativeValue.x = 1;
            nativeValue.y = 2;
            nativeValue.width = 3;
            nativeValue.height = 4;

            Rect wrappedValue{ 1, 2, 3, 4 };
            f.VerifySetPODAttribute(&CanvasSvgNamedElement::SetRectangleAttribute, nativeValue, wrappedValue);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetRectangleAttribute)
        {
            AttributeFixture f;
            f.ExpectedPODType = D2D1_SVG_ATTRIBUTE_POD_TYPE_VIEWBOX;

            D2D1_SVG_VIEWBOX nativeValue;
            nativeValue.x = 1;
            nativeValue.y = 2;
            nativeValue.width = 3;
            nativeValue.height = 4;

            Rect wrappedValue{ 1, 2, 3, 4 };
            f.VerifyGetPODAttribute(&CanvasSvgNamedElement::GetRectangleAttribute, nativeValue, wrappedValue);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetLengthAttribute)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            f.m_mockD2DElement->GetAttributeValue_POD_Method.SetExpectedCalls(1,
                [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_POD_TYPE podType, void* data, uint32_t dataSize)
                {
                    Assert::AreEqual(L"ABC", name);
                    Assert::AreEqual(D2D1_SVG_ATTRIBUTE_POD_TYPE_LENGTH, podType);

                    Assert::AreEqual(sizeof(D2D1_SVG_LENGTH), static_cast<size_t>(dataSize));

                    D2D1_SVG_LENGTH nativeValue;
                    nativeValue.value = 123.0f;
                    nativeValue.units = D2D1_SVG_LENGTH_UNITS_PERCENTAGE;

                    *(static_cast<D2D1_SVG_LENGTH*>(data)) = nativeValue;

                    return S_OK;
                });

            CanvasSvgLengthUnits resultUnits;
            float resultValue;
            Assert::AreEqual(S_OK, svgElement->GetLengthAttribute(WinString(L"ABC"), &resultUnits, &resultValue));
            Assert::AreEqual(CanvasSvgLengthUnits::Percentage, resultUnits);
            Assert::AreEqual(123.0f, resultValue);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetLengthAttribute)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            f.m_mockD2DElement->SetAttributeValue_POD_Method.SetExpectedCalls(1,
                [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_POD_TYPE podType, const void* data, uint32_t dataSize)
                {
                    Assert::AreEqual(L"ABC", name);
                    Assert::AreEqual(D2D1_SVG_ATTRIBUTE_POD_TYPE_LENGTH, podType);
                    Assert::AreEqual(sizeof(D2D1_SVG_LENGTH), static_cast<size_t>(dataSize));

                    D2D1_SVG_LENGTH nativeValue;
                    nativeValue.value = 123.0f;
                    nativeValue.units = D2D1_SVG_LENGTH_UNITS_PERCENTAGE;
                    Assert::AreEqual(nativeValue.units, (static_cast<const D2D1_SVG_LENGTH*>(data)->units));
                    Assert::AreEqual(nativeValue.value, (static_cast<const D2D1_SVG_LENGTH*>(data)->value));

                    return S_OK;
                });

            Assert::AreEqual(S_OK, svgElement->SetLengthAttribute(WinString(L"ABC"), 123.0f, CanvasSvgLengthUnits::Percentage));
        }

        TEST_METHOD_EX(CanvasSvgElementTests_GetAspectRatioAttribute)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            f.m_mockD2DElement->GetAttributeValue_POD_Method.SetExpectedCalls(1,
                [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_POD_TYPE podType, void* data, uint32_t dataSize)
                {
                    Assert::AreEqual(L"ABC", name);
                    Assert::AreEqual(D2D1_SVG_ATTRIBUTE_POD_TYPE_PRESERVE_ASPECT_RATIO, podType);

                    Assert::AreEqual(sizeof(D2D1_SVG_PRESERVE_ASPECT_RATIO), static_cast<size_t>(dataSize));

                    D2D1_SVG_PRESERVE_ASPECT_RATIO nativeValue;
                    nativeValue.align = D2D1_SVG_ASPECT_ALIGN_X_MAX_Y_MAX;
                    nativeValue.meetOrSlice = D2D1_SVG_ASPECT_SCALING_SLICE;

                    *(static_cast<D2D1_SVG_PRESERVE_ASPECT_RATIO*>(data)) = nativeValue;

                    return S_OK;
                });

            CanvasSvgAspectAlignment resultAlignment;
            CanvasSvgAspectScaling resultScaling;
            Assert::AreEqual(S_OK, svgElement->GetAspectRatioAttribute(WinString(L"ABC"), &resultScaling, &resultAlignment));
            Assert::AreEqual(CanvasSvgAspectAlignment::XMaxYMax, resultAlignment);
            Assert::AreEqual(CanvasSvgAspectScaling::Slice, resultScaling);
        }

        TEST_METHOD_EX(CanvasSvgElementTests_SetAspectRatioAttribute)
        {
            Fixture f;
            auto svgElement = f.CreateSvgElement();

            f.m_mockD2DElement->SetAttributeValue_POD_Method.SetExpectedCalls(1,
                [=](PCWSTR name, D2D1_SVG_ATTRIBUTE_POD_TYPE podType, const void* data, uint32_t dataSize)
                {
                    Assert::AreEqual(L"ABC", name);
                    Assert::AreEqual(D2D1_SVG_ATTRIBUTE_POD_TYPE_PRESERVE_ASPECT_RATIO, podType);
                    Assert::AreEqual(sizeof(D2D1_SVG_PRESERVE_ASPECT_RATIO), static_cast<size_t>(dataSize));

                    D2D1_SVG_PRESERVE_ASPECT_RATIO nativeValue;
                    nativeValue.align = D2D1_SVG_ASPECT_ALIGN_X_MAX_Y_MAX;
                    nativeValue.meetOrSlice = D2D1_SVG_ASPECT_SCALING_SLICE;
                    Assert::AreEqual(nativeValue.align, static_cast<const D2D1_SVG_PRESERVE_ASPECT_RATIO*>(data)->align);
                    Assert::AreEqual(nativeValue.meetOrSlice, static_cast<const D2D1_SVG_PRESERVE_ASPECT_RATIO*>(data)->meetOrSlice);

                    return S_OK;
                });

            Assert::AreEqual(S_OK, svgElement->SetAspectRatioAttribute(WinString(L"ABC"), CanvasSvgAspectAlignment::XMaxYMax, CanvasSvgAspectScaling::Slice));
        }
    };
}
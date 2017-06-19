// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

// This file was automatically generated. Please do not edit it manually.

#pragma once

namespace canvas
{
    class MockD2DSvgElement : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<ID2D1SvgElement, ID2D1Resource>>
    {
    public:
        MOCK_METHOD1(GetDocument, void(ID2D1SvgDocument**));
        MOCK_METHOD2(GetTagName, HRESULT(PWSTR, UINT32));
        MOCK_METHOD0(GetTagNameLength, UINT32());
        MOCK_METHOD0(IsTextContent, BOOL());
        MOCK_METHOD1(GetParent, void(ID2D1SvgElement**));
        MOCK_METHOD0(HasChildren, BOOL());
        MOCK_METHOD1(GetFirstChild, void(ID2D1SvgElement**));
        MOCK_METHOD1(GetLastChild, void(ID2D1SvgElement**));
        MOCK_METHOD2(GetPreviousChild, HRESULT(ID2D1SvgElement*, ID2D1SvgElement**));
        MOCK_METHOD2(GetNextChild, HRESULT(ID2D1SvgElement*, ID2D1SvgElement**));
        MOCK_METHOD2(InsertChildBefore, HRESULT(ID2D1SvgElement*, ID2D1SvgElement*));
        MOCK_METHOD1(AppendChild, HRESULT(ID2D1SvgElement*));
        MOCK_METHOD2(ReplaceChild, HRESULT(ID2D1SvgElement*, ID2D1SvgElement*));
        MOCK_METHOD1(RemoveChild, HRESULT(ID2D1SvgElement*));
        MOCK_METHOD2(CreateChild, HRESULT(PCWSTR, ID2D1SvgElement**));
        MOCK_METHOD2(IsAttributeSpecified, BOOL(PCWSTR, BOOL*));
        MOCK_METHOD0(GetSpecifiedAttributeCount, UINT32());
        MOCK_METHOD4(GetSpecifiedAttributeName, HRESULT(UINT32, PWSTR, UINT32, BOOL*));
        MOCK_METHOD3(GetSpecifiedAttributeNameLength, HRESULT(UINT32, UINT32*, BOOL*));
        MOCK_METHOD1(RemoveAttribute, HRESULT(PCWSTR));
        MOCK_METHOD2(SetTextValue, HRESULT(CONST WCHAR*, UINT32));
        MOCK_METHOD2(GetTextValue, HRESULT(PWSTR, UINT32));
        MOCK_METHOD0(GetTextValueLength, UINT32());

        MOCK_METHOD3(GetAttributeValueLength, HRESULT(PCWSTR, D2D1_SVG_ATTRIBUTE_STRING_TYPE, UINT32*));
        
        CALL_COUNTER_WITH_MOCK(GetAttributeValue_Object_Method, HRESULT(PCWSTR, REFIID, void**));
        CALL_COUNTER_WITH_MOCK(GetAttributeValue_String_Method, HRESULT(PCWSTR, D2D1_SVG_ATTRIBUTE_STRING_TYPE, PWSTR, UINT32));
        CALL_COUNTER_WITH_MOCK(GetAttributeValue_POD_Method, HRESULT(PCWSTR, D2D1_SVG_ATTRIBUTE_POD_TYPE, void*, UINT32));

        IFACEMETHODIMP GetAttributeValue(PCWSTR a1, REFIID a2, void** a3) override
        {
            return GetAttributeValue_Object_Method.WasCalled(a1, a2, a3);
        }

        IFACEMETHODIMP GetAttributeValue(PCWSTR a1, D2D1_SVG_ATTRIBUTE_STRING_TYPE a2, PWSTR a3, UINT32 a4) override
        {
            return GetAttributeValue_String_Method.WasCalled(a1, a2, a3, a4);
        }

        IFACEMETHODIMP GetAttributeValue(PCWSTR a1, D2D1_SVG_ATTRIBUTE_POD_TYPE a2, void* a3, UINT32 a4) override
        {
            return GetAttributeValue_POD_Method.WasCalled(a1, a2, a3, a4);
        }

        CALL_COUNTER_WITH_MOCK(SetAttributeValue_Object_Method, HRESULT(PCWSTR, ID2D1SvgAttribute*));
        CALL_COUNTER_WITH_MOCK(SetAttributeValue_String_Method, HRESULT(PCWSTR, D2D1_SVG_ATTRIBUTE_STRING_TYPE, PCWSTR));
        CALL_COUNTER_WITH_MOCK(SetAttributeValue_POD_Method, HRESULT(PCWSTR, D2D1_SVG_ATTRIBUTE_POD_TYPE, void const*, UINT32));
        
        IFACEMETHODIMP SetAttributeValue(PCWSTR a1, ID2D1SvgAttribute* a2) override
        {
            return SetAttributeValue_Object_Method.WasCalled(a1, a2);
        }

        IFACEMETHODIMP SetAttributeValue(PCWSTR a1, D2D1_SVG_ATTRIBUTE_STRING_TYPE a2, PCWSTR a3) override
        {
            return SetAttributeValue_String_Method.WasCalled(a1, a2, a3);
        }

        IFACEMETHODIMP SetAttributeValue(PCWSTR a1, D2D1_SVG_ATTRIBUTE_POD_TYPE a2, void const* a3, UINT32 a4) override
        {
            return SetAttributeValue_POD_Method.WasCalled(a1, a2, a3, a4);
        }

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}

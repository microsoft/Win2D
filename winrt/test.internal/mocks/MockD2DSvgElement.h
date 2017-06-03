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

        MOCK_METHOD3(GetAttributeValue, HRESULT(PCWSTR, REFIID, void**));

        IFACEMETHODIMP GetAttributeValue(PCWSTR, D2D1_SVG_ATTRIBUTE_STRING_TYPE, PWSTR, UINT32) override
        {
            Assert::Fail(L"Unexpected call to GetAttributeValue");
            return E_NOTIMPL;
        }
        IFACEMETHODIMP GetAttributeValue(PCWSTR, D2D1_SVG_ATTRIBUTE_POD_TYPE, void*, UINT32) override
        {
            Assert::Fail(L"Unexpected call to GetAttributeValue");
            return E_NOTIMPL;
        }

        MOCK_METHOD2(SetAttributeValue, HRESULT(PCWSTR, ID2D1SvgAttribute*));

        IFACEMETHODIMP SetAttributeValue(PCWSTR, D2D1_SVG_ATTRIBUTE_STRING_TYPE, PCWSTR) override
        {
            Assert::Fail(L"Unexpected call to SetAttributeValue");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP SetAttributeValue(PCWSTR, D2D1_SVG_ATTRIBUTE_POD_TYPE, void const*, UINT32) override
        {
            Assert::Fail(L"Unexpected call to SetAttributeValue");
            return E_NOTIMPL;
        }

        MOCK_METHOD1_CONST(GetFactory, void(ID2D1Factory**));

    };
}

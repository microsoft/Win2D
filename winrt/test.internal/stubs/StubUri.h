// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class StubUri : public RuntimeClass<IUriRuntimeClass, IUriRuntimeClassWithAbsoluteCanonicalUri>
    {
        WinString m_path;

    public:
        StubUri(WinString const& path)
            : m_path(path)
        {
        }

        MOCK_METHOD1(get_AbsoluteUri, HRESULT(HSTRING*));
        MOCK_METHOD1(get_DisplayUri, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Domain, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Extension, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Fragment, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Host, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Password, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Path, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Query, HRESULT(HSTRING*));
        MOCK_METHOD1(get_QueryParsed, HRESULT(IWwwFormUrlDecoderRuntimeClass**));
        MOCK_METHOD1(get_RawUri, HRESULT(HSTRING*));
        MOCK_METHOD1(get_SchemeName, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Suspicious, HRESULT(boolean*));
        MOCK_METHOD1(get_UserName, HRESULT(HSTRING*));
        MOCK_METHOD1(get_Port, HRESULT(int*));
        MOCK_METHOD2(Equals, HRESULT(IUriRuntimeClass*, boolean*));
        MOCK_METHOD2(CombineUri, HRESULT(HSTRING, IUriRuntimeClass**));

        IFACEMETHODIMP get_AbsoluteCanonicalUri(HSTRING* value)
        {
            m_path.CopyTo(value);
            return S_OK;
        }

        MOCK_METHOD1(get_DisplayIri, HRESULT(HSTRING*));
    };
}
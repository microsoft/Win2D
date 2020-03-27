// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Controls;
    using namespace ABI::Windows::UI::Xaml;
    using namespace ABI::Windows::Foundation::Numerics;

    typedef ITypedEventHandler<XamlRoot*, XamlRootChangedEventArgs*> XamlRootChangedEventHandler;

    class StubXamlRoot : public RuntimeClass<IXamlRoot>
    {
        boolean m_isHostVisible;
        double m_rasterizationScale;

        ComPtr<MockEventSource<XamlRootChangedEventHandler>> ChangedEventSource;

    public:
        StubXamlRoot()
            : m_isHostVisible(true)
            , m_rasterizationScale(1.0)
            , ChangedEventSource(Make<MockEventSource<XamlRootChangedEventHandler>>(L"Changed"))
        {
        }

        void SetIsHostVisible(boolean isHostVisible)
        {
            m_isHostVisible = isHostVisible;
            RaiseChanged();
        }

        void SetRasterizationScale(double rasterizationScale)
        {
            m_rasterizationScale = rasterizationScale;
            RaiseChanged();
        }

        void RaiseChanged()
        {
            class XamlRootChangedEventArgs : public RuntimeClass<IXamlRootChangedEventArgs>
            {
            public:
                XamlRootChangedEventArgs()
                {}
            };

            auto args = Make<XamlRootChangedEventArgs>();
            ChangedEventSource->InvokeAll(this, args.Get());
        }

        IFACEMETHODIMP get_Content(IUIElement**) override
        {
            Assert::Fail(L"Unexpected call to get_Content");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_Size(Size*) override
        {
            Assert::Fail(L"Unexpected call to get_Size");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_RasterizationScale(double* value) override
        {
            *value = m_rasterizationScale;
            return S_OK;
        }

        IFACEMETHODIMP get_IsHostVisible(boolean* value) override
        {
            *value = m_isHostVisible;
            return S_OK;
        }

        IFACEMETHODIMP get_UIContext(IUIContext**) override
        {
            Assert::Fail(L"Unexpected call to get_UIContext");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP add_Changed(XamlRootChangedEventHandler* handler, EventRegistrationToken* token) override
        {
            return ChangedEventSource->add_Event(handler, token);
        }

        IFACEMETHODIMP remove_Changed(EventRegistrationToken token) override
        {
            return ChangedEventSource->remove_Event(token);
        }
    };

    class StubUserControlWithXamlRoot : public StubUserControl, public RuntimeClass<IUIElement10>
    {
        bool m_enableXamlRoot;  // Optionally passes QI for IUIElement10, which returns a XamlRoot

    public:
        ComPtr<StubXamlRoot> m_stubXamlRoot;

        StubUserControlWithXamlRoot()
            : StubUserControl()
            , m_stubXamlRoot(Make<StubXamlRoot>())
            , m_enableXamlRoot(false)
        {
            get_ParentMethod.AllowAnyCall();
        }

        void EnableXamlRoot()
        {
            m_enableXamlRoot = true;
        }

        IFACEMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override
        {
            HRESULT hr = StubUserControl::QueryInterface(riid, ppvObject);
            // StubUserControl will fail the QI to IUIElement10. Let the QI succeed if it's enabled.
            if (hr == E_NOINTERFACE && m_enableXamlRoot)
            {
                return RuntimeClass<IUIElement10>::QueryInterface(riid, ppvObject);
            }
            return hr;
        }

        IFACEMETHODIMP_(ULONG) AddRef() override
        {
            return StubUserControl::AddRef();
        }

        IFACEMETHODIMP_(ULONG) Release() override
        {
            return StubUserControl::Release();
        }

        IFACEMETHODIMP get_ActualOffset(Vector3*) override
        {
            Assert::Fail(L"Unexpected call to get_ActualOffset");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_ActualSize(Vector2*) override
        {
            Assert::Fail(L"Unexpected call to get_ActualSize");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_XamlRoot(IXamlRoot** value) override
        {
            return m_stubXamlRoot.CopyTo(value);
        }

        IFACEMETHODIMP put_XamlRoot(IXamlRoot*) override
        {
            Assert::Fail(L"Unexpected call to put_XamlRoot");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_UIContext(IUIContext**) override
        {
            Assert::Fail(L"Unexpected call to get_UIContext");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP get_Shadow(IShadow**) override
        {
            Assert::Fail(L"Unexpected call to get_Shadow");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP put_Shadow(IShadow*) override
        {
            Assert::Fail(L"Unexpected call to put_Shadow");
            return E_NOTIMPL;
        }
    };
}


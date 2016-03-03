// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

//
// This mock object is given to a GraphicsDevice so that we can check that it
// calls the methods we expect to call on it.
//
class MockDxgiDevice : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    ChainInterfaces<IDXGIDevice3, IDXGIDevice2, IDXGIDevice1, IDXGIDevice>>
{
public:
    std::function<void()> MockTrim;

    std::function<HRESULT(IID const&, void **)> MockGetParent;

    std::function<HRESULT(IDXGIAdapter**)> MockGetAdapter;

    STDMETHODIMP SetPrivateData(GUID const&,UINT,const void *)
    {
        Assert::Fail(L"Unexpected call to SetPrivateData");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetPrivateDataInterface(GUID const&,const IUnknown *)
    {
        Assert::Fail(L"Unexpected call to SetPrivateDataInterface");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetPrivateData(GUID const&,UINT *,void *)
    {
        Assert::Fail(L"Unexpected call to GetPrivateData");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetParent(IID const& iid, void** out)
    {
        if (MockGetParent)
        {
            return MockGetParent(iid, out);
        }
        Assert::Fail(L"Unexpected call to GetParent");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetAdapter(IDXGIAdapter** adapter)
    {
        if (MockGetAdapter)
        {
            return MockGetAdapter(adapter);
        }
        Assert::Fail(L"Unexpected call to GetAdapter");
        return E_NOTIMPL;
    }

    STDMETHODIMP CreateSurface(const DXGI_SURFACE_DESC *,UINT,DXGI_USAGE,const DXGI_SHARED_RESOURCE *,IDXGISurface **)
    {
        Assert::Fail(L"Unexpected call to CreateSurface");
        return E_NOTIMPL;
    }

    STDMETHODIMP QueryResourceResidency(IUnknown *const *,DXGI_RESIDENCY *,UINT)
    {
        Assert::Fail(L"Unexpected call to QueryResourceResidency");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetGPUThreadPriority(INT)
    {
        Assert::Fail(L"Unexpected call to SetGPUThreadPriority");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetGPUThreadPriority(INT *)
    {
        Assert::Fail(L"Unexpected call to GetGPUThreadPriority");
        return E_NOTIMPL;
    }

    STDMETHODIMP SetMaximumFrameLatency(UINT)
    {
        Assert::Fail(L"Unexpected call to SetMaximumFrameLatency");
        return E_NOTIMPL;
    }

    STDMETHODIMP GetMaximumFrameLatency(UINT *)
    {
        Assert::Fail(L"Unexpected call to GetMaximumFrameLatency");
        return E_NOTIMPL;
    }

    STDMETHODIMP OfferResources(UINT,IDXGIResource *const *,DXGI_OFFER_RESOURCE_PRIORITY)
    {
        Assert::Fail(L"Unexpected call to OfferResources");
        return E_NOTIMPL;
    }

    STDMETHODIMP ReclaimResources(UINT,IDXGIResource *const *,BOOL *)
    {
        Assert::Fail(L"Unexpected call to ReclaimResources");
        return E_NOTIMPL;
    }

    STDMETHODIMP EnqueueSetEvent(HANDLE)
    {
        Assert::Fail(L"Unexpected call to EnqueueSetEvent");
        return E_NOTIMPL;
    }

    STDMETHODIMP_(void) Trim()
    {
        if (MockTrim)
        {
            MockTrim();
        }
        else
        {
            Assert::Fail(L"Unexpected call to Trim");
        }
    }
};

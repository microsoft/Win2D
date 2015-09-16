// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

// TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//      add this to the project files

class MockPrintDocumentPackageTarget 
    : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IPrintDocumentPackageTarget>
{
public:
    CALL_COUNTER_WITH_MOCK(GetPackageTargetTypesMethod, HRESULT(UINT32*, GUID**));
    CALL_COUNTER_WITH_MOCK(GetPackageTargetMethod, HRESULT(const GUID&, const IID&, void** ));
    CALL_COUNTER_WITH_MOCK(CancelMethod, HRESULT());

    IFACEMETHODIMP GetPackageTargetTypes(UINT32 *a, GUID** b) override
    {
        return GetPackageTargetTypesMethod.WasCalled(a, b);
    }

    IFACEMETHODIMP GetPackageTarget(const GUID& type, const IID& iid, void** p) override
    {
        return GetPackageTargetMethod.WasCalled(type, iid, p);
    }

    IFACEMETHODIMP Cancel() override
    {
        return CancelMethod.WasCalled();
    }
};

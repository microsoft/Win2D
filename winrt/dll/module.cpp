// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#include "pch.h"
#include <wrl/module.h>
#include "../inc/LifespanTracker.h"

STDAPI_(BOOL)
DllMain(
    _In_     HINSTANCE inst,
    _In_     DWORD reason,
    _In_opt_ void* pvreserved
    )
{
    UNREFERENCED_PARAMETER(inst);
    UNREFERENCED_PARAMETER(pvreserved);

    switch (reason)
    {
    case DLL_PROCESS_DETACH:
        // WRL normally takes care of this immediately after DllMain returns,
        // but we move it in front of LifespanInfo::ReportLiveObjects to
        // avoid false positive complaints about leaked activation factories.
        auto &module = ::Microsoft::WRL::Module< ::Microsoft::WRL::InProc >::GetModule();
        module.Terminate(nullptr, true);

        LifespanInfo::ReportLiveObjectsNoLock();
        break;
    }

    return TRUE;
}

STDAPI 
DllGetActivationFactory(
    _In_         HSTRING activatibleClassId, 
    _COM_Outptr_ IActivationFactory** factory)
{
    auto &module = ::Microsoft::WRL::Module< ::Microsoft::WRL::InProc >::GetModule();
    return module.GetActivationFactory(activatibleClassId, factory);
}

STDAPI
DllGetClassObject(
    _In_ REFCLSID clsid,
    _In_ REFIID iid,
    _Outptr_ void** obj
    )
{
    auto &module = ::Microsoft::WRL::Module< ::Microsoft::WRL::InProc >::GetModule();
    return module.GetClassObject(clsid, iid, obj);
}

STDAPI DllCanUnloadNow()
{
    auto &module = ::Microsoft::WRL::Module< ::Microsoft::WRL::InProc >::GetModule();
    return module.Terminate() ? S_OK : S_FALSE;
}

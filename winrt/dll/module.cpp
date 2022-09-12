// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"
#include <wrl/module.h>

#pragma warning(push)
#pragma warning(disable:4459)   // declaration hides global declaration
#include <win2d.etw.h>
#pragma warning(pop)

#include "../inc/LifespanTracker.h"
#include "../inc/MicrosoftTelemetry.h"
#include "../inc/Win2DTelemetry.h"

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
    case DLL_PROCESS_ATTACH:
        EventRegisterWin2D();
        RegisterTraceLogging();
        break;
        
    case DLL_PROCESS_DETACH:
        // WRL normally takes care of this immediately after DllMain returns,
        // but we move it in front of LifespanInfo::ReportLiveObjects to
        // avoid false positive complaints about leaked activation factories.
        auto &module = ::Microsoft::WRL::Module< ::Microsoft::WRL::InProc >::GetModule();
        module.Terminate(nullptr, true);

        LifespanInfo::ReportLiveObjectsNoLock();

        EventUnregisterWin2D();
        UnregisterTraceLogging();
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

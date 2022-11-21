// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include <pch.h>

// GUID for Microsoft.Graphics.Canvas.Win2DTelemetry : {1db1fbd3-83ff-5e28-6a19-c43c12470650}
TRACELOGGING_DEFINE_PROVIDER(
    g_hTelemetryProvider,
    TELEMETRY_PROVIDER_NAME,
    (0x1db1fbd3, 0x83ff, 0x5e28, 0x6a, 0x19, 0xc4, 0x3c, 0x12, 0x47, 0x06, 0x50),
    TraceLoggingOptionMicrosoftTelemetry());

bool g_IsTelemetryProviderEnabled{};
UCHAR g_TelemetryProviderLevel{};
ULONGLONG g_TelemetryProviderMatchAnyKeyword{};
GUID g_TelemetryProviderActivityId{};

void WINAPI TelemetryProviderEnabledCallback(
    _In_      LPCGUID /*sourceId*/,
    _In_      ULONG isEnabled,
    _In_      UCHAR level,
    _In_      ULONGLONG matchAnyKeyword,
    _In_      ULONGLONG /*matchAllKeywords*/,
    _In_opt_  PEVENT_FILTER_DESCRIPTOR /*filterData*/,
    _In_opt_  PVOID /*callbackContext*/)
{
    g_IsTelemetryProviderEnabled = !!isEnabled;
    g_TelemetryProviderLevel = level;
    g_TelemetryProviderMatchAnyKeyword = matchAnyKeyword;
}

void RegisterTraceLogging()
{
    HRESULT hr = S_OK;

    TraceLoggingRegisterEx(g_hTelemetryProvider, TelemetryProviderEnabledCallback, nullptr);
    //Generate the ActivityId used to track the session
    hr = CoCreateGuid(&g_TelemetryProviderActivityId);
    if (FAILED(hr))
    {
        TraceLoggingWriteActivity(
            g_hTelemetryProvider,
            "CreateGuidError",
            nullptr,
            nullptr,
            TraceLoggingHResult(hr),
            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));

        g_TelemetryProviderActivityId = GUID_NULL;
    };
}

void UnregisterTraceLogging()
{
    TraceLoggingUnregister(g_hTelemetryProvider);
}

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <TraceLoggingProvider.h>
#include "MicrosoftTelemetry.h"

// TraceLogging provider name for telemetry.
#define TELEMETRY_PROVIDER_NAME "Microsoft.Graphics.Canvas.Win2DTelemetry"

TRACELOGGING_DECLARE_PROVIDER(g_hTelemetryProvider);

extern void RegisterTraceLogging();
extern void UnregisterTraceLogging();

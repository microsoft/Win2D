// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

// Configuration macro for use in TRACELOGGING_DEFINE_PROVIDER. The definition
// in this file configures the provider as a normal (non-telemetry) provider.
#define TraceLoggingOptionMicrosoftTelemetry() \
    // Empty definition for TraceLoggingOptionMicrosoftTelemetry

#define TelemetryPrivacyDataTag(tag) TraceLoggingUInt64((tag), "PartA_PrivTags")
#define PDT_ProductAndServicePerformance       0x0000000001000000u
#define MICROSOFT_KEYWORD_MEASURES      0x0000400000000000 // Bit 46

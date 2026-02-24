// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{

    struct GuidComparer {
        bool operator()(GUID const& left, GUID const& right) const {
            return memcmp(&left, &right, sizeof(GUID)) < 0;
        }
    };

}}}}
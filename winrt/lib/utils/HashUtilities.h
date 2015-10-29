// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    ComArray<BYTE> GetSha1Hash(BYTE const* data, size_t dataSize);

    IID GetVersion5Uuid(IID const& namespaceId, BYTE const* name, size_t nameSize);
    
}}}}

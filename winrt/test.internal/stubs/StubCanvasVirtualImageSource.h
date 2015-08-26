// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "../mocks/MockCanvasVirtualImageSource.h"
#include "../mocks/MockVirtualSurfaceImageSource.h"

class StubCanvasVirtualImageSource : public MockCanvasVirtualImageSource
{
    EventSource<ImageSourceRegionsInvalidatedHandler> m_regionsInvalidatedEventSource;
    ComPtr<MockVirtualSurfaceImageSource> m_vsis;
    
public:
    StubCanvasVirtualImageSource()
        : m_vsis(Make<MockVirtualSurfaceImageSource>())
    {
    }
    
    IFACEMETHODIMP get_Source(IVirtualSurfaceImageSource** value) override
    {
        return m_vsis.CopyTo(value);
    }
    
    IFACEMETHODIMP add_RegionsInvalidated(ImageSourceRegionsInvalidatedHandler* h, EventRegistrationToken* t) override
    {
        return m_regionsInvalidatedEventSource.Add(h, t);
    }

    IFACEMETHODIMP remove_RegionsInvalidated(EventRegistrationToken t) override
    {
        return m_regionsInvalidatedEventSource.Remove(t);
    }

    void RaiseRegionsInvalidated(std::vector<Rect> regions, Rect visibleRegion)
    {
        auto args = Make<CanvasRegionsInvalidatedEventArgs>(std::move(regions), visibleRegion);
        ThrowIfFailed(m_regionsInvalidatedEventSource.InvokeAll(this, args.Get()));
    }
};

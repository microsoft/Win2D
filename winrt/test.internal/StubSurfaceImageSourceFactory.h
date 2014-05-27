// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "StubSurfaceImageSource.h"

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;

    class StubSurfaceImageSourceFactory : public MockSurfaceImageSourceFactory
    {
        ComPtr<ISurfaceImageSource> m_SurfaceImageSource;

    public:
        StubSurfaceImageSourceFactory(ISurfaceImageSource* sis = nullptr)
            : m_SurfaceImageSource(sis)
        {
        }

        IFACEMETHODIMP CreateInstanceWithDimensions(
            int32_t,
            int32_t,
            IInspectable*,
            IInspectable** inner,
            ISurfaceImageSource** surfaceImageSource) override
        {
            return CreateInstanceWithDimensionsAndOpacity(0, 0, false, nullptr, inner, surfaceImageSource);
        }

        IFACEMETHODIMP CreateInstanceWithDimensionsAndOpacity(
            int32_t,
            int32_t,
            boolean,
            IInspectable*,
            IInspectable** inner,
            ISurfaceImageSource** surfaceImageSource) override
        {
            return ExceptionBoundary(
                [&]()
                {
                    ComPtr<ISurfaceImageSource> s;

                    if (m_SurfaceImageSource)
                        s = m_SurfaceImageSource;
                    else
                        s = Make<StubSurfaceImageSource>();

                    ThrowIfFailed(s.CopyTo(inner));
                    ThrowIfFailed(s.CopyTo(surfaceImageSource));
                });
        }
        
    };
}

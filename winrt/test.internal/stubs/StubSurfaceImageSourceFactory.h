// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    using namespace ABI::Microsoft::UI::Xaml::Media::Imaging;

    class StubSurfaceImageSourceFactory : public MockSurfaceImageSourceFactory
    {
        ComPtr<ISurfaceImageSource> m_surfaceImageSource;

    public:
        StubSurfaceImageSourceFactory(ISurfaceImageSource* sis = nullptr)
            : m_surfaceImageSource(sis)
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
                [&]
                {
                    ComPtr<ISurfaceImageSource> s;

                    if (m_surfaceImageSource)
                        s = m_surfaceImageSource;
                    else
                        s = Make<StubSurfaceImageSource>();

                    ThrowIfFailed(s.CopyTo(inner));
                    ThrowIfFailed(s.CopyTo(surfaceImageSource));
                });
        }
        
    };
}

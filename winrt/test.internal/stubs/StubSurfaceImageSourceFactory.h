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

#pragma once

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;

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

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

    class MockSurfaceImageSourceFactory : public RuntimeClass<ISurfaceImageSourceFactory>
    {
    public:
        std::function<ComPtr<ISurfaceImageSource>(int32_t,int32_t,bool,IInspectable*)> MockCreateInstanceWithDimensionsAndOpacity;

        IFACEMETHODIMP CreateInstanceWithDimensions(int32_t,int32_t,IInspectable *,IInspectable **,ISurfaceImageSource **) override
        {
            Assert::Fail(L"Unexpected call to CreateInstanceWithDimensions");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateInstanceWithDimensionsAndOpacity(
            int32_t width,
            int32_t height,
            boolean opaque,
            IInspectable* outer,
            IInspectable** inner,
            ISurfaceImageSource** surfaceImageSource) override
        {
            if (!MockCreateInstanceWithDimensionsAndOpacity)
            {
                Assert::Fail(L"Unexpected call to CreateInstanceWithDimensionsAndOpacity");
                return E_NOTIMPL;
            }

            return ExceptionBoundary(
                [&]
                {
                    auto sis = MockCreateInstanceWithDimensionsAndOpacity(width, height, !!opaque, outer);
                    ThrowIfFailed(sis.CopyTo(inner));
                    ThrowIfFailed(sis.CopyTo(surfaceImageSource));
                });
        }
    };
}

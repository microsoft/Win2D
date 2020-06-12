// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    using namespace ABI::Microsoft::UI::Xaml::Media::Imaging;

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

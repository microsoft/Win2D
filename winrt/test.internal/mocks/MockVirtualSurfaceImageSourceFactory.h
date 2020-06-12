// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

using namespace ABI::Microsoft::UI::Xaml::Media::Imaging;

class MockVirtualSurfaceImageSourceFactory
    : public RuntimeClass<IVirtualSurfaceImageSourceFactory>
{
public:
    CALL_COUNTER_WITH_MOCK(CreateInstanceWithDimensionsAndOpacityMethod, HRESULT(int32_t, int32_t, boolean, IVirtualSurfaceImageSource**));
    
    IFACEMETHODIMP CreateInstanceWithDimensions(int32_t width, int32_t height, IVirtualSurfaceImageSource** value) override
    {
        return CreateInstanceWithDimensionsAndOpacity(width, height, FALSE, value);
    }
        
    IFACEMETHODIMP CreateInstanceWithDimensionsAndOpacity(int32_t width, int32_t height, boolean isOpaque, IVirtualSurfaceImageSource** value) override
    {
        return CreateInstanceWithDimensionsAndOpacityMethod.WasCalled(width, height, isOpaque, value);
    }
};

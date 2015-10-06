// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteRenderingParams : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<
#if WINVER > _WIN32_WINNT_WINBLUE
        IDWriteRenderingParams3,
#endif
        IDWriteRenderingParams2, IDWriteRenderingParams1, IDWriteRenderingParams>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetGammaMethod, FLOAT());
        CALL_COUNTER_WITH_MOCK(GetEnhancedContrastMethod, FLOAT());
        CALL_COUNTER_WITH_MOCK(GetClearTypeLevelMethod, FLOAT());
        CALL_COUNTER_WITH_MOCK(GetPixelGeometryMethod, DWRITE_PIXEL_GEOMETRY());

        CALL_COUNTER_WITH_MOCK(GetGrayscaleEnhancedContrastMethod, FLOAT());

        CALL_COUNTER_WITH_MOCK(GetGridFitModeMethod, DWRITE_GRID_FIT_MODE());

        CALL_COUNTER_WITH_MOCK(GetRenderingModeMethod, DWRITE_RENDERING_MODE());

#if WINVER > _WIN32_WINNT_WINBLUE
        CALL_COUNTER_WITH_MOCK(GetRenderingMode1Method, DWRITE_RENDERING_MODE1());
#endif

        //
        // IDWriteRenderingParams
        //

        IFACEMETHODIMP_(FLOAT) GetGamma()
        {
            return GetGammaMethod.WasCalled();
        }

        IFACEMETHODIMP_(FLOAT) GetEnhancedContrast()
        {
            return GetEnhancedContrastMethod.WasCalled();
        }

        IFACEMETHODIMP_(FLOAT) GetClearTypeLevel()
        {
            return GetClearTypeLevelMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_PIXEL_GEOMETRY) GetPixelGeometry()
        {
            return GetPixelGeometryMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_RENDERING_MODE) GetRenderingMode()
        {
            return GetRenderingModeMethod.WasCalled();
        }

        //
        // IDWriteRenderingParams1
        //

        IFACEMETHODIMP_(FLOAT) GetGrayscaleEnhancedContrast()
        {
            return GetGrayscaleEnhancedContrastMethod.WasCalled();
        }

        //
        // IDWriteRenderingParams2
        //

        IFACEMETHODIMP_(DWRITE_GRID_FIT_MODE) GetGridFitMode()
        {
            return GetGridFitModeMethod.WasCalled();
        }

        //
        // IDWriteRenderingParams3
        //

#if WINVER > _WIN32_WINNT_WINBLUE
        IFACEMETHODIMP_(DWRITE_RENDERING_MODE1) GetRenderingMode1()
        {
            return GetRenderingMode1Method.WasCalled();
        }
#endif
    };
}

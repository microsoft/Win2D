// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteFactory : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        ChainInterfaces<
#if WINVER > _WIN32_WINNT_WINBLUE
        IDWriteFactory3,
#endif
        IDWriteFactory2, IDWriteFactory1, IDWriteFactory>>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetSystemFontCollectionMethod, HRESULT(IDWriteFontCollection**,BOOL));
        CALL_COUNTER_WITH_MOCK(CreateCustomFontCollectionMethod, HRESULT(IDWriteFontCollectionLoader*,void const*,uint32_t,IDWriteFontCollection**));
        CALL_COUNTER_WITH_MOCK(RegisterFontCollectionLoaderMethod, HRESULT(IDWriteFontCollectionLoader*));
        CALL_COUNTER_WITH_MOCK(UnregisterFontCollectionLoaderMethod, HRESULT(IDWriteFontCollectionLoader*));
        CALL_COUNTER_WITH_MOCK(CreateFontFileReferenceMethod, HRESULT(WCHAR const*,FILETIME const*,IDWriteFontFile**));
        CALL_COUNTER_WITH_MOCK(CreateCustomFontFileReferenceMethod, HRESULT(void const*,uint32_t,IDWriteFontFileLoader*,IDWriteFontFile**));
        CALL_COUNTER_WITH_MOCK(CreateFontFaceMethod, HRESULT(DWRITE_FONT_FACE_TYPE,uint32_t,IDWriteFontFile* const*,uint32_t,DWRITE_FONT_SIMULATIONS,IDWriteFontFace**));
        CALL_COUNTER_WITH_MOCK(CreateRenderingParamsMethod, HRESULT(IDWriteRenderingParams**));
        CALL_COUNTER_WITH_MOCK(CreateMonitorRenderingParamsMethod, HRESULT(HMONITOR,IDWriteRenderingParams**));
        CALL_COUNTER_WITH_MOCK(CreateCustomRenderingParamsMethod, HRESULT(FLOAT,FLOAT,FLOAT,DWRITE_PIXEL_GEOMETRY,DWRITE_RENDERING_MODE,IDWriteRenderingParams**));
        CALL_COUNTER_WITH_MOCK(RegisterFontFileLoaderMethod, HRESULT(IDWriteFontFileLoader*));
        CALL_COUNTER_WITH_MOCK(UnregisterFontFileLoaderMethod, HRESULT(IDWriteFontFileLoader*));
        CALL_COUNTER_WITH_MOCK(CreateTextFormatMethod, HRESULT(WCHAR const*,IDWriteFontCollection*,DWRITE_FONT_WEIGHT,DWRITE_FONT_STYLE,DWRITE_FONT_STRETCH,FLOAT,WCHAR const*,IDWriteTextFormat**));
        CALL_COUNTER_WITH_MOCK(CreateTypographyMethod, HRESULT(IDWriteTypography**));
        CALL_COUNTER_WITH_MOCK(GetGdiInteropMethod, HRESULT(IDWriteGdiInterop**));
        CALL_COUNTER_WITH_MOCK(CreateTextLayoutMethod, HRESULT(WCHAR const*,uint32_t,IDWriteTextFormat*,FLOAT,FLOAT,IDWriteTextLayout**));
        CALL_COUNTER_WITH_MOCK(CreateGdiCompatibleTextLayoutMethod, HRESULT(WCHAR const*,uint32_t,IDWriteTextFormat*,FLOAT,FLOAT,FLOAT,DWRITE_MATRIX const*,BOOL,IDWriteTextLayout**));
        CALL_COUNTER_WITH_MOCK(CreateEllipsisTrimmingSignMethod, HRESULT(IDWriteTextFormat*,IDWriteInlineObject**));
        CALL_COUNTER_WITH_MOCK(CreateTextAnalyzerMethod, HRESULT(IDWriteTextAnalyzer**));
        CALL_COUNTER_WITH_MOCK(CreateNumberSubstitutionMethod, HRESULT(DWRITE_NUMBER_SUBSTITUTION_METHOD,WCHAR const*,BOOL,IDWriteNumberSubstitution**));
        CALL_COUNTER_WITH_MOCK(CreateGlyphRunAnalysisMethod, HRESULT(DWRITE_GLYPH_RUN const*,FLOAT,DWRITE_MATRIX const*,DWRITE_RENDERING_MODE,DWRITE_MEASURING_MODE,FLOAT,FLOAT,IDWriteGlyphRunAnalysis**));
 
        CALL_COUNTER_WITH_MOCK(CreateCustomRenderingParamsMethod1, HRESULT(IDWriteRenderingParams2**));

#if WINVER > _WIN32_WINNT_WINBLUE
        CALL_COUNTER_WITH_MOCK(CreateCustomRenderingParamsMethod2, HRESULT(FLOAT,FLOAT,FLOAT,FLOAT,DWRITE_PIXEL_GEOMETRY,DWRITE_RENDERING_MODE1,DWRITE_GRID_FIT_MODE,IDWriteRenderingParams3**));
        CALL_COUNTER_WITH_MOCK(GetSystemFontSetMethod, HRESULT(IDWriteFontSet**));
        CALL_COUNTER_WITH_MOCK(CreateFontSetBuilderMethod, HRESULT(IDWriteFontSetBuilder**));
#endif

        IFACEMETHODIMP GetSystemFontCollection(
            IDWriteFontCollection** fontCollection,
            BOOL checkForUpdates) override
        {
            return GetSystemFontCollectionMethod.WasCalled(fontCollection, checkForUpdates);
        }

        IFACEMETHODIMP CreateCustomFontCollection(
            IDWriteFontCollectionLoader* collectionLoader,
            void const* collectionKey,
            uint32_t collectionKeySize,
            IDWriteFontCollection** fontCollection) override
        {
            return CreateCustomFontCollectionMethod.WasCalled(collectionLoader, collectionKey, collectionKeySize, fontCollection);
        }

        IFACEMETHODIMP RegisterFontCollectionLoader(
            IDWriteFontCollectionLoader* fontCollectionLoader) override
        {
            return RegisterFontCollectionLoaderMethod.WasCalled(fontCollectionLoader);
        }

        IFACEMETHODIMP UnregisterFontCollectionLoader(
            IDWriteFontCollectionLoader* fontCollectionLoader) override
        {
            return UnregisterFontCollectionLoaderMethod.WasCalled(fontCollectionLoader);
        }

        IFACEMETHODIMP CreateFontFileReference(
            WCHAR const* filePath,
            FILETIME const* lastWriteTime,
            IDWriteFontFile** fontFile) override
        {
            return CreateFontFileReferenceMethod.WasCalled(filePath, lastWriteTime, fontFile);
        }

        IFACEMETHODIMP CreateCustomFontFileReference(
            void const* fontFileReferenceKey,
            uint32_t fontFileReferenceKeySize,
            IDWriteFontFileLoader* fontFileLoader,
            IDWriteFontFile** fontFile) override
        {
            return CreateCustomFontFileReferenceMethod.WasCalled(fontFileReferenceKey, fontFileReferenceKeySize, fontFileLoader, fontFile);
        }

        IFACEMETHODIMP CreateFontFace(
            DWRITE_FONT_FACE_TYPE fontFaceType,
            uint32_t numberOfFiles,
            IDWriteFontFile* const* fontFiles,
            uint32_t faceIndex,
            DWRITE_FONT_SIMULATIONS fontFaceSimulationFlags,
            IDWriteFontFace** fontFace) override
        {
            return CreateFontFaceMethod.WasCalled(fontFaceType, numberOfFiles, fontFiles, faceIndex, fontFaceSimulationFlags, fontFace);
        }

        IFACEMETHODIMP CreateRenderingParams(
            IDWriteRenderingParams** renderingParams) override
        {
            return CreateRenderingParamsMethod.WasCalled(renderingParams);
        }

        IFACEMETHODIMP CreateMonitorRenderingParams(
            HMONITOR monitor,
            IDWriteRenderingParams** renderingParams) override
        {
            return CreateMonitorRenderingParamsMethod.WasCalled(monitor, renderingParams);
        }

        IFACEMETHODIMP CreateCustomRenderingParams(
            FLOAT gamma,
            FLOAT enhancedContrast,
            FLOAT clearTypeLevel,
            DWRITE_PIXEL_GEOMETRY pixelGeometry,
            DWRITE_RENDERING_MODE renderingMode,
            IDWriteRenderingParams** renderingParams) override
        {
            return CreateCustomRenderingParamsMethod.WasCalled(gamma, enhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, renderingParams);
        }

        IFACEMETHODIMP RegisterFontFileLoader(
            IDWriteFontFileLoader* fontFileLoader) override
        {
            return RegisterFontFileLoaderMethod.WasCalled(fontFileLoader);
        }

        IFACEMETHODIMP UnregisterFontFileLoader(
            IDWriteFontFileLoader* fontFileLoader) override
        {
            return UnregisterFontFileLoaderMethod.WasCalled(fontFileLoader);
        }

        IFACEMETHODIMP CreateTextFormat(
            WCHAR const* fontFamilyName,
            IDWriteFontCollection* fontCollection,
            DWRITE_FONT_WEIGHT fontWeight,
            DWRITE_FONT_STYLE fontStyle,
            DWRITE_FONT_STRETCH fontStretch,
            FLOAT fontSize,
            WCHAR const* localeName,
            IDWriteTextFormat** textFormat) override
        {
            return CreateTextFormatMethod.WasCalled(fontFamilyName, fontCollection, fontWeight, fontStyle, fontStretch, fontSize, localeName, textFormat);
        }

        IFACEMETHODIMP CreateTypography(
            IDWriteTypography** typography) override
        {
            return CreateTypographyMethod.WasCalled(typography);
        }

        IFACEMETHODIMP GetGdiInterop(
            IDWriteGdiInterop** gdiInterop) override
        {
            return GetGdiInteropMethod.WasCalled(gdiInterop);
        }

        IFACEMETHODIMP CreateTextLayout(
            WCHAR const* string,
            uint32_t stringLength,
            IDWriteTextFormat* textFormat,
            FLOAT maxWidth,
            FLOAT maxHeight,
            IDWriteTextLayout** textLayout) override
        {
            return CreateTextLayoutMethod.WasCalled(string, stringLength, textFormat, maxWidth, maxHeight, textLayout);
        }

        IFACEMETHODIMP CreateGdiCompatibleTextLayout(
            WCHAR const* string,
            uint32_t stringLength,
            IDWriteTextFormat* textFormat,
            FLOAT layoutWidth,
            FLOAT layoutHeight,
            FLOAT pixelsPerDip,
            DWRITE_MATRIX const* transform,
            BOOL useGdiNatural,
            IDWriteTextLayout** textLayout) override
        {
            return CreateGdiCompatibleTextLayoutMethod.WasCalled(string, stringLength, textFormat, layoutWidth, layoutHeight, pixelsPerDip, transform, useGdiNatural, textLayout);
        }

        IFACEMETHODIMP CreateEllipsisTrimmingSign(
            IDWriteTextFormat* textFormat,
            IDWriteInlineObject** trimmingSign) override
        {
            return CreateEllipsisTrimmingSignMethod.WasCalled(textFormat, trimmingSign);
        }

        IFACEMETHODIMP CreateTextAnalyzer(
            IDWriteTextAnalyzer** textAnalyzer) override
        {
            return CreateTextAnalyzerMethod.WasCalled(textAnalyzer);
        }

        IFACEMETHODIMP CreateNumberSubstitution(
            DWRITE_NUMBER_SUBSTITUTION_METHOD substitutionMethod,
            WCHAR const* localeName,
            BOOL ignoreUserOverride,
            IDWriteNumberSubstitution** numberSubstitution) override
        {
            return CreateNumberSubstitutionMethod.WasCalled(substitutionMethod, localeName, ignoreUserOverride, numberSubstitution);
        }

        IFACEMETHODIMP CreateGlyphRunAnalysis(
            DWRITE_GLYPH_RUN const* glyphRun,
            FLOAT pixelsPerDip,
            DWRITE_MATRIX const* transform,
            DWRITE_RENDERING_MODE renderingMode,
            DWRITE_MEASURING_MODE measuringMode,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            IDWriteGlyphRunAnalysis** glyphRunAnalysis) override
        {
            return CreateGlyphRunAnalysisMethod.WasCalled(glyphRun, pixelsPerDip, transform, renderingMode, measuringMode, baselineOriginX, baselineOriginY, glyphRunAnalysis);
        }

        //
        // IDWriteFactory1
        //

        IFACEMETHODIMP GetEudcFontCollection(
            IDWriteFontCollection**,
            BOOL)
        {
            Assert::Fail(L"Unexpected call to GetEudcFontCollection");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCustomRenderingParams(
            FLOAT,
            FLOAT,
            FLOAT,
            FLOAT,
            DWRITE_PIXEL_GEOMETRY,
            DWRITE_RENDERING_MODE,
            IDWriteRenderingParams1**)
        {
            Assert::Fail(L"Unexpected call to CreateCustomRenderingParams");
            return E_NOTIMPL;
        }

        //
        // IDWriteFactory2
        //

        IFACEMETHODIMP GetSystemFontFallback(IDWriteFontFallback**)
        {
            Assert::Fail(L"Unexpected call to GetSystemFontFallback");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateFontFallbackBuilder(IDWriteFontFallbackBuilder**)
        {
            Assert::Fail(L"Unexpected call to CreateFontFallbackBuilder");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP TranslateColorGlyphRun(
            FLOAT,
            FLOAT,
            DWRITE_GLYPH_RUN const*,
            DWRITE_GLYPH_RUN_DESCRIPTION const*,
            DWRITE_MEASURING_MODE,
            DWRITE_MATRIX const*,
            uint32_t,
            IDWriteColorGlyphRunEnumerator**)
        {
            Assert::Fail(L"Unexpected call to TranslateColorGlyphRun");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCustomRenderingParams(
            FLOAT,
            FLOAT,
            FLOAT,
            FLOAT,
            DWRITE_PIXEL_GEOMETRY,
            DWRITE_RENDERING_MODE,
            DWRITE_GRID_FIT_MODE,
            IDWriteRenderingParams2** out)
        {
            return CreateCustomRenderingParamsMethod1.WasCalled(out);
        }


        IFACEMETHODIMP CreateGlyphRunAnalysis(
            DWRITE_GLYPH_RUN const*,
            DWRITE_MATRIX const*,
            DWRITE_RENDERING_MODE,
            DWRITE_MEASURING_MODE,
            DWRITE_GRID_FIT_MODE,
            DWRITE_TEXT_ANTIALIAS_MODE,
            FLOAT,
            FLOAT,
            IDWriteGlyphRunAnalysis**)
        {
            Assert::Fail(L"Unexpected call to CreateGlyphRunAnalysis");
            return E_NOTIMPL;
        }

        //
        // IDWriteFactory3
        //

#if WINVER > _WIN32_WINNT_WINBLUE

        IFACEMETHODIMP CreateGlyphRunAnalysis(
            DWRITE_GLYPH_RUN const*,
            DWRITE_MATRIX const*,
            DWRITE_RENDERING_MODE1,
            DWRITE_MEASURING_MODE,
            DWRITE_GRID_FIT_MODE,
            DWRITE_TEXT_ANTIALIAS_MODE,
            FLOAT,
            FLOAT,
            IDWriteGlyphRunAnalysis**)
        {
            Assert::Fail(L"Unexpected call to CreateGlyphRunAnalysis");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateCustomRenderingParams(
            FLOAT gamma,
            FLOAT enhancedContrast,
            FLOAT grayscaleEnhancedContrast,
            FLOAT clearTypeLevel,
            DWRITE_PIXEL_GEOMETRY pixelGeometry,
            DWRITE_RENDERING_MODE1 renderingMode,
            DWRITE_GRID_FIT_MODE gridFitMode,
            IDWriteRenderingParams3** renderingParams)
        {
            return CreateCustomRenderingParamsMethod2.WasCalled(gamma, enhancedContrast, grayscaleEnhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, gridFitMode, renderingParams);
        }

        IFACEMETHODIMP CreateFontFaceReference(
            WCHAR const*,
            FILETIME const*,
            UINT32,
            DWRITE_FONT_SIMULATIONS,
            IDWriteFontFaceReference**)
        {
            Assert::Fail(L"Unexpected call to CreateFontFaceReference");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP CreateFontFaceReference(
            IDWriteFontFile*,
            UINT32,
            DWRITE_FONT_SIMULATIONS,
            IDWriteFontFaceReference**)
        {
            Assert::Fail(L"Unexpected call to CreateFontFaceReference");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetSystemFontSet(
            IDWriteFontSet** fontSet)
        {
            return GetSystemFontSetMethod.WasCalled(fontSet);
        }

        IFACEMETHODIMP CreateFontSetBuilder(
            IDWriteFontSetBuilder** fontSetBuilder)
        {
            return CreateFontSetBuilderMethod.WasCalled(fontSetBuilder);
        }

        IFACEMETHODIMP CreateFontCollectionFromFontSet(
            IDWriteFontSet*,
            IDWriteFontCollection1**)
        {
            Assert::Fail(L"Unexpected call to CreateFontCollectionFromFontSet");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetSystemFontCollection(
            BOOL,
            IDWriteFontCollection1**,
            BOOL)
        {
            Assert::Fail(L"Unexpected call to GetSystemFontCollection");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetFontDownloadQueue(
            IDWriteFontDownloadQueue**)
        {
            Assert::Fail(L"Unexpected call to GetFontDownloadQueue");
            return E_NOTIMPL;
        }

#endif

    };
}

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
    class MockDWriteFactory : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IDWriteFactory>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetSystemFontCollectionMethod, HRESULT(IDWriteFontCollection**,BOOL));
        CALL_COUNTER_WITH_MOCK(CreateCustomFontCollectionMethod, HRESULT(IDWriteFontCollectionLoader*,void const*,UINT32,IDWriteFontCollection**));
        CALL_COUNTER_WITH_MOCK(RegisterFontCollectionLoaderMethod, HRESULT(IDWriteFontCollectionLoader*));
        CALL_COUNTER_WITH_MOCK(UnregisterFontCollectionLoaderMethod, HRESULT(IDWriteFontCollectionLoader*));
        CALL_COUNTER_WITH_MOCK(CreateFontFileReferenceMethod, HRESULT(WCHAR const*,FILETIME const*,IDWriteFontFile**));
        CALL_COUNTER_WITH_MOCK(CreateCustomFontFileReferenceMethod, HRESULT(void const*,UINT32,IDWriteFontFileLoader*,IDWriteFontFile**));
        CALL_COUNTER_WITH_MOCK(CreateFontFaceMethod, HRESULT(DWRITE_FONT_FACE_TYPE,UINT32,IDWriteFontFile* const*,UINT32,DWRITE_FONT_SIMULATIONS,IDWriteFontFace**));
        CALL_COUNTER_WITH_MOCK(CreateRenderingParamsMethod, HRESULT(IDWriteRenderingParams**));
        CALL_COUNTER_WITH_MOCK(CreateMonitorRenderingParamsMethod, HRESULT(HMONITOR,IDWriteRenderingParams**));
        CALL_COUNTER_WITH_MOCK(CreateCustomRenderingParamsMethod, HRESULT(FLOAT,FLOAT,FLOAT,DWRITE_PIXEL_GEOMETRY,DWRITE_RENDERING_MODE,IDWriteRenderingParams**));
        CALL_COUNTER_WITH_MOCK(RegisterFontFileLoaderMethod, HRESULT(IDWriteFontFileLoader*));
        CALL_COUNTER_WITH_MOCK(UnregisterFontFileLoaderMethod, HRESULT(IDWriteFontFileLoader*));
        CALL_COUNTER_WITH_MOCK(CreateTextFormatMethod, HRESULT(WCHAR const*,IDWriteFontCollection*,DWRITE_FONT_WEIGHT,DWRITE_FONT_STYLE,DWRITE_FONT_STRETCH,FLOAT,WCHAR const*,IDWriteTextFormat**));
        CALL_COUNTER_WITH_MOCK(CreateTypographyMethod, HRESULT(IDWriteTypography**));
        CALL_COUNTER_WITH_MOCK(GetGdiInteropMethod, HRESULT(IDWriteGdiInterop**));
        CALL_COUNTER_WITH_MOCK(CreateTextLayoutMethod, HRESULT(WCHAR const*,UINT32,IDWriteTextFormat*,FLOAT,FLOAT,IDWriteTextLayout**));
        CALL_COUNTER_WITH_MOCK(CreateGdiCompatibleTextLayoutMethod, HRESULT(WCHAR const*,UINT32,IDWriteTextFormat*,FLOAT,FLOAT,FLOAT,DWRITE_MATRIX const*,BOOL,IDWriteTextLayout**));
        CALL_COUNTER_WITH_MOCK(CreateEllipsisTrimmingSignMethod, HRESULT(IDWriteTextFormat*,IDWriteInlineObject**));
        CALL_COUNTER_WITH_MOCK(CreateTextAnalyzerMethod, HRESULT(IDWriteTextAnalyzer**));
        CALL_COUNTER_WITH_MOCK(CreateNumberSubstitutionMethod, HRESULT(DWRITE_NUMBER_SUBSTITUTION_METHOD,WCHAR const*,BOOL,IDWriteNumberSubstitution**));
        CALL_COUNTER_WITH_MOCK(CreateGlyphRunAnalysisMethod, HRESULT(DWRITE_GLYPH_RUN const*,FLOAT,DWRITE_MATRIX const*,DWRITE_RENDERING_MODE,DWRITE_MEASURING_MODE,FLOAT,FLOAT,IDWriteGlyphRunAnalysis**));
 
        IFACEMETHODIMP GetSystemFontCollection(
            IDWriteFontCollection** fontCollection,
            BOOL checkForUpdates) override
        {
            return GetSystemFontCollectionMethod.WasCalled(fontCollection, checkForUpdates);
        }

        IFACEMETHODIMP CreateCustomFontCollection(
            IDWriteFontCollectionLoader* collectionLoader,
            void const* collectionKey,
            UINT32 collectionKeySize,
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
            UINT32 fontFileReferenceKeySize,
            IDWriteFontFileLoader* fontFileLoader,
            IDWriteFontFile** fontFile) override
        {
            return CreateCustomFontFileReferenceMethod.WasCalled(fontFileReferenceKey, fontFileReferenceKeySize, fontFileLoader, fontFile);
        }

        IFACEMETHODIMP CreateFontFace(
            DWRITE_FONT_FACE_TYPE fontFaceType,
            UINT32 numberOfFiles,
            IDWriteFontFile* const* fontFiles,
            UINT32 faceIndex,
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
            UINT32 stringLength,
            IDWriteTextFormat* textFormat,
            FLOAT maxWidth,
            FLOAT maxHeight,
            IDWriteTextLayout** textLayout) override
        {
            return CreateTextLayoutMethod.WasCalled(string, stringLength, textFormat, maxWidth, maxHeight, textLayout);
        }

        IFACEMETHODIMP CreateGdiCompatibleTextLayout(
            WCHAR const* string,
            UINT32 stringLength,
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
    };
}

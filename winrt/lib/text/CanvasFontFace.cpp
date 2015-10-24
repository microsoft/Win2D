// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasFontFace.h"
#include "TextUtilities.h"
#include "effects/shader/PixelShaderEffect.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

static float DpiToPixelsPerDip(float dpi)
{
    return dpi / DEFAULT_DPI;
}

float CanvasFontFace::DesignSpaceToEmSpace(int designSpaceUnits, unsigned short designUnitsPerEm)
{
    return static_cast<float>(designSpaceUnits) / static_cast<float>(designUnitsPerEm);
}

CanvasFontFace::CanvasFontFace(DWriteFontReferenceType* fontFace)
    : ResourceWrapper(fontFace)
{
}

IFACEMETHODIMP CanvasFontFace::GetRecommendedRenderingMode(
    float fontSize,
    float dpi,
    CanvasTextMeasuringMode measuringMode,
    ICanvasTextRenderingParameters* renderingParameters,
    CanvasTextRenderingMode* renderingMode)
{ 
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(renderingMode);

            DWRITE_RENDERING_MODE dwriteRenderingModeBase;
                        
            ThrowIfFailed(static_cast<IDWriteFontFace*>(GetRealizedFontFace().Get())->GetRecommendedRenderingMode(
                fontSize,
                DpiToPixelsPerDip(dpi),
                ToDWriteMeasuringMode(measuringMode),
                renderingParameters ? GetWrappedResource<IDWriteRenderingParams>(renderingParameters).Get() : nullptr,
                &dwriteRenderingModeBase));

            *renderingMode = ToCanvasTextRenderingMode(static_cast<DWriteRenderingMode>(dwriteRenderingModeBase));
        });
}

IFACEMETHODIMP CanvasFontFace::GetRecommendedRenderingModeWithAllOptions(
    float fontSize,
    float dpi,
    CanvasTextMeasuringMode measuringMode,
    ICanvasTextRenderingParameters* renderingParameters,
    Matrix3x2 transform,
    boolean isSideways,
    CanvasAntialiasing outlineThreshold,
    CanvasTextRenderingMode* renderingMode)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(renderingMode);

            DWriteRenderingMode dwriteRenderingMode;
            DWRITE_GRID_FIT_MODE dwriteGridFitMode;
                        
            ThrowIfFailed(GetRealizedFontFace()->GetRecommendedRenderingMode(
                fontSize,
                dpi,
                dpi,
                ReinterpretAs<DWRITE_MATRIX*>(&transform),
                isSideways,
                ToDWriteOutlineThreshold(outlineThreshold),
                ToDWriteMeasuringMode(measuringMode),
                GetWrappedResource<IDWriteRenderingParams>(renderingParameters).Get(),
                &dwriteRenderingMode,
                &dwriteGridFitMode));

            *renderingMode = ToCanvasTextRenderingMode(dwriteRenderingMode);
        });
}

IFACEMETHODIMP CanvasFontFace::GetRecommendedGridFit(
    float fontSize,
    float dpi,
    CanvasTextMeasuringMode measuringMode,
    ICanvasTextRenderingParameters* renderingParameters,
    Matrix3x2 transform,
    boolean isSideways,
    CanvasAntialiasing outlineThreshold,
    CanvasTextGridFit* gridFit)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(gridFit);

            DWriteRenderingMode dwriteRenderingMode;
            DWRITE_GRID_FIT_MODE dwriteGridFitMode;
  
            ThrowIfFailed(GetRealizedFontFace()->GetRecommendedRenderingMode(
                fontSize,
                dpi,
                dpi,
                ReinterpretAs<DWRITE_MATRIX*>(&transform),
                isSideways,
                ToDWriteOutlineThreshold(outlineThreshold),
                ToDWriteMeasuringMode(measuringMode),
                GetWrappedResource<IDWriteRenderingParams>(renderingParameters).Get(),
                &dwriteRenderingMode,
                &dwriteGridFitMode));

            *gridFit = ToCanvasTextGridFit(dwriteGridFitMode);
        });
}

IFACEMETHODIMP CanvasFontFace::get_GlyphBox(Rect* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            D2D1_RECT_F glyphBox = { 
                DesignSpaceToEmSpace(metrics.glyphBoxLeft, metrics.designUnitsPerEm),
                DesignSpaceToEmSpace(metrics.glyphBoxTop, metrics.designUnitsPerEm),
                DesignSpaceToEmSpace(metrics.glyphBoxRight, metrics.designUnitsPerEm),
                DesignSpaceToEmSpace(metrics.glyphBoxBottom, metrics.designUnitsPerEm)
            };

            *value = FromD2DRect(glyphBox);
        });
}

IFACEMETHODIMP CanvasFontFace::get_SubscriptPosition(Vector2* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            value->X = DesignSpaceToEmSpace(metrics.subscriptPositionX, metrics.designUnitsPerEm);
            value->Y = DesignSpaceToEmSpace(metrics.subscriptPositionY, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_SubscriptSize(Size* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            value->Width = DesignSpaceToEmSpace(metrics.subscriptSizeX, metrics.designUnitsPerEm);
            value->Height = DesignSpaceToEmSpace(metrics.subscriptSizeY, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_SuperscriptPosition(Vector2* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            value->X = DesignSpaceToEmSpace(metrics.superscriptPositionX, metrics.designUnitsPerEm);
            value->Y = DesignSpaceToEmSpace(metrics.superscriptPositionY, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_SuperscriptSize(Size* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            value->Width = DesignSpaceToEmSpace(metrics.superscriptSizeX, metrics.designUnitsPerEm);
            value->Height = DesignSpaceToEmSpace(metrics.superscriptSizeY, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_HasTypographicMetrics(boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = !!metrics.hasTypographicMetrics;
        });
}

IFACEMETHODIMP CanvasFontFace::get_Ascent(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.ascent, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_Descent(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.descent, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_LineGap(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.lineGap, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_CapHeight(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.capHeight, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_LowercaseLetterHeight(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.xHeight, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_UnderlinePosition(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.underlinePosition, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_UnderlineThickness(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.underlineThickness, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_StrikethroughPosition(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.strikethroughPosition, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_StrikethroughThickness(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_FONT_METRICS1 metrics;                        
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(metrics.strikethroughThickness, metrics.designUnitsPerEm);
        });
}

IFACEMETHODIMP CanvasFontFace::get_CaretSlopeRise(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_CARET_METRICS metrics;                        
            GetRealizedFontFace()->GetCaretMetrics(&metrics);

            *value = metrics.slopeRise;
        });
}

IFACEMETHODIMP CanvasFontFace::get_CaretSlopeRun(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_CARET_METRICS metrics;                        
            GetRealizedFontFace()->GetCaretMetrics(&metrics);

            *value = metrics.slopeRun;
        });
}

IFACEMETHODIMP CanvasFontFace::get_CaretOffset(float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            DWRITE_CARET_METRICS caretMetrics;                        
            GetRealizedFontFace()->GetCaretMetrics(&caretMetrics);

            DWRITE_FONT_METRICS1 metrics;
            GetRealizedFontFace()->GetMetrics(&metrics);

            *value = DesignSpaceToEmSpace(caretMetrics.offset, metrics.designUnitsPerEm);
        });
}


IFACEMETHODIMP CanvasFontFace::get_UnicodeRanges(
    uint32_t* valueCount,
    CanvasUnicodeRange** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            uint32_t rangeCount;
            if (GetRealizedFontFace()->GetUnicodeRanges(0, nullptr, &rangeCount) != E_NOT_SUFFICIENT_BUFFER)
            {
                ThrowHR(E_UNEXPECTED);
            }

            ComArray<CanvasUnicodeRange> output(rangeCount);

            uint32_t actualRangeCount;
            ThrowIfFailed(GetRealizedFontFace()->GetUnicodeRanges(rangeCount, ReinterpretAs<DWRITE_UNICODE_RANGE*>(output.GetData()), &actualRangeCount));

            output.Detach(valueCount, valueElements);
        });
}


IFACEMETHODIMP CanvasFontFace::get_IsMonospaced(boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = !!GetRealizedFontFace()->IsMonospacedFont();
        });
}


IFACEMETHODIMP CanvasFontFace::GetVerticalGlyphVariants(
    uint32_t inputCount,
    int* inputElements,
    uint32_t* outputCount,
    int** outputElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(inputElements);
            CheckInPointer(outputCount);
            CheckAndClearOutPointer(outputElements);

            std::vector<unsigned short> nominalGlyphIndices;
            nominalGlyphIndices.reserve(inputCount);
            for (uint32_t i = 0; i < inputCount; ++i)
            {
                if (inputElements[i] < 0 || inputElements[i] > USHORT_MAX)
                    ThrowHR(E_INVALIDARG);

                nominalGlyphIndices.push_back(static_cast<unsigned short>(inputElements[i]));
            }

            std::vector<unsigned short> verticalGlyphIndices(inputCount);

            ThrowIfFailed(GetRealizedFontFace()->GetVerticalGlyphVariants(inputCount, nominalGlyphIndices.data(), verticalGlyphIndices.data()));

            ComArray<int> output(inputCount);

            for (uint32_t i = 0; i < inputCount; ++i)
                output[i] = static_cast<int>(verticalGlyphIndices[i]);

            output.Detach(outputCount, outputElements);
        });
}


IFACEMETHODIMP CanvasFontFace::get_HasVerticalGlyphVariants(boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = !!GetRealizedFontFace()->HasVerticalGlyphVariants();
        });
}


IFACEMETHODIMP CanvasFontFace::get_FileFormatType(CanvasFontFileFormatType* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = ToCanvasFontFileFormatType(GetRealizedFontFace()->GetType());
        });
}


IFACEMETHODIMP CanvasFontFace::get_Simulations(CanvasFontSimulations* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = ToCanvasFontSimulations(GetRealizedFontFace()->GetSimulations());
        });
}


IFACEMETHODIMP CanvasFontFace::get_IsSymbolFont(boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = !!GetRealizedFontFace()->IsSymbolFont();
        });
}


IFACEMETHODIMP CanvasFontFace::get_GlyphCount(uint32_t* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            
            *value = GetRealizedFontFace()->GetGlyphCount();
        });
}


IFACEMETHODIMP CanvasFontFace::GetGlyphIndices(
    uint32_t inputCount,
    uint32_t* inputElements,
    uint32_t* outputCount,
    int** outputElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(inputElements);
            CheckInPointer(outputCount);
            CheckAndClearOutPointer(outputElements);

            std::vector<unsigned short> glyphIndices(inputCount);

            ThrowIfFailed(GetRealizedFontFace()->GetGlyphIndices(inputElements, inputCount, glyphIndices.data()));

            ComArray<int> output(inputCount);

            for (uint32_t i = 0; i < inputCount; ++i)
                output[i] = static_cast<int>(glyphIndices[i]);

            output.Detach(outputCount, outputElements);
        });
}

IFACEMETHODIMP CanvasFontFace::get_Weight(FontWeight* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = ToWindowsFontWeight(GetPhysicalPropertyContainer()->GetWeight());
        });
}

IFACEMETHODIMP CanvasFontFace::get_Stretch(FontStretch* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = ToWindowsFontStretch(GetPhysicalPropertyContainer()->GetStretch());
        });
}

IFACEMETHODIMP CanvasFontFace::get_Style(FontStyle* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            *value = ToWindowsFontStyle(GetPhysicalPropertyContainer()->GetStyle());
        });
}

#if WINVER > _WIN32_WINNT_WINBLUE
IFACEMETHODIMP CanvasFontFace::get_FamilyNames(IMapView<HSTRING, HSTRING>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            ComPtr<IDWriteLocalizedStrings> localizedStrings;
            ThrowIfFailed(GetPhysicalPropertyContainer()->GetFamilyNames(&localizedStrings));

            CopyLocalizedStringsToMapView(localizedStrings, values);
        });
}
#endif

IFACEMETHODIMP CanvasFontFace::get_FaceNames(IMapView<HSTRING, HSTRING>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            ComPtr<IDWriteLocalizedStrings> localizedStrings;
            ThrowIfFailed(GetPhysicalPropertyContainer()->GetFaceNames(&localizedStrings));

            CopyLocalizedStringsToMapView(localizedStrings, values);
        });
}


IFACEMETHODIMP CanvasFontFace::GetInformationalStrings(
    CanvasFontInformation fontInformation,
    IMapView<HSTRING, HSTRING>** values)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(values);

            ComPtr<IDWriteLocalizedStrings> localizedStrings;
            BOOL exists;
            ThrowIfFailed(GetPhysicalPropertyContainer()->GetInformationalStrings(ToDWriteInformationalStringId(fontInformation), &localizedStrings, &exists));

            CopyLocalizedStringsToMapView(localizedStrings, values);
        });
}

IFACEMETHODIMP CanvasFontFace::HasCharacter(UINT32 unicodeValue, boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

#if WINVER > _WIN32_WINNT_WINBLUE
            *value = !!GetPhysicalPropertyContainer()->HasCharacter(unicodeValue);
#else
            BOOL exists;
            ThrowIfFailed(GetPhysicalPropertyContainer()->HasCharacter(unicodeValue, &exists));
            *value = !!exists;
#endif
        });
}

IFACEMETHODIMP CanvasFontFace::Close()
{
    return ResourceWrapper::Close();
}

ComPtr<DWriteFontFaceType> CanvasFontFace::GetRealizedFontFace()
{
    if (!m_realizedFontFace)
    {
        auto& resource = GetResource();

#if WINVER > _WIN32_WINNT_WINBLUE
        HRESULT hr = resource->CreateFontFace(&m_realizedFontFace);

        if (hr == DWRITE_E_REMOTEFONT)
        {
            ThrowHR(hr, Strings::RemoteFontUnavailable);
        }
        ThrowIfFailed(hr);
#else
        ComPtr<IDWriteFontFace> baseFontFace;
        ThrowIfFailed(resource->CreateFontFace(&baseFontFace));
        m_realizedFontFace = As<DWriteFontFaceType>(baseFontFace);
#endif
    }

    return m_realizedFontFace;
}

ComPtr<DWritePhysicalFontPropertyContainer> CanvasFontFace::GetPhysicalPropertyContainer()
{
#if WINVER > _WIN32_WINNT_WINBLUE
    return GetRealizedFontFace();
#else
    return GetResource();
#endif
}
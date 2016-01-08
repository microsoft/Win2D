// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "brushes/CanvasSolidColorBrush.h"
#include "brushes/CanvasLinearGradientBrush.h"
#include "brushes/CanvasRadialGradientBrush.h"
#include "brushes/CanvasImageBrush.h"

#include "CanvasTextLayout.h"
#include "CanvasFontFace.h"
#include "TextUtilities.h"
#include "InternalDWriteTextRenderer.h"

using namespace ABI::Microsoft::Graphics::Canvas;
using namespace ABI::Microsoft::Graphics::Canvas::Text;

template <class GET_ATTRIBUTE_LENGTH_METHOD, class GET_ATTRIBUTE_METHOD>
static WinString GetTextFromLayoutResource(
    GET_ATTRIBUTE_LENGTH_METHOD fnGetLength,
    GET_ATTRIBUTE_METHOD fnGet,
    int32_t characterIndex, 
    IDWriteTextLayout2* layout)
{
    WinStringBuilder stringBuilder;
    uint32_t attributeLength;
    ThrowIfFailed((layout->*fnGetLength)(characterIndex, &attributeLength, nullptr));
    attributeLength++; // Account for null terminator

    auto buffer = stringBuilder.Allocate(attributeLength);
    ThrowIfFailed((*layout.*fnGet)(characterIndex, buffer, attributeLength, nullptr));
    return stringBuilder.Get();
}

CanvasTextLayoutRegion ToHitTestDescription(DWRITE_HIT_TEST_METRICS const& hitTestMetrics)
{
    CanvasTextLayoutRegion description;
    description.CharacterIndex = hitTestMetrics.textPosition;
    description.CharacterCount = hitTestMetrics.length;
    description.LayoutBounds.X = hitTestMetrics.left;
    description.LayoutBounds.Y = hitTestMetrics.top;
    description.LayoutBounds.Width = hitTestMetrics.width;
    description.LayoutBounds.Height = hitTestMetrics.height;
    return description;
}


ComPtr<CanvasTextLayout> CanvasTextLayout::CreateNew(
    ICanvasResourceCreator* resourceCreator,
    HSTRING text,
    ICanvasTextFormat* textFormat,
    float requestedWidth,
    float requestedHeight)
{
    auto customFontManager = CustomFontManager::GetInstance();
    auto dwriteFactory = customFontManager->GetSharedFactory();

    uint32_t textLength;
    auto textBuffer = WindowsGetStringRawBuffer(text, &textLength);
    ThrowIfNullPointer(textBuffer, E_INVALIDARG);

    ComPtr<IDWriteTextLayout> dwriteTextLayout;
    ThrowIfFailed(dwriteFactory->CreateTextLayout(
        textBuffer,
        textLength,
        GetWrappedResource<IDWriteTextFormat>(textFormat).Get(),
        requestedWidth,
        requestedHeight,
        &dwriteTextLayout));

    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    auto textLayout = Make<CanvasTextLayout>(
        device.Get(),
        As<DWriteTextLayoutType>(dwriteTextLayout).Get());
    CheckMakeResult(textLayout);

    CanvasLineSpacingMode lineSpacingMode{};
#if WINVER > _WIN32_WINNT_WINBLUE
    ThrowIfFailed(textFormat->get_LineSpacingMode(&lineSpacingMode));
#endif
    textLayout->SetLineSpacingModeInternal(lineSpacingMode);

    CanvasTrimmingSign trimmingSign;
    ThrowIfFailed(textFormat->get_TrimmingSign(&trimmingSign));
    textLayout->SetTrimmingSignInternal(trimmingSign);

    return textLayout;
}


//
// CanvasTextLayoutFactory implementation
//

IFACEMETHODIMP CanvasTextLayoutFactory::Create(
    ICanvasResourceCreator* resourceCreator,
    HSTRING textString,
    ICanvasTextFormat* textFormat,
    float requestedWidth,
    float requestedHeight,
    ICanvasTextLayout** textLayout)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(textFormat);
            CheckAndClearOutPointer(textLayout);

            auto newTextLayout = CanvasTextLayout::CreateNew(
                resourceCreator,
                textString,
                textFormat,
                requestedWidth,
                requestedHeight);

            ThrowIfFailed(newTextLayout.CopyTo(textLayout));
        });
}


CanvasTextLayout::CanvasTextLayout(
    ICanvasDevice* device,
    DWriteTextLayoutType* layout)
    : ResourceWrapper(layout)
    , m_drawTextOptions(CanvasDrawTextOptions::Default)
    , m_device(device)
    , m_customFontManager(CustomFontManager::GetInstance())
    , m_lineSpacingMode(CanvasLineSpacingMode::Default)
{
    EnsureCustomTrimmingSignDevice(layout, device);
}

IFACEMETHODIMP CanvasTextLayout::GetFormatChangeIndices(
    uint32_t* positionCount,
    int32_t** positions)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(positionCount);
            CheckAndClearOutPointer(positions);

            std::vector<int32_t> positionVector;

            auto& resource = GetResource();

            uint32_t currentCharacterIndex = 0;
            BOOL unused;
            DWRITE_TEXT_RANGE formatRange;

            //
            // IDWriteTextLayout's getters will return discrete format
            // runs for any change in formatting, not just format 
            // changes related to that specific getter. GetUnderline 
            // is chosen here arbitrarily.
            //

            while (currentCharacterIndex < UINT_MAX)
            {
                ThrowIfFailed(resource->GetUnderline(currentCharacterIndex, &unused, &formatRange));

                assert(formatRange.startPosition == currentCharacterIndex);
                positionVector.push_back(formatRange.startPosition);

                currentCharacterIndex += formatRange.length;
            }

            ComArray<int32_t> array(positionVector.begin(), positionVector.end());
            array.Detach(positionCount, positions);
        });
}

using ABI::Microsoft::Graphics::Canvas::Text::ThrowIfInvalid;

template<>
void ThrowIfInvalid(float)
{
}

#define SIMPLE_GLOBAL_GETTER(name, valueType, dwriteMethod, conversionFunc) \
IFACEMETHODIMP CanvasTextLayout::##name(valueType* value)           \
{                                                                   \
    return ExceptionBoundary(                                       \
        [&]                                                         \
        {                                                           \
            CheckInPointer(value);                                  \
                                                                    \
            auto& resource = GetResource();                         \
                                                                    \
            auto nativeValue =                                      \
                conversionFunc(static_cast<IDWriteTextFormat*>(resource.Get())->dwriteMethod()); \
            *value = nativeValue;                                   \
        });                                                         \
}

#define SIMPLE_GLOBAL_SETTER(name, valueType, dwriteMethod, conversionFunc) \
IFACEMETHODIMP CanvasTextLayout::##name(valueType value)            \
{                                                                   \
    return ExceptionBoundary(                                       \
        [&]                                                         \
        {                                                           \
            auto& resource = GetResource();                         \
                                                                    \
            ThrowIfInvalid(value);                                  \
            resource->dwriteMethod(conversionFunc(value));          \
                                                                    \
            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get()); \
        });                                                         \
}

#define SIMPLE_STRING_GETTER(name, stringGetFn)                         \
IFACEMETHODIMP CanvasTextLayout::##name(HSTRING* value)             \
{                                                                   \
    return ExceptionBoundary(                                       \
        [&]                                                         \
        {                                                           \
            CheckInPointer(value);                                  \
            auto& resource = GetResource();                         \
            WinString winStr = stringGetFn(resource.Get());         \
            winStr.CopyTo(value);                                   \
        });                                                         \
}

SIMPLE_STRING_GETTER(get_DefaultFontFamily, GetFontFamilyName);

SIMPLE_GLOBAL_GETTER(get_DefaultFontSize, float, GetFontSize, );
SIMPLE_GLOBAL_GETTER(get_DefaultFontStretch, ABI::Windows::UI::Text::FontStretch, GetFontStretch, ToWindowsFontStretch);
SIMPLE_GLOBAL_GETTER(get_DefaultFontStyle, ABI::Windows::UI::Text::FontStyle, GetFontStyle, ToWindowsFontStyle);
SIMPLE_GLOBAL_GETTER(get_DefaultFontWeight, ABI::Windows::UI::Text::FontWeight, GetFontWeight, ToWindowsFontWeight);

SIMPLE_GLOBAL_GETTER(get_IncrementalTabStop, float, GetIncrementalTabStop, );
SIMPLE_GLOBAL_SETTER(put_IncrementalTabStop, float, SetIncrementalTabStop, );

SIMPLE_STRING_GETTER(get_DefaultLocaleName, ABI::Microsoft::Graphics::Canvas::GetLocaleName);

SIMPLE_GLOBAL_GETTER(get_VerticalAlignment, CanvasVerticalAlignment, GetParagraphAlignment, ToCanvasVerticalAlignment);
SIMPLE_GLOBAL_SETTER(put_VerticalAlignment, CanvasVerticalAlignment, SetParagraphAlignment, ToParagraphAlignment);

SIMPLE_GLOBAL_GETTER(get_HorizontalAlignment, CanvasHorizontalAlignment, GetTextAlignment, ToCanvasHorizontalAlignment);
SIMPLE_GLOBAL_SETTER(put_HorizontalAlignment, CanvasHorizontalAlignment, SetTextAlignment, ToTextAlignment);

SIMPLE_GLOBAL_GETTER(get_WordWrapping, CanvasWordWrapping, GetWordWrapping, ToCanvasWordWrapping);
SIMPLE_GLOBAL_SETTER(put_WordWrapping, CanvasWordWrapping, SetWordWrapping, ToWordWrapping);


IFACEMETHODIMP CanvasTextLayout::get_Direction(CanvasTextDirection* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource();

            *value = DWriteToCanvasTextDirection::Lookup(resource->GetReadingDirection(), resource->GetFlowDirection())->TextDirection;
        });
}


IFACEMETHODIMP CanvasTextLayout::put_Direction(CanvasTextDirection value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();
            auto entry = DWriteToCanvasTextDirection::Lookup(value);
            ThrowIfFailed(resource->SetReadingDirection(entry->ReadingDirection));
            ThrowIfFailed(resource->SetFlowDirection(entry->FlowDirection));

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}


IFACEMETHODIMP CanvasTextLayout::GetMinimumLineLength(
    float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource();

            ThrowIfFailed(resource->DetermineMinWidth(value));
        });
}

IFACEMETHODIMP CanvasTextLayout::get_LineSpacing(
    float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource(); 

            DWriteLineSpacing spacing(resource.Get());
            *value = spacing.GetAdjustedSpacing();
        });
}

IFACEMETHODIMP CanvasTextLayout::put_LineSpacing(
    float value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource(); 

            DWriteLineSpacing originalSpacing(resource.Get());

            CanvasLineSpacingMode lineSpacingMode{};
#if WINVER > _WIN32_WINNT_WINBLUE
            lineSpacingMode = ToCanvasLineSpacingMode(originalSpacing.Method);
#endif

            DWriteLineSpacing::Set(
                resource.Get(),
                lineSpacingMode,
                value, 
                originalSpacing.Baseline);

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

IFACEMETHODIMP CanvasTextLayout::get_LineSpacingBaseline(
    float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource();

            //
            // The Win10 IDWriteTextLayout3 interface definition omits a 'using' while
            // defining method overloads, requiring us to explicitly do a cast 
            // if we want IDWriteTextLayout2 overloads.
            //
            IDWriteTextLayout2* resource2 = static_cast<IDWriteTextLayout2*>(resource.Get());

            DWRITE_LINE_SPACING_METHOD unusedMethod;
            float unusedSpacing;
            ThrowIfFailed(resource2->GetLineSpacing(&unusedMethod, &unusedSpacing, value));
        });
}

IFACEMETHODIMP CanvasTextLayout::put_LineSpacingBaseline(
    float value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            //
            // The Win10 IDWriteTextLayout3 interface definition omits a 'using' while
            // defining method overloads, requiring us to explicitly do a cast 
            // if we want IDWriteTextLayout2 overloads.
            //
            IDWriteTextLayout2* resource2 = static_cast<IDWriteTextLayout2*>(resource.Get());

            DWRITE_LINE_SPACING_METHOD method;
            float spacing;
            float unusedBaseline;
            ThrowIfFailed(resource2->GetLineSpacing(&method, &spacing, &unusedBaseline));

            ThrowIfFailed(resource2->SetLineSpacing(method, spacing, value));

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

#if WINVER > _WIN32_WINNT_WINBLUE

IFACEMETHODIMP CanvasTextLayout::get_LineSpacingMode(
    CanvasLineSpacingMode* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource();

            DWriteLineSpacing spacing(resource.Get());
            bool allowUniform = m_lineSpacingMode == CanvasLineSpacingMode::Uniform;
            *value = spacing.GetAdjustedLineSpacingMode(allowUniform);
        });
}

IFACEMETHODIMP CanvasTextLayout::put_LineSpacingMode(
    CanvasLineSpacingMode value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            DWriteLineSpacing originalSpacing(resource.Get());

            DWriteLineSpacing::Set(
                resource.Get(),
                value,
                originalSpacing.GetAdjustedSpacing(),
                originalSpacing.Baseline);

            m_lineSpacingMode = value;

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

#endif

IFACEMETHODIMP CanvasTextLayout::get_TrimmingGranularity(
    CanvasTextTrimmingGranularity* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource(); 

            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> inlineObject;
            ThrowIfFailed(resource->GetTrimming(&trimming, &inlineObject));

            *value = ToCanvasTextTrimmingGranularity(trimming.granularity);
        });
}

IFACEMETHODIMP CanvasTextLayout::put_TrimmingGranularity(
    CanvasTextTrimmingGranularity value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource(); 

            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> inlineObject;
            ThrowIfFailed(resource->GetTrimming(&trimming, &inlineObject));

            ThrowIfInvalid<CanvasTextTrimmingGranularity>(value);
            trimming.granularity = static_cast<DWRITE_TRIMMING_GRANULARITY>(value);

            ThrowIfFailed(resource->SetTrimming(&trimming, inlineObject.Get()));

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

IFACEMETHODIMP CanvasTextLayout::get_TrimmingDelimiter(
    HSTRING* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource(); 

            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> inlineObject;
            ThrowIfFailed(resource->GetTrimming(&trimming, &inlineObject));

            WinString winStr = ConvertCharacterCodepointToString(trimming.delimiter);
            winStr.CopyTo(value);
        });
}

IFACEMETHODIMP CanvasTextLayout::put_TrimmingDelimiter(
    HSTRING value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource(); 

            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> inlineObject;
            ThrowIfFailed(resource->GetTrimming(&trimming, &inlineObject));

            ThrowIfInvalidTrimmingDelimiter(value);
            trimming.delimiter = ToTrimmingDelimiter(WinString(value));

            ThrowIfFailed(resource->SetTrimming(&trimming, inlineObject.Get()));

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

IFACEMETHODIMP CanvasTextLayout::get_TrimmingDelimiterCount(
    int32_t* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource(); 

            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> inlineObject;
            ThrowIfFailed(resource->GetTrimming(&trimming, &inlineObject));

            assert(trimming.delimiterCount <= INT_MAX);
            *value = static_cast<int32_t>(trimming.delimiterCount);
        });
}

IFACEMETHODIMP CanvasTextLayout::put_TrimmingDelimiterCount(
    int32_t value)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfNegative(value);
            auto& resource = GetResource();

            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> inlineObject;
            ThrowIfFailed(resource->GetTrimming(&trimming, &inlineObject));

            trimming.delimiterCount = static_cast<uint32_t>(value);

            ThrowIfFailed(resource->SetTrimming(&trimming, inlineObject.Get()));

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

IFACEMETHODIMP CanvasTextLayout::get_Options(
    CanvasDrawTextOptions* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            GetResource(); 

            *value = m_drawTextOptions;
        });
}

IFACEMETHODIMP CanvasTextLayout::put_Options(
    CanvasDrawTextOptions value)
{
    return ExceptionBoundary(
        [&]
        {
            GetResource(); 

            m_drawTextOptions = value;
        });
}

IFACEMETHODIMP CanvasTextLayout::get_RequestedSize(
    ABI::Windows::Foundation::Size* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource();

            value->Width = resource->GetMaxWidth();
            value->Height = resource->GetMaxHeight();
        });
}

IFACEMETHODIMP CanvasTextLayout::put_RequestedSize(
    ABI::Windows::Foundation::Size value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetMaxWidth(value.Width));
            ThrowIfFailed(resource->SetMaxHeight(value.Height));
        });
}

IFACEMETHODIMP CanvasTextLayout::GetBrush(
    int32_t characterIndex,
    ICanvasBrush** brush)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(brush);

            ComPtr<ICanvasBrush> wrappedBrush;

            auto brushInspectable = GetCustomBrushInternal(characterIndex);
            if (brushInspectable)
                wrappedBrush = As<ICanvasBrush>(brushInspectable.Get());

            ThrowIfFailed(wrappedBrush.CopyTo(brush));
    });
}

IFACEMETHODIMP CanvasTextLayout::GetFontFamily(
    int32_t characterIndex,
    HSTRING* fontFamily)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(fontFamily);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            auto fontFamilyName = GetTextFromLayoutResource(
                &IDWriteTextLayout::GetFontFamilyNameLength,
                &IDWriteTextLayout::GetFontFamilyName,
                characterIndex,
                resource.Get());

            fontFamilyName.CopyTo(fontFamily);
        });
}

IFACEMETHODIMP CanvasTextLayout::GetFontSize(
    int32_t characterIndex,
    float* fontSize)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(fontSize);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            ThrowIfFailed(resource->GetFontSize(characterIndex, fontSize));
        });
}

IFACEMETHODIMP CanvasTextLayout::GetFontStretch(
    int32_t characterIndex,
    ABI::Windows::UI::Text::FontStretch* fontStretch)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(fontStretch);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            DWRITE_FONT_STRETCH dwriteFontStretch;
            ThrowIfFailed(resource->GetFontStretch(characterIndex, &dwriteFontStretch));

            *fontStretch = ToWindowsFontStretch(dwriteFontStretch);
        });
}

IFACEMETHODIMP CanvasTextLayout::GetFontStyle(
    int32_t characterIndex,
    ABI::Windows::UI::Text::FontStyle* fontStyle)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(fontStyle);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            DWRITE_FONT_STYLE dwriteFontStyle;
            ThrowIfFailed(resource->GetFontStyle(characterIndex, &dwriteFontStyle));

            *fontStyle = ToWindowsFontStyle(dwriteFontStyle);
        });
}

IFACEMETHODIMP CanvasTextLayout::GetFontWeight(
    int32_t characterIndex,
    ABI::Windows::UI::Text::FontWeight* fontWeight)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(fontWeight);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            DWRITE_FONT_WEIGHT dwriteFontWeight;
            ThrowIfFailed(resource->GetFontWeight(characterIndex, &dwriteFontWeight));

            *fontWeight = ToWindowsFontWeight(dwriteFontWeight);
        });
}

IFACEMETHODIMP CanvasTextLayout::GetLocaleName(
    int32_t characterIndex,
    HSTRING* localeName)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(localeName);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            auto name = GetTextFromLayoutResource(
                &IDWriteTextLayout::GetLocaleNameLength,
                &IDWriteTextLayout::GetLocaleName,
                characterIndex,
                resource.Get());

            name.CopyTo(localeName);
        });
}

IFACEMETHODIMP CanvasTextLayout::GetStrikethrough(
    int32_t characterIndex,
    boolean* hasStrikethrough)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(hasStrikethrough);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            BOOL dwriteHasStrikethrough;
            ThrowIfFailed(resource->GetStrikethrough(characterIndex, &dwriteHasStrikethrough));

            *hasStrikethrough = !!dwriteHasStrikethrough;
        });
}

IFACEMETHODIMP CanvasTextLayout::GetUnderline(
    int32_t characterIndex,
    boolean* hasUnderline)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(hasUnderline);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            BOOL dwriteHasUnderline;
            ThrowIfFailed(resource->GetUnderline(characterIndex, &dwriteHasUnderline));

            *hasUnderline = !!dwriteHasUnderline;
        });
}

IFACEMETHODIMP CanvasTextLayout::SetColor(
    int32_t characterIndex,
    int32_t characterCount,
    Color color)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            auto textRange = ToDWriteTextRange(characterIndex, characterCount);

            auto& device = m_device.EnsureNotClosed();

            auto deviceInternal = As<ICanvasDeviceInternal>(device);

            auto d2dBrush = deviceInternal->CreateSolidColorBrush(ToD2DColor(color));

            ThrowIfFailed(resource->SetDrawingEffect(d2dBrush.Get(), textRange));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetBrush(
    int32_t characterIndex,
    int32_t characterCount,
    ICanvasBrush* brush)
{
    return ExceptionBoundary(
        [&]
        {
            SetCustomBrushInternal(characterIndex, characterCount, brush);
        });
}

IFACEMETHODIMP CanvasTextLayout::SetFontFamily(
    int32_t characterIndex,
    int32_t characterCount,
    HSTRING fontFamilyName)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            auto uriAndFontFamily = GetUriAndFontFamily(WinString(fontFamilyName));
            auto const& uri = uriAndFontFamily.first;
            auto const& fontFamily = uriAndFontFamily.second;

            ComPtr<IDWriteFontCollection> fontCollection = m_customFontManager->GetFontCollectionFromUri(uri);

            auto textRange = ToDWriteTextRange(characterIndex, characterCount);

            ThrowIfFailed(resource->SetFontCollection(fontCollection.Get(), textRange));
            ThrowIfFailed(resource->SetFontFamilyName(static_cast<const wchar_t*>(fontFamily), textRange));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetFontSize(
    int32_t characterIndex,
    int32_t characterCount,
    float fontSize)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetFontSize(fontSize, ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetFontStretch(
    int32_t characterIndex,
    int32_t characterCount,
    ABI::Windows::UI::Text::FontStretch fontStretch)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetFontStretch(ToFontStretch(fontStretch), ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetFontStyle(
    int32_t characterIndex,
    int32_t characterCount,
    ABI::Windows::UI::Text::FontStyle fontStyle)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetFontStyle(ToFontStyle(fontStyle), ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetFontWeight(
    int32_t characterIndex,
    int32_t characterCount,
    ABI::Windows::UI::Text::FontWeight fontWeight)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetFontWeight(ToFontWeight(fontWeight), ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetLocaleName(
    int32_t characterIndex,
    int32_t characterCount,
    HSTRING name)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            const wchar_t* localeNameBuffer = WindowsGetStringRawBuffer(name, nullptr);

            ThrowIfFailed(resource->SetLocaleName(localeNameBuffer, ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetStrikethrough(
    int32_t characterIndex,
    int32_t characterCount,
    boolean hasStrikethrough)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetStrikethrough(hasStrikethrough, ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetUnderline(
    int32_t characterIndex,
    int32_t characterCount,
    boolean hasUnderline)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetUnderline(hasUnderline, ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::GetPairKerning(
    int32_t characterIndex,
    boolean* hasPairKerning)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(hasPairKerning);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            BOOL dwriteHasPairKerning;
            ThrowIfFailed(resource->GetPairKerning(characterIndex, &dwriteHasPairKerning));

            *hasPairKerning = !!dwriteHasPairKerning;
        });
}

IFACEMETHODIMP CanvasTextLayout::SetPairKerning(
    int32_t characterIndex,
    int32_t characterCount,
    boolean hasPairKerning)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetPairKerning(hasPairKerning, ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::GetLeadingCharacterSpacing(
    int32_t characterIndex,
    float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvance;
            ThrowIfFailed(resource->GetCharacterSpacing(characterIndex, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvance));

            *value = leadingSpacing;
        });
}

IFACEMETHODIMP CanvasTextLayout::GetTrailingCharacterSpacing(
    int32_t characterIndex,
    float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvance;
            ThrowIfFailed(resource->GetCharacterSpacing(characterIndex, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvance));

            *value = trailingSpacing;
        });
}

IFACEMETHODIMP CanvasTextLayout::GetMinimumCharacterAdvance(
    int32_t characterIndex,
    float* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvance;
            ThrowIfFailed(resource->GetCharacterSpacing(characterIndex, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvance));

            *value = minimumCharacterAdvance;
        });
}

IFACEMETHODIMP CanvasTextLayout::SetCharacterSpacing(
    int32_t characterIndex,
    int32_t characterCount,
    float leadingSpacing,
    float trailingSpacing,
    float minimumAdvance)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetCharacterSpacing(
                leadingSpacing, 
                trailingSpacing, 
                minimumAdvance,
                ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::get_VerticalGlyphOrientation(
    CanvasVerticalGlyphOrientation* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource();

            *value = ToCanvasVerticalGlyphOrientation(resource->GetVerticalGlyphOrientation());
        });
}

IFACEMETHODIMP CanvasTextLayout::put_VerticalGlyphOrientation(
    CanvasVerticalGlyphOrientation value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetVerticalGlyphOrientation(ToVerticalGlyphOrientation(value)));

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

IFACEMETHODIMP CanvasTextLayout::get_OpticalAlignment(
    CanvasOpticalAlignment* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource();

            *value = ToCanvasOpticalAlignment(resource->GetOpticalAlignment());
        });
}

IFACEMETHODIMP CanvasTextLayout::put_OpticalAlignment(
    CanvasOpticalAlignment value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetOpticalAlignment(ToOpticalAlignment(value)));

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

IFACEMETHODIMP CanvasTextLayout::get_LastLineWrapping(
    boolean* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            CheckInPointer(value);

            auto& resource = GetResource();

            *value = !!resource->GetLastLineWrapping();
        });
}

IFACEMETHODIMP CanvasTextLayout::put_LastLineWrapping(
    boolean value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            ThrowIfFailed(resource->SetLastLineWrapping(value));

            m_trimmingSignInformation.RecreateInternalTrimmingSignIfNeeded(resource.Get());
        });
}

IFACEMETHODIMP CanvasTextLayout::get_TrimmingSign(
    CanvasTrimmingSign* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);

            auto& resource = GetResource();

            *value = m_trimmingSignInformation.GetTrimmingSignFromResource(resource.Get());
        });
}

IFACEMETHODIMP CanvasTextLayout::put_TrimmingSign(
    CanvasTrimmingSign value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            m_trimmingSignInformation.SetTrimmingSignOnResource(value, resource.Get());
        });
}


IFACEMETHODIMP CanvasTextLayout::get_CustomTrimmingSign(
    ICanvasTextInlineObject** value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);
            auto& resource = GetResource();

            DWRITE_TRIMMING unused;
            ComPtr<IDWriteInlineObject> dwriteInlineObject;
            ThrowIfFailed(resource->GetTrimming(&unused, &dwriteInlineObject));

            auto canvasTrimmingSign = GetCanvasInlineObjectFromDWriteInlineObject(dwriteInlineObject);

            ThrowIfFailed(canvasTrimmingSign.CopyTo(value));
        });
}

IFACEMETHODIMP CanvasTextLayout::put_CustomTrimmingSign(
    ICanvasTextInlineObject* value)
{
    return ExceptionBoundary(
        [&]
        {
            auto& resource = GetResource();

            auto dwriteInlineObject = Make<InternalDWriteInlineObject>(value, m_device.EnsureNotClosed());
            CheckMakeResult(dwriteInlineObject);

            DWRITE_TRIMMING dwriteTrimming;
            ComPtr<IDWriteInlineObject> unused;
            ThrowIfFailed(resource->GetTrimming(&dwriteTrimming, &unused));

            ThrowIfFailed(resource->SetTrimming(&dwriteTrimming, dwriteInlineObject.Get()));
        });
}

IFACEMETHODIMP CanvasTextLayout::get_LayoutBounds(Rect* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource();

            DWRITE_TEXT_METRICS1 dwriteMetrics;
            ThrowIfFailed(resource->GetMetrics(&dwriteMetrics));

            Rect rect{ dwriteMetrics.left, dwriteMetrics.top, dwriteMetrics.width, dwriteMetrics.height };

            //
            // There's an adjustment to do here because 'left' and 'top' fields of this 
            // struct are not always computed in the same way.
            //
            // On left-to-right reading direction, for example, the 'left' field is the left layout bound
            // and everything is straightforward. As for right-to-left reading direction, however, 'left'
            // is equal to the {right layout bound- not reported directly} - {widthIncludingTrailingWhitespace}. 
            // Since the width of the rect we return doesn't include whitespace, its left bound needs to be
            // adjusted accordingly. Likewise goes for bottom-to-top reading direction.
            //
            // Note that on horizontal reading directions, heightIncludingTrailingWhitespace == height.
            // And on vertical ones, widthIncludingTrailingWhitespace == width.
            //

            auto readingDirection = resource->GetReadingDirection();

            if (readingDirection == DWRITE_READING_DIRECTION_RIGHT_TO_LEFT)
            {
                const float whitespace = dwriteMetrics.widthIncludingTrailingWhitespace - dwriteMetrics.width;
                rect.X += whitespace;
            }
            else if (readingDirection == DWRITE_READING_DIRECTION_BOTTOM_TO_TOP)
            {
                const float whitespace = dwriteMetrics.heightIncludingTrailingWhitespace - dwriteMetrics.height;
                rect.Y += whitespace;
            }

            *value = rect;
        });
}

IFACEMETHODIMP CanvasTextLayout::get_LayoutBoundsIncludingTrailingWhitespace(Rect* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource();

            DWRITE_TEXT_METRICS1 dwriteMetrics;
            ThrowIfFailed(resource->GetMetrics(&dwriteMetrics));

            Rect rect{ dwriteMetrics.left, dwriteMetrics.top, dwriteMetrics.widthIncludingTrailingWhitespace, dwriteMetrics.heightIncludingTrailingWhitespace };

            *value = rect;
        });
}

IFACEMETHODIMP CanvasTextLayout::get_LineCount(int32_t* lineCount)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(lineCount);
            auto& resource = GetResource();

            DWRITE_TEXT_METRICS1 dwriteMetrics;
            ThrowIfFailed(resource->GetMetrics(&dwriteMetrics));

            *lineCount = dwriteMetrics.lineCount;
        });
}

IFACEMETHODIMP CanvasTextLayout::get_DrawBounds(Rect* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource();

            DWRITE_OVERHANG_METRICS dwriteOverhangMetrics;
            ThrowIfFailed(resource->GetOverhangMetrics(&dwriteOverhangMetrics));      

            const float left = -dwriteOverhangMetrics.left;
            const float right = dwriteOverhangMetrics.right + resource->GetMaxWidth();
            const float width = right - left;

            const float top = -dwriteOverhangMetrics.top;
            const float bottom = dwriteOverhangMetrics.bottom + resource->GetMaxHeight();
            const float height = bottom - top;

            Rect rect{ left, top, width, height };

            *value = rect;
        });
}

IFACEMETHODIMP CanvasTextLayout::get_MaximumBidiReorderingDepth(int32_t* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            auto& resource = GetResource();

            DWRITE_TEXT_METRICS1 dwriteMetrics;
            ThrowIfFailed(resource->GetMetrics(&dwriteMetrics));

            assert(dwriteMetrics.maxBidiReorderingDepth >= 0);

            *value = static_cast<int32_t>(dwriteMetrics.maxBidiReorderingDepth);
        });
}

IFACEMETHODIMP CanvasTextLayout::HitTest(
    Vector2 point,
    boolean* isHit)
{
    return HitTestWithDescriptionAndCoordsAndTrailingSide(point.X, point.Y, nullptr, nullptr, isHit);
}

IFACEMETHODIMP CanvasTextLayout::HitTestWithCoords(
    float x,
    float y,
    boolean* isHit)
{
    return HitTestWithDescriptionAndCoordsAndTrailingSide(x, y, nullptr, nullptr, isHit);
}

IFACEMETHODIMP CanvasTextLayout::HitTestWithDescription(
    Vector2 point,
    CanvasTextLayoutRegion* description,
    boolean* isHit)
{
    return HitTestWithDescriptionAndCoordsAndTrailingSide(point.X, point.Y, description, nullptr, isHit);
}

IFACEMETHODIMP CanvasTextLayout::HitTestWithDescriptionAndTrailingSide(
    Vector2 point,
    CanvasTextLayoutRegion* description,
    boolean* isTrailingSide,
    boolean* isHit)
{
    return HitTestWithDescriptionAndCoordsAndTrailingSide(point.X, point.Y, description, isTrailingSide, isHit);
}

IFACEMETHODIMP CanvasTextLayout::HitTestWithDescriptionAndCoords(
    float x,
    float y,
    CanvasTextLayoutRegion* description,
    boolean* isHit)
{
    return HitTestWithDescriptionAndCoordsAndTrailingSide(
        x,
        y,
        description,
        nullptr,
        isHit);
}

IFACEMETHODIMP CanvasTextLayout::HitTestWithDescriptionAndCoordsAndTrailingSide(
    float x,
    float y,
    CanvasTextLayoutRegion* description,
    boolean* isTrailingSide,
    boolean* isHit)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(isHit);
            auto& resource = GetResource();

            BOOL isTrailingHit;
            BOOL isInside;
            DWRITE_HIT_TEST_METRICS hitTestMetrics;
            ThrowIfFailed(resource->HitTestPoint(x, y, &isTrailingHit, &isInside, &hitTestMetrics));

            *isHit = !!isInside;

            if (description)
            {
                *description = ToHitTestDescription(hitTestMetrics);
            }
            if (isTrailingSide)
            {
                *isTrailingSide = !!isTrailingHit;
            }
        });
}

IFACEMETHODIMP CanvasTextLayout::GetCaretPosition(
    int32_t characterIndex,
    boolean trailingSideOfCharacter,
    Vector2* location)
{
    return GetCaretPositionWithDescription(characterIndex, trailingSideOfCharacter, nullptr, location);
}

IFACEMETHODIMP CanvasTextLayout::GetCaretPositionWithDescription(
    int32_t characterIndex,
    boolean trailingSideOfCharacter,
    CanvasTextLayoutRegion* description,
    Vector2* location)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(location);
            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            DWRITE_HIT_TEST_METRICS hitTestMetrics;
            ThrowIfFailed(resource->HitTestTextPosition(
                characterIndex, 
                trailingSideOfCharacter, 
                &location->X, 
                &location->Y, 
                &hitTestMetrics));

            if (description)
            {
                *description = ToHitTestDescription(hitTestMetrics);
            }
        });
}

IFACEMETHODIMP CanvasTextLayout::GetCharacterRegions(
    int32_t characterIndex,
    int32_t characterCount,
    uint32_t* descriptionCount,
    CanvasTextLayoutRegion** descriptions)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(descriptionCount);
            CheckAndClearOutPointer(descriptions);
            ThrowIfNegative(characterIndex);
            ThrowIfNegative(characterCount);

            auto& resource = GetResource();

            uint32_t hitTestMetricsCount;
            HRESULT hitTestHr = resource->HitTestTextRange(characterIndex, characterCount, 0, 0, nullptr, 0, &hitTestMetricsCount);
            if (hitTestHr != E_NOT_SUFFICIENT_BUFFER)
            {
                assert(hitTestHr != S_OK);
                ThrowHR(hitTestHr);
            }

            std::vector<DWRITE_HIT_TEST_METRICS> dwriteHitTestMetrics;
            dwriteHitTestMetrics.resize(hitTestMetricsCount);

            ThrowIfFailed(resource->HitTestTextRange(
                characterIndex, 
                characterCount, 
                0, 
                0, 
                &dwriteHitTestMetrics[0], 
                hitTestMetricsCount, 
                &hitTestMetricsCount));
            assert(dwriteHitTestMetrics.size() == hitTestMetricsCount);

            std::vector<CanvasTextLayoutRegion> hitDescriptions;
            hitDescriptions.resize(hitTestMetricsCount);

            for (uint32_t i = 0; i < hitTestMetricsCount; ++i)
            {
                hitDescriptions[i] = ToHitTestDescription(dwriteHitTestMetrics[i]);
            }

            ComArray<CanvasTextLayoutRegion> array(hitDescriptions.begin(), hitDescriptions.end());
            array.Detach(descriptionCount, descriptions);

        });
}


IFACEMETHODIMP CanvasTextLayout::DrawToTextRenderer(
    ICanvasTextRenderer* textRenderer,
    Vector2 position)
{
    return DrawToTextRendererWithCoords(textRenderer, position.X, position.Y);
}

IFACEMETHODIMP CanvasTextLayout::DrawToTextRendererWithCoords(
    ICanvasTextRenderer* textRenderer,
    float x,
    float y)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(textRenderer);

            auto& resource = GetResource();

            auto& device = m_device.EnsureNotClosed();

            auto dwriteTextRenderer = Make<InternalDWriteTextRenderer>(device, textRenderer);
            CheckMakeResult(dwriteTextRenderer);

            ThrowIfFailed(resource->Draw(
                nullptr,
                dwriteTextRenderer.Get(),
                x,
                y));
        });
}



IFACEMETHODIMP CanvasTextLayout::SetInlineObject(
    int32_t characterIndex,
    int32_t characterCount,
    ICanvasTextInlineObject* inlineObject)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfNegative(characterIndex);
            ThrowIfNegative(characterCount);

            auto& resource = GetResource();

            ComPtr<IDWriteInlineObject> dwriteInlineObject;
            if (inlineObject)
            {
                dwriteInlineObject = Make<InternalDWriteInlineObject>(inlineObject, m_device.EnsureNotClosed());
                CheckMakeResult(dwriteInlineObject);
            }

            ThrowIfFailed(resource->SetInlineObject(dwriteInlineObject.Get(), ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::GetInlineObject(
    int32_t characterIndex,
    ICanvasTextInlineObject** inlineObject)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(inlineObject);
            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            ComPtr<IDWriteInlineObject> dwriteInlineObject;
            ThrowIfFailed(resource->GetInlineObject(characterIndex, &dwriteInlineObject, nullptr));

            auto canvasInlineObject = GetCanvasInlineObjectFromDWriteInlineObject(dwriteInlineObject);

            ThrowIfFailed(canvasInlineObject.CopyTo(inlineObject));
        });
}


IFACEMETHODIMP CanvasTextLayout::get_LineMetrics(
    uint32_t* valueCount,
    CanvasLineMetrics** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            auto& resource = GetResource();

            uint32_t lineCount;
            HRESULT hr = resource->GetLineMetrics(nullptr, 0, &lineCount);

            assert(hr == E_NOT_SUFFICIENT_BUFFER);
            if (hr != E_NOT_SUFFICIENT_BUFFER)
                ThrowHR(E_UNEXPECTED);

            std::vector<DWriteMetricsType> dwriteMetrics(lineCount);
            ThrowIfFailed(resource->GetLineMetrics(dwriteMetrics.data(), lineCount, &lineCount));

            auto returnedMetrics = TransformToComArray<CanvasLineMetrics>(
                dwriteMetrics.begin(), 
                dwriteMetrics.end(),
                [](DWriteMetricsType const& dwriteMetrics)
                {                
                    CanvasLineMetrics metrics{};
                    metrics.CharacterCount = dwriteMetrics.length;
                    metrics.TrailingWhitespaceCount = dwriteMetrics.trailingWhitespaceLength;
                    metrics.TerminalNewlineCount = dwriteMetrics.newlineLength;
                    metrics.Height = dwriteMetrics.height;
                    metrics.Baseline = dwriteMetrics.baseline;
                    metrics.IsTrimmed = !!dwriteMetrics.isTrimmed;
#if WINVER > _WIN32_WINNT_WINBLUE
                    metrics.LeadingWhitespaceBefore = dwriteMetrics.leadingBefore;
                    metrics.LeadingWhitespaceAfter = dwriteMetrics.leadingAfter;
#endif
                    return metrics;
                });

            returnedMetrics.Detach(valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasTextLayout::get_ClusterMetrics(
    uint32_t* valueCount,
    CanvasClusterMetrics** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);

            auto& resource = GetResource();

            uint32_t clusterCount;
            HRESULT hr = resource->GetClusterMetrics(nullptr, 0, &clusterCount);

            //
            // GetClusterMetrics can return S_OK here, since it's valid for a 
            // text layout to contain no clusters.
            //

            if (FAILED(hr) && hr != E_NOT_SUFFICIENT_BUFFER)
                ThrowHR(E_UNEXPECTED);

            std::vector<DWRITE_CLUSTER_METRICS> dwriteMetrics(clusterCount);

            if (clusterCount > 0)
                ThrowIfFailed(resource->GetClusterMetrics(dwriteMetrics.data(), clusterCount, &clusterCount));

            auto returnedMetrics = TransformToComArray<CanvasClusterMetrics>(
                dwriteMetrics.begin(), 
                dwriteMetrics.end(),
                [](DWRITE_CLUSTER_METRICS const& dwriteMetrics)
                {                
                    CanvasClusterMetrics metrics{};
                    metrics.CharacterCount = dwriteMetrics.length;
                    metrics.Width = dwriteMetrics.width;

                    if(dwriteMetrics.canWrapLineAfter)
                        metrics.Properties |= CanvasClusterProperties::CanWrapLineAfter;

                    if (dwriteMetrics.isWhitespace)
                        metrics.Properties |= CanvasClusterProperties::Whitespace;

                    if(dwriteMetrics.isNewline) 
                        metrics.Properties |= CanvasClusterProperties::Newline;

                    if (dwriteMetrics.isSoftHyphen)
                        metrics.Properties |= CanvasClusterProperties::SoftHyphen;

                    if (dwriteMetrics.isRightToLeft)
                        metrics.Properties |= CanvasClusterProperties::RightToLeft;

                    return metrics;
                });

            returnedMetrics.Detach(valueCount, valueElements);
        });
}

IFACEMETHODIMP CanvasTextLayout::GetCustomBrush(
    int32_t characterIndex,
    IInspectable** brush)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(brush);

            auto inspectable = GetCustomBrushInternal(characterIndex);

            ThrowIfFailed(inspectable.CopyTo(brush));
        });
}

ComPtr<IInspectable> CanvasTextLayout::GetCustomBrushInternal(
    int32_t characterIndex)
{
    ThrowIfNegative(characterIndex);

    auto& resource = GetResource();
    auto& device = m_device.EnsureNotClosed();

    ComPtr<IUnknown> drawingEffect;
    ThrowIfFailed(resource->GetDrawingEffect(characterIndex, &drawingEffect, nullptr));

    ComPtr<IInspectable> inspectable;
    if (drawingEffect)
    {
        inspectable = GetCustomDrawingObjectInspectable(device.Get(), drawingEffect.Get());

        if (!inspectable)
            ThrowHR(E_NOINTERFACE);
    }

    return inspectable;
}

IFACEMETHODIMP CanvasTextLayout::SetCustomBrush(
    int32_t characterIndex,
    int32_t characterCount,
    IInspectable* brush)
{
    return ExceptionBoundary(
        [&]
        {
            SetCustomBrushInternal(characterIndex, characterCount, brush);
        });
}

void CanvasTextLayout::SetCustomBrushInternal(
    int32_t characterIndex,
    int32_t characterCount, 
    IInspectable* brush)
{
    auto& resource = GetResource();

    auto textRange = ToDWriteTextRange(characterIndex, characterCount);

    ComPtr<IUnknown> drawingEffect;

    if (brush)
    {
        //
        // This has the behavior of unwrapping brushes before setting them to
        // the text layout. All other types of objects are left unwrapped.
        //
        auto wrappedBrushInternal = MaybeAs<ICanvasBrushInternal>(brush);
        if (wrappedBrushInternal)
        {
            //
            // The device context passed to GetD2DBrush is *not* used to key off of DPI.
            // It is used to construct the appropriate DPI compensation effect, if necessary.
            //
            // The DPI compensation effect will be inserted into the effect graph and the resulting
            // d2dbrush will be committed to the text layout.
            //
            auto& device = m_device.EnsureNotClosed();
            auto deviceInternal = As<ICanvasDeviceInternal>(device);

            drawingEffect = wrappedBrushInternal->GetD2DBrush(deviceInternal->GetResourceCreationDeviceContext().Get(), GetBrushFlags::AlwaysInsertDpiCompensation);
        }
        else
        {
            drawingEffect = brush;
        }
    }

    ThrowIfFailed(resource->SetDrawingEffect(drawingEffect.Get(), textRange));
}

IFACEMETHODIMP CanvasTextLayout::GetTypography(
    int32_t characterIndex,
    ICanvasTypography** typography)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(typography);
            ThrowIfNegative(characterIndex);

            auto& resource = GetResource();

            ComPtr<IDWriteTypography> dwriteTypography;
            ThrowIfFailed(resource->GetTypography(characterIndex, &dwriteTypography, nullptr));

            ComPtr<ICanvasTypography> canvasTypography;

            if(dwriteTypography)
                canvasTypography = ResourceManager::GetOrCreate<ICanvasTypography>(dwriteTypography.Get());

            ThrowIfFailed(canvasTypography.CopyTo(typography));
        });
}

IFACEMETHODIMP CanvasTextLayout::SetTypography(
    int32_t characterIndex,
    int32_t characterCount,
    ICanvasTypography* typography)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfNegative(characterIndex);
            ThrowIfNegative(characterCount);

            auto& resource = GetResource();

            ComPtr<IDWriteTypography> dwriteTypography;

            if(typography)
                dwriteTypography = GetWrappedResource<IDWriteTypography>(typography);

            ThrowIfFailed(resource->SetTypography(dwriteTypography.Get(), ToDWriteTextRange(characterIndex, characterCount)));
        });
}

IFACEMETHODIMP CanvasTextLayout::Close()
{
    m_device.Close();

    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasTextLayout::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_device.EnsureNotClosed().CopyTo(device));
        });
}

void CanvasTextLayout::SetLineSpacingModeInternal(CanvasLineSpacingMode lineSpacingMode)
{
    m_lineSpacingMode = lineSpacingMode;
}

void CanvasTextLayout::SetTrimmingSignInternal(CanvasTrimmingSign trimmingSign)
{
    m_trimmingSignInformation.SetTrimmingSignOnResource(trimmingSign, GetResource().Get());
}


void CanvasTextLayout::EnsureCustomTrimmingSignDevice(IDWriteTextLayout2* layout, ICanvasDevice* device)
{
    DWRITE_TRIMMING trimming;
    ComPtr<IDWriteInlineObject> trimmingSign;
    ThrowIfFailed(layout->GetTrimming(&trimming, &trimmingSign));
    if (!trimmingSign) return;

    auto internalDWriteInlineObject = MaybeAs<IInternalDWriteInlineObject>(trimmingSign);
    if (!internalDWriteInlineObject) return;

    internalDWriteInlineObject->SetDevice(device);
}

ActivatableClassWithFactory(CanvasTextLayout, CanvasTextLayoutFactory);

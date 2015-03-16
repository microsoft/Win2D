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

#include "pch.h"
#include "CanvasTextLayout.h"
#include "TextUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
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

    //
    // CanvasTextLayoutManager implementation
    //

    CanvasTextLayoutManager::CanvasTextLayoutManager(std::shared_ptr<ICanvasTextFormatAdapter> adapter)
        : CustomFontManager(adapter)
    {
    }

    ComPtr<CanvasTextLayout> CanvasTextLayoutManager::CreateNew(
        HSTRING text,
        ICanvasTextFormat* textFormat,
        float maximumLayoutWidth,
        float maximumLayoutHeight)
    {
        auto dwriteFactory = GetSharedFactory();

        uint32_t textLength;
        auto textBuffer = WindowsGetStringRawBuffer(text, &textLength);
        ThrowIfNullPointer(textBuffer, E_INVALIDARG);

        ComPtr<IDWriteTextLayout> dwriteTextLayout;
        ThrowIfFailed(dwriteFactory->CreateTextLayout(
            textBuffer,
            textLength,
            GetWrappedResource<IDWriteTextFormat>(textFormat).Get(),
            maximumLayoutWidth,
            maximumLayoutHeight,
            &dwriteTextLayout));

        auto textLayout = Make<CanvasTextLayout>(
            shared_from_this(),
            As<IDWriteTextLayout2>(dwriteTextLayout).Get());
        CheckMakeResult(textLayout);

        return textLayout;
    }

    ComPtr<CanvasTextLayout> CanvasTextLayoutManager::CreateWrapper(
        IDWriteTextLayout2* resource)
    {
        auto canvasTextLayout = Make<CanvasTextLayout>(
            shared_from_this(),
            resource);
        CheckMakeResult(canvasTextLayout);

        return canvasTextLayout;
    }


    //
    // CanvasTextLayoutFactory implementation
    //


    CanvasTextLayoutFactory::CanvasTextLayoutFactory()
    {
    }

    std::shared_ptr<CanvasTextLayoutManager> CanvasTextLayoutFactory::CreateManager()
    {
        return std::make_shared<CanvasTextLayoutManager>(std::make_shared<CanvasTextFormatAdapter>());
    }

    IFACEMETHODIMP CanvasTextLayoutFactory::Create(
        HSTRING textString,
        ICanvasTextFormat* textFormat,
        float maximumLayoutWidth,
        float maximumLayoutHeight,
        ICanvasTextLayout** textLayout)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(textFormat);
                CheckAndClearOutPointer(textLayout);

                auto newTextLayout = GetManager()->Create(
                    textString,
                    textFormat,
                    maximumLayoutWidth,
                    maximumLayoutHeight);

                ThrowIfFailed(newTextLayout.CopyTo(textLayout));
            });
    }

    IFACEMETHODIMP CanvasTextLayoutFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                auto layout = GetManager()->GetOrCreate(As<IDWriteTextLayout2>(resource).Get());
                ThrowIfFailed(layout.CopyTo(wrapper));
            });
    }

    CanvasTextLayout::CanvasTextLayout(
        std::shared_ptr<CanvasTextLayoutManager> manager, 
        IDWriteTextLayout2* layout)
        : ResourceWrapper(manager, layout)
        , m_drawTextOptions(CanvasDrawTextOptions::Default)
    {
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

    template<>
    void ThrowIfInvalid(float)
    {
    }

#define SIMPLE_GLOBAL_GETTER(name, valueType, dwriteMethod, conversionFunc)                             \
    IFACEMETHODIMP CanvasTextLayout::##name(                                                            \
        valueType* value)                                                                               \
        {                                                                                               \
            return ExceptionBoundary(                                                                   \
            [&]                                                                                         \
            {                                                                                           \
                CheckInPointer(value);                                                                  \
                                                                                                        \
                auto& resource = GetResource();                                                         \
                                                                                                        \
                auto nativeValue =                                                                      \
                    conversionFunc(static_cast<IDWriteTextFormat*>(resource.Get())->dwriteMethod());    \
                *value = nativeValue;                                                                   \
            });                                                                                         \
        }

#define SIMPLE_GLOBAL_SETTER(name, valueType, dwriteMethod, conversionFunc)     \
    IFACEMETHODIMP CanvasTextLayout::##name(                                    \
        valueType value)                                                        \
        {                                                                       \
        return ExceptionBoundary(                                               \
        [&]                                                                     \
            {                                                                   \
                auto& resource = GetResource();                                 \
                                                                                \
                ThrowIfInvalid(value);                                          \
                resource->dwriteMethod(conversionFunc(value));                  \
        });                                                                     \
    }

#define SIMPLE_STRING_GETTER(name, stringGetFn)                                 \
    IFACEMETHODIMP CanvasTextLayout::##name(                                    \
        HSTRING* value)                                                         \
        {                                                                       \
            return ExceptionBoundary(                                           \
            [&]                                                                 \
            {                                                                   \
                CheckInPointer(value);                                          \
                auto& resource = GetResource();                                 \
                WinString winStr = stringGetFn(resource.Get());                 \
                winStr.CopyTo(value);                                           \
            });                                                                 \
        }

    SIMPLE_GLOBAL_GETTER(get_FlowDirection, CanvasTextDirection, GetFlowDirection, ToCanvasTextDirection);
    SIMPLE_GLOBAL_SETTER(put_FlowDirection, CanvasTextDirection, SetFlowDirection, ToFlowDirection);

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

    SIMPLE_GLOBAL_GETTER(get_ReadingDirection, CanvasTextDirection, GetReadingDirection, ToCanvasTextDirection);
    SIMPLE_GLOBAL_SETTER(put_ReadingDirection, CanvasTextDirection, SetReadingDirection, ToReadingDirection);

    SIMPLE_GLOBAL_GETTER(get_ParagraphAlignment, ABI::Windows::UI::Text::ParagraphAlignment, GetTextAlignment, ToWindowsParagraphAlignment);
    SIMPLE_GLOBAL_SETTER(put_ParagraphAlignment, ABI::Windows::UI::Text::ParagraphAlignment, SetTextAlignment, ToTextAlignment);

    SIMPLE_GLOBAL_GETTER(get_WordWrapping, CanvasWordWrapping, GetWordWrapping, ToCanvasWordWrapping);
    SIMPLE_GLOBAL_SETTER(put_WordWrapping, CanvasWordWrapping, SetWordWrapping, ToWordWrapping);


    IFACEMETHODIMP CanvasTextLayout::GetMinimumLayoutWidth(
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

    IFACEMETHODIMP CanvasTextLayout::get_LineSpacingMethod(
        CanvasLineSpacingMethod* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);
                auto& resource = GetResource(); 

                DWRITE_LINE_SPACING_METHOD method;
                float unused;
                ThrowIfFailed(resource->GetLineSpacing(&method, &unused, &unused));

                *value = ToCanvasLineSpacingMethod(method);
            });
    }

    IFACEMETHODIMP CanvasTextLayout::put_LineSpacingMethod(
        CanvasLineSpacingMethod value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource(); 

                DWRITE_LINE_SPACING_METHOD unused;
                float lineSpacing;
                float baseline;
                ThrowIfFailed(resource->GetLineSpacing(&unused, &lineSpacing, &baseline));

                ThrowIfInvalid<CanvasLineSpacingMethod>(value);

                ThrowIfFailed(resource->SetLineSpacing(static_cast<DWRITE_LINE_SPACING_METHOD>(value), lineSpacing, baseline));
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

                DWRITE_LINE_SPACING_METHOD unusedMethod;
                float unusedBaseline;
                ThrowIfFailed(resource->GetLineSpacing(&unusedMethod, value, &unusedBaseline));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::put_LineSpacing(
        float value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource(); 

                DWRITE_LINE_SPACING_METHOD method;
                float unusedSpacing;
                float baseline;
                ThrowIfFailed(resource->GetLineSpacing(&method, &unusedSpacing, &baseline));

                ThrowIfFailed(resource->SetLineSpacing(method, value, baseline));
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

                DWRITE_LINE_SPACING_METHOD unusedMethod;
                float unusedSpacing;
                ThrowIfFailed(resource->GetLineSpacing(&unusedMethod, &unusedSpacing, value));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::put_LineSpacingBaseline(
        float value)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource(); 

                DWRITE_LINE_SPACING_METHOD method;
                float spacing;
                float unusedBaseline;
                ThrowIfFailed(resource->GetLineSpacing(&method, &spacing, &unusedBaseline));

                ThrowIfFailed(resource->SetLineSpacing(method, spacing, value));
            });
    }

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

                WinString winStr = ToCanvasTrimmingDelimiter(trimming.delimiter);
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

    IFACEMETHODIMP CanvasTextLayout::get_MaximumLayoutSize(
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

    IFACEMETHODIMP CanvasTextLayout::put_MaximumLayoutSize(
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

                ComPtr<IDWriteFontCollection> fontCollection = Manager()->GetFontCollectionFromUri(uri);

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

                FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvanceWidth;
                ThrowIfFailed(resource->GetCharacterSpacing(characterIndex, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvanceWidth));

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

                FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvanceWidth;
                ThrowIfFailed(resource->GetCharacterSpacing(characterIndex, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvanceWidth));

                *value = trailingSpacing;
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetMinimumCharacterAdvanceWidth(
        int32_t characterIndex,
        float* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                ThrowIfNegative(characterIndex);

                auto& resource = GetResource();

                FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvanceWidth;
                ThrowIfFailed(resource->GetCharacterSpacing(characterIndex, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvanceWidth));

                *value = minimumCharacterAdvanceWidth;
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetCharacterSpacing(
        float leadingSpacing,
        float trailingSpacing,
        float minimumAdvanceWidth,
        int32_t characterIndex,
        int32_t characterCount)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                ThrowIfFailed(resource->SetCharacterSpacing(
                    leadingSpacing, 
                    trailingSpacing, 
                    minimumAdvanceWidth,
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

    IFACEMETHODIMP CanvasTextLayout::Close()
    {
        return ResourceWrapper::Close();
    }

    ActivatableClassWithFactory(CanvasTextLayout, CanvasTextLayoutFactory);

}}}}

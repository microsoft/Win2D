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
        int32_t positionInText, 
        IDWriteTextLayout2* layout)
    {
        WinStringBuilder stringBuilder;
        uint32_t attributeLength;
        ThrowIfFailed((layout->*fnGetLength)(positionInText, &attributeLength, nullptr));
        attributeLength++; // Account for null terminator

        auto buffer = stringBuilder.Allocate(attributeLength);
        ThrowIfFailed((*layout.*fnGet)(positionInText, buffer, attributeLength, nullptr));
        return stringBuilder.Get();
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


    IFACEMETHODIMP CanvasTextLayout::GetFormatChangePositions(
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

                uint32_t currentPositionInText = 0;
                BOOL unused;
                DWRITE_TEXT_RANGE formatRange;

                //
                // IDWriteTextLayout's getters will return discrete format
                // runs for any change in formatting, not just format 
                // changes related to that specific getter. GetUnderline 
                // is chosen here arbitrarily.
                //

                while (currentPositionInText < UINT_MAX)
                {
                    ThrowIfFailed(resource->GetUnderline(currentPositionInText, &unused, &formatRange));

                    assert(formatRange.startPosition == currentPositionInText);
                    positionVector.push_back(formatRange.startPosition);

                    currentPositionInText += formatRange.length;
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
        int32_t positionInText,
        HSTRING* fontFamily)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(fontFamily);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                auto fontFamilyName = GetTextFromLayoutResource(
                    &IDWriteTextLayout::GetFontFamilyNameLength,
                    &IDWriteTextLayout::GetFontFamilyName,
                    positionInText,
                    resource.Get());

                fontFamilyName.CopyTo(fontFamily);
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetFontSize(
        int32_t positionInText,
        float* fontSize)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(fontSize);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                ThrowIfFailed(resource->GetFontSize(positionInText, fontSize));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetFontStretch(
        int32_t positionInText,
        ABI::Windows::UI::Text::FontStretch* fontStretch)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(fontStretch);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                DWRITE_FONT_STRETCH dwriteFontStretch;
                ThrowIfFailed(resource->GetFontStretch(positionInText, &dwriteFontStretch));

                *fontStretch = ToWindowsFontStretch(dwriteFontStretch);
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetFontStyle(
        int32_t positionInText,
        ABI::Windows::UI::Text::FontStyle* fontStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(fontStyle);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                DWRITE_FONT_STYLE dwriteFontStyle;
                ThrowIfFailed(resource->GetFontStyle(positionInText, &dwriteFontStyle));

                *fontStyle = ToWindowsFontStyle(dwriteFontStyle);
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetFontWeight(
        int32_t positionInText,
        ABI::Windows::UI::Text::FontWeight* fontWeight)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(fontWeight);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                DWRITE_FONT_WEIGHT dwriteFontWeight;
                ThrowIfFailed(resource->GetFontWeight(positionInText, &dwriteFontWeight));

                *fontWeight = ToWindowsFontWeight(dwriteFontWeight);
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetLocaleName(
        int32_t positionInText,
        HSTRING* localeName)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(localeName);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                auto name = GetTextFromLayoutResource(
                    &IDWriteTextLayout::GetLocaleNameLength,
                    &IDWriteTextLayout::GetLocaleName,
                    positionInText,
                    resource.Get());

                name.CopyTo(localeName);
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetStrikethrough(
        int32_t positionInText,
        boolean* hasStrikethrough)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(hasStrikethrough);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                BOOL dwriteHasStrikethrough;
                ThrowIfFailed(resource->GetStrikethrough(positionInText, &dwriteHasStrikethrough));

                *hasStrikethrough = !!dwriteHasStrikethrough;
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetUnderline(
        int32_t positionInText,
        boolean* hasUnderline)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(hasUnderline);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                BOOL dwriteHasUnderline;
                ThrowIfFailed(resource->GetUnderline(positionInText, &dwriteHasUnderline));

                *hasUnderline = !!dwriteHasUnderline;
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetFontFamily(
        int32_t positionInText,
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

                auto textRange = ToDWriteTextRange(positionInText, characterCount);

                ThrowIfFailed(resource->SetFontCollection(fontCollection.Get(), textRange));
                ThrowIfFailed(resource->SetFontFamilyName(static_cast<const wchar_t*>(fontFamily), textRange));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetFontSize(
        int32_t positionInText,
        int32_t characterCount,
        float fontSize)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();
                
                ThrowIfFailed(resource->SetFontSize(fontSize, ToDWriteTextRange(positionInText, characterCount)));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetFontStretch(
        int32_t positionInText,
        int32_t characterCount,
        ABI::Windows::UI::Text::FontStretch fontStretch)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();
                
                ThrowIfFailed(resource->SetFontStretch(ToFontStretch(fontStretch), ToDWriteTextRange(positionInText, characterCount)));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetFontStyle(
        int32_t positionInText,
        int32_t characterCount,
        ABI::Windows::UI::Text::FontStyle fontStyle)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();
                
                ThrowIfFailed(resource->SetFontStyle(ToFontStyle(fontStyle), ToDWriteTextRange(positionInText, characterCount)));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetFontWeight(
        int32_t positionInText,
        int32_t characterCount,
        ABI::Windows::UI::Text::FontWeight fontWeight)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();
                
                ThrowIfFailed(resource->SetFontWeight(ToFontWeight(fontWeight), ToDWriteTextRange(positionInText, characterCount)));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetLocaleName(
        int32_t positionInText,
        int32_t characterCount,
        HSTRING name)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                const wchar_t* localeNameBuffer = WindowsGetStringRawBuffer(name, nullptr);

                ThrowIfFailed(resource->SetLocaleName(localeNameBuffer, ToDWriteTextRange(positionInText, characterCount)));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetStrikethrough(
        int32_t positionInText,
        int32_t characterCount,
        boolean hasStrikethrough)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                ThrowIfFailed(resource->SetStrikethrough(hasStrikethrough, ToDWriteTextRange(positionInText, characterCount)));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetUnderline(
        int32_t positionInText,
        int32_t characterCount,
        boolean hasUnderline)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                ThrowIfFailed(resource->SetUnderline(hasUnderline, ToDWriteTextRange(positionInText, characterCount)));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetPairKerning(
        int32_t positionInText,
        boolean* hasPairKerning)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(hasPairKerning);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                BOOL dwriteHasPairKerning;
                ThrowIfFailed(resource->GetPairKerning(positionInText, &dwriteHasPairKerning));

                *hasPairKerning = !!dwriteHasPairKerning;
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetPairKerning(
        int32_t positionInText,
        int32_t characterCount,
        boolean hasPairKerning)
    {
        return ExceptionBoundary(
            [&]
            {
                auto& resource = GetResource();

                ThrowIfFailed(resource->SetPairKerning(hasPairKerning, ToDWriteTextRange(positionInText, characterCount)));
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetLeadingCharacterSpacing(
        int32_t positionInText,
        float* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvanceWidth;
                ThrowIfFailed(resource->GetCharacterSpacing(positionInText, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvanceWidth));

                *value = leadingSpacing;
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetTrailingCharacterSpacing(
        int32_t positionInText,
        float* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvanceWidth;
                ThrowIfFailed(resource->GetCharacterSpacing(positionInText, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvanceWidth));

                *value = trailingSpacing;
            });
    }

    IFACEMETHODIMP CanvasTextLayout::GetMinimumCharacterAdvanceWidth(
        int32_t positionInText,
        float* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);

                ThrowIfNegative(positionInText);

                auto& resource = GetResource();

                FLOAT leadingSpacing, trailingSpacing, minimumCharacterAdvanceWidth;
                ThrowIfFailed(resource->GetCharacterSpacing(positionInText, &leadingSpacing, &trailingSpacing, &minimumCharacterAdvanceWidth));

                *value = minimumCharacterAdvanceWidth;
            });
    }

    IFACEMETHODIMP CanvasTextLayout::SetCharacterSpacing(
        float leadingSpacing,
        float trailingSpacing,
        float minimumAdvanceWidth,
        int32_t positionInText,
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
                    ToDWriteTextRange(positionInText, characterCount)));
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

    IFACEMETHODIMP CanvasTextLayout::Close()
    {
        return ResourceWrapper::Close();
    }

    ActivatableClassWithFactory(CanvasTextLayout, CanvasTextLayoutFactory);

}}}}

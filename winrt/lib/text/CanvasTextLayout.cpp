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
    // CanvasTextLayoutAdapter implementation
    //

    ComPtr<IDWriteFactory> CanvasTextLayoutAdapter::CreateDWriteFactory(DWRITE_FACTORY_TYPE type)
    {
        ComPtr<IDWriteFactory> factory;
        ThrowIfFailed(DWriteCreateFactory(type, __uuidof(factory), &factory));
        return factory;
    }

    //
    // CanvasTextLayoutManager implementation
    //

    CanvasTextLayoutManager::CanvasTextLayoutManager(std::shared_ptr<ICanvasTextLayoutAdapter> adapter)
        : m_adapter(adapter)
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

    ComPtr<IDWriteFactory> const& CanvasTextLayoutManager::GetSharedFactory()
    {
        if (!m_sharedFactory)
        {
            m_sharedFactory = m_adapter->CreateDWriteFactory(DWRITE_FACTORY_TYPE_SHARED);
        }

        return m_sharedFactory;
    }


    //
    // CanvasTextLayoutFactory implementation
    //


    CanvasTextLayoutFactory::CanvasTextLayoutFactory()
    {
    }

    std::shared_ptr<CanvasTextLayoutManager> CanvasTextLayoutFactory::CreateManager()
    {
        return std::make_shared<CanvasTextLayoutManager>(std::make_shared<CanvasTextLayoutAdapter>());
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
    {
        auto formatManager = CanvasTextFormatFactory::GetOrCreateManager();
        m_format = formatManager->Create(layout);
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

#define TEXTFORMAT_PROPERTY(op, x, prefix, type)                \
    IFACEMETHODIMP CanvasTextLayout::##op##_##prefix##x(        \
        type value)                                             \
    {                                                           \
        return ExceptionBoundary(                               \
            [&]                                                 \
            {                                                   \
                auto& format = m_format.EnsureNotClosed();      \
                ThrowIfFailed(format->##op##_##x(value));       \
            });                                                 \
    }

    TEXTFORMAT_PROPERTY(get, FlowDirection, , CanvasTextDirection*);
    TEXTFORMAT_PROPERTY(put, FlowDirection, , CanvasTextDirection);
    TEXTFORMAT_PROPERTY(get, FontFamily, Default, HSTRING*);
    TEXTFORMAT_PROPERTY(get, FontSize, Default, float*);
    TEXTFORMAT_PROPERTY(get, FontStretch, Default, ABI::Windows::UI::Text::FontStretch*);
    TEXTFORMAT_PROPERTY(get, FontStyle, Default, ABI::Windows::UI::Text::FontStyle*);
    TEXTFORMAT_PROPERTY(get, FontWeight, Default, ABI::Windows::UI::Text::FontWeight*);
    TEXTFORMAT_PROPERTY(get, IncrementalTabStop, , float*);
    TEXTFORMAT_PROPERTY(put, IncrementalTabStop, , float);
    TEXTFORMAT_PROPERTY(get, LineSpacingMethod, , CanvasLineSpacingMethod*);
    TEXTFORMAT_PROPERTY(put, LineSpacingMethod, , CanvasLineSpacingMethod);
    TEXTFORMAT_PROPERTY(get, LineSpacing, , float*);
    TEXTFORMAT_PROPERTY(put, LineSpacing, , float);
    TEXTFORMAT_PROPERTY(get, LineSpacingBaseline, , float*);
    TEXTFORMAT_PROPERTY(put, LineSpacingBaseline, , float);
    TEXTFORMAT_PROPERTY(get, LocaleName, Default, HSTRING*);
    TEXTFORMAT_PROPERTY(get, VerticalAlignment, , CanvasVerticalAlignment*);
    TEXTFORMAT_PROPERTY(put, VerticalAlignment, , CanvasVerticalAlignment);
    TEXTFORMAT_PROPERTY(get, ReadingDirection, , CanvasTextDirection*);
    TEXTFORMAT_PROPERTY(put, ReadingDirection, , CanvasTextDirection);
    TEXTFORMAT_PROPERTY(get, ParagraphAlignment, , ABI::Windows::UI::Text::ParagraphAlignment*);
    TEXTFORMAT_PROPERTY(put, ParagraphAlignment, , ABI::Windows::UI::Text::ParagraphAlignment);
    TEXTFORMAT_PROPERTY(get, TrimmingGranularity, , CanvasTextTrimmingGranularity*);
    TEXTFORMAT_PROPERTY(put, TrimmingGranularity, , CanvasTextTrimmingGranularity);
    TEXTFORMAT_PROPERTY(get, TrimmingDelimiter, , HSTRING*);
    TEXTFORMAT_PROPERTY(put, TrimmingDelimiter, , HSTRING);
    TEXTFORMAT_PROPERTY(get, TrimmingDelimiterCount, , int32_t*);
    TEXTFORMAT_PROPERTY(put, TrimmingDelimiterCount, , int32_t);
    TEXTFORMAT_PROPERTY(get, WordWrapping, , CanvasWordWrapping*);
    TEXTFORMAT_PROPERTY(put, WordWrapping, , CanvasWordWrapping);
    TEXTFORMAT_PROPERTY(get, Options, , CanvasDrawTextOptions*);
    TEXTFORMAT_PROPERTY(put, Options, , CanvasDrawTextOptions);
#undef TEXTFORMAT_PROPERTY

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
        HSTRING fontFamily)
    {
        return ExceptionBoundary(
            [&]
            {
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();

                const wchar_t* fontFamilyBuffer = WindowsGetStringRawBuffer(fontFamily, nullptr);

                ThrowIfFailed(resource->SetFontFamilyName(fontFamilyBuffer, DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();
                
                ThrowIfFailed(resource->SetFontSize(fontSize, DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();
                
                ThrowIfFailed(resource->SetFontStretch(ToFontStretch(fontStretch), DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();
                
                ThrowIfFailed(resource->SetFontStyle(ToFontStyle(fontStyle), DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();
                
                ThrowIfFailed(resource->SetFontWeight(ToFontWeight(fontWeight), DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();

                const wchar_t* localeNameBuffer = WindowsGetStringRawBuffer(name, nullptr);

                ThrowIfFailed(resource->SetLocaleName(localeNameBuffer, DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();

                ThrowIfFailed(resource->SetStrikethrough(hasStrikethrough, DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();

                ThrowIfFailed(resource->SetUnderline(hasUnderline, DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();

                ThrowIfFailed(resource->SetPairKerning(hasPairKerning, DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
                ThrowIfNegative(positionInText);
                ThrowIfNegative(characterCount);

                auto& resource = GetResource();

                ThrowIfFailed(resource->SetCharacterSpacing(
                    leadingSpacing, 
                    trailingSpacing, 
                    minimumAdvanceWidth,
                    DWRITE_TEXT_RANGE{ positionInText, characterCount }));
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
        m_format.Close();

        return ResourceWrapper::Close();
    }

    ActivatableClassWithFactory(CanvasTextLayout, CanvasTextLayoutFactory);

}}}}

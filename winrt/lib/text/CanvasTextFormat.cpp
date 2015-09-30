// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasTextFormat.h"
#include "TextUtilities.h"

using namespace ABI::Microsoft::Graphics::Canvas::Text;
using namespace ABI::Windows::Storage;


//
// Parameter validation functions
//

using ABI::Microsoft::Graphics::Canvas::Text::ThrowIfInvalid;

template<>
void ThrowIfInvalid(ABI::Windows::UI::Text::FontStretch value)
{
    switch (value)
    {
    case ABI::Windows::UI::Text::FontStretch_Undefined:
    case ABI::Windows::UI::Text::FontStretch_UltraCondensed:
    case ABI::Windows::UI::Text::FontStretch_ExtraCondensed:
    case ABI::Windows::UI::Text::FontStretch_Condensed:
    case ABI::Windows::UI::Text::FontStretch_SemiCondensed:
    case ABI::Windows::UI::Text::FontStretch_Normal:
    case ABI::Windows::UI::Text::FontStretch_SemiExpanded:
    case ABI::Windows::UI::Text::FontStretch_Expanded:
    case ABI::Windows::UI::Text::FontStretch_ExtraExpanded:
    case ABI::Windows::UI::Text::FontStretch_UltraExpanded:
        return;

    default:
        ThrowHR(E_INVALIDARG);
    }
}

template<>
void ThrowIfInvalid(ABI::Windows::UI::Text::FontStyle value)
{
    switch (value)
    {
    case ABI::Windows::UI::Text::FontStyle_Normal:
    case ABI::Windows::UI::Text::FontStyle_Oblique:
    case ABI::Windows::UI::Text::FontStyle_Italic:
        return;

    default:
        ThrowHR(E_INVALIDARG);
    }
}

static void ThrowIfInvalidFontWeight(ABI::Windows::UI::Text::FontWeight value)
{
    if (value.Weight >= 1 && value.Weight <= 999)
        return;

    ThrowHR(E_INVALIDARG);
}

static void ThrowIfNan(float value)
{
    if (isnan(value))
        ThrowHR(E_INVALIDARG);
}

static void ThrowIfNegativeOrNan(float value)
{
    if (value < 0 || isnan(value))
        ThrowHR(E_INVALIDARG);
}


//
// CanvasTextFormatFactory implementation
//

IFACEMETHODIMP CanvasTextFormatFactory::ActivateInstance(IInspectable** object)
{
    return ExceptionBoundary(
        [&]
        {
            auto format = Make<CanvasTextFormat>();
            CheckMakeResult(format);
            ThrowIfFailed(format.CopyTo(object));
        });
}


//
// CanvasTextFormat implementation
//


CanvasTextFormat::CanvasTextFormat()
    : ResourceWrapper(nullptr)
    , m_customFontManager(CustomFontManager::GetInstance())
    , m_closed(false)
    , m_direction(CanvasTextDirection::LeftToRightThenTopToBottom)
    , m_fontFamilyName(L"Segoe UI")
    , m_fontSize(20.0f)
    , m_fontStretch(ABI::Windows::UI::Text::FontStretch_Normal)
    , m_fontStyle(ABI::Windows::UI::Text::FontStyle_Normal)
    , m_fontWeight(ToWindowsFontWeight(DWRITE_FONT_WEIGHT_NORMAL))
    , m_incrementalTabStop(-1.0f)
    , m_lineSpacingMode(CanvasLineSpacingMode::Default)
    , m_lineSpacing(-1.0f)
    , m_lineSpacingBaseline(1.0f)
    , m_verticalAlignment(CanvasVerticalAlignment::Top)
    , m_horizontalAlignment(CanvasHorizontalAlignment::Left)
    , m_trimmingGranularity(CanvasTextTrimmingGranularity::None)
    , m_trimmingDelimiterCount(0)
    , m_wordWrapping(CanvasWordWrapping::Wrap)
    , m_drawTextOptions(CanvasDrawTextOptions::Default)
{
}


CanvasTextFormat::CanvasTextFormat(IDWriteTextFormat* format)
    : ResourceWrapper(format)
    , m_customFontManager(CustomFontManager::GetInstance())
    , m_closed(false)
    , m_drawTextOptions(CanvasDrawTextOptions::Default)
    , m_lineSpacingMode(CanvasLineSpacingMode::Default)
{
    SetShadowPropertiesFromDWrite();
}


IFACEMETHODIMP CanvasTextFormat::Close()
{
    m_closed = true;
    return ResourceWrapper::Close();
}


void CanvasTextFormat::ThrowIfClosed()
{
    if (m_closed)
        ThrowHR(RO_E_CLOSED);
}


IFACEMETHODIMP CanvasTextFormat::GetNativeResource(ICanvasDevice* device, float dpi, REFIID iid, void** value)
{
    UNREFERENCED_PARAMETER(device);
    UNREFERENCED_PARAMETER(dpi);

    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(value);
            ThrowIfClosed();
            ThrowIfFailed(GetRealizedTextFormat().CopyTo(iid, value));
        });
}


ComPtr<IDWriteTextFormat> CanvasTextFormat::GetRealizedTextFormat()
{
    auto lock = GetLock();

    auto& existingResource = MaybeGetResource();

    if (existingResource)
    {
        return existingResource;
    }
    else
    {
        auto newResource = CreateRealizedTextFormat();

        SetResource(newResource.Get());

        return newResource;
    }
}


ComPtr<IDWriteTextFormat> CanvasTextFormat::CreateRealizedTextFormat(bool skipWordWrapping)
{
    auto factory = m_customFontManager->GetSharedFactory();

    auto uriAndFontFamily = GetUriAndFontFamily(m_fontFamilyName);
    auto const& uri = uriAndFontFamily.first;
    auto const& fontFamily = uriAndFontFamily.second;

    ComPtr<IDWriteFontCollection> fontCollection = m_fontCollection;

    if (!fontCollection)
    {
        fontCollection = m_customFontManager->GetFontCollectionFromUri(uri);
    }

    ComPtr<IDWriteTextFormat> textFormat;

    ThrowIfFailed(factory->CreateTextFormat(
        static_cast<const wchar_t*>(fontFamily),
        fontCollection.Get(),
        ToFontWeight(m_fontWeight),
        ToFontStyle(m_fontStyle),
        ToFontStretch(m_fontStretch),
        m_fontSize,
        static_cast<const wchar_t*>(m_localeName),
        &textFormat));

    RealizeDirection(textFormat.Get());
    RealizeIncrementalTabStop(textFormat.Get());
    RealizeLineSpacing(textFormat.Get());
    RealizeParagraphAlignment(textFormat.Get());
    RealizeTextAlignment(textFormat.Get());
    RealizeTrimming(textFormat.Get());

    if (!skipWordWrapping) 
        RealizeWordWrapping(textFormat.Get());

    return textFormat;
}


ComPtr<IDWriteTextFormat> CanvasTextFormat::GetRealizedTextFormatClone(CanvasWordWrapping overrideWordWrapping)
{
    //
    // This is called when we want to create a IDWriteTextFormat that is exactly
    // what would be returned by GetRealizedTextFormat() except that it has an
    // overridden word wrapping property.  This is needed by
    // CanvasDrawingSession::DrawTextAtPointImpl which needs to set the word
    // wrapping to NoWrap.  It needs to operate on a copy because DrawText is
    // conceptually a read-only operation and we don't want DrawText on one
    // thread to interfere with a DrawText on another thread using the same text
    // format.
    //

    ThrowIfInvalid<CanvasWordWrapping>(overrideWordWrapping);

    auto lock = GetLock();

    if (HasResource())
    {
        SetShadowPropertiesFromDWrite();
    }

    auto newFormat = CreateRealizedTextFormat(true);

    ThrowIfFailed(newFormat->SetWordWrapping(ToWordWrapping(overrideWordWrapping)));

    return newFormat;
}


D2D1_DRAW_TEXT_OPTIONS CanvasTextFormat::GetDrawTextOptions()
{
    return static_cast<D2D1_DRAW_TEXT_OPTIONS>(m_drawTextOptions);
}


void CanvasTextFormat::Unrealize()
{
    //
    // We're about to throw away our resource, so we need to extract all the
    // values stored on it into our shadow copies.
    //
    if (HasResource())
    {
        SetShadowPropertiesFromDWrite();

        ReleaseResource();
    }
}


void CanvasTextFormat::SetShadowPropertiesFromDWrite()
{
    auto& textFormat = GetResource();

    ThrowIfFailed(textFormat->GetFontCollection(&m_fontCollection));

    m_direction          = DWriteToCanvasTextDirection::Lookup(textFormat->GetReadingDirection(), textFormat->GetFlowDirection())->TextDirection;
    m_fontFamilyName     = GetFontFamilyName(textFormat.Get());
    m_fontSize           = textFormat->GetFontSize();
    m_fontStretch        = ToWindowsFontStretch(textFormat->GetFontStretch());
    m_fontStyle          = ToWindowsFontStyle(textFormat->GetFontStyle());
    m_fontWeight         = ToWindowsFontWeight(textFormat->GetFontWeight());
    m_incrementalTabStop = textFormat->GetIncrementalTabStop();
    m_localeName         = GetLocaleName(textFormat.Get());
    m_verticalAlignment  = ToCanvasVerticalAlignment(textFormat->GetParagraphAlignment());
    m_horizontalAlignment = ToCanvasHorizontalAlignment(textFormat->GetTextAlignment());
    m_wordWrapping       = ToCanvasWordWrapping(textFormat->GetWordWrapping());

    DWriteLineSpacing spacing(textFormat.Get());
    m_lineSpacing = spacing.GetAdjustedSpacing();
    m_lineSpacingBaseline = spacing.Baseline;

#if WINVER > _WIN32_WINNT_WINBLUE
    bool isUniform = m_lineSpacingMode == CanvasLineSpacingMode::Uniform;
    m_lineSpacingMode = spacing.GetAdjustedLineSpacingMode(isUniform);
#endif

    DWRITE_TRIMMING trimmingOptions{};
    ComPtr<IDWriteInlineObject> inlineObject;
    ThrowIfFailed(textFormat->GetTrimming(
        &trimmingOptions,
        &inlineObject));

    m_trimmingGranularity = ToCanvasTextTrimmingGranularity(trimmingOptions.granularity);
    m_trimmingDelimiter = ToCanvasTrimmingDelimiter(trimmingOptions.delimiter);
    m_trimmingDelimiterCount = trimmingOptions.delimiterCount;
}


//
// PropertyGet/PropertyPut - these encapsulate the process of getting /
// setting properties that might be reflected on the realized
// IDWriteTextFormat object.
//

template<typename T>
static bool IsSame(T* outputValue, T const& value)
{
    return ((*outputValue) == value);
}

template<>
bool IsSame(ABI::Windows::UI::Text::FontWeight* outputValue, ABI::Windows::UI::Text::FontWeight const& value)
{
    return IsSame(&outputValue->Weight, value.Weight);
}

template<typename WinString>
static bool IsSame(HSTRING* outputValue, WinString const& value)
{
    return value.Equals(*outputValue);
}

template<typename HSTRING>
static bool IsSame(WinString* outputValue, HSTRING const& value)
{
    return outputValue->Equals(value);
}


template<typename T>
static void SetFrom(T* outputValue, T const& value)
{
    *outputValue = value;
}

template<>
void SetFrom(ABI::Windows::UI::Text::FontWeight* outputValue, ABI::Windows::UI::Text::FontWeight const& value)
{
    SetFrom(&outputValue->Weight, value.Weight);
}

template<typename WinString>
static void SetFrom(HSTRING* outputValue, WinString const& value)
{
    value.CopyTo(outputValue);
}

template<typename HSTRING>
static void SetFrom(WinString* outputValue, HSTRING const& value)
{
    *outputValue = value;
}


template<typename T, typename ST, typename FN>
HRESULT __declspec(nothrow) CanvasTextFormat::PropertyGet(T* value, ST const& shadowValue, FN realizedGetter)
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();

            CheckInPointer(value);
            ThrowIfClosed();

            auto& textFormat = MaybeGetResource();

            if (textFormat)
                SetFrom(value, realizedGetter(textFormat.Get()));
            else
                SetFrom(value, shadowValue);
        });
}


template<typename T, typename TT, typename FNV>
HRESULT __declspec(nothrow) CanvasTextFormat::PropertyPut(T value, TT* dest, FNV&& validator, void(CanvasTextFormat::*realizer)(IDWriteTextFormat*))
{
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();

            validator(value);

            ThrowIfClosed();

            if (!realizer)
            {
                if (IsSame(dest, value))
                {
                    // Don't do anything if the value we're setting is the same
                    // as the current value. This optimization is only valid for
                    // properties that do not have realizers, because realized
                    // dwrite state trumps our shadow state and can change out
                    // from under us in the presence of interop.
                    return;
                }

                // If there's no realizer set then we're going to have to
                // throw away the dwrite resource (ready to be recreated
                // the next time it is needed)
                Unrealize();
            }

            // Set the shadow value
            SetFrom(dest, value);

            // Realize the value on the dwrite object, if we can
            auto& textFormat = MaybeGetResource();

            if (textFormat && realizer)
            {
                (this->*realizer)(textFormat.Get());
            }
        });
}

template<typename T, typename TT>
HRESULT __declspec(nothrow) CanvasTextFormat::PropertyPut(T value, TT* dest, void(CanvasTextFormat::*realizer)(IDWriteTextFormat*))
{
    return PropertyPut(value, dest, [](T){}, realizer);
}

//
// CanvasTextFormat.Direction
//

static DWriteToCanvasTextDirection gDWriteToCanvasTextDirectionMappingTable[] =
{ 
    { DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM, CanvasTextDirection::LeftToRightThenTopToBottom },
    { DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM, CanvasTextDirection::RightToLeftThenTopToBottom },
    { DWRITE_READING_DIRECTION_LEFT_TO_RIGHT, DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP, CanvasTextDirection::LeftToRightThenBottomToTop },
    { DWRITE_READING_DIRECTION_RIGHT_TO_LEFT, DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP, CanvasTextDirection::RightToLeftThenBottomToTop },
    { DWRITE_READING_DIRECTION_TOP_TO_BOTTOM, DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT, CanvasTextDirection::TopToBottomThenLeftToRight },
    { DWRITE_READING_DIRECTION_BOTTOM_TO_TOP, DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT, CanvasTextDirection::BottomToTopThenLeftToRight },
    { DWRITE_READING_DIRECTION_TOP_TO_BOTTOM, DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT, CanvasTextDirection::TopToBottomThenRightToLeft },
    { DWRITE_READING_DIRECTION_BOTTOM_TO_TOP, DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT, CanvasTextDirection::BottomToTopThenRightToLeft },
};

/* static */
DWriteToCanvasTextDirection const* DWriteToCanvasTextDirection::Lookup(DWRITE_READING_DIRECTION readingDirection, DWRITE_FLOW_DIRECTION flowDirection)
{
    for (auto const& entry : gDWriteToCanvasTextDirectionMappingTable)
    {
        if (entry.ReadingDirection == readingDirection && entry.FlowDirection == flowDirection)
            return &entry;
    }

    ThrowHR(DWRITE_E_FLOWDIRECTIONCONFLICTS);
}

/* static */
DWriteToCanvasTextDirection const* DWriteToCanvasTextDirection::Lookup(CanvasTextDirection textDirection)
{
    for (auto const& entry : gDWriteToCanvasTextDirectionMappingTable)
    {
        if (entry.TextDirection == textDirection)
            return &entry;
    }

    ThrowHR(E_INVALIDARG);
}


void CanvasTextFormat::RealizeDirection(IDWriteTextFormat* textFormat)
{
    auto entry = DWriteToCanvasTextDirection::Lookup(m_direction);
    assert(entry);          // m_direction should always be set to a valid value

    ThrowIfFailed(textFormat->SetReadingDirection(entry->ReadingDirection));
    ThrowIfFailed(textFormat->SetFlowDirection(entry->FlowDirection));
}


IFACEMETHODIMP CanvasTextFormat::get_Direction(CanvasTextDirection* value)
{
    return PropertyGet(
        value,
        m_direction,
        [](IDWriteTextFormat* textFormat)
        {
            return DWriteToCanvasTextDirection::Lookup(textFormat->GetReadingDirection(), textFormat->GetFlowDirection())->TextDirection;
        });
}


IFACEMETHODIMP CanvasTextFormat::put_Direction(CanvasTextDirection value)
{
    return PropertyPut(
        value,
        &m_direction,
        ThrowIfInvalid<CanvasTextDirection>,
        &CanvasTextFormat::RealizeDirection);
}

//
// CanvasTextFormat.FontFamily
//

IFACEMETHODIMP CanvasTextFormat::get_FontFamily(HSTRING* value)
{
    return PropertyGet(
        value,
        m_fontFamilyName,
        [&](IDWriteTextFormat*)
        {
            //
            // If this were any other simple property, this would get the
            // font family name from the realized IDWriteTextFormat.
            //
            // We have no way of storing the full information required to
            // reconstruct the CanvasTextFormat's font family name (ie
            // including the URI from a "uri#family" style name).
            //
            // But, all is not lost, since the font family and the font
            // collection are immutable in IDWriteTextFormat we can safely
            // assume that the shadow property has not been modified since
            // we realized it.
            //
            return m_fontFamilyName;
        });
}


IFACEMETHODIMP CanvasTextFormat::put_FontFamily(HSTRING value)
{
    //
    // FontFamily not only needs to Unrealize (since the font family
    // property of IDWriteTextFormat is immutable), but also needs to wipe
    // the font collection (since the font collection is chosen based on the
    // font family string).
    //
    return ExceptionBoundary(
        [&]
        {
            auto lock = GetLock();
            ThrowIfClosed();

            if (IsSame(&m_fontFamilyName, value))
                return;

            auto uriAndFontFamily = GetUriAndFontFamily(WinString(value));
            auto const& uri = uriAndFontFamily.first;
            m_customFontManager->ValidateUri(uri);

            Unrealize();
            m_fontCollection.Reset();

            SetFrom(&m_fontFamilyName, value);
        });
}


//
// CanvasTextFormat.FontSize
//

IFACEMETHODIMP CanvasTextFormat::get_FontSize(float* value)
{
    return PropertyGet(
        value,
        m_fontSize,
        [](IDWriteTextFormat* textFormat) { return textFormat->GetFontSize(); });
}


IFACEMETHODIMP CanvasTextFormat::put_FontSize(float value)
{
    return PropertyPut(
        value, 
        &m_fontSize,
        ThrowIfNegativeOrNan);
}

//
// CanvasTextFormat.FontStretch
//

IFACEMETHODIMP CanvasTextFormat::get_FontStretch(ABI::Windows::UI::Text::FontStretch* value)
{
    return PropertyGet(
        value,
        m_fontStretch,
        [](IDWriteTextFormat* textFormat) { return ToWindowsFontStretch(textFormat->GetFontStretch()); });
}


IFACEMETHODIMP CanvasTextFormat::put_FontStretch(ABI::Windows::UI::Text::FontStretch value)
{
    return PropertyPut(
        value, 
        &m_fontStretch, 
        ThrowIfInvalid<ABI::Windows::UI::Text::FontStretch>);
}

//
// CanvasTextFormat.FontStyle
//

IFACEMETHODIMP CanvasTextFormat::get_FontStyle(ABI::Windows::UI::Text::FontStyle* value)
{
    return PropertyGet(
        value,
        m_fontStyle,
        [](IDWriteTextFormat* textFormat) { return ToWindowsFontStyle(textFormat->GetFontStyle()); });
}


IFACEMETHODIMP CanvasTextFormat::put_FontStyle(ABI::Windows::UI::Text::FontStyle value)
{
    return PropertyPut(
        value, 
        &m_fontStyle, 
        ThrowIfInvalid<ABI::Windows::UI::Text::FontStyle>);
}

//
// CanvasTextFormat.FontWeight
// 

IFACEMETHODIMP CanvasTextFormat::get_FontWeight(ABI::Windows::UI::Text::FontWeight* value)
{
    return PropertyGet(
        value,
        m_fontWeight,
        [](IDWriteTextFormat* textFormat) { return ToWindowsFontWeight(textFormat->GetFontWeight()); });
}


IFACEMETHODIMP CanvasTextFormat::put_FontWeight(ABI::Windows::UI::Text::FontWeight value)
{
    return PropertyPut(
        value, 
        &m_fontWeight,
        ThrowIfInvalidFontWeight);
}

//
// CanvasTextFormat.IncrementalTabTop
//

IFACEMETHODIMP CanvasTextFormat::get_IncrementalTabStop(float* value)
{
    return PropertyGet(
        value,
        m_incrementalTabStop,
        [](IDWriteTextFormat* textFormat) { return textFormat->GetIncrementalTabStop(); });
}


IFACEMETHODIMP CanvasTextFormat::put_IncrementalTabStop(float value)
{
    return PropertyPut(
        value, 
        &m_incrementalTabStop, 
        ThrowIfNegativeOrNan,
        &CanvasTextFormat::RealizeIncrementalTabStop);
}


void CanvasTextFormat::RealizeIncrementalTabStop(IDWriteTextFormat* textFormat)
{
    // Negative value indicates that it hasn't been set yet, so we want to
    // use dwrite's default.
    if (m_incrementalTabStop >= 0.0f)
        ThrowIfFailed(textFormat->SetIncrementalTabStop(m_incrementalTabStop));
}

//
// CanvasTextFormat.LineSpacing
//

IFACEMETHODIMP CanvasTextFormat::get_LineSpacing(float* value)
{
    return PropertyGet(
        value,
        m_lineSpacing,
        [](IDWriteTextFormat* textFormat)
        { 
            return DWriteLineSpacing(textFormat).GetAdjustedSpacing();
        });
}


IFACEMETHODIMP CanvasTextFormat::put_LineSpacing(float value)
{
    return PropertyPut(
        value, 
        &m_lineSpacing, 
        ThrowIfNan,
        &CanvasTextFormat::RealizeLineSpacing);
}

//
// CanvasTextFormat.LineSpacingBaseline
//

IFACEMETHODIMP CanvasTextFormat::get_LineSpacingBaseline(float* value)
{
    return PropertyGet(
        value,
        m_lineSpacingBaseline,
        [](IDWriteTextFormat* textFormat) { return DWriteLineSpacing(textFormat).Baseline; });
}


IFACEMETHODIMP CanvasTextFormat::put_LineSpacingBaseline(float value)
{
    return PropertyPut(
        value, 
        &m_lineSpacingBaseline, 
        ThrowIfNan,
        &CanvasTextFormat::RealizeLineSpacing);
}


void CanvasTextFormat::RealizeLineSpacing(IDWriteTextFormat* textFormat)
{
    DWriteLineSpacing::Set(
        textFormat, 
        m_lineSpacingMode,
        m_lineSpacing, 
        m_lineSpacingBaseline);
}


//
// CanvasTextFormat.LocaleName
//

IFACEMETHODIMP CanvasTextFormat::get_LocaleName(HSTRING* value)
{
    return PropertyGet(
        value,
        m_localeName,
        [](IDWriteTextFormat* textFormat)
        {
            return GetLocaleName(textFormat);
        });
}


IFACEMETHODIMP CanvasTextFormat::put_LocaleName(HSTRING value)
{
    return PropertyPut(
        value, 
        &m_localeName);
}

//
// CanvasTextFormat.VerticalAlignment
//

IFACEMETHODIMP CanvasTextFormat::get_VerticalAlignment(CanvasVerticalAlignment* value)
{
    // Canvas calls it "VerticalAlignment", but DWrite calls it "ParagraphAlignment"
    return PropertyGet(
        value,
        m_verticalAlignment,
        [](IDWriteTextFormat* textFormat) { return ToCanvasVerticalAlignment(textFormat->GetParagraphAlignment()); });
}


IFACEMETHODIMP CanvasTextFormat::put_VerticalAlignment(CanvasVerticalAlignment value)
{
    return PropertyPut(
        value, 
        &m_verticalAlignment,
        ThrowIfInvalid<CanvasVerticalAlignment>,
        &CanvasTextFormat::RealizeParagraphAlignment);
}


void CanvasTextFormat::RealizeParagraphAlignment(IDWriteTextFormat* textFormat)
{
    ThrowIfFailed(textFormat->SetParagraphAlignment(ToParagraphAlignment(m_verticalAlignment)));
}

//
// CanvasTextFormat.HorizontalAlignment
//

IFACEMETHODIMP CanvasTextFormat::get_HorizontalAlignment(CanvasHorizontalAlignment* value)
{
    // Canvas calls it "HorizontalAlignment", but DWrite calls it "TextAlignment"
    return PropertyGet(
        value,
        m_horizontalAlignment,
        [](IDWriteTextFormat* textFormat) { return ToCanvasHorizontalAlignment(textFormat->GetTextAlignment()); });
}


IFACEMETHODIMP CanvasTextFormat::put_HorizontalAlignment(CanvasHorizontalAlignment value)
{
    return PropertyPut(
        value, 
        &m_horizontalAlignment,
        ThrowIfInvalid<CanvasHorizontalAlignment>,
        &CanvasTextFormat::RealizeTextAlignment);
}

void CanvasTextFormat::RealizeTextAlignment(IDWriteTextFormat* textFormat)
{
    ThrowIfFailed(textFormat->SetTextAlignment(ToTextAlignment(m_horizontalAlignment)));        
}

//
// CanvasTextFormat.TrimmingGranulatity
//

IFACEMETHODIMP CanvasTextFormat::get_TrimmingGranularity(CanvasTextTrimmingGranularity* value)
{
    return PropertyGet(
        value,
        m_trimmingGranularity,
        [](IDWriteTextFormat* textFormat) { return ToCanvasTextTrimmingGranularity(DWriteTrimming(textFormat).Options.granularity); });
}


IFACEMETHODIMP CanvasTextFormat::put_TrimmingGranularity(CanvasTextTrimmingGranularity value)
{
    return PropertyPut(
        value, 
        &m_trimmingGranularity,
        ThrowIfInvalid<CanvasTextTrimmingGranularity>,
        &CanvasTextFormat::RealizeTrimming);        
}

//
// CanvasTextFormat.TrimmingDelimiter
//

IFACEMETHODIMP CanvasTextFormat::get_TrimmingDelimiter(HSTRING* value)
{
    return PropertyGet(
        value,
        m_trimmingDelimiter,
        [](IDWriteTextFormat* textFormat) { return ToCanvasTrimmingDelimiter(DWriteTrimming(textFormat).Options.delimiter); });
}


IFACEMETHODIMP CanvasTextFormat::put_TrimmingDelimiter(HSTRING value)
{
    return PropertyPut(
        value, 
        &m_trimmingDelimiter,
        ThrowIfInvalidTrimmingDelimiter,
        &CanvasTextFormat::RealizeTrimming);
}

//
// CanvasTextFormat.TrimmingDelimiterCount
//

IFACEMETHODIMP CanvasTextFormat::get_TrimmingDelimiterCount(int32_t* value)
{
    return PropertyGet(
        value,
        m_trimmingDelimiterCount,
        [](IDWriteTextFormat* textFormat) { return static_cast<int32_t>(DWriteTrimming(textFormat).Options.delimiterCount); });
}


IFACEMETHODIMP CanvasTextFormat::put_TrimmingDelimiterCount(int32_t value)
{
    return PropertyPut(
        value, 
        &m_trimmingDelimiterCount,
        ThrowIfNegative<int32_t>,
        &CanvasTextFormat::RealizeTrimming);
}


void CanvasTextFormat::RealizeTrimming(IDWriteTextFormat* textFormat)
{
    DWRITE_TRIMMING trimmingOptions{};
    trimmingOptions.granularity = ToTrimmingGranularity(m_trimmingGranularity);
    trimmingOptions.delimiter = ToTrimmingDelimiter(m_trimmingDelimiter);
    trimmingOptions.delimiterCount = m_trimmingDelimiterCount;

    ThrowIfFailed(textFormat->SetTrimming(
        &trimmingOptions,
        nullptr));        
}


//
// CanvasTextFormat.WordWrapping
//

IFACEMETHODIMP CanvasTextFormat::get_WordWrapping(CanvasWordWrapping* value)
{
    return PropertyGet(
        value,
        m_wordWrapping,
        [](IDWriteTextFormat* textFormat) { return ToCanvasWordWrapping(textFormat->GetWordWrapping()); });
}


IFACEMETHODIMP CanvasTextFormat::put_WordWrapping(CanvasWordWrapping value)
{
    return PropertyPut(
        value, 
        &m_wordWrapping,
        ThrowIfInvalid<CanvasWordWrapping>,
        &CanvasTextFormat::RealizeWordWrapping);
}


void CanvasTextFormat::RealizeWordWrapping(IDWriteTextFormat* textFormat)
{
    ThrowIfFailed(textFormat->SetWordWrapping(ToWordWrapping(m_wordWrapping)));        
}

//
// CanvasTextFormat.Options
//

IFACEMETHODIMP CanvasTextFormat::get_Options(CanvasDrawTextOptions* value)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(value);
            ThrowIfClosed();
            *value = m_drawTextOptions;
        });
}


IFACEMETHODIMP CanvasTextFormat::put_Options(CanvasDrawTextOptions value)
{
    return ExceptionBoundary(
        [&]
        {
            ThrowIfClosed();

            auto validOptions = 
                CanvasDrawTextOptions::NoPixelSnap |
                CanvasDrawTextOptions::Clip |
                CanvasDrawTextOptions::EnableColorFont;

            if ((value & ~validOptions) != CanvasDrawTextOptions::Default)
                ThrowHR(E_INVALIDARG);

            m_drawTextOptions = value;
        });
}

//
// CanvasTextFormat.LineSpacingMethod
//

#if WINVER > _WIN32_WINNT_WINBLUE

IFACEMETHODIMP CanvasTextFormat::get_LineSpacingMode(CanvasLineSpacingMode* value)
{
    bool allowUniformToBePreserved = m_lineSpacingMode == CanvasLineSpacingMode::Uniform;

    return PropertyGet(
        value,
        m_lineSpacingMode,
        [=](IDWriteTextFormat* textFormat) { return DWriteLineSpacing(textFormat).GetAdjustedLineSpacingMode(allowUniformToBePreserved); });
}


IFACEMETHODIMP CanvasTextFormat::put_LineSpacingMode(CanvasLineSpacingMode value)
{
    return PropertyPut(
        value, 
        &m_lineSpacingMode,
        ThrowIfInvalid<CanvasLineSpacingMode>,
        &CanvasTextFormat::RealizeLineSpacing);
}

#endif


static WinString GetTextFromLocalizedStrings(
    int32_t stringIndex,
    ComPtr<IDWriteLocalizedStrings> const& localizedStrings)
{
    WinStringBuilder stringBuilder;
    uint32_t attributeLength;
    ThrowIfFailed(localizedStrings->GetStringLength(stringIndex, &attributeLength));
    attributeLength++; // Account for null terminator

    auto buffer = stringBuilder.Allocate(attributeLength);
    ThrowIfFailed(localizedStrings->GetString(stringIndex, buffer, attributeLength));
    return stringBuilder.Get();
}


static bool TryGetLocalizedName(
    wchar_t const* locale,
    ComPtr<IDWriteLocalizedStrings> const& familyNames,
    WinString* nameIfFound)
{
    uint32_t index;
    BOOL found;
    ThrowIfFailed(familyNames->FindLocaleName(locale, &index, &found));
    if (found)
    {
        *nameIfFound = GetTextFromLocalizedStrings(index, familyNames);
    }
    return !!found;
}


static bool TryGetLocalizedNameUsingLocaleList(
    IVectorView<HSTRING>* localeList,
    ComPtr<IDWriteLocalizedStrings> const& familyNames,
    WinString* nameIfFound)
{
    if (!localeList) 
        return false;

    uint32_t localeListSize;
    ThrowIfFailed(localeList->get_Size(&localeListSize));

    for (uint32_t localeIndex = 0; localeIndex < localeListSize; ++localeIndex)
    {
        WinString locale;
        ThrowIfFailed(localeList->GetAt(localeIndex, locale.GetAddressOf()));

        if (TryGetLocalizedName(static_cast<wchar_t const*>(locale), familyNames, nameIfFound))
            return true;
    }

    return false;
}


static WinString GetFamilyName(
    ComPtr<IDWriteLocalizedStrings> const& familyNames,
    IVectorView<HSTRING>* localeList) // Optional
{
    //
    // This returns one of the following, by order of priority:
    // - passed-in list of locales by order of preference
    // - en-us
    // - whatever's available

    WinString result;

    if (TryGetLocalizedNameUsingLocaleList(localeList, familyNames, &result)) 
        return result;

    if (TryGetLocalizedName(L"en-us", familyNames, &result)) 
        return result;

    return GetTextFromLocalizedStrings(0, familyNames);
}


IFACEMETHODIMP CanvasTextFormatFactory::GetSystemFontFamilies(
    uint32_t* valueCount,
    HSTRING** valueElements)
{
    return GetSystemFontFamiliesFromLocaleList(nullptr, valueCount, valueElements);
}


IFACEMETHODIMP CanvasTextFormatFactory::GetSystemFontFamiliesFromLocaleList(
    IVectorView<HSTRING>* localeList,
    uint32_t* valueCount,
    HSTRING** valueElements)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(valueCount);
            CheckAndClearOutPointer(valueElements);
            
            auto factory = CustomFontManager::GetInstance()->GetSharedFactory();

            ComPtr<IDWriteFontCollection> systemFontCollection;
            ThrowIfFailed(factory->GetSystemFontCollection(&systemFontCollection));

            uint32_t familyCount = systemFontCollection->GetFontFamilyCount();

            ComArray<HSTRING> stringArray(familyCount);

            for (uint32_t i = 0; i < familyCount; ++i)
            {
                ComPtr<IDWriteFontFamily> fontFamily;
                ThrowIfFailed(systemFontCollection->GetFontFamily(i, &fontFamily));

                ComPtr<IDWriteLocalizedStrings> familyNames;
                ThrowIfFailed(fontFamily->GetFamilyNames(&familyNames));

                WinString familyName = GetFamilyName(familyNames, localeList);
                familyName.CopyTo(&stringArray[i]);
            }

            stringArray.Detach(valueCount, valueElements);
        });
}


ActivatableClassWithFactory(CanvasTextFormat, CanvasTextFormatFactory);

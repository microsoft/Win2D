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

#include "CanvasTextFormat.h"
#include "TextUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    //
    // Parameter validation functions
    //

    template<typename T>
    static void ThrowIfInvalid(T)
    {
        static_assert(false, "Specialization required");
    }

    template<>
    void ThrowIfInvalid(CanvasTextDirection value)
    {
        switch (value)
        {
        case CanvasTextDirection::TopToBottom:
        case CanvasTextDirection::BottomToTop:
        case CanvasTextDirection::LeftToRight:
        case CanvasTextDirection::RightToLeft:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    void ThrowIfInvalid(CanvasLineSpacingMethod value)
    {
        switch (value)
        {
        case CanvasLineSpacingMethod::Default:
        case CanvasLineSpacingMethod::Uniform:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

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

    template<>
    void ThrowIfInvalid(CanvasWordWrapping value)
    {
        switch (value)
        {
        case CanvasWordWrapping::Wrap:
        case CanvasWordWrapping::NoWrap:
        case CanvasWordWrapping::EmergencyBreak:
        case CanvasWordWrapping::WholeWord:
        case CanvasWordWrapping::Character:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    void ThrowIfInvalid(CanvasTextTrimmingGranularity value)
    {
        switch (value)
        {
        case CanvasTextTrimmingGranularity::None:
        case CanvasTextTrimmingGranularity::Character:
        case CanvasTextTrimmingGranularity::Word:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    void ThrowIfInvalid(ABI::Windows::UI::Text::ParagraphAlignment value)
    {
        switch (value)
        {
        case ABI::Windows::UI::Text::ParagraphAlignment_Undefined:
        case ABI::Windows::UI::Text::ParagraphAlignment_Left:
        case ABI::Windows::UI::Text::ParagraphAlignment_Center:
        case ABI::Windows::UI::Text::ParagraphAlignment_Right:
        case ABI::Windows::UI::Text::ParagraphAlignment_Justify:
            return;

        default:
            ThrowHR(E_INVALIDARG);
        }
    }

    template<>
    void ThrowIfInvalid(CanvasVerticalAlignment value)
    {
        switch (value)
        {
        case CanvasVerticalAlignment::Top:
        case CanvasVerticalAlignment::Bottom:
        case CanvasVerticalAlignment::Center:
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

    static void ThrowIfInvalidTrimmingDelimiter(HSTRING value)
    {
        // The delimiter must be a single code point and so cannot be more than
        // 2 UTF-16 code units long
        if (WindowsGetStringLen(value) > 2)
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

    static WinString GetFontFamilyName(IDWriteTextFormat* format)
    {
        WinStringBuilder stringBuilder;
        auto length = format->GetFontFamilyNameLength() + 1;
        auto buffer = stringBuilder.Allocate(length);
        ThrowIfFailed(format->GetFontFamilyName(buffer, length));
        return stringBuilder.Get();
    }

    static WinString GetLocaleName(IDWriteTextFormat* format)
    {
        WinStringBuilder stringBuilder;
        auto length = format->GetLocaleNameLength() + 1;
        auto buffer = stringBuilder.Allocate(length);
        ThrowIfFailed(format->GetLocaleName(buffer, length));
        return stringBuilder.Get();
    }


    //
    // CanvasTextFormatAdapter implementation
    //
    

    ComPtr<IDWriteFactory> CanvasTextFormatAdapter::CreateDWriteFactory(DWRITE_FACTORY_TYPE type)
    {
        ComPtr<IDWriteFactory> factory;
        ThrowIfFailed(DWriteCreateFactory(type, __uuidof(factory), &factory));
        return factory;
    }
    

    IStorageFileStatics* CanvasTextFormatAdapter::GetStorageFileStatics()
    {
        if (!m_storageFileStatics)
        {
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_Storage_StorageFile).Get(), 
                &m_storageFileStatics));
        }
        
        return m_storageFileStatics.Get();
    }


    //
    // Custom font loading
    //


    class CustomFontFileEnumerator 
        : public RuntimeClass<
            RuntimeClassFlags<ClassicCom>,
            IDWriteFontFileEnumerator>
        , private LifespanTracker<CustomFontFileEnumerator>

    {
        ComPtr<IDWriteFactory> m_factory;
        std::wstring m_filename;
        ComPtr<IDWriteFontFile> m_theFile;

    public:
        CustomFontFileEnumerator(IDWriteFactory* factory, void const* collectionKey, uint32_t collectionKeySize)
            : m_factory(factory)
            , m_filename(static_cast<wchar_t const*>(collectionKey), collectionKeySize / 2)
        {
        }

        IFACEMETHODIMP MoveNext(BOOL* hasCurrentFile) override
        {
            if (m_theFile)
            {
                *hasCurrentFile = FALSE;
            }
            else if (SUCCEEDED(m_factory->CreateFontFileReference(m_filename.c_str(), nullptr, &m_theFile)))
            {
                *hasCurrentFile = TRUE;
            }
            else
            {
                *hasCurrentFile = FALSE;
            }
            
            return S_OK;
        }

        IFACEMETHODIMP GetCurrentFontFile(IDWriteFontFile** fontFile) override
        {
            return m_theFile.CopyTo(fontFile);
        }
    };

    class CustomFontLoader 
        : public RuntimeClass<
            RuntimeClassFlags<ClassicCom>, 
            IDWriteFontCollectionLoader>
        , private LifespanTracker<CustomFontLoader>
    {
    public:
        IFACEMETHODIMP CreateEnumeratorFromKey(
            IDWriteFactory* factory,
            void const* collectionKey,
            uint32_t collectionKeySize,
            IDWriteFontFileEnumerator** fontFileEnumerator) override
        {
            return ExceptionBoundary(
                [=]
                {
                    auto enumerator = Make<CustomFontFileEnumerator>(factory, collectionKey, collectionKeySize);
                    CheckMakeResult(enumerator);
                    ThrowIfFailed(enumerator.CopyTo(fontFileEnumerator));
                });
        }
    };


    //
    // CanvasTextFormatManager implementation
    //
    
    CanvasTextFormatManager::CanvasTextFormatManager(std::shared_ptr<ICanvasTextFormatAdapter> adapter)
        : m_adapter(adapter)
    {
        ThrowIfFailed(GetActivationFactory(
            HStringReference(RuntimeClass_Windows_Foundation_Uri).Get(), 
            &m_uriFactory));
    }


    ComPtr<CanvasTextFormat> CanvasTextFormatManager::Create()
    {
        return Make<CanvasTextFormat>(shared_from_this());
    }


    ComPtr<CanvasTextFormat> CanvasTextFormatManager::Create(IDWriteTextFormat* format)
    {
        return Make<CanvasTextFormat>(shared_from_this(), format);
    }

    void CanvasTextFormatManager::ValidateUri(WinString const& uriString)
    {
        if (uriString == WinString())
            return;

        ComPtr<IUriRuntimeClass> uri;
        ThrowIfFailed(m_uriFactory->CreateWithRelativeUri(WinString(L"ms-appx://"), uriString, &uri));
        
        WinString schemeName;
        ThrowIfFailed(uri->get_SchemeName(schemeName.GetAddressOf()));

        if (!schemeName.Equals(HStringReference(L"ms-appx").Get()) &&
            !schemeName.Equals(HStringReference(L"ms-appdata").Get()))
        {
            ThrowHR(E_INVALIDARG, HStringReference(Strings::InvalidFontFamilyUriScheme).Get());
        }
    }

    WinString CanvasTextFormatManager::GetAbsolutePathFromUri(WinString const& uriString)
    {
        ComPtr<IUriRuntimeClass> uri;
        ThrowIfFailed(m_uriFactory->CreateWithRelativeUri(WinString(L"ms-appx://"), uriString, &uri));

        auto storageFileStatics = m_adapter->GetStorageFileStatics();
        ComPtr<IAsyncOperation<StorageFile*>> operation;

        HRESULT hr = storageFileStatics->GetFileFromApplicationUriAsync(uri.Get(), &operation);
        if (FAILED(hr))
        {
            ThrowHR(hr, HStringReference(Strings::InvalidFontFamilyUri).Get());
        }

        Event operationCompleted(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
        auto handler = Callback<AddFtmBase<IAsyncOperationCompletedHandler<StorageFile*>>::Type>(
            [&] (IAsyncOperation<StorageFile*>*, AsyncStatus)
            {
                SetEvent(operationCompleted.Get());
                return S_OK;
            });
        CheckMakeResult(handler);

        ThrowIfFailed(operation->put_Completed(handler.Get()));

        auto res = WaitForSingleObjectEx(operationCompleted.Get(), INFINITE, false);
        if (res != WAIT_OBJECT_0)
            ThrowHR(E_UNEXPECTED);

        ComPtr<IStorageFile> storageFile;
        ThrowIfFailed(operation->GetResults(&storageFile));

        WinString path;
        ThrowIfFailed(As<IStorageItem>(storageFile)->get_Path(path.GetAddressOf()));

        return path;
    }


    ComPtr<IDWriteFontCollection> CanvasTextFormatManager::GetFontCollectionFromUri(WinString const& uri)
    {
        //
        // No URI means no custom font collection - ie use the system font
        // collection.
        //
        if (uri == WinString())
        {
            return nullptr;
        }

        auto path = GetAbsolutePathFromUri(uri);

        auto pathBegin = begin(path);
        auto pathEnd = end(path);

        assert(pathBegin && pathEnd);

        void const* key = pathBegin;
        uint32_t keySize = static_cast<uint32_t>(std::distance(pathBegin, pathEnd) * sizeof(wchar_t));

        ComPtr<IDWriteFontCollection> collection;

        auto factory = GetIsolatedFactory();
        ThrowIfFailed(factory->CreateCustomFontCollection(m_customLoader.Get(), key, keySize, &collection));
        
        return collection;
    }


    ComPtr<IDWriteFactory> const& CanvasTextFormatManager::GetIsolatedFactory()
    {
        if (!m_isolatedFactory)
        {
            m_isolatedFactory = m_adapter->CreateDWriteFactory(DWRITE_FACTORY_TYPE_ISOLATED);
            m_customLoader = Make<CustomFontLoader>();
            ThrowIfFailed(m_isolatedFactory->RegisterFontCollectionLoader(m_customLoader.Get()));
        }
        
        return m_isolatedFactory;
    }



    //
    // CanvasTextFormatFactory implementation
    //


    CanvasTextFormatFactory::CanvasTextFormatFactory()
    {
    }


    std::shared_ptr<CanvasTextFormatManager> CanvasTextFormatFactory::CreateManager()
    {
        return std::make_shared<CanvasTextFormatManager>(std::make_shared<CanvasTextFormatAdapter>());
    }

    
    IFACEMETHODIMP CanvasTextFormatFactory::ActivateInstance(IInspectable** object)
    {
        return ExceptionBoundary(
            [&]
            {
                auto format = GetOrCreateManager()->Create();
                ThrowIfFailed(format.CopyTo(object));
            });
    }


    IFACEMETHODIMP CanvasTextFormatFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                auto format = GetOrCreateManager()->Create(As<IDWriteTextFormat>(resource).Get());
                ThrowIfFailed(format.CopyTo(wrapper));
            });
    }


    //
    // CanvasTextFormat implementation
    //


    CanvasTextFormat::CanvasTextFormat(std::shared_ptr<CanvasTextFormatManager> manager)
        : m_manager(manager)
        , m_closed(false)
        , m_flowDirection(CanvasTextDirection::TopToBottom)
        , m_fontFamilyName(L"Segoe UI")
        , m_fontSize(20.0f)
        , m_fontStretch(ABI::Windows::UI::Text::FontStretch_Normal)
        , m_fontStyle(ABI::Windows::UI::Text::FontStyle_Normal)
        , m_fontWeight(ToWindowsFontWeight(DWRITE_FONT_WEIGHT_NORMAL))
        , m_incrementalTabStop(-1.0f)          
        , m_lineSpacingMethod(CanvasLineSpacingMethod::Default)
        , m_lineSpacing(0.0f)
        , m_lineSpacingBaseline(0.0f)
        , m_verticalAlignment(CanvasVerticalAlignment::Top)
        , m_paragraphAlignment(ABI::Windows::UI::Text::ParagraphAlignment_Left)
        , m_readingDirection(CanvasTextDirection::LeftToRight)
        , m_trimmingGranularity(CanvasTextTrimmingGranularity::None)
        , m_trimmingDelimiterCount(0)
        , m_wordWrapping(CanvasWordWrapping::Wrap)
        , m_drawTextOptions(CanvasDrawTextOptions::Default)
    {
    }


    CanvasTextFormat::CanvasTextFormat(std::shared_ptr<CanvasTextFormatManager> manager, IDWriteTextFormat* format)
        : m_manager(manager)
        , m_closed(false)
        , m_format(format)
    {
        SetShadowPropertiesFromDWrite();
    }


    IFACEMETHODIMP CanvasTextFormat::Close()
    {
        m_closed = true;
        return S_OK;
    }


    void CanvasTextFormat::ThrowIfClosed()
    {
        if (m_closed)
            ThrowHR(RO_E_CLOSED);
    }


    IFACEMETHODIMP CanvasTextFormat::GetResource(REFIID iid, void** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);
                ThrowIfFailed(GetRealizedTextFormat().CopyTo(iid, value));
            });
    }


    static std::pair<WinString, WinString> GetUriAndFontFamily(WinString const& fontFamilyName)
    {
        auto beginIt = begin(fontFamilyName);
        auto endIt = end(fontFamilyName);

        auto hashPos = std::find(beginIt, endIt, L'#');

        if (hashPos == endIt)
            return std::make_pair(WinString(), fontFamilyName);
        else
            return std::make_pair(WinString(beginIt, hashPos), WinString(hashPos+1, endIt));
    }


    ComPtr<IDWriteTextFormat> CanvasTextFormat::GetRealizedTextFormat()
    {
        if (m_format)
            return m_format;

        ComPtr<IDWriteFactory2> factory;
        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(&factory),
            static_cast<IUnknown**>(&factory)));

        auto uriAndFontFamily = GetUriAndFontFamily(m_fontFamilyName);
        auto const& uri = uriAndFontFamily.first;
        auto const& fontFamily = uriAndFontFamily.second;

        ComPtr<IDWriteFontCollection> fontCollection = m_fontCollection;

        if (!fontCollection)
        {
            fontCollection = m_manager->GetFontCollectionFromUri(uri);
        }

        ThrowIfFailed(factory->CreateTextFormat(
            static_cast<const wchar_t*>(fontFamily),
            fontCollection.Get(),
            ToFontWeight(m_fontWeight),
            ToFontStyle(m_fontStyle),
            ToFontStretch(m_fontStretch),
            m_fontSize,
            static_cast<const wchar_t*>(m_localeName),
            &m_format));

        RealizeFlowDirection();
        RealizeIncrementalTabStop();
        RealizeLineSpacing();
        RealizeParagraphAlignment();
        RealizeReadingDirection();
        RealizeTextAlignment();
        RealizeTrimming();
        RealizeWordWrapping();

        return m_format;
    }


    CanvasDrawTextOptions CanvasTextFormat::GetDrawTextOptions()
    {
        return m_drawTextOptions;
    }


    void CanvasTextFormat::Unrealize()
    {
        //
        // We're about to throw away m_format, so we need to extract all the
        // values stored on it into our shadow copies.
        //
        if (m_format)
            SetShadowPropertiesFromDWrite();

        m_format.Reset();
    }


    void CanvasTextFormat::SetShadowPropertiesFromDWrite()
    {
        assert(m_format);

        ThrowIfFailed(m_format->GetFontCollection(&m_fontCollection));

        m_flowDirection      = ToCanvasTextDirection(m_format->GetFlowDirection());
        m_fontFamilyName     = GetFontFamilyName(m_format.Get());
        m_fontSize           = m_format->GetFontSize();
        m_fontStretch        = ToWindowsFontStretch(m_format->GetFontStretch());
        m_fontStyle          = ToWindowsFontStyle(m_format->GetFontStyle());
        m_fontWeight         = ToWindowsFontWeight(m_format->GetFontWeight());
        m_incrementalTabStop = m_format->GetIncrementalTabStop();
        m_localeName         = GetLocaleName(m_format.Get());
        m_verticalAlignment  = ToCanvasVerticalAlignment(m_format->GetParagraphAlignment());
        m_paragraphAlignment = ToWindowsParagraphAlignment(m_format->GetTextAlignment());
        m_readingDirection   = ToCanvasTextDirection(m_format->GetReadingDirection());
        m_wordWrapping       = ToCanvasWordWrapping(m_format->GetWordWrapping());
        m_drawTextOptions    = CanvasDrawTextOptions::Default;

        DWRITE_LINE_SPACING_METHOD method{};
        ThrowIfFailed(m_format->GetLineSpacing(&method, &m_lineSpacing, &m_lineSpacingBaseline));
        m_lineSpacingMethod = ToCanvasLineSpacingMethod(method);

        DWRITE_TRIMMING trimmingOptions{};
        ComPtr<IDWriteInlineObject> inlineObject;
        ThrowIfFailed(m_format->GetTrimming(
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
                CheckInPointer(value);
                ThrowIfClosed();
                
                if (m_format)
                    SetFrom(value, realizedGetter());
                else
                    SetFrom(value, shadowValue);
            });
    }

    
    template<typename T, typename TT, typename FNV>
    HRESULT __declspec(nothrow) CanvasTextFormat::PropertyPut(T value, TT* dest, FNV&& validator, void(CanvasTextFormat::*realizer)())
    {
        return ExceptionBoundary(
            [&]
            {
                validator(value);

                ThrowIfClosed();

                if (IsSame(dest, value))
                {
                    // Don't do anything if the value we're setting is the same
                    // as the current value.
                    return;
                }

                if (!realizer)
                {
                    // If there's no realizer set then we're going to have to
                    // throw away m_format (ready to be recreated the next time
                    // it is needed)
                    Unrealize();
                }

                // Set the shadow value
                SetFrom(dest, value);

                // Realize the value on the dwrite object, if we can
                if (m_format && realizer)
                    (this->*realizer)();
            });
    }

    template<typename T, typename TT>
    HRESULT __declspec(nothrow) CanvasTextFormat::PropertyPut(T value, TT* dest, void(CanvasTextFormat::*realizer)())
    {
        return PropertyPut(value, dest, [](T){}, realizer);
    }

    //
    // CanvasTextFormat.FlowDirection
    //
    
    IFACEMETHODIMP CanvasTextFormat::get_FlowDirection(CanvasTextDirection* value)
    {
        return PropertyGet(
            value,
            m_flowDirection,
            [&] { return ToCanvasTextDirection(m_format->GetFlowDirection()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_FlowDirection(CanvasTextDirection value)
    {
        return PropertyPut(
            value, 
            &m_flowDirection, 
            ThrowIfInvalid<CanvasTextDirection>,
            &CanvasTextFormat::RealizeFlowDirection);
    }


    void CanvasTextFormat::RealizeFlowDirection()
    {
        ThrowIfFailed(m_format->SetFlowDirection(ToFlowDirection(m_flowDirection)));        
    }

    //
    // CanvasTextFormat.FontFamily
    //

    IFACEMETHODIMP CanvasTextFormat::get_FontFamily(HSTRING* value)
    {
        return PropertyGet(
            value,
            m_fontFamilyName,
            [&]
            {
                //
                // If this were any other simple property, this would get the
                // font family name from the realized IDWriteTextFormat
                // (m_format).
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
                ThrowIfClosed();
                
                if (IsSame(&m_fontFamilyName, value))
                    return;

                auto uriAndFontFamily = GetUriAndFontFamily(WinString(value));
                auto const& uri = uriAndFontFamily.first;
                m_manager->ValidateUri(uri);
                
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
            [&] { return m_format->GetFontSize(); });
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
            [&] { return ToWindowsFontStretch(m_format->GetFontStretch()); });
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
            [&] { return ToWindowsFontStyle(m_format->GetFontStyle()); });
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
            [&] { return ToWindowsFontWeight(m_format->GetFontWeight()); });
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
            [&] { return m_format->GetIncrementalTabStop(); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_IncrementalTabStop(float value)
    {
        return PropertyPut(
            value, 
            &m_incrementalTabStop, 
            ThrowIfNegativeOrNan,
            &CanvasTextFormat::RealizeIncrementalTabStop);
    }


    void CanvasTextFormat::RealizeIncrementalTabStop()
    {
        // Negative value indicates that it hasn't been set yet, so we want to
        // use dwrite's default.
        if (m_incrementalTabStop >= 0.0f)
            ThrowIfFailed(m_format->SetIncrementalTabStop(m_incrementalTabStop));
    }

    //
    // CanvasTextFormat.LineSpacingMethod
    //

    IFACEMETHODIMP CanvasTextFormat::get_LineSpacingMethod(CanvasLineSpacingMethod* value)
    {
        return PropertyGet(
            value,
            m_lineSpacingMethod,
            [&] { return ToCanvasLineSpacingMethod(DWriteLineSpacing(m_format.Get()).Method); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_LineSpacingMethod(CanvasLineSpacingMethod value)
    {
        return PropertyPut(
            value, 
            &m_lineSpacingMethod, 
            ThrowIfInvalid<CanvasLineSpacingMethod>,
            &CanvasTextFormat::RealizeLineSpacing);
    }

    //
    // CanvasTextFormat.LineSpacing
    //

    IFACEMETHODIMP CanvasTextFormat::get_LineSpacing(float* value)
    {
        return PropertyGet(
            value,
            m_lineSpacing,
            [&] { return DWriteLineSpacing(m_format.Get()).Spacing; });
    }


    IFACEMETHODIMP CanvasTextFormat::put_LineSpacing(float value)
    {
        return PropertyPut(
            value, 
            &m_lineSpacing, 
            ThrowIfNegativeOrNan,
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
            [&] { return DWriteLineSpacing(m_format.Get()).Baseline; });
    }


    IFACEMETHODIMP CanvasTextFormat::put_LineSpacingBaseline(float value)
    {
        return PropertyPut(
            value, 
            &m_lineSpacingBaseline, 
            ThrowIfNan,
            &CanvasTextFormat::RealizeLineSpacing);
    }


    void CanvasTextFormat::RealizeLineSpacing()
    {
        ThrowIfFailed(m_format->SetLineSpacing(
            ToLineSpacingMethod(m_lineSpacingMethod),
            m_lineSpacing,
            m_lineSpacingBaseline));
    }


    //
    // CanvasTextFormat.LocaleName
    //

    IFACEMETHODIMP CanvasTextFormat::get_LocaleName(HSTRING* value)
    {
        return PropertyGet(
            value,
            m_localeName,
            [&]
            {
                return GetLocaleName(m_format.Get());
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
            [&] { return ToCanvasVerticalAlignment(m_format->GetParagraphAlignment()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_VerticalAlignment(CanvasVerticalAlignment value)
    {
        return PropertyPut(
            value, 
            &m_verticalAlignment,
            ThrowIfInvalid<CanvasVerticalAlignment>,
            &CanvasTextFormat::RealizeParagraphAlignment);
    }


    void CanvasTextFormat::RealizeParagraphAlignment()
    {
        ThrowIfFailed(m_format->SetParagraphAlignment(ToParagraphAlignment(m_verticalAlignment)));
    }

    //
    // CanvasTextFormat.ReadingDirection
    //

    IFACEMETHODIMP CanvasTextFormat::get_ReadingDirection(CanvasTextDirection* value)
    {
        return PropertyGet(
            value,
            m_readingDirection,
            [&] { return ToCanvasTextDirection(m_format->GetReadingDirection()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_ReadingDirection(CanvasTextDirection value)
    {
        return PropertyPut(
            value, 
            &m_readingDirection,
            ThrowIfInvalid<CanvasTextDirection>,
            &CanvasTextFormat::RealizeReadingDirection);
    }


    void CanvasTextFormat::RealizeReadingDirection()
    {
        ThrowIfFailed(m_format->SetReadingDirection(ToReadingDirection(m_readingDirection)));
    }

    //
    // CanvasTextFormat.ParagraphAlignment
    //

    IFACEMETHODIMP CanvasTextFormat::get_ParagraphAlignment(ABI::Windows::UI::Text::ParagraphAlignment* value)
    {
        // Canvas calls is "ParagraphAlignment", but DWrite calls it "TextAlignment"
        return PropertyGet(
            value,
            m_paragraphAlignment,
            [&] { return ToWindowsParagraphAlignment(m_format->GetTextAlignment()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_ParagraphAlignment(ABI::Windows::UI::Text::ParagraphAlignment value)
    {
        return PropertyPut(
            value, 
            &m_paragraphAlignment,
            ThrowIfInvalid<ABI::Windows::UI::Text::ParagraphAlignment>,
            &CanvasTextFormat::RealizeTextAlignment);
    }

    void CanvasTextFormat::RealizeTextAlignment()
    {
        ThrowIfFailed(m_format->SetTextAlignment(ToTextAlignment(m_paragraphAlignment)));        
    }

    //
    // CanvasTextFormat.TrimmingGranulatity
    //

    IFACEMETHODIMP CanvasTextFormat::get_TrimmingGranularity(CanvasTextTrimmingGranularity* value)
    {
        return PropertyGet(
            value,
            m_trimmingGranularity,
            [&] { return ToCanvasTextTrimmingGranularity(DWriteTrimming(m_format.Get()).Options.granularity); });
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
            [&] { return ToCanvasTrimmingDelimiter(DWriteTrimming(m_format.Get()).Options.delimiter); });
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
            [&] { return static_cast<int32_t>(DWriteTrimming(m_format.Get()).Options.delimiterCount); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_TrimmingDelimiterCount(int32_t value)
    {
        return PropertyPut(
            value, 
            &m_trimmingDelimiterCount,
            ThrowIfNegative<int32_t>,
            &CanvasTextFormat::RealizeTrimming);
    }


    void CanvasTextFormat::RealizeTrimming()
    {
        DWRITE_TRIMMING trimmingOptions{};
        trimmingOptions.granularity = ToTrimmingGranularity(m_trimmingGranularity);
        trimmingOptions.delimiter = ToTrimmingDelimiter(m_trimmingDelimiter);
        trimmingOptions.delimiterCount = m_trimmingDelimiterCount;
            
        ThrowIfFailed(m_format->SetTrimming(
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
            [&] { return ToCanvasWordWrapping(m_format->GetWordWrapping()); });
    }


    IFACEMETHODIMP CanvasTextFormat::put_WordWrapping(CanvasWordWrapping value)
    {
        return PropertyPut(
            value, 
            &m_wordWrapping,
            ThrowIfInvalid<CanvasWordWrapping>,
            &CanvasTextFormat::RealizeWordWrapping);
    }


    void CanvasTextFormat::RealizeWordWrapping()
    {
        ThrowIfFailed(m_format->SetWordWrapping(ToWordWrapping(m_wordWrapping)));        
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


    ActivatableClassWithFactory(CanvasTextFormat, CanvasTextFormatFactory);
}}}}

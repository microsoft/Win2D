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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;

    //
    // CanvasTextFormatAdapter
    //

    class ICanvasTextFormatAdapter
    {
    public:
        virtual ~ICanvasTextFormatAdapter() = default;

        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) = 0;
        virtual IStorageFileStatics* GetStorageFileStatics() = 0;
    };

    class CanvasTextFormatAdapter : public ICanvasTextFormatAdapter
    {
        ComPtr<IStorageFileStatics> m_storageFileStatics;

    public:
        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) override;
        virtual IStorageFileStatics* GetStorageFileStatics() override;
    };

    class CanvasTextFormat;

    //
    // CanvasTextFormatManager
    //
    // This follows the pattern of CanvasTextFormatFactory /
    // PerApplicationManager / CanvasTextFormatManager since it plays nicely
    // with our established leak tracking code.  
    //
    // Note that CanvasTextFormatManager does not derive from ResourceManager,
    // since CanvasTextFormat is not a wrapped resource and doesn't support
    // idempotent interop.
    //

    class CanvasTextFormatManager 
        : public std::enable_shared_from_this<CanvasTextFormatManager>
        , public StoredInPropertyMap
        , private LifespanTracker<CanvasTextFormatManager>
    {
        std::shared_ptr<ICanvasTextFormatAdapter> m_adapter;
        ComPtr<IDWriteFactory> m_isolatedFactory;
        ComPtr<IDWriteFontCollectionLoader> m_customLoader;
        ComPtr<IUriRuntimeClassFactory> m_uriFactory;

    public:
        CanvasTextFormatManager(std::shared_ptr<ICanvasTextFormatAdapter> adapter);

        ComPtr<CanvasTextFormat> Create();
        ComPtr<CanvasTextFormat> Create(IDWriteTextFormat* format);

        void ValidateUri(WinString const& uri);
        ComPtr<IDWriteFontCollection> GetFontCollectionFromUri(WinString const& uri);        

    private:
        ComPtr<IDWriteFactory> const& GetIsolatedFactory();        

        WinString GetAbsolutePathFromUri(WinString const& uri);
    };


    //
    // CanvasTextFormatFactory
    //

    class CanvasTextFormatFactory 
        : public ActivationFactory<
            CloakedIid<ICanvasFactoryNative>>,
          public PerApplicationManager<CanvasTextFormatFactory, CanvasTextFormatManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasTextFormat, BaseTrust);

    public:
        CanvasTextFormatFactory();

        //
        // ActivationFactory
        //

        IFACEMETHOD(ActivateInstance)(IInspectable** obj) override;

        //
        // ICanvasFactoryNative
        //

        IFACEMETHOD(GetOrCreate)(
            IUnknown* resource,
            IInspectable** wrapper) override;

        //
        // Used by PerApplicationManager
        //
        static std::shared_ptr<CanvasTextFormatManager> CreateManager();
    };



    //
    // CanvasTextFormat
    //

    [uuid(E295AC1E-B763-49D4-9AE3-6E75D0C429AA)]
    class ICanvasTextFormatInternal : public IUnknown
    {
    public:
        virtual ComPtr<IDWriteTextFormat> GetRealizedTextFormat() = 0;
        virtual CanvasDrawTextOptions GetDrawTextOptions() = 0;
    };


    //
    // CanvasTextFormat provides a IDWriteTextFormat object.  Since some members
    // of IDWriteTextFormat are immutable (eg FontFamily), CanvasTextFormat
    // keeps track of shadow copies of properties and recreates ("realizes") an
    // IDWriteTextFormat as necessary.
    //
    class CanvasTextFormat : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasTextFormat,
        ABI::Windows::Foundation::IClosable,
        CloakedIid<ICanvasTextFormatInternal>,
        CloakedIid<ICanvasResourceWrapperNative>>,
        private LifespanTracker<CanvasTextFormat>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasTextFormat, BaseTrust);

        std::shared_ptr<CanvasTextFormatManager> m_manager;
        
        //
        // Has Close() been called?  It is tempting to use a null m_format to
        // indicated closed, but we need to be able to tell the difference
        // between not having created m_format yet and being closed.
        //
        bool m_closed;

        //
        // Shadow properties.  These values are used to recreate m_format when
        // it is required.
        //
        ComPtr<IDWriteFontCollection> m_fontCollection;
        CanvasTextDirection m_flowDirection;
        WinString m_fontFamilyName;
        float m_fontSize;
        ABI::Windows::UI::Text::FontStretch m_fontStretch;
        ABI::Windows::UI::Text::FontStyle m_fontStyle;
        ABI::Windows::UI::Text::FontWeight m_fontWeight;
        float m_incrementalTabStop;
        CanvasLineSpacingMethod m_lineSpacingMethod;
        float m_lineSpacing;
        float m_lineSpacingBaseline;
        WinString m_localeName;
        CanvasVerticalAlignment m_verticalAlignment;
        CanvasTextDirection m_readingDirection;
        ABI::Windows::UI::Text::ParagraphAlignment m_paragraphAlignment;
        CanvasTextTrimmingGranularity m_trimmingGranularity;
        WinString m_trimmingDelimiter;
        int32_t m_trimmingDelimiterCount;
        CanvasWordWrapping m_wordWrapping;

        //
        // Draw text options are not part of m_format, but are stored in
        // CanvasTextFormat.
        //
        CanvasDrawTextOptions m_drawTextOptions;

        //
        // The IDWriteTextFormat object itself.  This is created on demand from
        // the shadow properties.
        //
        ComPtr<IDWriteTextFormat> m_format;

    public:
        CanvasTextFormat(std::shared_ptr<CanvasTextFormatManager> manager);
        CanvasTextFormat(std::shared_ptr<CanvasTextFormatManager> manager, IDWriteTextFormat* format);

        //
        // ICanvasTextFormat
        //
#define PROPERTY(NAME, TYPE)                            \
        IFACEMETHOD(get_##NAME)(TYPE* value) override;   \
        IFACEMETHOD(put_##NAME)(TYPE value) override

        PROPERTY(FlowDirection,          CanvasTextDirection);
        PROPERTY(FontFamily,             HSTRING);
        PROPERTY(FontSize,               float);
        PROPERTY(FontStretch,            ABI::Windows::UI::Text::FontStretch);
        PROPERTY(FontStyle,              ABI::Windows::UI::Text::FontStyle);
        PROPERTY(FontWeight,             ABI::Windows::UI::Text::FontWeight);
        PROPERTY(IncrementalTabStop,     float);
        PROPERTY(LineSpacingMethod,      CanvasLineSpacingMethod);
        PROPERTY(LineSpacing,            float);
        PROPERTY(LineSpacingBaseline,    float);
        PROPERTY(LocaleName,             HSTRING);
        PROPERTY(VerticalAlignment,      CanvasVerticalAlignment);
        PROPERTY(ReadingDirection,       CanvasTextDirection);
        PROPERTY(ParagraphAlignment,     ABI::Windows::UI::Text::ParagraphAlignment);
        PROPERTY(TrimmingGranularity,    CanvasTextTrimmingGranularity);
        PROPERTY(TrimmingDelimiter,      HSTRING);
        PROPERTY(TrimmingDelimiterCount, int32_t);
        PROPERTY(WordWrapping,           CanvasWordWrapping);
        PROPERTY(Options,                CanvasDrawTextOptions);

#undef PROPERTY

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        //
        // ICanvasTextFormatInternal
        //

        virtual ComPtr<IDWriteTextFormat> GetRealizedTextFormat() override;
        virtual CanvasDrawTextOptions GetDrawTextOptions() override;

        //
        // ICanvasResourceWrapperNative
        //
        
        IFACEMETHOD(GetResource)(REFIID iid, void** resource) override;


    private:
        void ThrowIfClosed();

        template<typename T, typename ST, typename FN>
        HRESULT __declspec(nothrow) PropertyGet(T* value, ST const& shadowValue, FN realizedGetter);

        template<typename T, typename TT, typename FNV>
        HRESULT __declspec(nothrow) PropertyPut(T value, TT* dest, FNV&& validator, void(CanvasTextFormat::*realizer)() = nullptr);
        
        template<typename T, typename TT>
        HRESULT __declspec(nothrow) PropertyPut(T value, TT* dest, void(CanvasTextFormat::*realizer)() = nullptr);

        void SetShadowPropertiesFromDWrite();

        void Unrealize();
        void RealizeFlowDirection();
        void RealizeIncrementalTabStop();
        void RealizeLineSpacing();
        void RealizeParagraphAlignment();
        void RealizeReadingDirection();
        void RealizeTextAlignment();
        void RealizeTrimming();
        void RealizeWordWrapping();
    };


    // Helper for getting line spacing from DWrite   
    struct DWriteLineSpacing
    {
        DWriteLineSpacing(IDWriteTextFormat* format)
        {
            ThrowIfFailed(format->GetLineSpacing(&Method, &Spacing, &Baseline));
        }

        DWRITE_LINE_SPACING_METHOD Method;
        float Spacing;
        float Baseline;
    };


    // Helper for getting trimming from DWrite
    struct DWriteTrimming
    {
        DWriteTrimming(IDWriteTextFormat* format)
        {
            ThrowIfFailed(format->GetTrimming(
                &Options,
                &Sign));
        }

        DWRITE_TRIMMING Options;
        ComPtr<IDWriteInlineObject> Sign;
    };

}}}}

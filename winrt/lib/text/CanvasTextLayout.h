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

    class ICanvasTextLayoutAdapter
    {
    public:
        virtual ~ICanvasTextLayoutAdapter() = default;

        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) = 0;
    };

    class CanvasTextLayoutAdapter : public ICanvasTextLayoutAdapter
    {
    public:
        virtual ComPtr<IDWriteFactory> CreateDWriteFactory(DWRITE_FACTORY_TYPE type) override;
    };

    class CanvasTextLayout;
    class CanvasTextLayoutManager;

    struct CanvasTextLayoutTraits
    {
        typedef IDWriteTextLayout2 resource_t;
        typedef CanvasTextLayout wrapper_t;
        typedef ICanvasTextLayout wrapper_interface_t;
        typedef CanvasTextLayoutManager manager_t;
    };

    //
    // CanvasTextLayoutFactory
    //

    class CanvasTextLayoutFactory
        : public ActivationFactory<
        ICanvasTextLayoutFactory,
        CloakedIid<ICanvasFactoryNative>> ,
        public PerApplicationManager<CanvasTextLayoutFactory, CanvasTextLayoutManager>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_CanvasTextLayout, BaseTrust);

    public:
        CanvasTextLayoutFactory();

        IFACEMETHOD(Create)(
            HSTRING textString,
            ICanvasTextFormat* textFormat,
            float maximumLayoutWidth,
            float maximumLayoutHeight,
            ICanvasTextLayout** textLayout);

        //
        // ICanvasFactoryNative
        //

        IFACEMETHOD(GetOrCreate)(
            IUnknown* resource,
            IInspectable** wrapper) override;

        //
        // Used by PerApplicationManager
        //
        static std::shared_ptr<CanvasTextLayoutManager> CreateManager();
    };

    class CanvasTextLayout : RESOURCE_WRAPPER_RUNTIME_CLASS(
        CanvasTextLayoutTraits,
        IClosable)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasTextLayout, BaseTrust);

        ClosablePtr<ICanvasTextFormat> m_format;

    public:
        CanvasTextLayout(std::shared_ptr<CanvasTextLayoutManager> manager, IDWriteTextLayout2* layout);

        IFACEMETHOD(GetFormatChangePositions)(
            uint32_t* positionCount,
            int32_t** positions) override;

        IFACEMETHOD(get_FlowDirection)(
            CanvasTextDirection* value) override;

        IFACEMETHOD(put_FlowDirection)(
            CanvasTextDirection value) override;

        IFACEMETHOD(get_DefaultFontFamily)(
            HSTRING* value) override;

        IFACEMETHOD(get_DefaultFontSize)(
            float* value) override;

        IFACEMETHOD(get_DefaultFontStretch)(
            ABI::Windows::UI::Text::FontStretch* value) override;

        IFACEMETHOD(get_DefaultFontStyle)(
            ABI::Windows::UI::Text::FontStyle* value) override;

        IFACEMETHOD(get_DefaultFontWeight)(
            ABI::Windows::UI::Text::FontWeight* value) override;

        IFACEMETHOD(get_IncrementalTabStop)(
            float* value) override;

        IFACEMETHOD(put_IncrementalTabStop)(
            float value) override;

        IFACEMETHOD(get_LineSpacingMethod)(
            CanvasLineSpacingMethod* value) override;

        IFACEMETHOD(put_LineSpacingMethod)(
            CanvasLineSpacingMethod value) override;

        IFACEMETHOD(get_LineSpacing)(
            float* value) override;

        IFACEMETHOD(put_LineSpacing)(
            float value) override;

        IFACEMETHOD(get_LineSpacingBaseline)(
            float* value) override;

        IFACEMETHOD(put_LineSpacingBaseline)(
            float value) override;

        IFACEMETHOD(get_DefaultLocaleName)(
            HSTRING* value) override;

        IFACEMETHOD(get_VerticalAlignment)(
            CanvasVerticalAlignment* value) override;

        IFACEMETHOD(put_VerticalAlignment)(
            CanvasVerticalAlignment value) override;

        IFACEMETHOD(get_ReadingDirection)(
            CanvasTextDirection* value) override;

        IFACEMETHOD(put_ReadingDirection)(
            CanvasTextDirection value) override;

        IFACEMETHOD(get_ParagraphAlignment)(
            ABI::Windows::UI::Text::ParagraphAlignment* value) override;

        IFACEMETHOD(put_ParagraphAlignment)(
            ABI::Windows::UI::Text::ParagraphAlignment value) override;

        IFACEMETHOD(get_TrimmingGranularity)(
            CanvasTextTrimmingGranularity* value) override;

        IFACEMETHOD(put_TrimmingGranularity)(
            CanvasTextTrimmingGranularity value) override;

        IFACEMETHOD(get_TrimmingDelimiter)(
            HSTRING* value) override;

        IFACEMETHOD(put_TrimmingDelimiter)(
            HSTRING value) override;

        IFACEMETHOD(get_TrimmingDelimiterCount)(
            int32_t* value) override;

        IFACEMETHOD(put_TrimmingDelimiterCount)(
            int32_t value) override;

        IFACEMETHOD(get_WordWrapping)(
            CanvasWordWrapping* value) override;

        IFACEMETHOD(put_WordWrapping)(
            CanvasWordWrapping value) override;

        IFACEMETHOD(get_Options)(
            CanvasDrawTextOptions* value) override;

        IFACEMETHOD(put_Options)(
            CanvasDrawTextOptions value) override;

        IFACEMETHOD(get_MaximumLayoutSize)(
            ABI::Windows::Foundation::Size* value) override;

        IFACEMETHOD(put_MaximumLayoutSize)(
            ABI::Windows::Foundation::Size value) override;

        IFACEMETHOD(GetMinimumLayoutWidth)(
            float* value) override;

        IFACEMETHOD(GetFontFamily)(
            int32_t positionInText,
            HSTRING *fontFamily) override;

        IFACEMETHOD(GetFontSize)(
            int32_t positionInText,
            float *fontSize) override;

        IFACEMETHOD(GetFontStretch)(
            int32_t positionInText,
            ABI::Windows::UI::Text::FontStretch *fontStretch) override;

        IFACEMETHOD(GetFontStyle)(
            int32_t positionInText,
            ABI::Windows::UI::Text::FontStyle *fontStyle) override;

        IFACEMETHOD(GetFontWeight)(
            int32_t positionInText,
            ABI::Windows::UI::Text::FontWeight *fontWeight) override;

        IFACEMETHOD(GetLocaleName)(
            int32_t positionInText,
            HSTRING *localeName) override;

        IFACEMETHOD(GetStrikethrough)(
            int32_t positionInText,
            boolean *hasStrikethrough) override;

        IFACEMETHOD(GetUnderline)(
            int32_t positionInText,
            boolean *hasUnderline) override;

        IFACEMETHOD(SetFontFamily)(
            int32_t positionInText,
            int32_t characterCount,
            HSTRING fontFamily) override;

        IFACEMETHOD(SetFontSize)(
            int32_t positionInText,
            int32_t characterCount,
            float fontSize) override;

        IFACEMETHOD(SetFontStretch)(
            int32_t positionInText,
            int32_t characterCount,
            ABI::Windows::UI::Text::FontStretch fontStretch) override;

        IFACEMETHOD(SetFontStyle)(
            int32_t positionInText,
            int32_t characterCount,
            ABI::Windows::UI::Text::FontStyle fontStyle) override;

        IFACEMETHOD(SetFontWeight)(
            int32_t positionInText,
            int32_t characterCount,
            ABI::Windows::UI::Text::FontWeight fontWeight) override;

        IFACEMETHOD(SetLocaleName)(
            int32_t positionInText,
            int32_t characterCount,
            HSTRING name) override;

        IFACEMETHOD(SetStrikethrough)(
            int32_t positionInText,
            int32_t characterCount,
            boolean hasStrikethrough) override;

        IFACEMETHOD(SetUnderline)(
            int32_t positionInText,
            int32_t characterCount,
            boolean hasUnderline) override;

        IFACEMETHOD(GetPairKerning)(
            int32_t positionInText,
            boolean *hasPairKerning) override;

        IFACEMETHOD(SetPairKerning)(
            int32_t positionInText,
            int32_t characterCount,
            boolean hasPairKerning) override;

        IFACEMETHOD(GetLeadingCharacterSpacing)(
            int32_t positionInText,
            float *leadingSpacing) override;

        IFACEMETHOD(GetTrailingCharacterSpacing)(
            int32_t positionInText,
            float *trailingSpacing) override;

        IFACEMETHOD(GetMinimumCharacterAdvanceWidth)(
            int32_t positionInText,
            float *minimumAdvanceWidth) override;

        IFACEMETHOD(SetCharacterSpacing)(
            float leadingSpacing,
            float trailingSpacing,
            float minimumAdvanceWidth,
            int32_t positionInText,
            int32_t characterCount) override;

        IFACEMETHOD(get_VerticalGlyphOrientation)(
            CanvasVerticalGlyphOrientation* value) override;

        IFACEMETHOD(put_VerticalGlyphOrientation)(
            CanvasVerticalGlyphOrientation value) override;

        IFACEMETHOD(get_OpticalAlignment)(
            CanvasOpticalAlignment* value) override;

        IFACEMETHOD(put_OpticalAlignment)(
            CanvasOpticalAlignment value) override;

        IFACEMETHOD(get_LastLineWrapping)(
            boolean* value) override;

        IFACEMETHOD(put_LastLineWrapping)(
            boolean value) override;

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;
    };

    class CanvasTextLayoutManager : public ResourceManager<CanvasTextLayoutTraits>
    {
        std::shared_ptr<ICanvasTextLayoutAdapter> m_adapter;
        ComPtr<IDWriteFactory> m_sharedFactory;

    public:
        CanvasTextLayoutManager(std::shared_ptr<ICanvasTextLayoutAdapter> adapter);

        virtual ~CanvasTextLayoutManager() = default;

        ComPtr<CanvasTextLayout> CreateNew(
            HSTRING textString,
            ICanvasTextFormat* textFormat,
            float maximumLayoutWidth,
            float maximumLayoutHeight);

        virtual ComPtr<CanvasTextLayout> CreateWrapper(
            IDWriteTextLayout2* resource);

    private:
        ComPtr<IDWriteFactory> const& GetSharedFactory();
    };
}}}}

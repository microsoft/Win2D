// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "CustomFontManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    using namespace ::Microsoft::WRL;

    class CanvasTextLayout : RESOURCE_WRAPPER_RUNTIME_CLASS(
        IDWriteTextLayout2,
        CanvasTextLayout,
        ICanvasTextLayout,
        CloakedIid<ICanvasResourceWrapperWithDevice>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextLayout, BaseTrust);

        CanvasDrawTextOptions m_drawTextOptions;

        ClosablePtr<ICanvasDevice> m_device;

        std::shared_ptr<CustomFontManager> m_customFontManager;

        CanvasLineSpacingMode m_lineSpacingMode;

    public:
        static ComPtr<CanvasTextLayout> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            HSTRING textString,
            ICanvasTextFormat* textFormat,
            float requestedWidth,
            float requestedHeight);

        CanvasTextLayout(
            ICanvasDevice* device,
            IDWriteTextLayout2* layout);

        IFACEMETHOD(GetFormatChangeIndices)(
            uint32_t* positionCount,
            int32_t** positions) override;

        IFACEMETHOD(get_Direction)(
            CanvasTextDirection* value) override;

        IFACEMETHOD(put_Direction)(
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

        IFACEMETHOD(get_LineSpacing)(
            float* value) override;

        IFACEMETHOD(put_LineSpacing)(
            float value) override;

        IFACEMETHOD(get_LineSpacingBaseline)(
            float* value) override;

        IFACEMETHOD(put_LineSpacingBaseline)(
            float value) override;

#if WINVER > _WIN32_WINNT_WINBLUE
        IFACEMETHOD(get_LineSpacingMode)(
            CanvasLineSpacingMode* value) override;

        IFACEMETHOD(put_LineSpacingMode)(
            CanvasLineSpacingMode value) override;
#endif

        IFACEMETHOD(get_DefaultLocaleName)(
            HSTRING* value) override;

        IFACEMETHOD(get_VerticalAlignment)(
            CanvasVerticalAlignment* value) override;

        IFACEMETHOD(put_VerticalAlignment)(
            CanvasVerticalAlignment value) override;

        IFACEMETHOD(get_HorizontalAlignment)(
            CanvasHorizontalAlignment* value) override;

        IFACEMETHOD(put_HorizontalAlignment)(
            CanvasHorizontalAlignment value) override;

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

        IFACEMETHOD(get_RequestedSize)(
            ABI::Windows::Foundation::Size* value) override;

        IFACEMETHOD(put_RequestedSize)(
            ABI::Windows::Foundation::Size value) override;

        IFACEMETHOD(GetMinimumLineLength)(
            float* value) override;

        IFACEMETHOD(GetBrush)(
            int32_t characterIndex,
            ICanvasBrush** brush) override;

        IFACEMETHOD(GetFontFamily)(
            int32_t characterIndex,
            HSTRING *fontFamily) override;

        IFACEMETHOD(GetFontSize)(
            int32_t characterIndex,
            float *fontSize) override;

        IFACEMETHOD(GetFontStretch)(
            int32_t characterIndex,
            ABI::Windows::UI::Text::FontStretch *fontStretch) override;

        IFACEMETHOD(GetFontStyle)(
            int32_t characterIndex,
            ABI::Windows::UI::Text::FontStyle *fontStyle) override;

        IFACEMETHOD(GetFontWeight)(
            int32_t characterIndex,
            ABI::Windows::UI::Text::FontWeight *fontWeight) override;

        IFACEMETHOD(GetLocaleName)(
            int32_t characterIndex,
            HSTRING *localeName) override;

        IFACEMETHOD(GetStrikethrough)(
            int32_t characterIndex,
            boolean *hasStrikethrough) override;

        IFACEMETHOD(GetUnderline)(
            int32_t characterIndex,
            boolean *hasUnderline) override;

        IFACEMETHOD(SetColor)(
            int32_t characterIndex,
            int32_t characterCount,
            Color color) override;

        IFACEMETHOD(SetBrush)(
            int32_t characterIndex,
            int32_t characterCount,
            ICanvasBrush* brush) override;

        IFACEMETHOD(SetFontFamily)(
            int32_t characterIndex,
            int32_t characterCount,
            HSTRING fontFamily) override;

        IFACEMETHOD(SetFontSize)(
            int32_t characterIndex,
            int32_t characterCount,
            float fontSize) override;

        IFACEMETHOD(SetFontStretch)(
            int32_t characterIndex,
            int32_t characterCount,
            ABI::Windows::UI::Text::FontStretch fontStretch) override;

        IFACEMETHOD(SetFontStyle)(
            int32_t characterIndex,
            int32_t characterCount,
            ABI::Windows::UI::Text::FontStyle fontStyle) override;

        IFACEMETHOD(SetFontWeight)(
            int32_t characterIndex,
            int32_t characterCount,
            ABI::Windows::UI::Text::FontWeight fontWeight) override;

        IFACEMETHOD(SetLocaleName)(
            int32_t characterIndex,
            int32_t characterCount,
            HSTRING name) override;

        IFACEMETHOD(SetStrikethrough)(
            int32_t characterIndex,
            int32_t characterCount,
            boolean hasStrikethrough) override;

        IFACEMETHOD(SetUnderline)(
            int32_t characterIndex,
            int32_t characterCount,
            boolean hasUnderline) override;

        IFACEMETHOD(GetPairKerning)(
            int32_t characterIndex,
            boolean *hasPairKerning) override;

        IFACEMETHOD(SetPairKerning)(
            int32_t characterIndex,
            int32_t characterCount,
            boolean hasPairKerning) override;

        IFACEMETHOD(GetLeadingCharacterSpacing)(
            int32_t characterIndex,
            float *leadingSpacing) override;

        IFACEMETHOD(GetTrailingCharacterSpacing)(
            int32_t characterIndex,
            float *trailingSpacing) override;

        IFACEMETHOD(GetMinimumCharacterAdvance)(
            int32_t characterIndex,
            float *minimumAdvance) override;

        IFACEMETHOD(SetCharacterSpacing)(
            int32_t characterIndex,
            int32_t characterCount,
            float leadingSpacing,
            float trailingSpacing,
            float minimumAdvance) override;

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
        // Hit-testing and metrics-related methods
        //
        IFACEMETHOD(get_LayoutBounds)(Rect* rect);

        IFACEMETHOD(get_LineCount)(int32_t* lineCount);

        IFACEMETHOD(get_DrawBounds)(Rect* rect);

        IFACEMETHOD(HitTest)(
            Vector2 point,
            boolean* isHit);

        IFACEMETHOD(HitTestWithCoords)(
            float x,
            float y,
            boolean* isHit);

        IFACEMETHOD(HitTestWithDescription)(
            Vector2 point,
            CanvasTextLayoutRegion* description,
            boolean* isHit);

        IFACEMETHOD(HitTestWithDescriptionAndTrailingSide)(
            Vector2 point,
            CanvasTextLayoutRegion* description,
            boolean* isTrailingSide,
            boolean* isHit);

        IFACEMETHOD(HitTestWithDescriptionAndCoords)(
            float x,
            float y,
            CanvasTextLayoutRegion* description,
            boolean* isHit);

        IFACEMETHOD(HitTestWithDescriptionAndCoordsAndTrailingSide)(
            float x,
            float y,
            CanvasTextLayoutRegion* description,
            boolean* isTrailingSide,
            boolean* isHit);

        IFACEMETHOD(GetCaretPosition(
            int32_t characterIndex,
            boolean trailingSideOfCharacter,
            Vector2* location));

        IFACEMETHOD(GetCaretPositionWithDescription(
            int32_t characterIndex,
            boolean trailingSideOfCharacter,
            CanvasTextLayoutRegion* description,
            Vector2* location));

        IFACEMETHOD(GetCharacterRegions(
            int32_t characterIndex,
            int32_t characterCount,
            uint32_t* descriptionCount,
            CanvasTextLayoutRegion** descriptions));

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(get_Device)(ICanvasDevice** device);

        //
        // Internal
        //
        void SetLineSpacingModeInternal(CanvasLineSpacingMode lineSpacingMode);
    };


    //
    // CanvasTextLayoutFactory
    //

    class CanvasTextLayoutFactory
        : public ActivationFactory<ICanvasTextLayoutFactory>
        , private LifespanTracker<CanvasTextLayoutFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextLayout, BaseTrust);

    public:
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            HSTRING textString,
            ICanvasTextFormat* textFormat,
            float requestedWidth,
            float requestedHeight,
            ICanvasTextLayout** textLayout);
    };
}}}}}

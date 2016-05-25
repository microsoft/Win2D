// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "CustomFontManager.h"
#include "TrimmingSignInformation.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    using namespace ::Microsoft::WRL;

#if WINVER > _WIN32_WINNT_WINBLUE
    typedef IDWriteTextLayout3 DWriteTextLayoutType;
    typedef DWRITE_LINE_METRICS1 DWriteMetricsType;
#else
    typedef IDWriteTextLayout2 DWriteTextLayoutType;
    typedef DWRITE_LINE_METRICS DWriteMetricsType;
#endif

    class CanvasTextLayout : RESOURCE_WRAPPER_RUNTIME_CLASS(
        DWriteTextLayoutType,
        CanvasTextLayout,
        ICanvasTextLayout,
        CloakedIid<ICanvasResourceWrapperWithDevice>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextLayout, BaseTrust);

        CanvasDrawTextOptions m_drawTextOptions;

        ClosablePtr<ICanvasDevice> m_device;

        std::shared_ptr<CustomFontManager> m_customFontManager;

        CanvasLineSpacingMode m_lineSpacingMode;

        TrimmingSignInformation m_trimmingSignInformation;

    public:
        static ComPtr<CanvasTextLayout> CreateNew(
            ICanvasResourceCreator* resourceCreator,
            HSTRING textString,
            ICanvasTextFormat* textFormat,
            float requestedWidth,
            float requestedHeight);

        CanvasTextLayout(
            ICanvasDevice* device,
            DWriteTextLayoutType* layout);

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

        IFACEMETHOD(get_TrimmingSign)(
            CanvasTrimmingSign* value) override;

        IFACEMETHOD(put_TrimmingSign)(
            CanvasTrimmingSign value) override;

        IFACEMETHOD(get_CustomTrimmingSign)(
            ICanvasTextInlineObject** value) override;

        IFACEMETHOD(put_CustomTrimmingSign)(
            ICanvasTextInlineObject* value) override;

        //
        // Hit-testing and metrics-related methods
        //
        IFACEMETHOD(get_LayoutBounds)(Rect* rect) override;

        IFACEMETHOD(get_LayoutBoundsIncludingTrailingWhitespace)(Rect* rect) override;

        IFACEMETHOD(get_LineCount)(int32_t* lineCount) override;

        IFACEMETHOD(get_DrawBounds)(Rect* rect) override;

        IFACEMETHOD(get_MaximumBidiReorderingDepth)(int32_t* depth) override;

        IFACEMETHOD(HitTest)(
            Vector2 point,
            boolean* isHit) override;

        IFACEMETHOD(HitTestWithCoords)(
            float x,
            float y,
            boolean* isHit) override;

        IFACEMETHOD(HitTestWithDescription)(
            Vector2 point,
            CanvasTextLayoutRegion* description,
            boolean* isHit) override;

        IFACEMETHOD(HitTestWithDescriptionAndTrailingSide)(
            Vector2 point,
            CanvasTextLayoutRegion* description,
            boolean* isTrailingSide,
            boolean* isHit) override;

        IFACEMETHOD(HitTestWithDescriptionAndCoords)(
            float x,
            float y,
            CanvasTextLayoutRegion* description,
            boolean* isHit) override;

        IFACEMETHOD(HitTestWithDescriptionAndCoordsAndTrailingSide)(
            float x,
            float y,
            CanvasTextLayoutRegion* description,
            boolean* isTrailingSide,
            boolean* isHit) override;

        IFACEMETHOD(GetCaretPosition(
            int32_t characterIndex,
            boolean trailingSideOfCharacter,
            Vector2* location)) override;

        IFACEMETHOD(GetCaretPositionWithDescription(
            int32_t characterIndex,
            boolean trailingSideOfCharacter,
            CanvasTextLayoutRegion* description,
            Vector2* location)) override;

        IFACEMETHOD(GetCharacterRegions(
            int32_t characterIndex,
            int32_t characterCount,
            uint32_t* descriptionCount,
            CanvasTextLayoutRegion** descriptions)) override;

        IFACEMETHOD(DrawToTextRenderer(
            ICanvasTextRenderer* textRenderer,
            Vector2 position)) override;

        IFACEMETHOD(DrawToTextRendererWithCoords(
            ICanvasTextRenderer* textRenderer,
            float x,
            float y)) override;

        IFACEMETHOD(SetInlineObject)(
            int32_t characterIndex,
            int32_t characterCount,
            ICanvasTextInlineObject* inlineObject) override;

        IFACEMETHOD(GetInlineObject)(
            int32_t characterIndex,
            ICanvasTextInlineObject** inlineObject) override;

        IFACEMETHOD(get_LineMetrics)(
            uint32_t* valueCount,
            CanvasLineMetrics** valueElements) override;

        IFACEMETHOD(get_ClusterMetrics)(
            uint32_t* valueCount,
            CanvasClusterMetrics** valueElements) override;

        IFACEMETHOD(GetCustomBrush)(
            int32_t characterIndex,
            IInspectable** brush) override;

        IFACEMETHOD(SetCustomBrush)(
            int32_t characterIndex,
            int32_t characterCount,
            IInspectable* brush) override;

        IFACEMETHOD(GetTypography)(
            int32_t characterIndex,
            ICanvasTypography** typography) override;

        IFACEMETHOD(SetTypography)(
            int32_t characterIndex,
            int32_t characterCount,
            ICanvasTypography* typography) override;

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(get_Device)(ICanvasDevice** device) override;

        //
        // Internal
        //
        void SetLineSpacingModeInternal(CanvasLineSpacingMode lineSpacingMode);

        void SetTrimmingSignInternal(CanvasTrimmingSign trimmingSign);

        void EnsureCustomTrimmingSignDevice(IDWriteTextLayout2* layout, ICanvasDevice* device);

    private:
        ComPtr<IInspectable> GetCustomBrushInternal(int32_t characterIndex);

        void SetCustomBrushInternal(
            int32_t characterIndex,
            int32_t characterCount,
            IInspectable* brush);
    };


    //
    // CanvasTextLayoutFactory
    //

    class CanvasTextLayoutFactory
        : public AgileActivationFactory<ICanvasTextLayoutFactory, ICanvasTextLayoutStatics>
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

        IFACEMETHOD(GetGlyphOrientationTransform)(
            CanvasGlyphOrientation glyphOrientation,
            boolean isSideways,
            Vector2 position,
            Matrix3x2* transform) override;
    };
}}}}}

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "pch.h"

#include "InternalDWriteInlineObject.h"
#include "InternalDWriteTextRenderer.h"
#include "DrawGlyphRunHelper.h"

class CanvasToDWriteTextRendererAdapter : public RuntimeClass<
    RuntimeClassFlags<WinRtClassicComMix>,
    ICanvasTextRenderer>,
    private LifespanTracker<CanvasToDWriteTextRendererAdapter>
{
    //
    // This is exposed to the app through the ICanvasTextRenderer interface,
    // and not as a runtime class.
    //
    InspectableClass(L"Microsoft.Graphics.Canvas.Text.ICanvasTextRenderer", BaseTrust);

    ComPtr<IDWriteTextRenderer1> m_dwriteTextRenderer;
    ComPtr<ICanvasDevice> m_device;
public:
    CanvasToDWriteTextRendererAdapter(ComPtr<IDWriteTextRenderer1> const& textRenderer, ComPtr<ICanvasDevice> const& device)
        : m_dwriteTextRenderer(textRenderer)
        , m_device(device)
    {
    }

    void ReleaseResources()
    {
        m_dwriteTextRenderer.Reset();
        m_device.Reset();
    }

    IFACEMETHODIMP DrawGlyphRun(
        Vector2 baselineOrigin,
        ICanvasFontFace* fontFace,
        float fontSize,
        uint32_t glyphCount,
        CanvasGlyph* glyphs,
        boolean isSideways,
        uint32_t bidiLevel,
        IInspectable* brush,
        CanvasTextMeasuringMode measuringMode,
        HSTRING locale,
        HSTRING text,
        uint32_t clusterMapIndicesCount,
        int* clusterMapIndices,
        unsigned int characterIndex,
        CanvasGlyphOrientation glyphOrientation) override
    {
        return ExceptionBoundary(
            [&]
            {
                EnsureValidResource();

                auto deviceContextLease = As<ICanvasDeviceInternal>(m_device)->GetResourceCreationDeviceContext();

                DrawGlyphRunHelper helper(
                    fontFace,
                    fontSize,
                    glyphCount,
                    glyphs,
                    isSideways,
                    bidiLevel,
                    brush,
                    measuringMode,
                    locale,
                    text,
                    clusterMapIndicesCount,
                    clusterMapIndices,
                    characterIndex,
                    deviceContextLease.Get());

                ThrowIfFailed(m_dwriteTextRenderer->DrawGlyphRun(
                    nullptr,
                    baselineOrigin.X,
                    baselineOrigin.Y,
                    ToDWriteGlyphOrientationAngle(glyphOrientation),
                    helper.MeasuringMode,
                    &helper.DWriteGlyphRun,
                    &helper.DWriteGlyphRunDescription,
                    helper.ClientDrawingEffect.Get())); 
            });
    }

    IFACEMETHODIMP DrawStrikethrough(
        Vector2 baselineOrigin,
        float width,
        float thickness,
        float offset,
        CanvasTextDirection textDirection,
        IInspectable* brush,
        CanvasTextMeasuringMode measuringMode,
        HSTRING locale,
        CanvasGlyphOrientation glyphOrientation)
    {
        return ExceptionBoundary(
            [&]
            {
                EnsureValidResource();

                DWRITE_STRIKETHROUGH strikethrough{};
                strikethrough.width = width;
                strikethrough.thickness = thickness;
                strikethrough.offset = offset;

                auto textDirectionLookup = DWriteToCanvasTextDirection::Lookup(textDirection);
                strikethrough.flowDirection = textDirectionLookup->FlowDirection;
                strikethrough.readingDirection = textDirectionLookup->ReadingDirection;
                strikethrough.measuringMode = ToDWriteMeasuringMode(measuringMode);

                if (locale)
                    strikethrough.localeName = WindowsGetStringRawBuffer(locale, nullptr);

                auto deviceContextLease = As<ICanvasDeviceInternal>(m_device)->GetResourceCreationDeviceContext();

                ThrowIfFailed(m_dwriteTextRenderer->DrawStrikethrough(
                    nullptr,
                    baselineOrigin.X,
                    baselineOrigin.Y,
                    ToDWriteGlyphOrientationAngle(glyphOrientation),
                    &strikethrough,
                    DrawGlyphRunHelper::GetClientDrawingEffect(brush, deviceContextLease.Get()).Get()));
            });
    }

    IFACEMETHODIMP DrawUnderline(
        Vector2 baselineOrigin,
        float width,
        float thickness,
        float offset,
        float runHeight,
        CanvasTextDirection textDirection,
        IInspectable* brush,
        CanvasTextMeasuringMode measuringMode,
        HSTRING locale,
        CanvasGlyphOrientation glyphOrientation)
    {
        return ExceptionBoundary(
            [&]
            {
                EnsureValidResource();

                DWRITE_UNDERLINE underline{};
                underline.width = width;
                underline.thickness = thickness;
                underline.offset = offset;
                underline.runHeight = runHeight;

                auto textDirectionLookup = DWriteToCanvasTextDirection::Lookup(textDirection);
                underline.flowDirection = textDirectionLookup->FlowDirection;
                underline.readingDirection = textDirectionLookup->ReadingDirection;
                underline.measuringMode = ToDWriteMeasuringMode(measuringMode);

                auto deviceContextLease = As<ICanvasDeviceInternal>(m_device)->GetResourceCreationDeviceContext();

                if (locale)
                    underline.localeName = WindowsGetStringRawBuffer(locale, nullptr);

                ThrowIfFailed(m_dwriteTextRenderer->DrawUnderline(
                    nullptr,
                    baselineOrigin.X,
                    baselineOrigin.Y,
                    ToDWriteGlyphOrientationAngle(glyphOrientation),
                    &underline,
                    DrawGlyphRunHelper::GetClientDrawingEffect(brush, deviceContextLease.Get()).Get()));
            });
    }

    IFACEMETHODIMP DrawInlineObject(
        Vector2 baselineOrigin,
        ICanvasTextInlineObject* inlineObject,
        boolean isSideways,
        boolean isRightToLeft,
        IInspectable* brush,
        CanvasGlyphOrientation glyphOrientation) override
    {
        return ExceptionBoundary(
            [&]
            {
                EnsureValidResource();

                auto dwriteInlineObject = Make<InternalDWriteInlineObject>(inlineObject, m_device);
                CheckMakeResult(dwriteInlineObject);

                auto deviceContextLease = As<ICanvasDeviceInternal>(m_device)->GetResourceCreationDeviceContext();

                ThrowIfFailed(m_dwriteTextRenderer->DrawInlineObject(
                    nullptr,
                    baselineOrigin.X,
                    baselineOrigin.Y,
                    ToDWriteGlyphOrientationAngle(glyphOrientation),
                    nullptr,
                    isSideways,
                    isRightToLeft,
                    DrawGlyphRunHelper::GetClientDrawingEffect(brush, deviceContextLease.Get()).Get()));
            });
    }

    virtual IFACEMETHODIMP get_Dpi(float* value)
    {
        return ExceptionBoundary(
            [&]
            {
                EnsureValidResource();

                float pixelsPerDip;
                ThrowIfFailed(m_dwriteTextRenderer->GetPixelsPerDip(nullptr, &pixelsPerDip));

                *value = pixelsPerDip * DEFAULT_DPI;
            });
    }

    virtual IFACEMETHODIMP get_PixelSnappingDisabled(boolean* value)
    {
        return ExceptionBoundary(
            [&]
            {
                EnsureValidResource();

                BOOL isPixelSnappingDisabled;
                ThrowIfFailed(m_dwriteTextRenderer->IsPixelSnappingDisabled(nullptr, &isPixelSnappingDisabled));

                *value = !!isPixelSnappingDisabled;
            });
    }

    virtual IFACEMETHODIMP get_Transform(Matrix3x2* value)
    {
        return ExceptionBoundary(
            [&]
            {
                EnsureValidResource();

                DWRITE_MATRIX transform;
                ThrowIfFailed(m_dwriteTextRenderer->GetCurrentTransform(nullptr, &transform));

                *value = *(ReinterpretAs<Matrix3x2*>(&transform));
            });
    }

private:
    void EnsureValidResource()
    {
        if (!m_dwriteTextRenderer)
        {
            //
            // The app can hit this if it uses an inline object callback to hang onto 
            // a reference to a text renderer past its allowed lifespan, and then
            // tries calling methods on that text renderer.
            //
            ThrowHR(E_INVALIDARG, Strings::TextRendererNotValid);
        }
    }
};

IFACEMETHODIMP InternalDWriteInlineObject::Draw(
    void*,
    IDWriteTextRenderer* textRenderer,
    FLOAT originX,
    FLOAT originY,
    BOOL isSideways,
    BOOL isRightToLeft,
    IUnknown* clientDrawingEffect)
{
    return ExceptionBoundary(
        [&]
        {
            ComPtr<CanvasToDWriteTextRendererAdapter> internalTextRenderer;
            if (textRenderer)
            {
                auto textRenderer1 = As<IDWriteTextRenderer1>(textRenderer);
                internalTextRenderer = Make<CanvasToDWriteTextRendererAdapter>(textRenderer1, m_device.Get());
            }

            auto brushInspectable = GetCustomDrawingObjectInspectable(m_device.Get(), clientDrawingEffect);

            ThrowIfFailed(m_inlineObject->Draw(
                internalTextRenderer.Get(),
                Vector2{ originX, originY },
                !!isSideways,
                !!isRightToLeft,
                brushInspectable.Get()));

            if (internalTextRenderer)
            {
                //
                // An explicit 'release resources' operation is necessary here so that the 
                // runtime will relinquish its references to this object. 
                //
                // Compared to other wrapped objects involved in callbacks, the D2D text renderer
                // is special. D2D polices it and fails if it looks like the app kept a reference
                // to it beyond the lifespan of DrawTextLayout. The D2D text renderer isn't
                // guaranteed to be valid beyond that.
                //
                internalTextRenderer->ReleaseResources();
            }
        });
}

IFACEMETHODIMP InternalDWriteInlineObject::GetMetrics(DWRITE_INLINE_OBJECT_METRICS* out)
{
    return ExceptionBoundary(
        [&]
        {
            float baseline;
            ThrowIfFailed(m_inlineObject->get_Baseline(&baseline));

            Size size;
            ThrowIfFailed(m_inlineObject->get_Size(&size));

            boolean supportsSideways;
            ThrowIfFailed(m_inlineObject->get_SupportsSideways(&supportsSideways));

            out->baseline = baseline;
            out->width = size.Width;
            out->height = size.Height;
            out->supportsSideways = supportsSideways;                
        });
}

IFACEMETHODIMP InternalDWriteInlineObject::GetOverhangMetrics(DWRITE_OVERHANG_METRICS* out)
{
    return ExceptionBoundary(
        [&]
        {
            Rect drawBounds;
            ThrowIfFailed(m_inlineObject->get_DrawBounds(&drawBounds));

            Size size;
            ThrowIfFailed(m_inlineObject->get_Size(&size));

            *out = GetOverhangMetricsFromRect(drawBounds, size.Width, size.Height);
        });
}

IFACEMETHODIMP InternalDWriteInlineObject::GetBreakConditions(
    DWRITE_BREAK_CONDITION* breakConditionBefore,
    DWRITE_BREAK_CONDITION* breakConditionAfter)
{
    return ExceptionBoundary(
        [&]
        {
            CanvasLineBreakCondition breakBefore;
            ThrowIfFailed(m_inlineObject->get_BreakBefore(&breakBefore));

            CanvasLineBreakCondition breakAfter;
            ThrowIfFailed(m_inlineObject->get_BreakAfter(&breakAfter));

            *breakConditionBefore = ToDWriteBreakCondition(breakBefore);
            *breakConditionAfter = ToDWriteBreakCondition(breakAfter);
        });
}

ComPtr<ICanvasTextInlineObject> InternalDWriteInlineObject::GetCanvasTextInlineObject() const
{
    return m_inlineObject;
}

//static 
DWRITE_OVERHANG_METRICS InternalDWriteInlineObject::GetOverhangMetricsFromRect(Rect const& rect, float resourceWidth, float resourceHeight)
{
    DWRITE_OVERHANG_METRICS ret;

    ret.left = -rect.X;
    ret.right = rect.Width - resourceWidth;

    ret.top = -rect.Y;
    ret.bottom = rect.Height - resourceHeight;

    return ret;
}
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class CustomInlineObject : public RuntimeClass<
        RuntimeClassFlags<WinRtClassicComMix>,
        ICanvasTextInlineObject>
    {
    public:

        CALL_COUNTER_WITH_MOCK(DrawMethod, HRESULT(ICanvasTextRenderer*, Vector2, boolean, boolean, IInspectable*));

        Size ObjectSize;
        float Baseline;
        bool SupportsSideways;
        Rect DrawBounds;
        CanvasLineBreakCondition BreakBefore, BreakAfter;

        CustomInlineObject()
            : ObjectSize{}
            , Baseline(0)
            , SupportsSideways(false)
            , DrawBounds{}
            , BreakBefore(CanvasLineBreakCondition::Neutral)
            , BreakAfter(CanvasLineBreakCondition::Neutral)
        {
            DrawMethod.AllowAnyCall();
        }

        IFACEMETHODIMP Draw(
            ICanvasTextRenderer* textRenderer,
            Vector2 baselineOrigin,
            boolean isSideways,
            boolean isRightToLeft,
            IInspectable* brush) override
        {
            return DrawMethod.WasCalled(textRenderer, baselineOrigin, isSideways, isRightToLeft, brush);
        }

        virtual IFACEMETHODIMP get_Size(Size* value)
        {
            *value = ObjectSize;
            return S_OK;
        }

        virtual IFACEMETHODIMP get_Baseline(float* value)
        {
            *value = Baseline;
            return S_OK;
        }

        virtual IFACEMETHODIMP get_SupportsSideways(boolean* value)
        {
            *value = SupportsSideways;
            return S_OK;
        }

        virtual IFACEMETHODIMP get_DrawBounds(Rect* value)
        {
            *value = DrawBounds;
            return S_OK;
        }

        virtual IFACEMETHODIMP get_BreakBefore(CanvasLineBreakCondition* value)
        {
            *value = BreakBefore;
            return S_OK;
        }

        virtual IFACEMETHODIMP get_BreakAfter(CanvasLineBreakCondition* value)
        {
            *value = BreakAfter;
            return S_OK;
        }
    };
}
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockDWriteInlineObject : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IDWriteInlineObject>
    {
    public:
        CALL_COUNTER_WITH_MOCK(DrawMethod, HRESULT(void*, IDWriteTextRenderer*, FLOAT, FLOAT, BOOL, BOOL, IUnknown*));
        CALL_COUNTER_WITH_MOCK(GetMetricsMethod, HRESULT(DWRITE_INLINE_OBJECT_METRICS*));
        CALL_COUNTER_WITH_MOCK(GetOverhangMetricsMethod, HRESULT(DWRITE_OVERHANG_METRICS*));
        CALL_COUNTER_WITH_MOCK(GetBreakConditionsMethod, HRESULT(DWRITE_BREAK_CONDITION*, DWRITE_BREAK_CONDITION*));

        IFACEMETHODIMP Draw(
            void* clientDrawingContext,
            IDWriteTextRenderer* renderer,
            FLOAT originX,
            FLOAT originY,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* clientDrawingEffect)
        {
            return DrawMethod.WasCalled(clientDrawingContext, renderer, originX, originY, isSideways, isRightToLeft, clientDrawingEffect);
        }

        IFACEMETHODIMP GetMetrics(DWRITE_INLINE_OBJECT_METRICS* metrics)
        {
            return GetMetricsMethod.WasCalled(metrics);
        }

        IFACEMETHODIMP GetOverhangMetrics(DWRITE_OVERHANG_METRICS* overhangs) override
        {
            return GetOverhangMetricsMethod.WasCalled(overhangs);
        }

        IFACEMETHODIMP GetBreakConditions(
            DWRITE_BREAK_CONDITION* breakConditionBefore,
            DWRITE_BREAK_CONDITION* breakConditionAfter) override
        {
            return GetBreakConditionsMethod.WasCalled(breakConditionBefore, breakConditionAfter);
        }
    };
}

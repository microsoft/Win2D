// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    [uuid(49320795-5DC9-42B8-97A3-230E44846BD8)]
    class IInternalDWriteInlineObject : public IUnknown
    {
    public:
        virtual void SetDevice(ICanvasDevice* device) = 0;

        virtual ComPtr<ICanvasTextInlineObject> GetCanvasTextInlineObject() const = 0;
    };

    class InternalDWriteInlineObject : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IDWriteInlineObject,
        CloakedIid<IInternalDWriteInlineObject>>
    {
        ComPtr<ICanvasTextInlineObject> m_inlineObject;
        ComPtr<ICanvasDevice> m_device;

    public:

        InternalDWriteInlineObject(
            ComPtr<ICanvasTextInlineObject> inlineObject,
            ComPtr<ICanvasDevice> device)
            : m_inlineObject(inlineObject)
            , m_device(device)
        {
        }

        IFACEMETHODIMP Draw(
            void*,
            IDWriteTextRenderer* renderer,
            FLOAT originX,
            FLOAT originY,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* clientDrawingEffect) override;

        IFACEMETHODIMP GetMetrics(DWRITE_INLINE_OBJECT_METRICS* out) override;

        IFACEMETHODIMP GetOverhangMetrics(DWRITE_OVERHANG_METRICS* out) override;

        IFACEMETHODIMP GetBreakConditions(
            DWRITE_BREAK_CONDITION* breakConditionBefore,
            DWRITE_BREAK_CONDITION* breakConditionAfter);

        virtual ComPtr<ICanvasTextInlineObject> GetCanvasTextInlineObject() const override;

        virtual void SetDevice(ICanvasDevice* device) override
        {
            m_device = device;
        }

    private:

        static DWRITE_OVERHANG_METRICS GetOverhangMetricsFromRect(Rect const& rect, float resourceWidth, float resourceHeight);
    };

    // Utility function
    inline ComPtr<ICanvasTextInlineObject> GetCanvasInlineObjectFromDWriteInlineObject(ComPtr<IDWriteInlineObject> const& dwriteInlineObject)
    {
        ComPtr<ICanvasTextInlineObject> canvasInlineObject;

        if (dwriteInlineObject)
        {
            ComPtr<IInternalDWriteInlineObject> internalDwriteInlineObject;

            HRESULT hr = dwriteInlineObject.As(&internalDwriteInlineObject);
            if (hr == E_NOINTERFACE)
            {
                ThrowHR(E_NOINTERFACE, Strings::ExternalInlineObject);
            }

            ThrowIfFailed(hr);

            canvasInlineObject = internalDwriteInlineObject->GetCanvasTextInlineObject();
        }

        return canvasInlineObject;
    }

}}}}}

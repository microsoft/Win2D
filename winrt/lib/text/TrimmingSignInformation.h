// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "InternalDWriteInlineObject.h"
#include "TextUtilities.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    class TrimmingSignInformation
    {
        ComPtr<IDWriteInlineObject> m_internalEllipsisTrimmingSign;
        CanvasTrimmingSign m_trimmingSign;

        ComPtr<ICanvasTextInlineObject> m_customTrimmingSign;

    public:

        TrimmingSignInformation()
            : m_trimmingSign(CanvasTrimmingSign::None)
        {}

        void RecreateInternalTrimmingSignIfNeeded(IDWriteTextFormat* textFormat, bool trustShadowState=true)
        {
            bool trimmingWasRead = false;
            DWRITE_TRIMMING trimming;
            if (!trustShadowState || GetTrimmingSignFromResource(textFormat, &trimming, &trimmingWasRead) == CanvasTrimmingSign::Ellipsis)
            {
                auto dwriteFactory = CustomFontManager::GetInstance()->GetSharedFactory();
                ThrowIfFailed(dwriteFactory->CreateEllipsisTrimmingSign(textFormat, &m_internalEllipsisTrimmingSign));

                if (!trimmingWasRead)
                {
                    ComPtr<IDWriteInlineObject> unused;
                    ThrowIfFailed(textFormat->GetTrimming(&trimming, &unused));
                }
                ThrowIfFailed(textFormat->SetTrimming(&trimming, m_internalEllipsisTrimmingSign.Get()));
            }
            else
            {
                m_trimmingSign = CanvasTrimmingSign::None;
                m_internalEllipsisTrimmingSign.Reset();
            }
        }

        void RealizeTrimmingSign(IDWriteTextFormat* textFormat)
        {
            //
            // If there's a custom trimming sign set, don't clobber it.
            if (m_customTrimmingSign)
            {
                return;
            }

            if (m_trimmingSign == CanvasTrimmingSign::None)
            {
                //
                // Null out the trimming sign object if there is one.
                //
                DWRITE_TRIMMING trimming;
                ComPtr<IDWriteInlineObject> trimmingSignObject;
                ThrowIfFailed(textFormat->GetTrimming(&trimming, &trimmingSignObject));

                if (trimmingSignObject)
                    ThrowIfFailed(textFormat->SetTrimming(&trimming, nullptr));
            }
            else if (m_trimmingSign == CanvasTrimmingSign::Ellipsis)
            {
                RecreateInternalTrimmingSignIfNeeded(textFormat, false);
            }
            else
            {
                ThrowHR(E_INVALIDARG);
            }
        }

        void RealizeCustomTrimmingSign(IDWriteTextFormat* textFormat)
        {
            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> previousObject;
            ThrowIfFailed(textFormat->GetTrimming(&trimming, &previousObject));

            if (m_customTrimmingSign)
            {
                auto dwriteInlineObject = Make<InternalDWriteInlineObject>(m_customTrimmingSign, nullptr);
                ThrowIfFailed(textFormat->SetTrimming(&trimming, dwriteInlineObject.Get()));
            }
        }

        ComPtr<ICanvasTextInlineObject> GetCustomTrimmingSignFromResource(
            IDWriteTextFormat* textFormat)
        {
            DWRITE_TRIMMING unused;
            ComPtr<IDWriteInlineObject> trimmingSignObject;
            ThrowIfFailed(textFormat->GetTrimming(&unused, &trimmingSignObject));

            return GetCanvasInlineObjectFromDWriteInlineObject(trimmingSignObject);
        }

        CanvasTrimmingSign GetTrimmingSignFromResource(
            IDWriteTextFormat* textFormat, 
            DWRITE_TRIMMING* returnedTrimming = nullptr,
            bool* trimmingWasRead = nullptr)
        {
            if (trimmingWasRead)
                *trimmingWasRead = false;

            if (m_trimmingSign == CanvasTrimmingSign::None)
            {
                return CanvasTrimmingSign::None;
            }
            else
            {
                DWRITE_TRIMMING trimming;
                ComPtr<IDWriteInlineObject> trimmingSignObject;
                ThrowIfFailed(textFormat->GetTrimming(&trimming, &trimmingSignObject));

                if (returnedTrimming)
                {
                    *returnedTrimming = trimming;
                    *trimmingWasRead = true;
                }

                // 
                // Our shadow state says we have an ellipsis trimming sign, but interop
                // might have changed things from behind our back, so we verify it here.
                //
                if (trimmingSignObject && IsSameInstance(trimmingSignObject.Get(), m_internalEllipsisTrimmingSign.Get()))
                {
                    return CanvasTrimmingSign::Ellipsis;
                }
                else
                {
                    // Some custom, otherwise-specified trimming sign.

                    m_trimmingSign = CanvasTrimmingSign::None; // Allow future queries to be earlied out.

                    return CanvasTrimmingSign::None; //  As a convention we return None.
                }
            }
        }

        void SetTrimmingSignOnResource(CanvasTrimmingSign value, IDWriteTextFormat* textFormat)
        {
            bool signChanged = m_trimmingSign != value;

            m_trimmingSign = value;

            if (signChanged)
            {
                RealizeTrimmingSign(textFormat);
            }
        }

        void SetTrimmingSignFromResource(IDWriteTextFormat* textFormat) 
        { 
            m_trimmingSign = GetTrimmingSignFromResource(textFormat);
        }

        CanvasTrimmingSign GetTrimmingSignShadowState() const { return m_trimmingSign; }

        ComPtr<ICanvasTextInlineObject> GetCustomTrimmingSignShadowState() const { return m_customTrimmingSign; }

        CanvasTrimmingSign* GetAddressOfTrimmingSign() { return &m_trimmingSign; }

        ComPtr<ICanvasTextInlineObject>* GetAddressOfCustomTrimmingSign() { return &m_customTrimmingSign; }
    };
}}}}}

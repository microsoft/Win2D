// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    class CanvasScaledFont : public RuntimeClass<ICanvasScaledFont>
        , private LifespanTracker<CanvasScaledFont>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasScaledFont, BaseTrust);

        ComPtr<ICanvasFontFace> m_fontFace;
        float m_scaleFactor;

    public:
        CanvasScaledFont(
            ICanvasFontFace* fontFace,
            float scaleFactor);

        IFACEMETHOD(get_FontFace)(ICanvasFontFace** value);

        IFACEMETHOD(get_ScaleFactor)(float* value);
    };
    
}}}}}

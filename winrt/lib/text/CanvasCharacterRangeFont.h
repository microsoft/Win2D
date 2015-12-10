// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    class CanvasCharacterRangeFont : public RuntimeClass<ICanvasCharacterRangeFont>
        , private LifespanTracker<CanvasCharacterRangeFont>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasCharacterRangeFont, BaseTrust);

        int m_characterIndex;
        int m_characterCount;
        ComPtr<ICanvasFontFace> m_fontFace;
        float m_scaleFactor;

    public:
        CanvasCharacterRangeFont(
            int characterIndex,
            int characterCount,
            ICanvasFontFace* fontFace,
            float scaleFactor);

        IFACEMETHOD(get_CharacterIndex)(int* value);

        IFACEMETHOD(get_CharacterCount)(int* value);

        IFACEMETHOD(get_FontFace)(ICanvasFontFace** value);

        IFACEMETHOD(get_ScaleFactor)(float* value);
    };
    
}}}}}

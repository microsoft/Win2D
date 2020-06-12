// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{
    using namespace ABI::Microsoft::Graphics::Canvas;
    using namespace ABI::Microsoft::Graphics::Canvas::Geometry;
    using namespace ABI::Microsoft::UI;
    using namespace ABI::Windows::Foundation::Numerics;
    using namespace ABI::Windows::Foundation;
    
    class CanvasSvgNamedElement : 
        public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasSvgNamedElement,
            ICanvasSvgElement,
            ChainInterfaces<
                MixIn<CanvasSvgNamedElement, ResourceWrapper<ID2D1SvgElement, CanvasSvgNamedElement, ICanvasSvgNamedElement>>,
                IClosable,
                CloakedIid<ICanvasResourceWrapperNative>>>,
        public CloakedIid<ICanvasResourceWrapperWithDevice>,
        public ResourceWrapper<ID2D1SvgElement, CanvasSvgNamedElement, ICanvasSvgNamedElement>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgNamedElement, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:
        CanvasSvgNamedElement(
            ICanvasDevice* canvasDevice,
            ID2D1SvgElement* d2dSvgElement);

        IFACEMETHOD(Close)();

        IFACEMETHOD(AppendChild)(ICanvasSvgElement* child);

        IFACEMETHOD(CreateAndAppendNamedChildElement)(HSTRING, ICanvasSvgNamedElement**);
        IFACEMETHOD(CreateAndAppendTextChildElement)(HSTRING, ICanvasSvgTextElement**);

        IFACEMETHOD(get_FirstChild)(ICanvasSvgElement**);
        IFACEMETHOD(get_LastChild)(ICanvasSvgElement**);

        IFACEMETHOD(GetPreviousSibling)(ICanvasSvgElement*, ICanvasSvgElement**);
        IFACEMETHOD(GetNextSibling)(ICanvasSvgElement*, ICanvasSvgElement**);

        IFACEMETHOD(get_SpecifiedAttributes)(UINT32*, HSTRING **);
        IFACEMETHOD(get_Tag)(HSTRING*);
        IFACEMETHOD(get_HasChildren)(boolean*);
        IFACEMETHOD(InsertChildBefore)(ICanvasSvgElement*, ICanvasSvgElement*);

        IFACEMETHOD(IsAttributeSpecified)(HSTRING, boolean*);
        IFACEMETHOD(IsAttributeSpecifiedWithInherhited)(HSTRING, boolean, boolean*);

        IFACEMETHOD(RemoveAttribute)(HSTRING);
        IFACEMETHOD(RemoveChild)(ICanvasSvgElement*);
        IFACEMETHOD(ReplaceChild)(ICanvasSvgElement*, ICanvasSvgElement*);
        IFACEMETHOD(get_ContainingDocument)(ICanvasSvgDocument**);
        IFACEMETHOD(get_Parent)(ICanvasSvgNamedElement**);

        IFACEMETHOD(SetAttribute)(HSTRING, ICanvasSvgAttribute*);
        IFACEMETHOD(GetAttribute)(HSTRING, ICanvasSvgAttribute**);
        IFACEMETHOD(SetIdAttribute)(HSTRING, HSTRING);
        IFACEMETHOD(GetIdAttribute)(HSTRING, HSTRING*);
        IFACEMETHOD(SetStringAttribute)(HSTRING, HSTRING);
        IFACEMETHOD(GetStringAttribute)(HSTRING, HSTRING*);
        IFACEMETHOD(SetFloatAttribute)(HSTRING, float);
        IFACEMETHOD(GetFloatAttribute)(HSTRING, float*);
        IFACEMETHOD(SetColorAttribute)(HSTRING, Color);
        IFACEMETHOD(GetColorAttribute)(HSTRING, Color*);
        IFACEMETHOD(SetFilledRegionDeterminationAttribute)(HSTRING, CanvasFilledRegionDetermination);
        IFACEMETHOD(GetFilledRegionDeterminationAttribute)(HSTRING, CanvasFilledRegionDetermination*);
        IFACEMETHOD(SetDisplayAttribute)(HSTRING, CanvasSvgDisplay);
        IFACEMETHOD(GetDisplayAttribute)(HSTRING, CanvasSvgDisplay*);
        IFACEMETHOD(SetOverflowAttribute)(HSTRING, CanvasSvgOverflow);
        IFACEMETHOD(GetOverflowAttribute)(HSTRING, CanvasSvgOverflow*);
        IFACEMETHOD(SetCapStyleAttribute)(HSTRING, CanvasCapStyle);
        IFACEMETHOD(GetCapStyleAttribute)(HSTRING, CanvasCapStyle*);
        IFACEMETHOD(SetLineJoinAttribute)(HSTRING, CanvasLineJoin);
        IFACEMETHOD(GetLineJoinAttribute)(HSTRING, CanvasLineJoin*);
        IFACEMETHOD(SetVisibilityAttribute)(HSTRING, CanvasSvgVisibility);
        IFACEMETHOD(GetVisibilityAttribute)(HSTRING, CanvasSvgVisibility*);
        IFACEMETHOD(SetTransformAttribute)(HSTRING, Matrix3x2);
        IFACEMETHOD(GetTransformAttribute)(HSTRING, Matrix3x2*);
        IFACEMETHOD(SetUnitsAttribute)(HSTRING, CanvasSvgUnits);
        IFACEMETHOD(GetUnitsAttribute)(HSTRING, CanvasSvgUnits*);
        IFACEMETHOD(SetEdgeBehaviorAttribute)(HSTRING, CanvasEdgeBehavior);
        IFACEMETHOD(GetEdgeBehaviorAttribute)(HSTRING, CanvasEdgeBehavior*);
        IFACEMETHOD(SetRectangleAttribute)(HSTRING, Rect);
        IFACEMETHOD(GetRectangleAttribute)(HSTRING, Rect*);
        IFACEMETHOD(SetLengthAttribute)(HSTRING, float, CanvasSvgLengthUnits);
        IFACEMETHOD(GetLengthAttribute)(HSTRING, CanvasSvgLengthUnits*, float*);
        IFACEMETHOD(SetAspectRatioAttribute)(HSTRING, CanvasSvgAspectAlignment, CanvasSvgAspectScaling);
        IFACEMETHOD(GetAspectRatioAttribute)(HSTRING, CanvasSvgAspectScaling*, CanvasSvgAspectAlignment*);
        
        IFACEMETHOD(get_Device)(ICanvasDevice**);

        // No exception boundary
        ComPtr<ICanvasDevice> GetDevice() { return m_canvasDevice.EnsureNotClosed(); }

    private:
        void IsAttributeSpecifiedImpl(HSTRING attributeName, boolean* isSpecified, boolean* isInherited);
        void GetStringAttributeImpl(HSTRING attributeName, D2D1_SVG_ATTRIBUTE_STRING_TYPE stringType, HSTRING* attributeValue);
        void SetStringAttributeImpl(HSTRING attributeName, D2D1_SVG_ATTRIBUTE_STRING_TYPE stringType, HSTRING attributeValue);
    };

    class CanvasSvgTextElement : 
        public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasSvgTextElement,
            ICanvasSvgElement,
            ChainInterfaces<
                MixIn<CanvasSvgTextElement, ResourceWrapper<ID2D1SvgElement, CanvasSvgTextElement, ICanvasSvgTextElement>>,
                IClosable,
                CloakedIid<ICanvasResourceWrapperNative>>>,
        public CloakedIid<ICanvasResourceWrapperWithDevice>,
        public ResourceWrapper<ID2D1SvgElement, CanvasSvgTextElement, ICanvasSvgTextElement>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgTextElement, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:

        CanvasSvgTextElement(
            ICanvasDevice* canvasDevice,
            ID2D1SvgElement* d2dSvgElement);

        IFACEMETHOD(Close)();
        IFACEMETHOD(get_Text)(HSTRING*);
        IFACEMETHOD(put_Text)(HSTRING);
        IFACEMETHOD(get_ContainingDocument)(ICanvasSvgDocument**);
        IFACEMETHOD(get_Parent)(ICanvasSvgNamedElement**);

        IFACEMETHOD(get_Device)(ICanvasDevice**);

        // No exception boundary
        ComPtr<ICanvasDevice> GetDevice() { return m_canvasDevice.EnsureNotClosed(); }
    };

    ComPtr<ICanvasSvgElement> CreateNewElementFromStream(CanvasSvgDocument* parent, IStream* stream);
    ComPtr<CanvasSvgNamedElement> CreateNewNamedElementFromStream(CanvasSvgDocument* parent, IStream* stream);

    bool IsSvgTextElement(ID2D1SvgElement* d2dSvgElement);

}}}}}

#endif
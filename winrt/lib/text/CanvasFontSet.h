// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation::Collections;
    using namespace ABI::Windows::UI::Text;

#if WINVER > _WIN32_WINNT_WINBLUE
    typedef IDWriteFontSet DWriteFontSetType;
#else
    typedef IDWriteFontCollection DWriteFontSetType;
#endif

    class CanvasFontSet : RESOURCE_WRAPPER_RUNTIME_CLASS(
        DWriteFontSetType,
        CanvasFontSet,
        ICanvasFontSet)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasFontSet, BaseTrust);

#if WINVER <= _WIN32_WINNT_WINBLUE
        std::vector<ComPtr<IDWriteFont>> m_flatCollection;
#endif
        std::shared_ptr<CustomFontManager> m_customFontManager;

    public:

        CanvasFontSet(
            DWriteFontSetType* dwriteFontSet);

        IFACEMETHOD(get_Fonts)(IVectorView<CanvasFontFace*>** value) override;

#if WINVER > _WIN32_WINNT_WINBLUE

        IFACEMETHOD(TryFindFontFace)(ICanvasFontFace* fontFace, int* index, boolean* succeeded) override;

        IFACEMETHOD(GetMatchingFontsFromProperties)(
            UINT32 propertyCount,
            CanvasFontProperty* propertyElements,
            ICanvasFontSet** matchingFonts) override;

        IFACEMETHOD(GetMatchingFontsFromWwsFamily)(
            HSTRING familyName,
            FontWeight weight,
            FontStretch stretch,
            FontStyle style,
            ICanvasFontSet** matchingFonts) override;

        IFACEMETHOD(CountFontsMatchingProperty)(
            CanvasFontProperty property,
            UINT32* count) override;

        IFACEMETHOD(GetPropertyValuesFromIndex)(
            UINT32 fontIndex,
            CanvasFontPropertyIdentifier propertyIdentifier,
            IMapView<HSTRING, HSTRING>** values) override;

        IFACEMETHOD(GetPropertyValuesFromIdentifier)(
            CanvasFontPropertyIdentifier propertyIdentifier,
            HSTRING preferredLocaleNames,
            UINT32* valueCount,
            CanvasFontProperty** valueElements) override;

        IFACEMETHOD(GetPropertyValues)(
            CanvasFontPropertyIdentifier propertyIdentifier,
            UINT32* valueCount,
            CanvasFontProperty** valueElements) override;
#endif

#if WINVER <= _WIN32_WINNT_WINBLUE
    private:
        void EnsureFlatCollection(ComPtr<IDWriteFontCollection> const& resource);
#endif
    };

    //
    // CanvasFontSetFactory
    //

    class CanvasFontSetFactory
        : public AgileActivationFactory<ICanvasFontSetFactory, ICanvasFontSetStatics>
        , private LifespanTracker<CanvasFontSetFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasFontSet, BaseTrust);

    public:
        //
        // ICanvasFontSetFactory
        //
        IFACEMETHOD(Create)(
            IUriRuntimeClass* uri,
            ICanvasFontSet** fontSet);

        //
        // ICanvasFontSetStatics
        //
        IFACEMETHOD(GetSystemFontSet)(
            ICanvasFontSet** systemFontSet);
    };
}}}}}

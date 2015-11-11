// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "utils/LockUtilities.h"
#include "CustomFontManager.h"
#include "TrimmingSignInformation.h"

//
// CanvasLineSpacingMode is a type that is only available on Win10.
// To reduce the number of guards around places that consume this type,
// we define a placeholder for 8.1.
//
#if WINVER <= _WIN32_WINNT_WINBLUE
enum CanvasLineSpacingMode { Default };
#endif

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Text
{
    using namespace ::Microsoft::WRL;


    //
    // CanvasTextFormatFactory
    //
    class CanvasTextFormatFactory
        : public ActivationFactory<ICanvasTextFormatStatics>
        , private LifespanTracker<CanvasTextFormatFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextFormat, BaseTrust);

    public:
        IFACEMETHOD(ActivateInstance)(IInspectable** obj) override;

        IFACEMETHOD(GetSystemFontFamilies)(
            uint32_t* valueCount,
            HSTRING** valueElements) override;

        IFACEMETHOD(GetSystemFontFamiliesFromLocaleList)(
            IVectorView<HSTRING>* localeList,
            uint32_t* valueCount,
            HSTRING** valueElements) override;
    };


    //
    // ICanvasTextFormatInternal
    //

    [uuid(E295AC1E-B763-49D4-9AE3-6E75D0C429AA)]
    class ICanvasTextFormatInternal : public IUnknown
    {
    public:
        virtual ComPtr<IDWriteTextFormat1> GetRealizedTextFormat() = 0;
        virtual ComPtr<IDWriteTextFormat> GetRealizedTextFormatClone(CanvasWordWrapping overrideWordWrapping) = 0;
        virtual D2D1_DRAW_TEXT_OPTIONS GetDrawTextOptions() = 0;
    };


    //
    // CanvasTextFormat provides a IDWriteTextFormat object.  Since some members
    // of IDWriteTextFormat are immutable (eg FontFamily), CanvasTextFormat
    // keeps track of shadow copies of properties and recreates ("realizes") an
    // IDWriteTextFormat as necessary.
    //
    class CanvasTextFormat : RESOURCE_WRAPPER_RUNTIME_CLASS(
        IDWriteTextFormat1,
        CanvasTextFormat,
        ICanvasTextFormat,
        CloakedIid<ICanvasTextFormatInternal>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Text_CanvasTextFormat, BaseTrust);

        std::shared_ptr<CustomFontManager> m_customFontManager;

        //
        // Has Close() been called?  It is tempting to use a null resource to
        // indicated closed, but we need to be able to tell the difference
        // between not having created the resource yet and being closed.
        //
        bool m_closed;

        //
        // The various bits of shadow state and the realized format all need to
        // be updated atomically.
        //
        std::mutex m_mutex;
        
        //
        // Shadow properties.  These values are used to recreate the IDWriteTextFormat when
        // it is required.
        //
        ComPtr<IDWriteFontCollection> m_fontCollection;
        CanvasTextDirection m_direction;
        WinString m_fontFamilyName;
        float m_fontSize;
        ABI::Windows::UI::Text::FontStretch m_fontStretch;
        ABI::Windows::UI::Text::FontStyle m_fontStyle;
        ABI::Windows::UI::Text::FontWeight m_fontWeight;
        float m_incrementalTabStop;
        float m_lineSpacing;
        float m_lineSpacingBaseline;
        WinString m_localeName;
        CanvasVerticalAlignment m_verticalAlignment;
        CanvasHorizontalAlignment m_horizontalAlignment;
        CanvasTextTrimmingGranularity m_trimmingGranularity;
        WinString m_trimmingDelimiter;
        int32_t m_trimmingDelimiterCount;
        CanvasWordWrapping m_wordWrapping;
        CanvasVerticalGlyphOrientation m_verticalGlyphOrientation;
        CanvasOpticalAlignment m_opticalAlignment;
        bool m_lastLineWrapping;

        TrimmingSignInformation m_trimmingSignInformation;

        //
        // Draw text options are not part of IDWriteTextFormat, but are stored
        // in CanvasTextFormat.  These are not protected by the mutex since they
        // are independent from the shadow state / format.
        //
        CanvasDrawTextOptions m_drawTextOptions;

        //
        // This type is used on Win10, and is defined as a placeholder on 8.1.
        //
        CanvasLineSpacingMode m_lineSpacingMode;

    public:
        CanvasTextFormat();
        CanvasTextFormat(IDWriteTextFormat1* format);

        //
        // ICanvasTextFormat
        //
#define PROPERTY(NAME, TYPE)                            \
        IFACEMETHOD(get_##NAME)(TYPE* value) override;   \
        IFACEMETHOD(put_##NAME)(TYPE value) override

        PROPERTY(Direction,                CanvasTextDirection);
        PROPERTY(FontFamily,               HSTRING);
        PROPERTY(FontSize,                 float);
        PROPERTY(FontStretch,              ABI::Windows::UI::Text::FontStretch);
        PROPERTY(FontStyle,                ABI::Windows::UI::Text::FontStyle);
        PROPERTY(FontWeight,               ABI::Windows::UI::Text::FontWeight);
        PROPERTY(IncrementalTabStop,       float);
        PROPERTY(LineSpacing,              float);
        PROPERTY(LineSpacingBaseline,      float);
        PROPERTY(LocaleName,               HSTRING);
        PROPERTY(VerticalAlignment,        CanvasVerticalAlignment);
        PROPERTY(HorizontalAlignment,      CanvasHorizontalAlignment);
        PROPERTY(TrimmingGranularity,      CanvasTextTrimmingGranularity);
        PROPERTY(TrimmingDelimiter,        HSTRING);
        PROPERTY(TrimmingDelimiterCount,   int32_t);
        PROPERTY(WordWrapping,             CanvasWordWrapping);
        PROPERTY(Options,                  CanvasDrawTextOptions);
        PROPERTY(VerticalGlyphOrientation, CanvasVerticalGlyphOrientation);
        PROPERTY(OpticalAlignment,         CanvasOpticalAlignment);
        PROPERTY(LastLineWrapping,         boolean);
        PROPERTY(TrimmingSign,             CanvasTrimmingSign);
        PROPERTY(CustomTrimmingSign,       ICanvasTextInlineObject*);

#if WINVER > _WIN32_WINNT_WINBLUE
        PROPERTY(LineSpacingMode,          CanvasLineSpacingMode);
#endif

#undef PROPERTY

        //
        // IClosable
        //

        IFACEMETHOD(Close)() override;

        //
        // ICanvasTextFormatInternal
        //

        virtual ComPtr<IDWriteTextFormat1> GetRealizedTextFormat() override;
        virtual ComPtr<IDWriteTextFormat> GetRealizedTextFormatClone(CanvasWordWrapping overrideWordWrapping) override;
        virtual D2D1_DRAW_TEXT_OPTIONS GetDrawTextOptions() override;

        //
        // ICanvasResourceWrapperNative
        //
        
        IFACEMETHOD(GetNativeResource)(ICanvasDevice* device, float dpi, REFIID iid, void** resource) override;

    private:
        Lock GetLock()
        {
            return Lock(m_mutex);
        }
        
        void ThrowIfClosed();

        template<typename T, typename ST, typename FN>
        HRESULT __declspec(nothrow) PropertyGet(T* value, ST const& shadowValue, FN realizedGetter);

        template<typename T, typename TT, typename FNV>
        HRESULT __declspec(nothrow) PropertyPut(T value, TT* dest, FNV&& validator, void(CanvasTextFormat::*realizer)(IDWriteTextFormat1*) = nullptr);
        
        template<typename T, typename TT>
        HRESULT __declspec(nothrow) PropertyPut(T value, TT* dest, void(CanvasTextFormat::*realizer)(IDWriteTextFormat1*) = nullptr);

        void SetShadowPropertiesFromDWrite();

        void Unrealize();

        void RealizeDirection(IDWriteTextFormat1* textFormat);
        void RealizeIncrementalTabStop(IDWriteTextFormat1* textFormat);
        void RealizeLineSpacing(IDWriteTextFormat1* textFormat);
        void RealizeParagraphAlignment(IDWriteTextFormat1* textFormat);
        void RealizeTextAlignment(IDWriteTextFormat1* textFormat);
        void RealizeTrimming(IDWriteTextFormat1* textFormat);
        void RealizeWordWrapping(IDWriteTextFormat1* textFormat);
        void RealizeVerticalGlyphOrientation(IDWriteTextFormat1* textFormat);
        void RealizeOpticalAlignment(IDWriteTextFormat1* textFormat);
        void RealizeLastLineWrapping(IDWriteTextFormat1* textFormat);
        void RealizeTrimmingSign(IDWriteTextFormat1* textFormat);
        void RealizeCustomTrimmingSign(IDWriteTextFormat1* textFormat);

        ComPtr<IDWriteTextFormat1> CreateRealizedTextFormat(bool skipWordWrapping = false);
};


    // Helper for getting line spacing from DWrite   
    struct DWriteLineSpacing
    {
        DWriteLineSpacing(IDWriteTextFormat* format)
        {
            ThrowIfFailed(format->GetLineSpacing(&Method, &Spacing, &Baseline));
        }

        DWRITE_LINE_SPACING_METHOD Method;
        float Spacing;
        float Baseline;

        float GetAdjustedSpacing() const
        {
            switch (Method)
            {
            case DWRITE_LINE_SPACING_METHOD_DEFAULT:
                return -Spacing;

            case DWRITE_LINE_SPACING_METHOD_UNIFORM:

#if WINVER > _WIN32_WINNT_WINBLUE
            case DWRITE_LINE_SPACING_METHOD_PROPORTIONAL:
#endif
            default:
                return Spacing;
            }
        }

#if WINVER > _WIN32_WINNT_WINBLUE
        CanvasLineSpacingMode GetAdjustedLineSpacingMode(bool allowUniform = false) const
        {
            if (Method == DWRITE_LINE_SPACING_METHOD_PROPORTIONAL)
            {
                return CanvasLineSpacingMode::Proportional;
            }
            else
            {
                return allowUniform? CanvasLineSpacingMode::Uniform : CanvasLineSpacingMode::Default;
            }
        }
#endif

        static void Set(
            IDWriteTextFormat* format,
            CanvasLineSpacingMode lineSpacingMode,
            float spacing, 
            float baseline)
        {
            //
            // The DWrite line spacing method is determined by the value of the
            // LineSpacing property, together with the LineSpacingMode
            // property (when available, on Win10+).  
            //
            //  If LineSpacingMode == Default (or always, on Win8.1):
            //     Use DWRITE_LINE_SPACING_METHOD_UNIFORM if LineSpacing is positive.
            //     Use DWRITE_LINE_SPACING_METHOD_DEFAULT if LineSpacing is negative.
            //
            //  If LineSpacingMode == Uniform
            //     Use DWRITE_LINE_SPACING_METHOD_UNIFORM.
            //
            //  If LineSpacingMode == Proportional
            //     Use DWRITE_LINE_SPACING_METHOD_PROPORTIONAL.
            //
            //
            // In the DEFAULT method, the actual value of m_lineSpacing is ignored, 
            // although it is still validated to be non-negative.  
            // For this reason we pass in the fabs of the value
            // that was set.  This also allows us to round-trip the values in that 
            // case.
            //

            //
            // signbit() is used so we can tell the difference between 0.0 and
            // -0.0.
            //
            DWRITE_LINE_SPACING_METHOD method = signbit(spacing)
                ? DWRITE_LINE_SPACING_METHOD_DEFAULT
                : DWRITE_LINE_SPACING_METHOD_UNIFORM;

#if WINVER > _WIN32_WINNT_WINBLUE
            if (lineSpacingMode == CanvasLineSpacingMode::Uniform)
            {
                method = DWRITE_LINE_SPACING_METHOD_UNIFORM;
            }
            else if (lineSpacingMode == CanvasLineSpacingMode::Proportional)
            {
                method = DWRITE_LINE_SPACING_METHOD_PROPORTIONAL;
            }
#else
            UNREFERENCED_PARAMETER(lineSpacingMode);
#endif

            ThrowIfFailed(format->SetLineSpacing(
                method,
                fabs(spacing),
                baseline));
        }
    };


    // Helper for getting trimming from DWrite
    struct DWriteTrimming
    {
        DWriteTrimming(IDWriteTextFormat* format)
        {
            ThrowIfFailed(format->GetTrimming(
                &Options,
                &Sign));
        }

        DWRITE_TRIMMING Options;
        ComPtr<IDWriteInlineObject> Sign;
    };

    // Struct that maps CanvasTextDirection to DWRITE_READING|FLOW_DIRECTION
    struct DWriteToCanvasTextDirection
    {
        DWRITE_READING_DIRECTION ReadingDirection;
        DWRITE_FLOW_DIRECTION FlowDirection;
        CanvasTextDirection TextDirection;

        static DWriteToCanvasTextDirection const* Lookup(DWRITE_READING_DIRECTION readingDirection, DWRITE_FLOW_DIRECTION flowDirection);
        static DWriteToCanvasTextDirection const* Lookup(CanvasTextDirection textDirection);
    };
    
}}}}}

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/brushes/CanvasSolidColorBrush.h>
#include <lib/brushes/CanvasLinearGradientBrush.h>
#include <lib/text/CanvasTextLayout.h>

#include "stubs/StubCanvasBrush.h"
#include "stubs/StubCanvasTextLayoutAdapter.h"
#include "stubs/TestEffect.h"
#include "stubs/CustomInlineObject.h"

#include "mocks/MockDWriteTextRenderer.h"

namespace canvas
{
    using namespace ABI::Windows::UI::Text;

    static const CanvasTrimmingSign sc_trimmingSigns[] = { CanvasTrimmingSign::None, CanvasTrimmingSign::Ellipsis };

    TEST_CLASS(CanvasTextLayoutTests)
    {
    public:

        struct Fixture
        {
            ComPtr<ICanvasTextFormat> Format;
            std::shared_ptr<StubCanvasTextLayoutAdapter> Adapter;
            ComPtr<MockD2DDeviceContext> DeviceContext;
            ComPtr<StubCanvasDevice> Device;
            std::vector<ComPtr<MockD2DEffectThatCountsCalls>> MockEffects;

            Fixture()
                : Adapter(std::make_shared<StubCanvasTextLayoutAdapter>())
                , Device(Make<StubCanvasDevice>())
                , DeviceContext(Make<MockD2DDeviceContext>())
            {
                CustomFontManagerAdapter::SetInstance(Adapter);

                Format = Make<CanvasTextFormat>();

                Device->GetResourceCreationDeviceContextMethod.AllowAnyCall(
                    [=]
                    {
                        return DeviceContextLease(DeviceContext);
                    });

                Device->MockCreateBitmapBrush =
                    [&](ID2D1Bitmap1*)
                    {
                        auto bitmapBrush = Make<MockD2DBitmapBrush>();
                        bitmapBrush->MockGetBitmap = [&](ID2D1Bitmap** bitmap){  *bitmap = nullptr; };
                        bitmapBrush->MockSetBitmap = [&](ID2D1Bitmap* bitmap){};
                        bitmapBrush->MockGetExtendModeX = [&]() { return D2D1_EXTEND_MODE_MIRROR; };
                        bitmapBrush->MockGetExtendModeY = [&]() { return D2D1_EXTEND_MODE_WRAP; };
                        bitmapBrush->MockGetInterpolationMode1 = [&]() { return D2D1_INTERPOLATION_MODE_ANISOTROPIC; };
                        bitmapBrush->MockGetOpacity = [&]() { return 0.1f; };
                        bitmapBrush->MockGetTransform = [&](D2D1_MATRIX_3X2_F* transform) { *transform = D2D1_MATRIX_3X2_F{}; };
                        bitmapBrush->MockSetExtendModeX = [&](D2D1_EXTEND_MODE extend) {};
                        bitmapBrush->MockSetExtendModeY = [&](D2D1_EXTEND_MODE extend) {};
                        bitmapBrush->MockSetInterpolationMode1 = [&](D2D1_INTERPOLATION_MODE mode) {};
                        bitmapBrush->MockSetOpacity = [&](float opacity) {};
                        bitmapBrush->MockSetTransform = [&](const D2D1_MATRIX_3X2_F* transform) {};
                        return bitmapBrush;
                    };

                Device->MockCreateImageBrush =
                    [&](ID2D1Image* initialImage)
                    {
                        auto imageBrush = Make<MockD2DImageBrush>();
                        auto currentImage = std::make_shared<ComPtr<ID2D1Image>>(initialImage);
                        imageBrush->MockGetImage = [currentImage](ID2D1Image** image) { currentImage->CopyTo(image); };
                        imageBrush->MockSetImage = [currentImage](ID2D1Image* image) { *currentImage = image; };
                        imageBrush->MockGetExtendModeX = [&]() { return D2D1_EXTEND_MODE_MIRROR; };
                        imageBrush->MockGetExtendModeY = [&]() { return D2D1_EXTEND_MODE_WRAP; };
                        imageBrush->MockGetInterpolationMode = [&]() { return D2D1_INTERPOLATION_MODE_ANISOTROPIC; };
                        imageBrush->MockGetOpacity = [&]() { return 0.1f; };
                        imageBrush->MockGetTransform = [&](D2D1_MATRIX_3X2_F* transform) { *transform = D2D1_MATRIX_3X2_F{}; };
                        imageBrush->MockGetSourceRectangle = [&](D2D1_RECT_F* rect) { *rect = D2D1::RectF(0, 0, 10, 10); };
                        imageBrush->MockSetExtendModeX = [&](D2D1_EXTEND_MODE extend) {};
                        imageBrush->MockSetExtendModeY = [&](D2D1_EXTEND_MODE extend) {};
                        imageBrush->MockSetInterpolationMode = [&](D2D1_INTERPOLATION_MODE mode) {};
                        imageBrush->MockSetOpacity = [&](float opacity) {};
                        imageBrush->MockSetTransform = [&](const D2D1_MATRIX_3X2_F* transform) {};
                        imageBrush->MockSetSourceRectangle = [&](const D2D1_RECT_F* rect) {};
                        return imageBrush;
                    };                

                DeviceContext->CreateEffectMethod.AllowAnyCall(
                    [=](IID const& effectId, ID2D1Effect** effect)
                    {
                        MockEffects.push_back(Make<MockD2DEffectThatCountsCalls>(effectId));
                        return MockEffects.back().CopyTo(effect);
                    });

                DeviceContext->GetDeviceMethod.AllowAnyCallAlwaysCopyValueToParam(Device->GetD2DDevice());

                DeviceContext->GetDpiMethod.AllowAnyCall(
                    [&](float* dpiX, float* dpiY)
                    {
                        *dpiX = DEFAULT_DPI;
                        *dpiY = DEFAULT_DPI;
                    });
            }

            ComPtr<CanvasTextLayout> CreateSimpleTextLayout()
            {
                return CanvasTextLayout::CreateNew(Device.Get(), WinString(L"A string"), Format.Get(), 0.0f, 0.0f);
            }
        };

        TEST_METHOD_EX(CanvasTextLayoutTests_Implements_Expected_Interfaces)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            ASSERT_IMPLEMENTS_INTERFACE(textLayout, ICanvasTextLayout);
            ASSERT_IMPLEMENTS_INTERFACE(textLayout, ABI::Windows::Foundation::IClosable);
            ASSERT_IMPLEMENTS_INTERFACE(textLayout, ICanvasResourceWrapperNative);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_Closure)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();
            
            Assert::AreEqual(S_OK, textLayout->Close());

            CanvasTextDirection direction{};
            HSTRING str{};
            float fl{};
            FontStretch fontStretch{};
            FontStyle fontStyle{};
            FontWeight fontWeight{};
            CanvasVerticalAlignment verticalAlignment{};
            CanvasHorizontalAlignment horizontalAlignment{};
            CanvasTextTrimmingGranularity trimmingGranularity{};
            INT32 i{};
            INT32* arr{};
            uint32_t u{};
            CanvasWordWrapping wordWrapping{};
            CanvasDrawTextOptions drawTextOptions{};
            Size size{};
            boolean b{};
            CanvasVerticalGlyphOrientation verticalGlyphOrientation{};
            CanvasOpticalAlignment opticalAlignment{};
            Rect rect{};
            CanvasTextLayoutRegion hitTestDesc{};
            Vector2 pt{};
            CanvasTextLayoutRegion* hitTestDescArr{};
            ComPtr<ICanvasBrush> canvasBrush;
            ComPtr<ICanvasDevice> canvasDevice;
            CanvasTrimmingSign ts;
            ComPtr<ICanvasTextInlineObject> inlineObj;
            CanvasLineMetrics* lm{};

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetFormatChangeIndices(&u, &arr));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_Direction(&direction));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_Direction(direction));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_DefaultFontFamily(&str));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_DefaultFontSize(&fl));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_DefaultFontStretch(&fontStretch));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_DefaultFontStyle(&fontStyle));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_DefaultFontWeight(&fontWeight));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_IncrementalTabStop(&fl));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_IncrementalTabStop(fl));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_LineSpacing(&fl));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_LineSpacing(fl));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_LineSpacingBaseline(&fl));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_LineSpacingBaseline(fl));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_DefaultLocaleName(&str));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_VerticalAlignment(&verticalAlignment));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_VerticalAlignment(verticalAlignment));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_HorizontalAlignment(&horizontalAlignment));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_HorizontalAlignment(horizontalAlignment));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_TrimmingGranularity(&trimmingGranularity));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_TrimmingGranularity(trimmingGranularity));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_TrimmingDelimiter(&str));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_TrimmingDelimiter(str));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_TrimmingDelimiterCount(&i));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_TrimmingDelimiterCount(i));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_WordWrapping(&wordWrapping));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_WordWrapping(wordWrapping));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_Options(&drawTextOptions));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_Options(drawTextOptions));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_RequestedSize(&size));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_RequestedSize(size));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetMinimumLineLength(&fl));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetFontFamily(0, &str));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetFontFamily(0, 0, str));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetFontSize(0, &fl));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetFontSize(0, 0, fl));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetFontStretch(0, &fontStretch));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetFontStretch(0, 0, fontStretch));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetFontStyle(0, &fontStyle));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetFontStyle(0, 0, fontStyle));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetFontWeight(0, &fontWeight));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetFontWeight(0, 0, fontWeight));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetLocaleName(0, &str));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetLocaleName(0, 0, str));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetStrikethrough(0, &b));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetStrikethrough(0, 0, b));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetUnderline(0, &b));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetUnderline(0, 0, b));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetPairKerning(0, &b));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetPairKerning(0, 0, b));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetLeadingCharacterSpacing(0, &fl));
            Assert::AreEqual(RO_E_CLOSED, textLayout->GetTrailingCharacterSpacing(0, &fl));
            Assert::AreEqual(RO_E_CLOSED, textLayout->GetMinimumCharacterAdvance(0, &fl));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetCharacterSpacing(0, 0, fl, fl, fl));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_VerticalGlyphOrientation(&verticalGlyphOrientation));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_VerticalGlyphOrientation(verticalGlyphOrientation));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_OpticalAlignment(&opticalAlignment));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_OpticalAlignment(opticalAlignment));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_LastLineWrapping(&b));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_LastLineWrapping(b));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_TrimmingSign(&ts));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_TrimmingSign(ts));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_LayoutBounds(&rect));
            Assert::AreEqual(RO_E_CLOSED, textLayout->get_LineCount(&i));
            Assert::AreEqual(RO_E_CLOSED, textLayout->get_DrawBounds(&rect));
            Assert::AreEqual(RO_E_CLOSED, textLayout->HitTest(Vector2{}, &b));
            Assert::AreEqual(RO_E_CLOSED, textLayout->HitTestWithCoords(0, 0, &b));
            Assert::AreEqual(RO_E_CLOSED, textLayout->HitTestWithDescription(Vector2{}, &hitTestDesc, &b));
            Assert::AreEqual(RO_E_CLOSED, textLayout->HitTestWithDescriptionAndCoords(0, 0, &hitTestDesc, &b));
            Assert::AreEqual(RO_E_CLOSED, textLayout->GetCaretPosition(0, b, &pt));
            Assert::AreEqual(RO_E_CLOSED, textLayout->GetCaretPositionWithDescription(0, b, &hitTestDesc, &pt));
            Assert::AreEqual(RO_E_CLOSED, textLayout->GetCharacterRegions(0, 0, &u, &hitTestDescArr));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetBrush(0, &canvasBrush));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetBrush(0, 0, Make<StubCanvasBrush>().Get()));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetColor(0, 0, Color{}));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_Device(&canvasDevice));

            Assert::AreEqual(RO_E_CLOSED, textLayout->DrawToTextRenderer(reinterpret_cast<ICanvasTextRenderer*>(0x12345678), Vector2{ 0, 0 }));
            Assert::AreEqual(RO_E_CLOSED, textLayout->DrawToTextRendererWithCoords(reinterpret_cast<ICanvasTextRenderer*>(0x12345678), 0, 0));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_CustomTrimmingSign(&inlineObj));
            Assert::AreEqual(RO_E_CLOSED, textLayout->put_CustomTrimmingSign(inlineObj.Get()));

            Assert::AreEqual(RO_E_CLOSED, textLayout->GetInlineObject(0, &inlineObj));
            Assert::AreEqual(RO_E_CLOSED, textLayout->SetInlineObject(0, 0, inlineObj.Get()));

            Assert::AreEqual(RO_E_CLOSED, textLayout->get_LineMetrics(&u, &lm));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_NullArgs)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            INT32* arr;
            CanvasTextLayoutRegion hitTestDesc{};
            CanvasTextLayoutRegion* hitTestDescArr{};
            boolean b{};
            CanvasLineMetrics* lm{};
            uint32_t u{};
            Assert::AreEqual(E_INVALIDARG, textLayout->GetFormatChangeIndices(nullptr, &arr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_Direction(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_DefaultFontFamily(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_DefaultFontSize(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_DefaultFontStretch(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_DefaultFontStyle(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_DefaultFontWeight(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_IncrementalTabStop(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_LineSpacing(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_LineSpacingBaseline(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_DefaultLocaleName(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_VerticalAlignment(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_HorizontalAlignment(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_TrimmingGranularity(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_TrimmingDelimiter(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_TrimmingDelimiterCount(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_WordWrapping(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_RequestedSize(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetMinimumLineLength(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontFamily(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontSize(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontStretch(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontStyle(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontWeight(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetLocaleName(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetStrikethrough(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetUnderline(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetPairKerning(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetLeadingCharacterSpacing(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetTrailingCharacterSpacing(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetMinimumCharacterAdvance(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_VerticalGlyphOrientation(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_OpticalAlignment(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_LastLineWrapping(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_LayoutBounds(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_LineCount(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_DrawBounds(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->HitTest(Vector2{}, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->HitTestWithCoords(0, 0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->HitTestWithDescription(Vector2{}, &hitTestDesc, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->HitTestWithDescriptionAndCoords(0, 0, &hitTestDesc, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetCaretPosition(0, b, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetCaretPositionWithDescription(0, b, &hitTestDesc, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetCharacterRegions(0, 0, nullptr, &hitTestDescArr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetBrush(0, nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_Device(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_TrimmingSign(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_CustomTrimmingSign(nullptr));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_LineMetrics(nullptr, &lm));
            Assert::AreEqual(E_INVALIDARG, textLayout->get_LineMetrics(&u, nullptr));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_NegativeIntegralArgs)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            WinString stringIn(L"any_string");
            HSTRING stringOut;
            float fl;
            FontStretch fontStretch;
            FontStyle fontStyle;
            FontWeight fontWeight;
            boolean b;
            CanvasTextLayoutRegion hitTestDesc{};
            Vector2 pt{};
            CanvasTextLayoutRegion* hitTestDescArr{};
            uint32_t u{};
            ComPtr<ICanvasTextInlineObject> inlineObj;

            Assert::AreEqual(E_INVALIDARG, textLayout->put_TrimmingDelimiterCount(-1));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontFamily(-1, &stringOut));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontFamily(-1, 0, stringIn));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontFamily(0, -1, stringIn));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontSize(-1, &fl));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontSize(-1, 0, fl));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontSize(0, -1, fl));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontStretch(-1, &fontStretch));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontStretch(-1, 0, fontStretch));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontStretch(0, -1, fontStretch));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontStyle(-1, &fontStyle));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontStyle(-1, 0, fontStyle));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontStyle(0, -1, fontStyle));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetFontWeight(-1, &fontWeight));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontWeight(-1, 0, fontWeight));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetFontWeight(0, -1, fontWeight));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetLocaleName(-1, &stringOut));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetLocaleName(-1, 0, stringIn));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetLocaleName(0, -1, stringIn));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetStrikethrough(-1, &b));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetStrikethrough(-1, 0, b));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetStrikethrough(0, -1, b));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetUnderline(-1, &b));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetUnderline(-1, 0, b));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetUnderline(0, -1, b));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetPairKerning(-1, &b));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetPairKerning(-1, 0, b));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetPairKerning(0, -1, b));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetLeadingCharacterSpacing(-1, &fl));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetTrailingCharacterSpacing(-1, &fl));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetMinimumCharacterAdvance(-1, &fl));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetCharacterSpacing(-1, 0, fl, fl, fl));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetCharacterSpacing(0, -1, fl, fl, fl));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetCaretPosition(-1, false, &pt));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetCaretPositionWithDescription(-1, false, &hitTestDesc, &pt));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetCharacterRegions(-1, 0, &u, &hitTestDescArr));
            Assert::AreEqual(E_INVALIDARG, textLayout->GetCharacterRegions(0, -1, &u, &hitTestDescArr));

            ComPtr<ICanvasBrush> stubBrush = Make<StubCanvasBrush>();
            Assert::AreEqual(E_INVALIDARG, textLayout->GetBrush(-1, &stubBrush));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetBrush(-1, 0, stubBrush.Get()));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetBrush(0, -1, stubBrush.Get()));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetColor(-1, 0, Color{}));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetColor(0, -1, Color{}));

            Assert::AreEqual(E_INVALIDARG, textLayout->GetInlineObject(-1, &inlineObj));
            Assert::AreEqual(E_INVALIDARG, textLayout->SetInlineObject(-1, 0, inlineObj.Get()));
        }

        //
        // CanvasTextLayout.Direction is special since it actually sets two
        // IDWriteTextLayout properties.
        //
        TEST_METHOD_EX(CanvasTextLayoutTests_get_Direction)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            f.Adapter->MockTextLayout->GetReadingDirectionMethod.SetExpectedCalls(1, []{ return DWRITE_READING_DIRECTION_BOTTOM_TO_TOP; });
            f.Adapter->MockTextLayout->GetFlowDirectionMethod.SetExpectedCalls(1, []{ return DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT; });

            CanvasTextDirection actualDirection;
            Assert::AreEqual(S_OK, textLayout->get_Direction(&actualDirection));
            Assert::AreEqual(CanvasTextDirection::BottomToTopThenRightToLeft, actualDirection);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_put_Direction)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            f.Adapter->MockTextLayout->SetReadingDirectionMethod.SetExpectedCalls(1,
                [] (DWRITE_READING_DIRECTION readingDirection)
                {
                    Assert::AreEqual(DWRITE_READING_DIRECTION_BOTTOM_TO_TOP, readingDirection);
                    return S_OK;
                });

            f.Adapter->MockTextLayout->SetFlowDirectionMethod.SetExpectedCalls(1,
                [] (DWRITE_FLOW_DIRECTION flowDirection)
                {
                    Assert::AreEqual(DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT, flowDirection);
                    return S_OK;
                });

            Assert::AreEqual(S_OK, textLayout->put_Direction(CanvasTextDirection::BottomToTopThenRightToLeft));
        }

        //
        // All of the 'simple' properties which don't operate on a string range of text layout are
        // grouped together and tested using the macros below.
        //

#define TEST_METHOD_GLOBAL_GETTER(name, mockName, dwriteValue, canvasValue) \
        TEST_METHOD_EX(CanvasTextLayoutTests_get_##name)                \
        {                                                               \
            Fixture f;                                                  \
                                                                        \
            auto textLayout = f.CreateSimpleTextLayout();               \
                                                                        \
            f.Adapter->MockTextLayout->##mockName##.SetExpectedCalls(1, \
                [&]()                                                   \
            {                                                           \
                return dwriteValue;                                     \
            });                                                         \
                                                                        \
            decltype(canvasValue) value;                                \
            Assert::AreEqual(S_OK, textLayout->get_##name##(&value));   \
            Assert::AreEqual(canvasValue, value);                       \
        }  

#define TEST_METHOD_GLOBAL_SETTER(name, mockName, dwriteValue, canvasValue)                       \
        TEST_METHOD_EX(CanvasTextLayoutTests_put_##name)                                          \
        {                                                                                         \
            Fixture f;                                                                            \
                                                                                                  \
            auto textLayout = f.CreateSimpleTextLayout();                                         \
                                                                                                  \
            f.Adapter->MockTextLayout->##mockName##.SetExpectedCalls(1,                           \
                [&](decltype(dwriteValue) value)                                                  \
                {                                                                                 \
                    Assert::AreEqual(dwriteValue, value);                                         \
                    return S_OK;                                                                  \
                });                                                                               \
            Assert::AreEqual(S_OK, textLayout->put_##name##(canvasValue));                        \
        }

        TEST_METHOD_GLOBAL_GETTER(DefaultFontSize, GetFontSize_BaseFormat_Method, 99.0f, 99.0f);
        TEST_METHOD_GLOBAL_GETTER(DefaultFontStretch, GetFontStretch_BaseFormat_Method, DWRITE_FONT_STRETCH_SEMI_EXPANDED, FontStretch_SemiExpanded);
        TEST_METHOD_GLOBAL_GETTER(DefaultFontStyle, GetFontStyle_BaseFormat_Method, DWRITE_FONT_STYLE_ITALIC, FontStyle_Italic);
        TEST_METHOD_GLOBAL_GETTER(DefaultFontWeight, GetFontWeight_BaseFormat_Method, DWRITE_FONT_WEIGHT_DEMI_BOLD, FontWeight{ 600 });

        TEST_METHOD_GLOBAL_GETTER(IncrementalTabStop, GetIncrementalTabStopMethod, 99.0f, 99.0f);
        TEST_METHOD_GLOBAL_SETTER(IncrementalTabStop, SetIncrementalTabStopMethod, 99.0f, 99.0f);

        TEST_METHOD_GLOBAL_GETTER(VerticalAlignment, GetParagraphAlignmentMethod, DWRITE_PARAGRAPH_ALIGNMENT_FAR, CanvasVerticalAlignment::Bottom);
        TEST_METHOD_GLOBAL_SETTER(VerticalAlignment, SetParagraphAlignmentMethod, DWRITE_PARAGRAPH_ALIGNMENT_FAR, CanvasVerticalAlignment::Bottom);

        TEST_METHOD_GLOBAL_GETTER(HorizontalAlignment, GetTextAlignmentMethod, DWRITE_TEXT_ALIGNMENT_TRAILING, CanvasHorizontalAlignment::Right);
        TEST_METHOD_GLOBAL_SETTER(HorizontalAlignment, SetTextAlignmentMethod, DWRITE_TEXT_ALIGNMENT_TRAILING, CanvasHorizontalAlignment::Right);

        TEST_METHOD_GLOBAL_GETTER(WordWrapping, GetWordWrappingMethod, DWRITE_WORD_WRAPPING_EMERGENCY_BREAK, CanvasWordWrapping::EmergencyBreak);
        TEST_METHOD_GLOBAL_SETTER(WordWrapping, SetWordWrappingMethod, DWRITE_WORD_WRAPPING_EMERGENCY_BREAK, CanvasWordWrapping::EmergencyBreak);

        TEST_METHOD_GLOBAL_GETTER(VerticalGlyphOrientation, GetVerticalGlyphOrientationMethod, DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED, CanvasVerticalGlyphOrientation::Stacked);
        TEST_METHOD_GLOBAL_SETTER(VerticalGlyphOrientation, SetVerticalGlyphOrientationMethod, DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED, CanvasVerticalGlyphOrientation::Stacked);

        TEST_METHOD_GLOBAL_GETTER(OpticalAlignment, GetOpticalAlignmentMethod, DWRITE_OPTICAL_ALIGNMENT_NO_SIDE_BEARINGS, CanvasOpticalAlignment::NoSideBearings);
        TEST_METHOD_GLOBAL_SETTER(OpticalAlignment, SetOpticalAlignmentMethod, DWRITE_OPTICAL_ALIGNMENT_NO_SIDE_BEARINGS, CanvasOpticalAlignment::NoSideBearings);

        TEST_METHOD_GLOBAL_GETTER(LastLineWrapping, GetLastLineWrappingMethod, true, static_cast<boolean>(true));
        TEST_METHOD_GLOBAL_SETTER(LastLineWrapping, SetLastLineWrappingMethod, static_cast<BOOL>(true), static_cast<boolean>(true));


        //
        // The simple properties which operate on a string range are grouped together, and tested below.
        //        

#define TEST_METHOD_RANGE_GETTER(name, mockName, dwriteValue, canvasValue)                          \
        TEST_METHOD_EX(CanvasTextLayoutTests_Get##name##)                                           \
        {                                                                                           \
            Fixture f;                                                                              \
                                                                                                    \
            f.Adapter->MockTextLayout->##mockName##.SetExpectedCalls(1,                             \
                [&](UINT32 currentPosition, decltype(dwriteValue)* out, DWRITE_TEXT_RANGE* textRange) \
                {                                                                                   \
                    Assert::AreEqual(12345u, currentPosition);                                      \
                    Assert::IsNull(textRange);                                                      \
                    *out = dwriteValue;                                                             \
                    return S_OK;                                                                    \
                });                                                                                 \
                                                                                                    \
            auto textLayout = f.CreateSimpleTextLayout();                                           \
                                                                                                    \
            decltype(canvasValue) value;                                                            \
            Assert::AreEqual(S_OK, textLayout->Get##name##(12345, &value));                         \
            Assert::AreEqual(canvasValue, value);                                                   \
        }

#define TEST_METHOD_RANGE_SETTER(name, mockName, dwriteValue, canvasValue)                          \
        TEST_METHOD_EX(CanvasTextLayoutTests_Set##name##)                                           \
        {                                                                                           \
            Fixture f;                                                                              \
                                                                                                    \
            f.Adapter->MockTextLayout->##mockName##.SetExpectedCalls(1,                             \
                [&](decltype(dwriteValue) value, DWRITE_TEXT_RANGE textRange)                       \
                {                                                                                   \
                    Assert::AreEqual(123u, textRange.startPosition);                                \
                    Assert::AreEqual(456u, textRange.length);                                       \
                    Assert::AreEqual(dwriteValue, value);                                           \
                    return S_OK;                                                                    \
                });                                                                                 \
                                                                                                    \
            auto textLayout = f.CreateSimpleTextLayout();                                           \
                                                                                                    \
            Assert::AreEqual(S_OK, textLayout->Set##name##(123, 456, canvasValue));                 \
        }

        TEST_METHOD_RANGE_GETTER(FontSize, GetFontSizeMethod, 16.0f, 16.0f);
        TEST_METHOD_RANGE_GETTER(FontStretch, GetFontStretchMethod, DWRITE_FONT_STRETCH_SEMI_EXPANDED, FontStretch_SemiExpanded);
        TEST_METHOD_RANGE_GETTER(FontStyle, GetFontStyleMethod, DWRITE_FONT_STYLE_ITALIC, FontStyle_Italic);
        TEST_METHOD_RANGE_GETTER(FontWeight, GetFontWeightMethod, DWRITE_FONT_WEIGHT_DEMI_BOLD, FontWeight{ 600 });

        TEST_METHOD_RANGE_GETTER(Strikethrough, GetStrikethroughMethod, TRUE, static_cast<boolean>(true));
        TEST_METHOD_RANGE_GETTER(Underline, GetUnderlineMethod, TRUE, static_cast<boolean>(true));

        TEST_METHOD_RANGE_SETTER(FontSize, SetFontSizeMethod, 99.0f, 99.0f);
        TEST_METHOD_RANGE_SETTER(FontStretch, SetFontStretchMethod, DWRITE_FONT_STRETCH_SEMI_EXPANDED, FontStretch_SemiExpanded);

        TEST_METHOD_RANGE_SETTER(FontStyle, SetFontStyleMethod, DWRITE_FONT_STYLE_ITALIC, FontStyle_Italic);
        TEST_METHOD_RANGE_SETTER(FontWeight, SetFontWeightMethod, DWRITE_FONT_WEIGHT_DEMI_BOLD, FontWeight{ 600 });

        TEST_METHOD_RANGE_SETTER(Strikethrough, SetStrikethroughMethod, TRUE, static_cast<boolean>(true));
        TEST_METHOD_RANGE_SETTER(Underline, SetUnderlineMethod, TRUE, static_cast<boolean>(true));

        TEST_METHOD_RANGE_GETTER(PairKerning, GetPairKerningMethod, TRUE, static_cast<boolean>(true));
        TEST_METHOD_RANGE_SETTER(PairKerning, SetPairKerningMethod, TRUE, static_cast<boolean>(true));

        //
        // The properties which don't follow common patterns are tested below.
        //

        TEST_METHOD_EX(CanvasTextLayoutTests_get_DefaultFontFamily)
        {
            Fixture f;

            std::wstring fontFamily = L"SomeFontFamily";

            f.Adapter->MockTextLayout->GetFontFamilyNameLength_BaseFormat_Method.AllowAnyCall([&] { return static_cast<uint32_t>(fontFamily.length()); });
            
            f.Adapter->MockTextLayout->GetFontFamilyName_BaseFormat_Method.SetExpectedCalls(1,
                [&](WCHAR* fontFamilyName, UINT32 nameSize)
                {        
                    Assert::AreEqual(static_cast<uint32_t>(fontFamily.length()) + 1, nameSize);
                    wmemcpy_s(fontFamilyName, nameSize, fontFamily.c_str(), fontFamily.length());
                    fontFamilyName[nameSize - 1] = 0;
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();
         
            HSTRING value;
            Assert::AreEqual(S_OK, textLayout->get_DefaultFontFamily(&value));

            Assert::AreEqual(fontFamily.c_str(), WindowsGetStringRawBuffer(value, nullptr));
        }

        template<class GET_LINE_SPACING_METHOD>
        void InitializeGetLineSpacingMethod(GET_LINE_SPACING_METHOD& getLineSpacingMethod, DWRITE_LINE_SPACING_METHOD method, FLOAT spacing, FLOAT baseline)
        {            
            getLineSpacingMethod.SetExpectedCalls(1,
                [method, spacing, baseline](DWRITE_LINE_SPACING_METHOD* lineSpacingMethod, FLOAT* lineSpacing, FLOAT* lineBaseline)
                {
                    *lineSpacingMethod = method;
                    *lineSpacing = spacing;
                    *lineBaseline = baseline;
                    return S_OK;
                });
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_LineSpacing)
        {
            float const anyBaseline = 123.0f;

            struct Case { float DWriteValue; DWRITE_LINE_SPACING_METHOD Method; float Value; };

            Case cases[] = { {  0.0f, DWRITE_LINE_SPACING_METHOD_DEFAULT,  0.0f },
                             {  0.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM,  0.0f },
                             {  1.0f, DWRITE_LINE_SPACING_METHOD_DEFAULT, -1.0f },
                             {  1.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM,  1.0f } };            

            for (auto c : cases)
            {
                Fixture f;
                auto l = f.CreateSimpleTextLayout();

                InitializeGetLineSpacingMethod(f.Adapter->MockTextLayout->GetLineSpacingMethod, c.Method, c.DWriteValue, anyBaseline);

                float spacing{};
                ThrowIfFailed(l->get_LineSpacing(&spacing));

                Assert::AreEqual(c.Value, spacing);
            }
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_LineSpacingBaseline)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();
            InitializeGetLineSpacingMethod(f.Adapter->MockTextLayout->GetLineSpacingMethod, DWRITE_LINE_SPACING_METHOD_DEFAULT, 0, 456.0f);

            float fl;
            Assert::AreEqual(S_OK, textLayout->get_LineSpacingBaseline(&fl));
            Assert::AreEqual(456.0f, fl);
        }

#if WINVER > _WIN32_WINNT_WINBLUE

        TEST_METHOD_EX(CanvasTextLayoutTests_LineSpacingMode)
        {
            struct TestCase
            {
                CanvasLineSpacingMode OriginalLineSpacingMode;
                float OriginalLineSpacing;

                DWRITE_LINE_SPACING_METHOD UnwrappedLineSpacingMethod;
                float UnwrappedLineSpacing;

                CanvasLineSpacingMode WrappedLineSpacingMode;
                float WrappedLineSpacing;
            } testCases[]
            {
                { CanvasLineSpacingMode::Default, 5.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM, 5.0f, CanvasLineSpacingMode::Default, 5.0f },
                { CanvasLineSpacingMode::Default, -1.0f, DWRITE_LINE_SPACING_METHOD_DEFAULT, 1.0f, CanvasLineSpacingMode::Default, -1.0f },

                { CanvasLineSpacingMode::Uniform, 5.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM, 5.0f, CanvasLineSpacingMode::Default, 5.0f },
                { CanvasLineSpacingMode::Uniform, -11.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM, 11.0f, CanvasLineSpacingMode::Default, 11.0f },

                { CanvasLineSpacingMode::Proportional, 5.0f, DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, 5.0f, CanvasLineSpacingMode::Proportional, 5.0f },
                { CanvasLineSpacingMode::Proportional, -22.0f, DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, 22.0f, CanvasLineSpacingMode::Proportional, 22.0f }
            };

            for (auto testCase : testCases)
            {
                Fixture f;
                auto textLayout = f.CreateSimpleTextLayout();

                textLayout->put_LineSpacingMode(testCase.OriginalLineSpacingMode);
                textLayout->put_LineSpacing(testCase.OriginalLineSpacing);

                auto dtl = GetWrappedResource<DWriteTextLayoutType>(textLayout);

                // Make sure the resulting DWrite text layout has the correct properties.
                DWRITE_LINE_SPACING_METHOD dwriteMethod;
                float dwriteSpacing, unusedBaseline;
                ThrowIfFailed(static_cast<IDWriteTextLayout2*>(dtl.Get())->GetLineSpacing(&dwriteMethod, &dwriteSpacing, &unusedBaseline));
                Assert::AreEqual(testCase.UnwrappedLineSpacing, dwriteSpacing);
                Assert::AreEqual(testCase.UnwrappedLineSpacingMethod, dwriteMethod);

                textLayout.Reset();

                textLayout = Make<CanvasTextLayout>(f.Device.Get(), dtl.Get());

                float wrappedSpacingValue;
                ThrowIfFailed(textLayout->get_LineSpacing(&wrappedSpacingValue));
                Assert::AreEqual(testCase.WrappedLineSpacing, wrappedSpacingValue);

                CanvasLineSpacingMode wrappedSpacingMode;
                ThrowIfFailed(textLayout->get_LineSpacingMode(&wrappedSpacingMode));
                Assert::AreEqual(testCase.WrappedLineSpacingMode, wrappedSpacingMode);
            }
        }

        TEST_METHOD_EX(CanvasTextLayout_LineSpacingMode_RealizationDoesntClobberUniform)
        {
            Fixture f;
            auto ctl = f.CreateSimpleTextLayout();

            ThrowIfFailed(ctl->put_LineSpacing(5.0f));
            ThrowIfFailed(ctl->put_LineSpacingMode(CanvasLineSpacingMode::Uniform));

            // Force realization
            auto dtl = GetWrappedResource<IDWriteTextLayout2>(ctl);

            // Ensure still Uniform
            CanvasLineSpacingMode spacingMode;
            ThrowIfFailed(ctl->get_LineSpacingMode(&spacingMode));
            Assert::AreEqual(CanvasLineSpacingMode::Uniform, spacingMode);
        }

#endif

        template<class SET_LINE_SPACING_METHOD>
        void InitializeSetLineSpacingMethod(SET_LINE_SPACING_METHOD& setLineSpacingMethod, DWRITE_LINE_SPACING_METHOD method, FLOAT spacing, FLOAT baseline)
        {
            setLineSpacingMethod.SetExpectedCalls(1,
                [method, spacing, baseline](DWRITE_LINE_SPACING_METHOD lineSpacingMethod, FLOAT lineSpacing, FLOAT lineBaseline)
                {
                    Assert::AreEqual(method, lineSpacingMethod);
                    Assert::AreEqual(spacing, lineSpacing);
                    Assert::AreEqual(baseline, lineBaseline);
                    return S_OK;
                });
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_put_LineSpacing)
        {
            float const anyBaseline = 0.0f;

            struct Case { float Value; DWRITE_LINE_SPACING_METHOD Method; float DWriteValue; };

            Case cases[] = { {  0.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM, 0.0f },
                             { -1.0f, DWRITE_LINE_SPACING_METHOD_DEFAULT, 1.0f },
                             { -5.0f, DWRITE_LINE_SPACING_METHOD_DEFAULT, 5.0f },
                             {  1.0f, DWRITE_LINE_SPACING_METHOD_UNIFORM, 1.0f } };

            for (auto c : cases)
            {
                Fixture f;
                auto l = f.CreateSimpleTextLayout();

                InitializeSetLineSpacingMethod(f.Adapter->MockTextLayout->SetLineSpacingMethod, c.Method, c.DWriteValue, anyBaseline);
                
                ThrowIfFailed(l->put_LineSpacing(c.Value));
            }
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_put_LineSpacingBaseline)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            InitializeSetLineSpacingMethod(f.Adapter->MockTextLayout->SetLineSpacingMethod, DWRITE_LINE_SPACING_METHOD_DEFAULT, 0, 456.0f);
            Assert::AreEqual(S_OK, textLayout->put_LineSpacingBaseline(456.0f));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_DefaultLocaleName)
        {
            Fixture f;

            std::wstring locale = L"SomeLocaleName";

            auto textLayout = f.CreateSimpleTextLayout();

            f.Adapter->MockTextLayout->GetLocaleNameLength_BaseFormat_Method.AllowAnyCall([&] { return static_cast<uint32_t>(locale.length()); });
            
            f.Adapter->MockTextLayout->GetLocaleName_BaseFormat_Method.SetExpectedCalls(1,
                [&](WCHAR* localeName, UINT32 nameSize)
                {        
                    Assert::AreEqual(static_cast<uint32_t>(locale.length()) + 1, nameSize);
                    wmemcpy_s(localeName, nameSize, locale.c_str(), locale.length());
                    localeName[nameSize - 1] = 0;
                    return S_OK;
                });
         
            HSTRING value;
            Assert::AreEqual(S_OK, textLayout->get_DefaultLocaleName(&value));

            Assert::AreEqual(locale.c_str(), WindowsGetStringRawBuffer(value, nullptr));
        }

        template<class GET_TRIMMING_METHOD>
        void InitializeGetTrimmingMethod(GET_TRIMMING_METHOD& trimMethod, DWRITE_TRIMMING_GRANULARITY trimmingGranularity, UINT32 delimiter, UINT32 delimiterCount)
        {
            trimMethod.SetExpectedCalls(1,
                [trimmingGranularity, delimiter, delimiterCount](DWRITE_TRIMMING* trimmingOptions, IDWriteInlineObject** trimmingSign)
                {
                    trimmingOptions->granularity = trimmingGranularity;
                    trimmingOptions->delimiter = delimiter;
                    trimmingOptions->delimiterCount = delimiterCount;
                    return S_OK;
                });
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_TrimmingGranularityMethod)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();
            InitializeGetTrimmingMethod(f.Adapter->MockTextLayout->GetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0);

            CanvasTextTrimmingGranularity trimmingGranularity;
            Assert::AreEqual(S_OK, textLayout->get_TrimmingGranularity(&trimmingGranularity));
            Assert::AreEqual(CanvasTextTrimmingGranularity::Character, trimmingGranularity);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_TrimmingDelimiter)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();
            InitializeGetTrimmingMethod(f.Adapter->MockTextLayout->GetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_NONE, L'@', 0);

            HSTRING str;
            Assert::AreEqual(S_OK, textLayout->get_TrimmingDelimiter(&str));
            Assert::AreEqual(L"@", WindowsGetStringRawBuffer(str, nullptr));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_TrimmingGranularityDelimiterCount)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();
            InitializeGetTrimmingMethod(f.Adapter->MockTextLayout->GetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_NONE, 0, 456);

            INT32 count;
            Assert::AreEqual(S_OK, textLayout->get_TrimmingDelimiterCount(&count));
            Assert::AreEqual(456, count);
        }        

        template<class SET_TRIMMING_METHOD>
        void InitializeSetTrimmingMethod(SET_TRIMMING_METHOD& setTrimmingMethod, DWRITE_TRIMMING_GRANULARITY granularity, UINT32 delimiter, UINT32 delimiterCount)
        {
            setTrimmingMethod.SetExpectedCalls(1,
                [granularity, delimiter, delimiterCount](DWRITE_TRIMMING const* trimmingOptions, IDWriteInlineObject* trimmingSign)
                {
                    Assert::AreEqual(granularity, trimmingOptions->granularity);
                    Assert::AreEqual(delimiter, trimmingOptions->delimiter);
                    Assert::AreEqual(delimiterCount, trimmingOptions->delimiterCount);
                    return S_OK;
                });
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_put_TrimmingGranularity)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            InitializeGetTrimmingMethod(f.Adapter->MockTextLayout->GetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_NONE, 0, 0);
            InitializeSetTrimmingMethod(f.Adapter->MockTextLayout->SetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0);
            Assert::AreEqual(S_OK, textLayout->put_TrimmingGranularity(CanvasTextTrimmingGranularity::Character));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_put_TrimmingDelimiter)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            InitializeGetTrimmingMethod(f.Adapter->MockTextLayout->GetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_NONE, 0, 0);
            InitializeSetTrimmingMethod(f.Adapter->MockTextLayout->SetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_NONE, L'@', 0);
            Assert::AreEqual(S_OK, textLayout->put_TrimmingDelimiter(WinString(L"@")));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_put_TrimmingDelimiterCount)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            InitializeGetTrimmingMethod(f.Adapter->MockTextLayout->GetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_NONE, 0, 0);
            InitializeSetTrimmingMethod(f.Adapter->MockTextLayout->SetTrimmingMethod, DWRITE_TRIMMING_GRANULARITY_NONE, 0, 123);
            Assert::AreEqual(S_OK, textLayout->put_TrimmingDelimiterCount(123));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_RequestedSize)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();
            
            f.Adapter->MockTextLayout->GetMaxWidthMethod.SetExpectedCalls(1, [&]() { return 123.0f; });
            f.Adapter->MockTextLayout->GetMaxHeightMethod.SetExpectedCalls(1, [&]() { return 456.0f; });

            Size size;
            Assert::AreEqual(S_OK, textLayout->get_RequestedSize(&size));
            Assert::AreEqual(123.0f, size.Width);
            Assert::AreEqual(456.0f, size.Height);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_put_RequestedSize)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            f.Adapter->MockTextLayout->SetMaxWidthMethod.SetExpectedCalls(1, [&](FLOAT value) { Assert::AreEqual(123.0f, value); return S_OK; });
            f.Adapter->MockTextLayout->SetMaxHeightMethod.SetExpectedCalls(1, [&](FLOAT value) { Assert::AreEqual(456.0f, value); return S_OK; });

            Assert::AreEqual(S_OK, textLayout->put_RequestedSize(Size{ 123.0f, 456.0f }));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetMinimumLineLength)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            f.Adapter->MockTextLayout->DetermineMinWidthMethod.SetExpectedCalls(1,
                [&](FLOAT* out)
            {
                *out = 123.0f;
                return S_OK;
            });

            float value;
            Assert::AreEqual(S_OK, textLayout->GetMinimumLineLength(&value));
            Assert::AreEqual(123.0f, value);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetFontFamily)
        {
            Fixture f;

            std::wstring fontFamily = L"SomeFontFamily";

            f.Adapter->MockTextLayout->GetFontFamilyNameLengthMethod.SetExpectedCalls(1,
                [&](UINT32 currentPosition, UINT32* nameLength, DWRITE_TEXT_RANGE* textRange) 
                { 
                    Assert::AreEqual(12345u, currentPosition);
                    Assert::IsNull(textRange);
                    *nameLength = static_cast<uint32_t>(fontFamily.length());
                    return S_OK;
                });

            f.Adapter->MockTextLayout->GetFontFamilyNameMethod.SetExpectedCalls(1,
                [&](UINT32 currentPosition, WCHAR* fontFamilyName, UINT32 nameSize, DWRITE_TEXT_RANGE* textRange)
                {
                    Assert::AreEqual(12345u, currentPosition);
                    Assert::AreEqual(static_cast<uint32_t>(fontFamily.length()) + 1, nameSize);
                    wmemcpy_s(fontFamilyName, nameSize, fontFamily.c_str(), fontFamily.length());
                    fontFamilyName[nameSize - 1] = 0;
                    Assert::IsNull(textRange);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            HSTRING value;
            Assert::AreEqual(S_OK, textLayout->GetFontFamily(12345, &value));

            Assert::AreEqual(fontFamily.c_str(), WindowsGetStringRawBuffer(value, nullptr));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetLocaleName)
        {
            Fixture f;

            std::wstring locale = L"SomeLocale";

            f.Adapter->MockTextLayout->GetLocaleNameLengthMethod.SetExpectedCalls(1,
                [&](UINT32 currentPosition, UINT32* nameLength, DWRITE_TEXT_RANGE* textRange)
                {
                    Assert::AreEqual(12345u, currentPosition);
                    Assert::IsNull(textRange);
                    *nameLength = static_cast<uint32_t>(locale.length());
                    return S_OK;
                });

            f.Adapter->MockTextLayout->GetLocaleNameMethod.SetExpectedCalls(1,
                [&](UINT32 currentPosition, WCHAR* localeName, UINT32 nameSize, DWRITE_TEXT_RANGE* textRange)
                {
                    Assert::AreEqual(12345u, currentPosition);
                    Assert::AreEqual(static_cast<uint32_t>(locale.length()) + 1, nameSize);
                    wmemcpy_s(localeName, nameSize, locale.c_str(), locale.length());
                    localeName[nameSize - 1] = 0;
                    Assert::IsNull(textRange);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            HSTRING value;
            Assert::AreEqual(S_OK, textLayout->GetLocaleName(12345, &value));

            Assert::AreEqual(locale.c_str(), WindowsGetStringRawBuffer(value, nullptr));
        }

        //
        // The logic for URI manipulation and loading custom fonts is shared
        // between CanvasTextFormat and CanvasTextLayout. There are tests 
        // exercising the custom font loading in the text format
        // unit tests, and so they are not exhaustively duplicated here.
        // 

        TEST_METHOD_EX(CanvasTextLayoutTests_SetFontFamily_EmptyUri)
        {
            Fixture f;

            std::wstring fontFamily = L"SomeFontFamily";

            f.Adapter->MockTextLayout->SetFontCollectionMethod.SetExpectedCalls(1,
                [&](IDWriteFontCollection* fontCollection, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::IsNull(fontCollection); // System font collection should be used
                    Assert::AreEqual(123u, textRange.startPosition);
                    Assert::AreEqual(456u, textRange.length);
                    return S_OK;
                });

            f.Adapter->MockTextLayout->SetFontFamilyNameMethod.SetExpectedCalls(1,
                [&](WCHAR const* fontFamilyName, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::AreEqual(fontFamily.c_str(), fontFamilyName);
                    Assert::AreEqual(123u, textRange.startPosition);
                    Assert::AreEqual(456u, textRange.length);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->SetFontFamily(123, 456, WinString(fontFamily)));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_SetFontFamily_NonEmptyUri)
        {
            Fixture f;

            std::wstring uri = L"SomeUri";
            std::wstring fontFamily = L"SomeFontFamily";
            std::wstring fullName = uri + L"#" + fontFamily;

            auto mockFontCollection = Make<MockDWriteFontCollection>();

            f.Adapter->GetMockDWriteFactory()->CreateCustomFontCollectionMethod.SetExpectedCalls(1,
                [&](
                IDWriteFontCollectionLoader*,
                void const* key,
                UINT32 keySize,
                IDWriteFontCollection** out)
                {
                    std::wstring keyString(static_cast<const wchar_t*>(key));
                    std::wstring expectedKey = L"pathof(ms-appx:///" + uri + L")";
                    Assert::AreEqual(expectedKey, keyString);
                    Assert::AreEqual(static_cast<uint32_t>(expectedKey.length() * sizeof(wchar_t)), keySize);

                    return mockFontCollection.CopyTo(out);
                });


            f.Adapter->MockTextLayout->SetFontCollectionMethod.SetExpectedCalls(1,
                [&](IDWriteFontCollection* fontCollection, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::IsNotNull(fontCollection);
                    Assert::IsTrue(IsSameInstance(mockFontCollection.Get(), fontCollection));
                    Assert::AreEqual(123u, textRange.startPosition);
                    Assert::AreEqual(456u, textRange.length);
                    return S_OK;
                });

            f.Adapter->MockTextLayout->SetFontFamilyNameMethod.SetExpectedCalls(1,
                [&](WCHAR const* fontFamilyName, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::AreEqual(fontFamily.c_str(), fontFamilyName);
                    Assert::AreEqual(123u, textRange.startPosition);
                    Assert::AreEqual(456u, textRange.length);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->SetFontFamily(123, 456, WinString(fullName)));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_SetLocaleName)
        {
            Fixture f;

            std::wstring locale = L"SomeLocale";

            f.Adapter->MockTextLayout->SetLocaleNameMethod.SetExpectedCalls(1,
                [&](WCHAR const* localeName, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::AreEqual(locale.c_str(), localeName);
                    Assert::AreEqual(123u, textRange.startPosition);
                    Assert::AreEqual(456u, textRange.length);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->SetLocaleName(123, 456, WinString(locale)));
        }
        
        template<class GetCharacterSpacingMethodType>
        void InitializeGetCharacterSpacingMethod(GetCharacterSpacingMethodType& characterSpacingMethod, FLOAT leadingSpacing, FLOAT trailingSpacing, FLOAT minimumAdvance)
        {            
            characterSpacingMethod.SetExpectedCalls(1,
                [leadingSpacing, trailingSpacing, minimumAdvance](UINT32 currentPosition, FLOAT* leading, FLOAT* trailing, FLOAT* min, DWRITE_TEXT_RANGE* textRange)
                {
                    Assert::AreEqual(currentPosition, 123u);
                    Assert::IsNull(textRange);

                    *leading = leadingSpacing;
                    *trailing = trailingSpacing;
                    *min = minimumAdvance;

                    return S_OK;
                });
        }
        
        TEST_METHOD_EX(CanvasTextLayoutTests_GetLeadingCharacterSpacing)
        {
            Fixture f;

            InitializeGetCharacterSpacingMethod(f.Adapter->MockTextLayout->GetCharacterSpacingMethod, 456.0f, 0.0f, 0.0f);

            auto textLayout = f.CreateSimpleTextLayout();

            float value;
            Assert::AreEqual(S_OK, textLayout->GetLeadingCharacterSpacing(123, &value));
            Assert::AreEqual(456.0f, value);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetTrailingCharacterSpacing)
        {
            Fixture f;

            InitializeGetCharacterSpacingMethod(f.Adapter->MockTextLayout->GetCharacterSpacingMethod, 0.0f, 456.0f, 0.0f);

            auto textLayout = f.CreateSimpleTextLayout();

            float value;
            Assert::AreEqual(S_OK, textLayout->GetTrailingCharacterSpacing(123, &value));
            Assert::AreEqual(456.0f, value);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetMinimumCharacterAdvance)
        {
            Fixture f;

            InitializeGetCharacterSpacingMethod(f.Adapter->MockTextLayout->GetCharacterSpacingMethod, 0.0f, 0.0f, 456.0f);

            auto textLayout = f.CreateSimpleTextLayout();

            float value;
            Assert::AreEqual(S_OK, textLayout->GetMinimumCharacterAdvance(123, &value));
            Assert::AreEqual(456.0f, value);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_SetCharacterSpacing)
        {
            Fixture f;

            f.Adapter->MockTextLayout->SetCharacterSpacingMethod.SetExpectedCalls(1,
                [&](FLOAT leadingSpacing, FLOAT trailingSpacing, FLOAT minimumAdvance, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::AreEqual(12.0f, leadingSpacing);
                    Assert::AreEqual(34.0f, trailingSpacing);
                    Assert::AreEqual(56.0f, minimumAdvance);
                    Assert::AreEqual(78u, textRange.startPosition);
                    Assert::AreEqual(90u, textRange.length);
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->SetCharacterSpacing(78, 90, 12.0f, 34.0f, 56.0f));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_DefaultOptions)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            CanvasDrawTextOptions drawTextOptions;
            Assert::AreEqual(S_OK, textLayout->get_Options(&drawTextOptions));
            Assert::AreEqual(CanvasDrawTextOptions::Default, drawTextOptions);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_OptionsSetAndGet)
        {
            Fixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->put_Options(CanvasDrawTextOptions::NoPixelSnap));

            CanvasDrawTextOptions drawTextOptions;
            Assert::AreEqual(S_OK, textLayout->get_Options(&drawTextOptions));
            Assert::AreEqual(CanvasDrawTextOptions::NoPixelSnap, drawTextOptions);
        }


        TEST_METHOD_EX(CanvasTextLayoutTests_get_LayoutBounds)
        {
            Fixture f;

            f.Adapter->MockTextLayout->GetMetricsMethod.SetExpectedCalls(1,
                [&](DWRITE_TEXT_METRICS1* out)
                {
                    Assert::IsNotNull(out);
                    DWRITE_TEXT_METRICS1 metrics{};
                    metrics.left = 1;
                    metrics.top = 2;
                    metrics.width = 3;
                    metrics.height = 4;
                    *out = metrics;
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Rect bounds;
            Assert::AreEqual(S_OK, textLayout->get_LayoutBounds(&bounds));
            Assert::AreEqual(Rect{ 1, 2, 3, 4 }, bounds);
        }
        

        TEST_METHOD_EX(CanvasTextLayoutTests_get_LineCount)
        {
            Fixture f;

            f.Adapter->MockTextLayout->GetMetricsMethod.SetExpectedCalls(1,
                [&](DWRITE_TEXT_METRICS1* out)
                {
                    Assert::IsNotNull(out);
                    DWRITE_TEXT_METRICS1 metrics{};
                    metrics.lineCount = 1234;
                    *out = metrics;
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            int32_t lineCount;
            Assert::AreEqual(S_OK, textLayout->get_LineCount(&lineCount));
            Assert::AreEqual(1234, lineCount);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_DrawBounds)
        {
            Fixture f;

            f.Adapter->MockTextLayout->GetMaxWidthMethod.SetExpectedCalls(1,
                [&](){ return 10.0f;  });

            f.Adapter->MockTextLayout->GetMaxHeightMethod.SetExpectedCalls(1,
                [&](){ return 20.0f;  });

            f.Adapter->MockTextLayout->GetOverhangMetricsMethod.SetExpectedCalls(1,
                [&](DWRITE_OVERHANG_METRICS* out)
                {
                    Assert::IsNotNull(out);
                    DWRITE_OVERHANG_METRICS metrics{};
                    // Choose an overhang rect that extends out of the left-top edges,
                    // but inside the right-bottom edges.
                    metrics.left = 1;
                    metrics.top = 2;
                    metrics.right = -3;
                    metrics.bottom = -4;
                    *out = metrics;
                    return S_OK;
                });

            auto textLayout = f.CreateSimpleTextLayout();

            Rect bounds;
            Assert::AreEqual(S_OK, textLayout->get_DrawBounds(&bounds));

            // The result rect is computed based on the layout size and overhang metrics.
            Assert::AreEqual(Rect{ -1, -2, 10 + 1 + -3, 20 + 2 + -4 }, bounds);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_HitTest)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;

                f.Adapter->MockTextLayout->HitTestPointMethod.SetExpectedCalls(1,
                    [&](FLOAT x, FLOAT y, BOOL* isTrailingHit, BOOL* isInside, DWRITE_HIT_TEST_METRICS* hitTestMetrics)
                {
                    Assert::AreEqual(1.0f, x);
                    Assert::AreEqual(2.0f, y);
                    *isTrailingHit = FALSE;
                    *isInside = TRUE;
                    *hitTestMetrics = DWRITE_HIT_TEST_METRICS{};
                    return S_OK;
                });

                auto textLayout = f.CreateSimpleTextLayout();

                boolean isHit;
                if (i==0)
                    Assert::AreEqual(S_OK, textLayout->HitTest(Vector2{ 1, 2 }, &isHit));
                else
                    Assert::AreEqual(S_OK, textLayout->HitTestWithCoords(1, 2, &isHit));

                Assert::IsTrue(!!isHit);
            }
        }

        void WriteHitTestDescription(DWRITE_HIT_TEST_METRICS* textLayoutRegion, uint32_t increment = 0)
        {
            textLayoutRegion->textPosition = 1 + increment;
            textLayoutRegion->length = 2 + increment;
            const float floatIncrement = static_cast<float>(increment);
            textLayoutRegion->left = 3 + floatIncrement;
            textLayoutRegion->top = 4 + floatIncrement;
            textLayoutRegion->width = 5 + floatIncrement;
            textLayoutRegion->height = 6 + floatIncrement;
        }

        void VerifyHitTestDescription(CanvasTextLayoutRegion const& textLayoutRegion, uint32_t increment = 0)
        {
            Assert::AreEqual(1 + static_cast<int>(increment), textLayoutRegion.CharacterIndex);
            Assert::AreEqual(2 + static_cast<int>(increment), textLayoutRegion.CharacterCount);
            const float floatIncrement = static_cast<float>(increment);
            Assert::AreEqual(Rect{ 
                3 + floatIncrement,
                4 + floatIncrement,
                5 + floatIncrement,
                6 + floatIncrement }, textLayoutRegion.LayoutBounds);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_HitTestWithDescription)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;

                f.Adapter->MockTextLayout->HitTestPointMethod.SetExpectedCalls(1,
                    [&](FLOAT x, FLOAT y, BOOL* isTrailingHit, BOOL* isInside, DWRITE_HIT_TEST_METRICS* hitTestMetrics)
                    {
                        Assert::AreEqual(1.0f, x);
                        Assert::AreEqual(2.0f, y);
                        *isTrailingHit = FALSE;
                        *isInside = FALSE;
                        *hitTestMetrics = DWRITE_HIT_TEST_METRICS{};
                        WriteHitTestDescription(hitTestMetrics);
                        return S_OK;
                    });

                auto textLayout = f.CreateSimpleTextLayout();

                boolean isHit;
                CanvasTextLayoutRegion description;
                if (i == 0)
                    Assert::AreEqual(S_OK, textLayout->HitTestWithDescription(Vector2{ 1, 2 }, &description, &isHit));
                else
                    Assert::AreEqual(S_OK, textLayout->HitTestWithDescriptionAndCoords(1, 2, &description, &isHit));

                VerifyHitTestDescription(description);
                Assert::IsFalse(!!isHit);
            }
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_HitTestWithDescriptionAndTrailingSide)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;

                f.Adapter->MockTextLayout->HitTestPointMethod.SetExpectedCalls(1,
                    [&](FLOAT x, FLOAT y, BOOL* isTrailingHit, BOOL* isInside, DWRITE_HIT_TEST_METRICS* hitTestMetrics)
                    {
                        Assert::AreEqual(1.0f, x);
                        Assert::AreEqual(2.0f, y);
                        *isTrailingHit = TRUE;
                        *isInside = TRUE;
                        *hitTestMetrics = DWRITE_HIT_TEST_METRICS{};
                        WriteHitTestDescription(hitTestMetrics);
                        return S_OK;
                    });

                auto textLayout = f.CreateSimpleTextLayout();

                boolean isHit;
                CanvasTextLayoutRegion description;
                boolean trailingSideOfCharacter;
                if (i == 0)
                    Assert::AreEqual(S_OK, textLayout->HitTestWithDescriptionAndTrailingSide(Vector2{ 1, 2 }, &description, &trailingSideOfCharacter, &isHit));
                else
                    Assert::AreEqual(S_OK, textLayout->HitTestWithDescriptionAndCoordsAndTrailingSide(1, 2, &description, &trailingSideOfCharacter, &isHit));

                VerifyHitTestDescription(description);
                Assert::IsTrue(!!isHit);
                Assert::IsTrue(!!trailingSideOfCharacter);
            }
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetCaretPosition)
        {
            for (int i = 0; i < 2; ++i)
            {
                Fixture f;

                f.Adapter->MockTextLayout->HitTestTextPositionMethod.SetExpectedCalls(1,
                    [&](UINT32 textPosition, BOOL isTrailingHit, FLOAT* pointX, FLOAT* pointY, DWRITE_HIT_TEST_METRICS* hitTestMetrics)
                    {
                        Assert::AreEqual(1u, textPosition);
                        Assert::IsTrue(!!isTrailingHit);
                        *pointX = 2.0f;
                        *pointY = 3.0f;
                        if(i==1)
                            WriteHitTestDescription(hitTestMetrics);
                        return S_OK;
                    });

                auto textLayout = f.CreateSimpleTextLayout();

                Vector2 caretLocation;
                CanvasTextLayoutRegion textLayoutRegion;
                if (i == 0)
                    Assert::AreEqual(S_OK, textLayout->GetCaretPosition(1, true, &caretLocation));
                else
                {
                    Assert::AreEqual(S_OK, textLayout->GetCaretPositionWithDescription(1, true, &textLayoutRegion, &caretLocation));
                    VerifyHitTestDescription(textLayoutRegion);
                }

                Assert::AreEqual(Vector2{ 2, 3 }, caretLocation);
            }
        }               

        TEST_METHOD_EX(CanvasTextLayoutTests_GetCharacterRegions)
        {
            Fixture f;

            int callCount = 0;

            f.Adapter->MockTextLayout->HitTestTextRangeMethod.SetExpectedCalls(2,
                [&](UINT32 textPosition, UINT32 textLength, FLOAT originX, FLOAT originY, DWRITE_HIT_TEST_METRICS* hitTestMetrics, UINT32 maxCount, UINT32* actualCount)
                {
                    Assert::AreEqual(1u, textPosition);
                    Assert::AreEqual(2u, textLength);
                    Assert::AreEqual(0.0f, originX);
                    Assert::AreEqual(0.0f, originY);

                    *actualCount = 3;
                    if (callCount == 0)
                    {
                        Assert::AreEqual(0u, maxCount);
                        callCount++;
                        return E_NOT_SUFFICIENT_BUFFER;
                    }
                    else
                    {
                        Assert::AreEqual(3u, maxCount);

                        for (int i = 0; i < 3; i++)
                            WriteHitTestDescription(&hitTestMetrics[i], i);

                        callCount++;
                        return S_OK;
                    }
                });

            auto textLayout = f.CreateSimpleTextLayout();

            uint32_t descriptionCount;
            CanvasTextLayoutRegion* hitTestDescriptionArray;
            Assert::AreEqual(S_OK, textLayout->GetCharacterRegions(1, 2, &descriptionCount, &hitTestDescriptionArray));
            Assert::AreEqual(3u, descriptionCount);

            for (int i = 0; i < 3; i++)
                VerifyHitTestDescription(hitTestDescriptionArray[i], i);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_get_Device)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            ComPtr<ICanvasDevice> retrievedDevice;
            Assert::AreEqual(S_OK, textLayout->get_Device(&retrievedDevice));

            Assert::IsTrue(IsSameInstance(f.Device.Get(), retrievedDevice.Get()));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_SetBrush)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            auto stubCanvasBrush = Make<StubCanvasBrush>();

            f.Adapter->MockTextLayout->SetDrawingEffectMethod.SetExpectedCalls(1,
                [&](IUnknown* drawingEffectObject, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::AreEqual(static_cast<IUnknown*>(stubCanvasBrush->GetD2DBrush(nullptr, GetBrushFlags::None).Get()), drawingEffectObject);
                    Assert::AreEqual(123u, textRange.startPosition);
                    Assert::AreEqual(456u, textRange.length);
                    return S_OK;
                });

            Assert::AreEqual(S_OK, textLayout->SetBrush(123, 456, stubCanvasBrush.Get()));                    
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_SetBrush_NullBrushIsOk)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            f.Adapter->MockTextLayout->SetDrawingEffectMethod.SetExpectedCalls(1,
                [&](IUnknown* drawingEffectObject, DWRITE_TEXT_RANGE)
                {
                    Assert::IsNull(drawingEffectObject);
                    return S_OK;
                });

            Assert::AreEqual(S_OK, textLayout->SetBrush(0, 0, nullptr));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_SetColor)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            bool createSolidColorBrushCalled = false;
            auto d2dSolidColorBrush = Make<MockD2DSolidColorBrush>();
            
            f.Device->MockCreateSolidColorBrush =
                [&](D2D1_COLOR_F const& color)
                {
                    Assert::AreEqual(D2D1_COLOR_F{ 1, 0, 0, 1 }, color);
                    Assert::IsFalse(createSolidColorBrushCalled);
                    createSolidColorBrushCalled = true;
                    return d2dSolidColorBrush;
                };

            f.Adapter->MockTextLayout->SetDrawingEffectMethod.SetExpectedCalls(1,
                [&](IUnknown* drawingEffectObject, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::IsTrue(IsSameInstance(d2dSolidColorBrush.Get(), drawingEffectObject));
                    Assert::AreEqual(123u, textRange.startPosition);
                    Assert::AreEqual(456u, textRange.length);
                    return S_OK;
                });

            Color testColor{ 255, 255, 0, 0 };
            Assert::AreEqual(S_OK, textLayout->SetColor(123, 456, testColor));

            Assert::IsTrue(createSolidColorBrushCalled);
        }

        template<class D2D_MOCK_BRUSH_TYPE>
        void VerifyGetDrawingEffect_WithBrushType()
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            auto d2dBrush = Make<D2D_MOCK_BRUSH_TYPE>();

            f.Adapter->MockTextLayout->GetDrawingEffectMethod.SetExpectedCalls(1,
                [&](UINT32 characterIndex, IUnknown** drawingEffectObject, DWRITE_TEXT_RANGE* textRange)
                {
                    Assert::AreEqual(123u, characterIndex);
                    Assert::IsNull(textRange);

                    d2dBrush.CopyTo(drawingEffectObject);

                    return S_OK;
                });

            ComPtr<ICanvasBrush> retrievedBrush;
            Assert::AreEqual(S_OK, textLayout->GetBrush(123, &retrievedBrush));

            auto retrievedBrushInternal = As<ICanvasBrushInternal>(retrievedBrush);

            auto retrievedD2DResource = retrievedBrushInternal->GetD2DBrush(nullptr, GetBrushFlags::None);

            Assert::AreEqual(
                static_cast<ID2D1Brush*>(d2dBrush.Get()),
                retrievedD2DResource.Get());
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetBrush_BrushTypes)
        {
            VerifyGetDrawingEffect_WithBrushType<MockD2DSolidColorBrush>();

            VerifyGetDrawingEffect_WithBrushType<MockD2DLinearGradientBrush>();

            VerifyGetDrawingEffect_WithBrushType<MockD2DRadialGradientBrush>();

            VerifyGetDrawingEffect_WithBrushType<MockD2DBitmapBrush>();

            VerifyGetDrawingEffect_WithBrushType<MockD2DImageBrush>();
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetBrush_NullDrawingEffect)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            f.Adapter->MockTextLayout->GetDrawingEffectMethod.SetExpectedCalls(1,
                [&](UINT32 characterIndex, IUnknown** drawingEffectObject, DWRITE_TEXT_RANGE* textRange)
                {
                    ComPtr<IUnknown> noDrawingEffect;
                    noDrawingEffect.CopyTo(drawingEffectObject);

                    return S_OK;
                });

            ComPtr<ICanvasBrush> retrievedBrush;
            Assert::AreEqual(S_OK, textLayout->GetBrush(123, &retrievedBrush));

            Assert::IsNull(retrievedBrush.Get());
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetBrush_NonBrushDrawingEffect)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            f.Adapter->MockTextLayout->GetDrawingEffectMethod.SetExpectedCalls(1,
                [&](UINT32 characterIndex, IUnknown** drawingEffectObject, DWRITE_TEXT_RANGE* textRange)
                {
                    auto notABrush = Make<MockDWriteFontCollection>();
                    notABrush.CopyTo(drawingEffectObject);
                    return S_OK;
                });

            ComPtr<ICanvasBrush> retrievedBrush;
            Assert::AreEqual(E_NOINTERFACE, textLayout->GetBrush(123, &retrievedBrush));

            Assert::IsNull(retrievedBrush.Get());
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_When_BitmapSource_DrawnIntoCommandList_DpiCompensationAlwaysAdded)
        {
            for (auto dpi : { DEFAULT_DPI / 2, DEFAULT_DPI, DEFAULT_DPI * 2 })
            {
                Check_BitmapSource_SetAsImageBrushImage_DpiCompensationAdded(dpi);
            }
        }

        void Check_BitmapSource_SetAsImageBrushImage_DpiCompensationAdded(float dpi)
        {
            Fixture f;

            auto testBitmap = CreateStubCanvasBitmap(dpi, f.Device.Get());
            auto testEffect = Make<TestEffect>(CLSID_D2D1GaussianBlur, 0, 1, true);
            ThrowIfFailed(testEffect->put_Source(testBitmap.Get()));

            auto imageBrush = Make<CanvasImageBrush>(f.Device.Get());
            ThrowIfFailed(imageBrush->put_Image(testEffect.Get()));
            auto sourceRectangle = Make<Nullable<Rect>>(Rect{ 0, 0, 1, 1 });
            Assert::AreEqual(S_OK, imageBrush->put_SourceRectangle(sourceRectangle.Get()));

            auto textLayout = f.CreateSimpleTextLayout();
            f.Adapter->MockTextLayout->SetDrawingEffectMethod.SetExpectedCalls(1,
                [&](IUnknown* drawingEffectObject, DWRITE_TEXT_RANGE textRange)
                {
                    Assert::IsNotNull(drawingEffectObject);
                    return S_OK;
                });
            Assert::AreEqual(S_OK, textLayout->SetBrush(0, 1, imageBrush.Get()));

            Assert::AreEqual<size_t>(2, f.MockEffects.size());
            CheckEffectTypeAndInput(f.MockEffects[0].Get(), CLSID_D2D1GaussianBlur, f.MockEffects[1].Get());
            CheckEffectTypeAndInput(f.MockEffects[1].Get(), CLSID_D2D1DpiCompensation, testBitmap.Get(), f.DeviceContext.Get(), dpi);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_TrimmingSign_DefaultIsNone)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            CanvasTrimmingSign sign;
            ThrowIfFailed(textLayout->get_TrimmingSign(&sign));

            Assert::AreEqual(CanvasTrimmingSign::None, sign);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_TrimmingSign_Property)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            ThrowIfFailed(textLayout->put_TrimmingSign(CanvasTrimmingSign::Ellipsis));

            CanvasTrimmingSign sign;
            ThrowIfFailed(textLayout->get_TrimmingSign(&sign));
            Assert::AreEqual(CanvasTrimmingSign::Ellipsis, sign);

            ThrowIfFailed(textLayout->put_TrimmingSign(CanvasTrimmingSign::None));
            ThrowIfFailed(textLayout->get_TrimmingSign(&sign));
            Assert::AreEqual(CanvasTrimmingSign::None, sign);
        }

        bool HasTrimmingSign(ComPtr<IDWriteTextLayout> const& dtl)
        {
            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> trimmingSign;
            ThrowIfFailed(dtl->GetTrimming(&trimming, &trimmingSign));

            return trimmingSign;
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_TrimmingSign_AffectsWrappedResource)
        {
            Fixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            ThrowIfFailed(textLayout->put_TrimmingSign(CanvasTrimmingSign::Ellipsis));
            auto dtl1 = GetWrappedResource<IDWriteTextLayout2>(textLayout);
            Assert::IsTrue(HasTrimmingSign(dtl1));

            ThrowIfFailed(textLayout->put_TrimmingSign(CanvasTrimmingSign::None));
            auto dtl2 = GetWrappedResource<IDWriteTextLayout2>(textLayout);
            Assert::IsFalse(HasTrimmingSign(dtl2));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_TrimmingSign_InteroppedCustomTrimmingSign_ResetsPropertyToNone)
        {
            for (auto sign : sc_trimmingSigns)
            {
                Fixture f;
                auto textLayout = f.CreateSimpleTextLayout();

                ThrowIfFailed(textLayout->put_TrimmingSign(sign));

                auto dtl = GetWrappedResource<IDWriteTextLayout2>(textLayout);
                DWRITE_TRIMMING trimming{};

                auto factory = f.Adapter->CreateDWriteFactory(DWRITE_FACTORY_TYPE_SHARED);
                ComPtr<IDWriteInlineObject> customTrimmingSign;
                ThrowIfFailed(factory->CreateEllipsisTrimmingSign(dtl.Get(), &customTrimmingSign));
                dtl->SetTrimming(&trimming, customTrimmingSign.Get());

                CanvasTrimmingSign actual;
                ThrowIfFailed(textLayout->get_TrimmingSign(&actual));
                Assert::AreEqual(CanvasTrimmingSign::None, actual);
            }
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_TrimmingSign_InheritedFromFormat)
        {
            for (auto sign : sc_trimmingSigns)
            {
                Fixture f;

                auto textFormat = Make<CanvasTextFormat>();
                ThrowIfFailed(textFormat->put_TrimmingSign(sign));

                auto textLayout = CanvasTextLayout::CreateNew(f.Device.Get(), WinString(L"A string"), textFormat.Get(), 0.0f, 0.0f);

                CanvasTrimmingSign actual;
                ThrowIfFailed(textLayout->get_TrimmingSign(&actual));
                Assert::AreEqual(sign, actual);
            }
        }


        class TrimmingSignFixture : public Fixture
        {
            DWRITE_TRIMMING m_trimming;
            ComPtr<IDWriteInlineObject> m_trimmingSign;

        public:
            TrimmingSignFixture()
            {
                Adapter->MockTextLayout->SetReadingDirectionMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetFlowDirectionMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetTextAlignmentMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetIncrementalTabStopMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetLastLineWrappingMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetLineSpacingMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetOpticalAlignmentMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetTrimmingMethod.AllowAnyCall(
                    [=](DWRITE_TRIMMING const* trimming, IDWriteInlineObject* sign)
                    {
                        m_trimming = *trimming;
                        m_trimmingSign = sign;
                        return S_OK;
                    });

                Adapter->MockTextLayout->GetTrimmingMethod.AllowAnyCall(
                    [=](DWRITE_TRIMMING* trimming, IDWriteInlineObject** sign)
                    {
                        *trimming = m_trimming;
                        m_trimmingSign.CopyTo(sign);
                        return S_OK;
                    });

                Adapter->MockTextLayout->SetParagraphAlignmentMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetVerticalGlyphOrientationMethod.AllowAnyCall();
                Adapter->MockTextLayout->SetWordWrappingMethod.AllowAnyCall();
            }
        };

        ComPtr<IDWriteInlineObject> GetTrimmingSign(ComPtr<IDWriteTextLayout> const& textLayout)
        {
            DWRITE_TRIMMING unused;

            ComPtr<IDWriteInlineObject> trimmingSign;
            ThrowIfFailed(textLayout->GetTrimming(&unused, &trimmingSign));

            return trimmingSign;
        }

        template<typename SetterType>
        void CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
            HRESULT (__stdcall CanvasTextLayout::*setFn)(SetterType value),
            SetterType setTo,
            bool setterRequiresRecreatingEllipsisTrimmingSign = true)
        {
            for (auto expected : sc_trimmingSigns)
            {
                TrimmingSignFixture f;
                auto textLayout = f.CreateSimpleTextLayout();

                ThrowIfFailed(textLayout->put_TrimmingSign(expected));
                auto sign1 = GetTrimmingSign(textLayout->GetResource());

                ((*textLayout.Get()).*setFn)(setTo);

                CanvasTrimmingSign actual;
                ThrowIfFailed(textLayout->get_TrimmingSign(&actual));
                auto sign2 = GetTrimmingSign(textLayout->GetResource());

                Assert::AreEqual(expected, actual);

                //
                // For ellipsis, the trimming sign object should still exist,
                // but has been changed.
                // It isn't enough to just check whether we created a new
                // text layout. There's cases where we need
                // to create a new trimming sign object, even when the
                // underlying text Layout resource stays the same.
                //
                if (expected == CanvasTrimmingSign::Ellipsis)
                {
                    Assert::IsNotNull(sign1.Get());
                    Assert::IsNotNull(sign2.Get());
                    if (setterRequiresRecreatingEllipsisTrimmingSign)
                    {
                        Assert::AreNotEqual(sign1.Get(), sign2.Get());
                    }
                    else
                    {
                        Assert::AreEqual(sign1.Get(), sign2.Get());
                    }
                }
            }
        }

        TEST_METHOD_EX(CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState)
        {
            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_Direction,
                CanvasTextDirection::RightToLeftThenBottomToTop);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_HorizontalAlignment,
                CanvasHorizontalAlignment::Center);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_IncrementalTabStop,
                3.9f);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_LastLineWrapping,
                static_cast<boolean>(false));

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_LineSpacing,
                201.0f);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_LineSpacingBaseline,
                202.0f);

#if WINVER > _WIN32_WINNT_WINBLUE
            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_LineSpacingMode,
                CanvasLineSpacingMode::Proportional);
#endif

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_OpticalAlignment,
                CanvasOpticalAlignment::NoSideBearings);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_Options,
                CanvasDrawTextOptions::EnableColorFont,
                false);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_TrimmingDelimiter,
                static_cast<HSTRING>(WinString(L"K")));
            
            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_TrimmingDelimiterCount,
                2);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_TrimmingGranularity,
                CanvasTextTrimmingGranularity::Character);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_VerticalAlignment,
                CanvasVerticalAlignment::Bottom);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_VerticalGlyphOrientation,
                CanvasVerticalGlyphOrientation::Stacked);

            CanvasTextLayout_TrimmingSign_SurvivesModifyingTextLayoutState_TestCase(
                &CanvasTextLayout::put_WordWrapping,
                CanvasWordWrapping::EmergencyBreak);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_TrimmingSign_InteropNullsOutSign_ThenCallSetterRequiringNewSign)
        {
            TrimmingSignFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            ThrowIfFailed(textLayout->put_TrimmingSign(CanvasTrimmingSign::Ellipsis));
            auto dtl = GetWrappedResource<IDWriteTextLayout2>(textLayout);
            Assert::IsTrue(HasTrimmingSign(dtl));

            DWRITE_TRIMMING trimming = {};
            ThrowIfFailed(dtl->SetTrimming(&trimming, nullptr));

            ThrowIfFailed(textLayout->put_Direction(CanvasTextDirection::RightToLeftThenBottomToTop));

            CanvasTrimmingSign sign;
            ThrowIfFailed(textLayout->get_TrimmingSign(&sign));
            Assert::AreEqual(CanvasTrimmingSign::None, sign);
            Assert::IsFalse(HasTrimmingSign(dtl));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_put_CustomTrimmingSign_NullIsOk)
        {
            TrimmingSignFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->put_CustomTrimmingSign(nullptr));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_CustomTrimmingSign_Property)
        {
            TrimmingSignFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            auto customTrimmingSign = Make<CustomInlineObject>();

            Assert::AreEqual(S_OK, textLayout->put_CustomTrimmingSign(customTrimmingSign.Get()));

            ComPtr<ICanvasTextInlineObject> actual;
            Assert::AreEqual(S_OK, textLayout->get_CustomTrimmingSign(&actual));

            Assert::IsTrue(IsSameInstance(customTrimmingSign.Get(), actual.Get()));
        }

        struct NonStubbedFixture
        {
            ComPtr<ICanvasTextFormat> Format;
            ComPtr<CanvasDevice> Device;

            NonStubbedFixture()
            {
                Format = Make<CanvasTextFormat>();
                Device = CanvasDevice::CreateNew(false);
            }

            ComPtr<CanvasTextLayout> CreateSimpleTextLayout()
            {
                return CanvasTextLayout::CreateNew(Device.Get(), WinString(L"A string"), Format.Get(), 0.0f, 0.0f);
            }

            void DrawTextLayoutToSomewhere(ComPtr<CanvasTextLayout> const& textLayout)
            {
                auto renderTarget = CanvasRenderTarget::CreateNew(
                    Device.Get(),
                    1.0f,
                    1.0f,
                    DEFAULT_DPI,
                    PIXEL_FORMAT(B8G8R8A8UIntNormalized),
                    CanvasAlphaMode::Premultiplied);

                ComPtr<ICanvasDrawingSession> drawingSession;
                ThrowIfFailed(renderTarget->CreateDrawingSession(&drawingSession));

                ThrowIfFailed(drawingSession->DrawTextLayoutAtCoordsWithColor(textLayout.Get(), 0, 0, Color{}));

                ThrowIfFailed(static_cast<CanvasDrawingSession*>(drawingSession.Get())->Close());
            }
        };

        TEST_METHOD_EX(CanvasTextLayoutTests_CustomTrimmingSign_SetsDWriteResource)
        {
            NonStubbedFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            auto customTrimmingSign = Make<CustomInlineObject>();

            Assert::AreEqual(S_OK, textLayout->put_CustomTrimmingSign(customTrimmingSign.Get()));

            auto dtl = GetWrappedResource<IDWriteTextLayout2>(textLayout);

            DWRITE_TRIMMING trimming;
            ComPtr<IDWriteInlineObject> dwriteInlineObject;
            ThrowIfFailed(dtl->GetTrimming(&trimming, &dwriteInlineObject));

            customTrimmingSign->DrawMethod.SetExpectedCalls(1);
            auto mockTextRenderer = Make<MockDWriteTextRenderer>();
            dwriteInlineObject->Draw(nullptr, mockTextRenderer.Get(), 0, 0, FALSE, FALSE, nullptr);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_CustomTrimmingSign_SetTrimmingSignOnDWriteResource_get_CustomTrimmingSignThrows)
        {
            TrimmingSignFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            auto dtl = GetWrappedResource<IDWriteTextLayout2>(textLayout);

            auto dwriteInlineObject = Make<MockDWriteInlineObject>();

            DWRITE_TRIMMING trimming{};
            ThrowIfFailed(dtl->SetTrimming(&trimming, dwriteInlineObject.Get()));

            ComPtr<ICanvasTextInlineObject> value;
            Assert::AreEqual(E_NOINTERFACE, textLayout->get_CustomTrimmingSign(&value));

            ValidateStoredErrorState(E_NOINTERFACE, Strings::ExternalInlineObject);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_SetInlineObject_NullIsOk)
        {
            NonStubbedFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            Assert::AreEqual(S_OK, textLayout->SetInlineObject(0, 0, nullptr));
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_GetInlineObject_DefaultIsNull)
        {
            NonStubbedFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            static const int numCharactersInLayout = 8;

            for (int i = 0; i < numCharactersInLayout; ++i)
            {
                ComPtr<ICanvasTextInlineObject> inlineObject;
                Assert::AreEqual(S_OK, textLayout->GetInlineObject(i, &inlineObject));
                Assert::IsNull(inlineObject.Get());
            }
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_InlineObject_SetAndGetOnRange)
        {
            NonStubbedFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            static const int startRange = 2;
            static const int endRange = 6;
            static const int numCharactersInLayout = 8;

            auto inlineObject = Make<CustomInlineObject>();

            Assert::AreEqual(S_OK, textLayout->SetInlineObject(startRange, endRange - startRange, inlineObject.Get()));

            for (int i = 0; i < numCharactersInLayout; ++i)
            {
                ComPtr<ICanvasTextInlineObject> actual;
                Assert::AreEqual(S_OK, textLayout->GetInlineObject(i, &actual));

                if (i >= startRange && i < endRange)
                    Assert::IsTrue(IsSameInstance(inlineObject.Get(), actual.Get()));
                else
                    Assert::IsNull(actual.Get());
            }
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_MultipleInlineObjects)
        {
            //
            // Verifies that inline object of a character doesn't
            // somehow trample over the inline objects set to other characters.
            //

            NonStubbedFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            ComPtr<CustomInlineObject> inlineObjects[8];

            for (int i = 0; i < 8; ++i)
            {
                inlineObjects[i] = Make<CustomInlineObject>();
                Assert::AreEqual(S_OK, textLayout->SetInlineObject(i, 1, inlineObjects[i].Get()));
            }

            for (int i = 0; i < 8; ++i)
            {
                ComPtr<ICanvasTextInlineObject> actual;
                Assert::AreEqual(S_OK, textLayout->GetInlineObject(i, &actual));

                Assert::IsTrue(IsSameInstance(inlineObjects[i].Get(), actual.Get()));
            }
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_InlineObject_ImplementedViaInterop_GetInlineObject_Throws)
        {
            NonStubbedFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            auto dtl = GetWrappedResource<IDWriteTextLayout2>(textLayout);

            auto dwriteInlineObject = Make<MockDWriteInlineObject>();

            DWRITE_TEXT_RANGE textRange{0, 1};
            ThrowIfFailed(dtl->SetInlineObject(dwriteInlineObject.Get(), textRange));

            ComPtr<ICanvasTextInlineObject> value;
            Assert::AreEqual(E_NOINTERFACE, textLayout->GetInlineObject(0, &value));

            ValidateStoredErrorState(E_NOINTERFACE, Strings::ExternalInlineObject);
        }

        TEST_METHOD_EX(CanvasTextLayoutTests_InlineObject_DrawGlyphRunReturnsValidRenderer)
        {
            NonStubbedFixture f;
            auto textLayout = f.CreateSimpleTextLayout();

            auto inlineObject = Make<CustomInlineObject>();
            Assert::AreEqual(S_OK, textLayout->SetInlineObject(0, 1, inlineObject.Get()));

            inlineObject->DrawMethod.SetExpectedCalls(1,
                [&](
                ICanvasTextRenderer* textRenderer,
                Vector2 baselineOrigin,
                boolean isSideways,
                boolean isRightToLeft,
                IInspectable* brush)
            {
                // Sanity check the returned renderer.
                Assert::IsNotNull(textRenderer);

                float dpi;
                ThrowIfFailed(textRenderer->get_Dpi(&dpi));
                Assert::AreEqual(DEFAULT_DPI, dpi);

                Matrix3x2 transform;
                ThrowIfFailed(textRenderer->get_Transform(&transform));
                Assert::AreEqual(Matrix3x2{ 1, 0, 0, 1, 0, 0 }, transform);

                return S_OK;
            });

            f.DrawTextLayoutToSomewhere(textLayout);
        }

        struct BadRendererReferenceFixture : public NonStubbedFixture
        {
            ComPtr<ICanvasTextRenderer> BadReference;

            BadRendererReferenceFixture()
            {
                auto textLayout = CreateSimpleTextLayout();

                auto inlineObject = Make<CustomInlineObject>();
                Assert::AreEqual(S_OK, textLayout->SetInlineObject(0, 1, inlineObject.Get()));

                inlineObject->DrawMethod.SetExpectedCalls(1,
                    [&](
                    ICanvasTextRenderer* textRenderer,
                    Vector2 baselineOrigin,
                    boolean isSideways,
                    boolean isRightToLeft,
                    IInspectable* brush)
                    {
                        BadReference = textRenderer;

                        return S_OK;
                    });

                DrawTextLayoutToSomewhere(textLayout);

                Assert::IsNotNull(BadReference.Get());
            }

        };

        TEST_METHOD_EX(CanvasTextLayoutTests_InlineObject_DrawImplementation_ShouldntHangOnToRenderer)
        {
            BadRendererReferenceFixture f;

            Assert::AreEqual(E_INVALIDARG, f.BadReference->DrawGlyphRun(Vector2{}, nullptr, 0, 0, nullptr, false, 0, false, CanvasTextMeasuringMode::Natural, nullptr, nullptr, 0, nullptr, 0, CanvasGlyphOrientation::Upright));
            ValidateStoredErrorState(E_INVALIDARG, Strings::TextRendererNotValid);

            Assert::AreEqual(E_INVALIDARG, f.BadReference->DrawStrikethrough(Vector2{}, 0, 0, 0, CanvasTextDirection::LeftToRightThenTopToBottom, nullptr, CanvasTextMeasuringMode::Natural, nullptr, CanvasGlyphOrientation::Upright));
            ValidateStoredErrorState(E_INVALIDARG, Strings::TextRendererNotValid);

            Assert::AreEqual(E_INVALIDARG, f.BadReference->DrawUnderline(Vector2{}, 0, 0, 0, 0, CanvasTextDirection::LeftToRightThenTopToBottom, nullptr, CanvasTextMeasuringMode::Natural, nullptr, CanvasGlyphOrientation::Upright));
            ValidateStoredErrorState(E_INVALIDARG, Strings::TextRendererNotValid);

            Assert::AreEqual(E_INVALIDARG, f.BadReference->DrawInlineObject(Vector2{}, nullptr, false, false, nullptr, CanvasGlyphOrientation::Upright));
            ValidateStoredErrorState(E_INVALIDARG, Strings::TextRendererNotValid);

            Assert::AreEqual(E_INVALIDARG, f.BadReference->get_Dpi(nullptr));
            ValidateStoredErrorState(E_INVALIDARG, Strings::TextRendererNotValid);

            Assert::AreEqual(E_INVALIDARG, f.BadReference->get_PixelSnappingDisabled(nullptr));
            ValidateStoredErrorState(E_INVALIDARG, Strings::TextRendererNotValid);

            Assert::AreEqual(E_INVALIDARG, f.BadReference->get_Transform(nullptr));
            ValidateStoredErrorState(E_INVALIDARG, Strings::TextRendererNotValid);
        }

        struct GetLineMetricsFixture : public Fixture
        {
            DWriteMetricsType GetDWriteLineMetrics(int seed)
            {
                uint32_t inc = seed * 7;

                DWriteMetricsType metrics{};
                metrics.length = 1u + inc;
                metrics.trailingWhitespaceLength = 2u + inc;
                metrics.newlineLength = 3u + inc;
                metrics.height = 4.0f + inc;
                metrics.baseline = 5.0f + inc;
                metrics.isTrimmed = seed % 2 == 0 ? TRUE : FALSE;

#if WINVER > _WIN32_WINNT_WINBLUE
                metrics.leadingBefore = 6.0f + inc;
                metrics.leadingAfter = 7.0f + inc;
#endif              
                return metrics;
            }

            CanvasLineMetrics GetLineMetrics(int seed)
            {
                int inc = seed * 7;

                CanvasLineMetrics metrics{};
                metrics.CharacterCount = 1 + inc;
                metrics.TrailingWhitespaceCount = 2 + inc;
                metrics.TerminalNewlineCount = 3 + inc;
                metrics.Height = 4.0f + inc;
                metrics.Baseline = 5.0f + inc;
                metrics.IsTrimmed = seed % 2 == 0;

#if WINVER > _WIN32_WINNT_WINBLUE
                metrics.LeadingWhitespaceBefore = 6.0f + inc;
                metrics.LeadingWhitespaceAfter = 7.0f + inc;
#endif                    
                return metrics;
            }

            void ExpectGetLineMetrics(int callCount, std::function<HRESULT(DWriteMetricsType*, uint32_t, uint32_t*)> mock)
            {
#if WINVER > _WIN32_WINNT_WINBLUE
                auto& lineMetricsMethod = Adapter->MockTextLayout->GetLineMetricsMethod1;
#else
                auto& lineMetricsMethod = Adapter->MockTextLayout->GetLineMetricsMethod;
#endif
                lineMetricsMethod.SetExpectedCalls(callCount, mock);
            }
        };

        TEST_METHOD_EX(CanvasTextLayoutTests_LineMetrics_CallsThrough)
        {
            GetLineMetricsFixture f;

            auto textLayout = f.CreateSimpleTextLayout();

            uint32_t callCount = 0;

            f.ExpectGetLineMetrics(2,
                [&](DWriteMetricsType* lineMetrics, UINT32 maxLineCount, UINT32* actualLineCount)
                {
                    callCount++;
                    if (callCount == 1)
                    {
                        Assert::IsNull(lineMetrics);
                        Assert::AreEqual(0u, maxLineCount);
                        *actualLineCount = 3;
                        return E_NOT_SUFFICIENT_BUFFER;
                    }
                    else
                    {
                        for (uint32_t i = 0; i < 3; ++i)
                        {
                            lineMetrics[i] = f.GetDWriteLineMetrics(i);
                        }
                        Assert::AreEqual(3u, maxLineCount);
                        return S_OK;
                    }
                });

            uint32_t valueCount;
            CanvasLineMetrics* valueElements;
            Assert::AreEqual(S_OK, textLayout->get_LineMetrics(&valueCount, &valueElements));
            Assert::AreEqual(3u, valueCount);
            for (uint32_t i = 0; i < 3; ++i)
            {
                CanvasLineMetrics expected = f.GetLineMetrics(i);
                Assert::AreEqual(0, memcmp(&expected, &valueElements[i], sizeof(expected)));
            }
        }
    };
}

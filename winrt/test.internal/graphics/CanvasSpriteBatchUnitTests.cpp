// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include <WindowsNumerics.h>

#include <lib/drawing/CanvasSpriteBatch.h>
#include "../mocks/MockD2DSpriteBatch.h"

using namespace Windows::Foundation::Numerics;


static ComPtr<ICanvasSpriteBatchStatics> GetSpriteBatchStatics()
{
    ComPtr<ICanvasSpriteBatchStatics> spriteBatch;
    ThrowIfFailed(MakeAndInitialize<CanvasSpriteBatchStatics>(&spriteBatch));
    return spriteBatch;
}


struct MockD2DDeviceContextThatDoesNotSupportSpriteBatch : public MockD2DDeviceContext
{
    STDMETHOD(QueryInterface)(REFIID riid, _Outptr_result_nullonfailure_ void **ppvObject)
    {
        if (riid == __uuidof(ID2D1DeviceContext3))
            return E_NOINTERFACE;
            
        return RuntimeClass::QueryInterface(riid, ppvObject);
    }
};


static ComPtr<CanvasDevice> MakeCanvasDeviceThatDoesNotSupportSpriteBatch()
{
    auto d2dDevice = Make<MockD2DDevice>();
    d2dDevice->MockCreateDeviceContext = [&] (auto, auto deviceContext)
    {
        Make<MockD2DDeviceContextThatDoesNotSupportSpriteBatch>().CopyTo(deviceContext);
    };

    return Make<CanvasDevice>(d2dDevice.Get());
}


auto gSortModes =
{
    CanvasSpriteSortMode::None,
    CanvasSpriteSortMode::Bitmap
};


auto gValidInterpolations =
{
    CanvasImageInterpolation::NearestNeighbor,
    CanvasImageInterpolation::Linear
};


auto gInvalidInterpolations =
{
    CanvasImageInterpolation::Cubic,
    CanvasImageInterpolation::MultiSampleLinear,
    CanvasImageInterpolation::Anisotropic,
    CanvasImageInterpolation::HighQualityCubic
};


auto gSpriteOptions =
{
    CanvasSpriteOptions::None,
    CanvasSpriteOptions::ClampToSourceRect
};


auto gOffsets =
{
    float2::zero(),
    float2(1.0f),
    float2(2.0f, 3.0f)
};


Rect gAnyRect{ 0.0f, 1.0f, 2.0f, 3.0f };


auto gRects =
{
    Rect{ 0.0f, 1.0f, 2.0f, 3.0f },
    Rect{ -100.0f, 200.0f, 300.0f, 400.0f },
    Rect{ 0.0f, -1.0f, -2.0f, -3.0f }
};


auto gMatrices =
{
    Matrix3x2{ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },
    Matrix3x2{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f }
};


Vector4 gAnyTint{ 0.0f, 1.0f, 2.0f, 3.0f };


auto gTints =
{
    Vector4{ 1.0f, 1.0f, 1.0f, 1.0f },
    Vector4{ 0.0f, 0.0f, 0.0f, 0.0f },
    Vector4{ 1.0f, 2.0f, 3.0f, 4.0f }
};

struct ExpectedSprite
{
    D2D1_RECT_F DestinationRect;
    D2D1_RECT_U SourceRect;
    D2D1_COLOR_F Color;
    D2D1_MATRIX_3X2_F Transform;
    
    ExpectedSprite(D2D1_RECT_F dest, D2D1_RECT_U source, D2D1_COLOR_F color = { 1, 1, 1, 1 }, D2D1_MATRIX_3X2_F transform = D2D1::IdentityMatrix())
        : DestinationRect(dest)
        , SourceRect(source)
        , Color(color)
        , Transform(transform)
    {
    }
    
    void Validate(D2D1_RECT_F dest, D2D1_RECT_U source, D2D1_COLOR_F color, D2D1_MATRIX_3X2_F transform)
    {
        Assert::AreEqual(DestinationRect, dest);
        Assert::AreEqual(SourceRect, source);
        Assert::AreEqual(Color, color);
        Assert::AreEqual(Transform, transform);
    }
};

template<typename T = ExpectedSprite>
class AddSpritesValidator
{
    std::vector<T> m_expectedSprites;
    
public:
    template<typename... ARGS>
    void Expect(ARGS... args)
    {
        m_expectedSprites.emplace_back(args...);
    }
    
    auto GetValidator()
    {
        return [=] (uint32_t count, const D2D1_RECT_F* dstRects, const D2D1_RECT_U* srcRects, const D2D1_COLOR_F* colors, const D2D1_MATRIX_3X2_F* transforms, uint32_t dstStride, uint32_t srcStride, uint32_t colorStride, uint32_t transformStride)
        {
            Assert::AreEqual<size_t>(m_expectedSprites.size(), count);
            
            for (auto i = 0U; i < count; ++i)
            {
                auto dstRect   = Get(dstRects,   dstStride,       i);
                auto srcRect   = Get(srcRects,   srcStride,       i);
                auto color     = Get(colors,     colorStride,     i);
                auto transform = Get(transforms, transformStride, i);
                    
                m_expectedSprites[i].Validate(dstRect, srcRect, color, transform);
            }
            
            return S_OK;
        };
    }

    uint32_t Count() const
    {
        return static_cast<uint32_t>(m_expectedSprites.size());
    }
    
private:
    template<typename T>
    static T const& Get(T const* base, uint32_t stride, uint32_t index)
    {
        return *reinterpret_cast<T const*>(reinterpret_cast<uint8_t const*>(base) + (stride * index));
    }
};


TEST_CLASS(CanvasSpriteBatchUnitTests)
{
public:

    //
    // CanvasSpriteBatch.IsSupported
    //
    
    TEST_METHOD_EX(CanvasSpriteBatch_IsSupported_FailsWhenPassedNullParameters)
    {
        auto s = GetSpriteBatchStatics();

        boolean value;
        
        Assert::AreEqual(E_INVALIDARG, s->IsSupported(nullptr, &value));
        Assert::AreEqual(E_INVALIDARG, s->IsSupported(Make<MockCanvasDevice>().Get(), nullptr));
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_IsSupported_ReturnsTrueWhenSupported)
    {
        auto s = GetSpriteBatchStatics();
        auto device = Make<CanvasDevice>(Make<StubD2DDevice>().Get());
        
        boolean value{};
        ThrowIfFailed(s->IsSupported(device.Get(), &value));

        Assert::IsTrue(!!value);
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_IsSupported_ReturnsFalse_WhenSpriteBatchNotSupported)
    {
        auto s = GetSpriteBatchStatics();

        auto device = MakeCanvasDeviceThatDoesNotSupportSpriteBatch();

        boolean value{};
        ThrowIfFailed(s->IsSupported(device.Get(), &value));

        Assert::IsFalse(!!value);
    }

    
    struct Fixture
    {
        ComPtr<MockD2DDeviceContext> DeviceContext;
        ComPtr<CanvasDrawingSession> DrawingSession;

        float BitmapDpi;
        D2D1_SIZE_F BitmapSize;
        D2D1_SIZE_U BitmapSizeInPixels;
        ComPtr<StubD2DBitmap> D2DBitmap;
        ComPtr<CanvasBitmap> Bitmap;

        AddSpritesValidator<> ExpectedSprites;

        Fixture(ComPtr<MockD2DDeviceContext> deviceContext = Make<MockD2DDeviceContext>())
            : DeviceContext(deviceContext)
            , DrawingSession(Make<CanvasDrawingSession>(deviceContext.Get()))
            , BitmapDpi(DEFAULT_DPI * 2)
            , BitmapSize{ 100.0f, 100.0f }
            , BitmapSizeInPixels{ 200U, 200U }
            , D2DBitmap(Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_NONE, BitmapDpi))
            , Bitmap(Make<CanvasBitmap>(Make<MockCanvasDevice>().Get(), D2DBitmap.Get()))
        {
            D2DBitmap->GetSizeMethod.AllowAnyCall([=] { return BitmapSize; });
            D2DBitmap->GetPixelSizeMethod.AllowAnyCall([=] { return BitmapSizeInPixels; });

            DeviceContext->GetUnitModeMethod.AllowAnyCall([] { return D2D1_UNIT_MODE_DIPS; });
            DeviceContext->GetAntialiasModeMethod.AllowAnyCall([] { return D2D1_ANTIALIAS_MODE_ALIASED; });
        }

        ComPtr<MockD2DSpriteBatch> ExpectCreateSpriteBatch()
        {
            auto spriteBatch = Make<MockD2DSpriteBatch>();
            
            DeviceContext->CreateSpriteBatchMethod.SetExpectedCalls(1,
                [=] (ID2D1SpriteBatch** value)
                {
                    return spriteBatch.CopyTo(value);
                });

            spriteBatch->AddSpritesMethod.AllowAnyCall();

            return spriteBatch;
        }

        ComPtr<MockD2DSpriteBatch> ExpectAndValidateCreateSpriteBatch()
        {
            auto spriteBatch = ExpectCreateSpriteBatch();
            spriteBatch->AddSpritesMethod.SetExpectedCalls(1, ExpectedSprites.GetValidator());
            return spriteBatch;
        }

        template<typename... ARGS>
        void ExpectSprite(ARGS... args)
        {
            ExpectedSprites.Expect(args...);
        }

        D2D1_RECT_F FullBitmapDestRect(float2 offset)
        {
            return D2D1_RECT_F{ offset.x, offset.y, offset.x + BitmapSize.width, offset.y + BitmapSize.height };
        }

        D2D1_RECT_U FullBitmapSourceRect()
        {
            return D2D1_RECT_U{ 0U, 0U, BitmapSizeInPixels.width, BitmapSizeInPixels.height };
        }

        Fixture(Fixture const&) = delete;
        Fixture& operator=(Fixture const&) = delete;
    };


    //
    // CanvasDrawingSession.CreateSpriteBatch
    //

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatch_Fails_WhenSpriteBatchNotSupported)
    {
        Fixture f(Make<MockD2DDeviceContextThatDoesNotSupportSpriteBatch>());

        ComPtr<ICanvasSpriteBatch> spriteBatch;
        
        Assert::AreEqual(E_NOTIMPL, f.DrawingSession->CreateSpriteBatch(&spriteBatch));
        ValidateStoredErrorState(E_NOTIMPL, Strings::SpriteBatchNotAvailable);
        
        Assert::AreEqual(E_NOTIMPL, f.DrawingSession->CreateSpriteBatchWithSortMode(CanvasSpriteSortMode::None, &spriteBatch));
        ValidateStoredErrorState(E_NOTIMPL, Strings::SpriteBatchNotAvailable);
        
        Assert::AreEqual(E_NOTIMPL, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolation(CanvasSpriteSortMode::None, CanvasImageInterpolation::Linear, &spriteBatch));
        ValidateStoredErrorState(E_NOTIMPL, Strings::SpriteBatchNotAvailable);

        Assert::AreEqual(E_NOTIMPL, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolationAndOptions(CanvasSpriteSortMode::None, CanvasImageInterpolation::Linear, CanvasSpriteOptions::None, &spriteBatch));
        ValidateStoredErrorState(E_NOTIMPL, Strings::SpriteBatchNotAvailable);
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatch_Succeeds_WhenSpriteBatchSupported)
    {
        Fixture f;

        ComPtr<ICanvasSpriteBatch> spriteBatch;
        
        Assert::AreEqual(S_OK, f.DrawingSession->CreateSpriteBatch(&spriteBatch));
        Assert::AreEqual(S_OK, f.DrawingSession->CreateSpriteBatchWithSortMode(CanvasSpriteSortMode::None, &spriteBatch));
        Assert::AreEqual(S_OK, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolation(CanvasSpriteSortMode::None, CanvasImageInterpolation::Linear, &spriteBatch));
        Assert::AreEqual(S_OK, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolationAndOptions(CanvasSpriteSortMode::None, CanvasImageInterpolation::Linear, CanvasSpriteOptions::None, &spriteBatch));
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatch_FailsWhenPassedNullParameter)
    {
        Fixture f;
        Assert::AreEqual(E_INVALIDARG, f.DrawingSession->CreateSpriteBatch(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DrawingSession->CreateSpriteBatchWithSortMode(CanvasSpriteSortMode::None, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolation(CanvasSpriteSortMode::None, CanvasImageInterpolation::Linear, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolationAndOptions(CanvasSpriteSortMode::None, CanvasImageInterpolation::Linear, CanvasSpriteOptions::None, nullptr));
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_When_NoSpritesAreDrawn_SpriteBatchIsCreatedOrDrawn)
    {
        Fixture f;

        ComPtr<ICanvasSpriteBatch> spriteBatch;
        ThrowIfFailed(f.DrawingSession->CreateSpriteBatch(&spriteBatch));

        ThrowIfFailed(As<IClosable>(spriteBatch)->Close());
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatch_CallsDrawSpriteBatch_WithDefaultOptions)
    {
        TestDrawSpriteBatchOptions(D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE,
            [] (CanvasDrawingSession* ds, ICanvasSpriteBatch** spriteBatch)
            {
                return ds->CreateSpriteBatch(spriteBatch);
            });
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatchWithSortMode_CallsDrawSpriteBatch_WithDefaults)
    {
        for (auto sortMode : gSortModes)
        {
            TestDrawSpriteBatchOptions(D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE,
                [=] (CanvasDrawingSession* ds, ICanvasSpriteBatch** spriteBatch)
                {
                    return ds->CreateSpriteBatchWithSortMode(sortMode, spriteBatch);
                });            
        }
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatchWithSortModeAndInterpolation_CallsDrawSpriteBatch_WithCorrectValue)
    {
        for (auto sortMode : gSortModes)
        {
            for (auto interpolation : gValidInterpolations)
            {
                TestDrawSpriteBatchOptions(static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(interpolation), D2D1_SPRITE_OPTIONS_NONE,
                    [=] (CanvasDrawingSession* ds, ICanvasSpriteBatch** spriteBatch)
                    {
                        return ds->CreateSpriteBatchWithSortModeAndInterpolation(sortMode, interpolation, spriteBatch);
                    });            
            }
        }
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatchWithSortModeAndInterpolation_FallsWhenPassedInvalidInterpolationMode)
    {
        Fixture f;
        
        for (auto interpolation : gInvalidInterpolations)
        {
            ComPtr<ICanvasSpriteBatch> spriteBatch;
            Assert::AreEqual(E_INVALIDARG, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolation(CanvasSpriteSortMode::None, interpolation, &spriteBatch));
            ValidateStoredErrorState(E_INVALIDARG, Strings::SpriteBatchInvalidInterpolation);
        }
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatchWithSortModeAndInterpolationAndOptions_CallsDrawSpriteBatch_WithCorrectValue)
    {
        for (auto sortMode : gSortModes)
        {
            for (auto interpolation : gValidInterpolations)
            {
                for (auto options : gSpriteOptions)
                {
                    TestDrawSpriteBatchOptions(static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(interpolation), static_cast<D2D1_SPRITE_OPTIONS>(options),
                        [=] (CanvasDrawingSession* ds, ICanvasSpriteBatch** spriteBatch)
                        {
                            return ds->CreateSpriteBatchWithSortModeAndInterpolationAndOptions(sortMode, interpolation, options, spriteBatch);
                        });            
                }
            }
        }
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatchWithSortModeAndInterpolationAndOptions_FallsWhenPassedInvalidInterpolationMode)
    {
        Fixture f;
        
        for (auto interpolation : gInvalidInterpolations)
        {
            for (auto options : gSpriteOptions)
            {
                ComPtr<ICanvasSpriteBatch> spriteBatch;
                Assert::AreEqual(E_INVALIDARG, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolationAndOptions(CanvasSpriteSortMode::None, interpolation, options, &spriteBatch));
                ValidateStoredErrorState(E_INVALIDARG, Strings::SpriteBatchInvalidInterpolation);
            }
        }
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_CreateSpriteBatchWithSortModeAndInterpolationAndOptions_FailsWhenPassedInvalidOptions)
    {
        Fixture f;

        ComPtr<ICanvasSpriteBatch> spriteBatch;

        auto invalidOptions = static_cast<CanvasSpriteOptions>(static_cast<int>(CanvasSpriteOptions::ClampToSourceRect) + 1);
        Assert::AreEqual(E_INVALIDARG, f.DrawingSession->CreateSpriteBatchWithSortModeAndInterpolationAndOptions(CanvasSpriteSortMode::None, CanvasImageInterpolation::Linear, invalidOptions, &spriteBatch));
    }

    
    void TestDrawSpriteBatchOptions(
        D2D1_BITMAP_INTERPOLATION_MODE expectedInterpolationMode,
        D2D1_SPRITE_OPTIONS expectedOptions,
        std::function<HRESULT(CanvasDrawingSession*, ICanvasSpriteBatch**)> fn)
    {        
        Fixture f;

        // Create a sprite batch using the supplied function
        ComPtr<ICanvasSpriteBatch> spriteBatch;
        ThrowIfFailed(fn(f.DrawingSession.Get(), &spriteBatch));

        // Draw a single sprite - just to trigger creating / drawing a sprite
        // batch.  This test is only interested in the options passed to
        // DrawSpriteBatch, not the details of the sprites.
        ThrowIfFailed(spriteBatch->DrawAtOffset(f.Bitmap.Get(), float2::zero()));

        auto expectedD2DSpriteBatch = f.ExpectCreateSpriteBatch();

        f.DeviceContext->DrawSpriteBatchMethod.SetExpectedCalls(1,
            [&] (auto actualD2DSpriteBatch, auto, auto, auto, auto interpolationMode, auto options)
            {
                Assert::IsTrue(IsSameInstance(expectedD2DSpriteBatch.Get(), actualD2DSpriteBatch));
                Assert::AreEqual(expectedInterpolationMode, interpolationMode);
                Assert::AreEqual(expectedOptions, options);
            });

        ThrowIfFailed(As<IClosable>(spriteBatch)->Close());
    }


    TEST_METHOD_EX(CanvasSpriteBatch_DestructionIsTheSameAsClosing)
    {
        Fixture f;

        ComPtr<ICanvasSpriteBatch> spriteBatch;
        ThrowIfFailed(f.DrawingSession->CreateSpriteBatch(&spriteBatch));

        ThrowIfFailed(spriteBatch->DrawAtOffset(f.Bitmap.Get(), float2::zero()));

        auto d2dSpriteBatch = f.ExpectCreateSpriteBatch();
        f.DeviceContext->DrawSpriteBatchMethod.SetExpectedCalls(1);

        spriteBatch.Reset();
    }


    TEST_METHOD_EX(CanvasSpriteBatch_ErrorsDuringDestructionAreSwallowed)
    {
        Fixture f;

        ComPtr<ICanvasSpriteBatch> spriteBatch;
        ThrowIfFailed(f.DrawingSession->CreateSpriteBatch(&spriteBatch));

        ThrowIfFailed(spriteBatch->DrawAtOffset(f.Bitmap.Get(), float2::zero()));

        f.DeviceContext->CreateSpriteBatchMethod.SetExpectedCalls(1,
            [=] (auto)
            {
                return E_UNEXPECTED;
            });
        
        spriteBatch.Reset();
    }


    struct DrawFixture : public Fixture
    {
        ComPtr<ICanvasSpriteBatch> SpriteBatch;

        DrawFixture(D2D1_UNIT_MODE unitMode = D2D1_UNIT_MODE_DIPS)
        {
            DeviceContext->GetUnitModeMethod.AllowAnyCall([=] { return unitMode; });
            
            ThrowIfFailed(DrawingSession->CreateSpriteBatch(&SpriteBatch));
        }

        void Validate()
        {
            auto expectedD2DSpriteBatch = ExpectAndValidateCreateSpriteBatch();

            DeviceContext->DrawSpriteBatchMethod.SetExpectedCalls(1,
                [&] (auto d2dSpriteBatch, auto startIndex, auto spriteCount, auto bitmap, auto, auto)
                {
                    Assert::IsTrue(IsSameInstance(expectedD2DSpriteBatch.Get(), d2dSpriteBatch));
                    Assert::AreEqual(0U, startIndex);
                    Assert::AreEqual(ExpectedSprites.Count(), spriteCount);
                    Assert::IsTrue(IsSameInstance(D2DBitmap.Get(), bitmap));
                });

            ThrowIfFailed(As<IClosable>(SpriteBatch)->Close());
        }
    };


    //
    // CanvasSpriteBatch.Device
    //

    
    TEST_METHOD_EX(CanvasSpriteBatch_get_Device_FailsWhenPassedNull)
    {
        DrawFixture f;

        Assert::AreEqual(E_INVALIDARG, As<ICanvasResourceCreator>(f.SpriteBatch)->get_Device(nullptr));
    }


    TEST_METHOD_EX(CanvasSpriteBatch_get_Device_ReturnsDeviceFromDeviceContext)
    {
        DrawFixture f;

        auto d2dDevice = Make<MockD2DDevice>();
        f.DeviceContext->GetDeviceMethod.SetExpectedCalls(1,
            [=] (ID2D1Device** d)
            {
                return d2dDevice.CopyTo(d);
            });

        ComPtr<ICanvasDevice> retrievedDevice;
        ThrowIfFailed(As<ICanvasResourceCreator>(f.SpriteBatch)->get_Device(&retrievedDevice));

        auto retrievedD2DDevice = GetWrappedResource<ID2D1Device>(retrievedDevice);

        Assert::IsTrue(IsSameInstance(d2dDevice.Get(), retrievedD2DDevice.Get()));
    }

    //
    // ICanvasResourceCreatorWithDpi methods
    //

    
    TEST_METHOD_EX(CanvasSpriteBatch_get_Dpi_FailsWhenPassedNull)
    {
        DrawFixture f;

        Assert::AreEqual(E_INVALIDARG, As<ICanvasResourceCreatorWithDpi>(f.SpriteBatch)->get_Dpi(nullptr));
    }


    TEST_METHOD_EX(CanvasSpriteBatch_DpiProperties)
    {
        const float dpi = 144;

        DrawFixture f;

        f.DeviceContext->GetDpiMethod.AllowAnyCall([&](float* dpiX, float* dpiY)
        {
            *dpiX = dpi;
            *dpiY = dpi;
        });

        auto spriteBatchDpi = As<ICanvasResourceCreatorWithDpi>(f.SpriteBatch);

        float actualDpi = 0;
        ThrowIfFailed(spriteBatchDpi->get_Dpi(&actualDpi));
        Assert::AreEqual(dpi, actualDpi);

        VerifyConvertDipsToPixels(dpi, spriteBatchDpi);

        const float testValue = 100;
        float dips = 0;
        ThrowIfFailed(spriteBatchDpi->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / dpi, dips);
    }


    //
    // Draw Methods
    //

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawMethodsFailWhenPassedNullBitmap)
    {
        DrawFixture f;

        Rect destRect{};
        Vector2 offset{};
        Matrix3x2 transform{};
        Vector4 tint{};
        CanvasSpriteFlip flip{};
        Vector2 origin{};
        float rotation{};
        Vector2 scale{};
        Rect sourceRect{};
    
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawToRect(nullptr, destRect)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawAtOffset(nullptr, offset)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawWithTransform(nullptr, transform));
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawToRectWithTint(nullptr, destRect, tint)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawAtOffsetWithTint(nullptr, offset, tint)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawWithTransformAndTint(nullptr, transform, tint)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawToRectWithTintAndFlip(nullptr, destRect, tint, flip)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawWithTransformAndTintAndFlip(nullptr, transform, tint, flip)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawAtOffsetWithTintAndTransform(nullptr, offset, tint, origin, rotation, scale, flip)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetToRect(nullptr, destRect, sourceRect));
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetAtOffset(nullptr, offset, sourceRect)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetWithTransform(nullptr, transform, sourceRect)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetToRectWithTint(nullptr, destRect, sourceRect, tint)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetAtOffsetWithTint(nullptr, offset, sourceRect, tint)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetWithTransformAndTint(nullptr, transform, sourceRect, tint)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetToRectWithTintAndFlip(nullptr, destRect, sourceRect, tint, flip)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetWithTransformAndTintAndFlip(nullptr, transform, sourceRect, tint, flip)); 
        Assert::AreEqual(E_INVALIDARG, f.SpriteBatch->DrawFromSpriteSheetAtOffsetWithTintAndTransform(nullptr, offset, sourceRect, tint, origin, rotation, scale, flip)); 
    }


    TEST_METHOD_EX(CanvasSpriteBatch_MethodsFail_AfterClosed)
    {
        DrawFixture f;

        ThrowIfFailed(As<IClosable>(f.SpriteBatch)->Close());

        auto bitmap = f.Bitmap.Get();
        Rect destRect{};
        Vector2 offset{};
        Matrix3x2 transform{};
        Vector4 tint{};
        CanvasSpriteFlip flip{};
        Vector2 origin{};
        float rotation{};
        Vector2 scale{};
        Rect sourceRect{};
    
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawToRect(bitmap, destRect)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawAtOffset(bitmap, offset)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawWithTransform(bitmap, transform));
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawToRectWithTint(bitmap, destRect, tint)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawAtOffsetWithTint(bitmap, offset, tint)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawWithTransformAndTint(bitmap, transform, tint)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawToRectWithTintAndFlip(bitmap, destRect, tint, flip)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawWithTransformAndTintAndFlip(bitmap, transform, tint, flip)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawAtOffsetWithTintAndTransform(bitmap, offset, tint, origin, rotation, scale, flip)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetToRect(bitmap, destRect, sourceRect));
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetAtOffset(bitmap, offset, sourceRect)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetWithTransform(bitmap, transform, sourceRect)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetToRectWithTint(bitmap, destRect, sourceRect, tint)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetAtOffsetWithTint(bitmap, offset, sourceRect, tint)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetWithTransformAndTint(bitmap, transform, sourceRect, tint)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetToRectWithTintAndFlip(bitmap, destRect, sourceRect, tint, flip)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetWithTransformAndTintAndFlip(bitmap, transform, sourceRect, tint, flip)); 
        Assert::AreEqual(RO_E_CLOSED, f.SpriteBatch->DrawFromSpriteSheetAtOffsetWithTintAndTransform(bitmap, offset, sourceRect, tint, origin, rotation, scale, flip));

        ComPtr<ICanvasDevice> device;
        Assert::AreEqual(RO_E_CLOSED, As<ICanvasResourceCreator>(f.SpriteBatch)->get_Device(&device));

        float dpi{};
        float dips{};
        CanvasDpiRounding dpiRounding{};
        int32_t pixels{};

        Assert::AreEqual(RO_E_CLOSED, As<ICanvasResourceCreatorWithDpi>(f.SpriteBatch)->get_Dpi(&dpi));
        Assert::AreEqual(RO_E_CLOSED, As<ICanvasResourceCreatorWithDpi>(f.SpriteBatch)->ConvertPixelsToDips(pixels, &dips));
        Assert::AreEqual(RO_E_CLOSED, As<ICanvasResourceCreatorWithDpi>(f.SpriteBatch)->ConvertDipsToPixels(dips, dpiRounding, &pixels));
    }


    TEST_METHOD_EX(CanvasSpriteBatch_DrawAtOffset)
    {
        DrawFixture f;

        for (auto offset : gOffsets)
        {
            ThrowIfFailed(f.SpriteBatch->DrawAtOffset(f.Bitmap.Get(), offset));

            f.ExpectSprite(
                f.FullBitmapDestRect(offset),
                f.FullBitmapSourceRect());
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawAtOffsetWithTint)
    {
        DrawFixture f;

        for (auto tint : gTints)
        {
            ThrowIfFailed(f.SpriteBatch->DrawAtOffsetWithTint(f.Bitmap.Get(), float2::zero(), tint));

            f.ExpectSprite(
                f.FullBitmapDestRect(float2::zero()),
                f.FullBitmapSourceRect(),
                *ReinterpretAs<D2D1_COLOR_F*>(&tint));
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetAtOffset)
    {
        DrawFixture f;

        for (auto offset : gOffsets)
        {
            auto width = 30.0f;
            auto height = 40.0f;
            Rect sourceRect{ 10.0f, 20.0f, width, height };
            
            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetAtOffset(f.Bitmap.Get(), offset, sourceRect));

            f.ExpectSprite(
                D2D1_RECT_F{ offset.x, offset.y, offset.x + width, offset.y + height },
                D2D1_RECT_U{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) });
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetAtOffsetWithTint)
    {
        DrawFixture f;

        for (auto tint : gTints)
        {
            auto width = 30.0f;
            auto height = 40.0f;
            Rect sourceRect{ 10.0f, 20.0f, width, height };
            
            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetAtOffsetWithTint(f.Bitmap.Get(), float2::zero(), sourceRect, tint));

            f.ExpectSprite(
                D2D1_RECT_F{ 0.0f, 0.0f, width, height },
                D2D1_RECT_U{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) },
                *ReinterpretAs<D2D1_COLOR_F*>(&tint));
        }

        f.Validate();
    }

    TEST_METHOD_EX(CanvasSpriteBatch_DrawToRect)
    {
        DrawFixture f;

        for (auto rect: gRects)
        {
            ThrowIfFailed(f.SpriteBatch->DrawToRect(f.Bitmap.Get(), rect));

            f.ExpectSprite(
                ToD2DRect(rect),
                f.FullBitmapSourceRect());                
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawToRectWithTint)
    {
        DrawFixture f;

        for (auto tint : gTints)
        {
            auto rect = gAnyRect;

            ThrowIfFailed(f.SpriteBatch->DrawToRectWithTint(f.Bitmap.Get(), rect, tint));

            f.ExpectSprite(
                ToD2DRect(rect),
                f.FullBitmapSourceRect(),
                *ReinterpretAs<D2D1_COLOR_F*>(&tint));
        }

        f.Validate();
    }

    
    static auto GenerateFlipTestCases(D2D1_RECT_U s)
    {
        std::array<std::pair<CanvasSpriteFlip, D2D1_RECT_U>, 4> cases =
            {{
                    { CanvasSpriteFlip::None,       D2D1_RECT_U{ s.left, s.top, s.right, s.bottom } },
                    { CanvasSpriteFlip::Horizontal, D2D1_RECT_U{ s.right, s.top, s.left, s.bottom } },
                    { CanvasSpriteFlip::Vertical,   D2D1_RECT_U{ s.left, s.bottom, s.right, s.top } },
                    { CanvasSpriteFlip::Both,       D2D1_RECT_U{ s.right, s.bottom, s.left, s.top } }
                }};
        return cases;
    }


    TEST_METHOD_EX(CanvasSpriteBatch_DrawToRectWithTintAndFlip)
    {
        DrawFixture f;

        auto rect = gAnyRect;
        auto tint = gAnyTint;
        D2D1_COLOR_F color = *ReinterpretAs<D2D1_COLOR_F*>(&tint);

        for (auto& t : GenerateFlipTestCases(f.FullBitmapSourceRect()))
        {
            auto flip = t.first;
            auto srcRect = t.second;

            ThrowIfFailed(f.SpriteBatch->DrawToRectWithTintAndFlip(f.Bitmap.Get(), rect, tint, flip));

            f.ExpectSprite(
                ToD2DRect(rect),
                srcRect,
                color);
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetToRect)
    {
        DrawFixture f;

        for (auto rect : gRects)
        {
            auto width = 30.0f;
            auto height = 40.0f;
            Rect sourceRect{ 10.0f, 20.0f, width, height };

            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetToRect(f.Bitmap.Get(), rect, sourceRect));

            f.ExpectSprite(
                ToD2DRect(rect),
                D2D1_RECT_U{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) });
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetToRectWithTint)
    {
        DrawFixture f;

        for (auto tint : gTints)
        {
            auto rect = gAnyRect;

            auto width = 30.0f;
            auto height = 40.0f;
            Rect sourceRect{ 10.0f, 20.0f, width, height };

            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetToRectWithTint(f.Bitmap.Get(), rect, sourceRect, tint));

            f.ExpectSprite(
                ToD2DRect(rect),
                D2D1_RECT_U{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) },
                *ReinterpretAs<D2D1_COLOR_F*>(&tint));
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetToRectWithTintAndFlip)
    {
        DrawFixture f;

        auto rect = gAnyRect;
        auto tint = gAnyTint;
        D2D1_COLOR_F color = *ReinterpretAs<D2D1_COLOR_F*>(&tint);

        auto width = 30.0f;
        auto height = 40.0f;
        Rect sourceRect{ 10.0f, 20.0f, width, height };
        D2D1_RECT_U s{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) };

        for (auto& t : GenerateFlipTestCases(s))
        {
            auto flip = t.first;
            auto srcRect = t.second;

            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetToRectWithTintAndFlip(f.Bitmap.Get(), rect, sourceRect, tint, flip));

            f.ExpectSprite(
                ToD2DRect(rect),
                srcRect,
                color);
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawWithTransform)
    {
        DrawFixture f;

        for (auto matrix : gMatrices)
        {
            ThrowIfFailed(f.SpriteBatch->DrawWithTransform(f.Bitmap.Get(), matrix));
            
            f.ExpectSprite(
                f.FullBitmapDestRect(float2::zero()),
                f.FullBitmapSourceRect(),
                D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 1.0f },
                *ReinterpretAs<D2D1_MATRIX_3X2_F*>(&matrix));
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawWithTransformAndTint)
    {
        DrawFixture f;

        for (auto tint : gTints)
        {
            ThrowIfFailed(f.SpriteBatch->DrawWithTransformAndTint(f.Bitmap.Get(), float3x2::identity(), tint));
            
            f.ExpectSprite(
                f.FullBitmapDestRect(float2::zero()),
                f.FullBitmapSourceRect(),
                *ReinterpretAs<D2D1_COLOR_F*>(&tint),                
                D2D1::IdentityMatrix());
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawWithTransformAndTintAndFlip)
    {
        DrawFixture f;

        for (auto& t : GenerateFlipTestCases(f.FullBitmapSourceRect()))
        {
            auto flip = t.first;
            auto srcRect = t.second;

            ThrowIfFailed(f.SpriteBatch->DrawWithTransformAndTintAndFlip(f.Bitmap.Get(), float3x2::identity(), CanvasSpriteBatch::DEFAULT_TINT, flip));

            f.ExpectSprite(
                f.FullBitmapDestRect(float2::zero()),
                srcRect,
                D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 1.0f},
                D2D1::IdentityMatrix());
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetWithTransform)
    {
        DrawFixture f;

        for (auto matrix : gMatrices)
        {
            auto width = 30.0f;
            auto height = 40.0f;
            Rect sourceRect{ 10.0f, 20.0f, width, height };
            
            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetWithTransform(f.Bitmap.Get(), matrix, sourceRect));

            f.ExpectSprite(
                D2D1_RECT_F{ 0.0f, 0.0f, width, height },
                D2D1_RECT_U{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) },
                D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 1.0f},
                *ReinterpretAs<D2D1_MATRIX_3X2_F*>(&matrix));
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetWithTransformAndTint)
    {
        DrawFixture f;

        for (auto tint : gTints)
        {
            auto width = 30.0f;
            auto height = 40.0f;
            Rect sourceRect{ 10.0f, 20.0f, width, height };

            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetWithTransformAndTint(f.Bitmap.Get(), float3x2::identity(), sourceRect, tint));
            
            f.ExpectSprite(
                D2D1_RECT_F{ 0.0f, 0.0f, width, height },
                D2D1_RECT_U{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) },
                *ReinterpretAs<D2D1_COLOR_F*>(&tint),                
                D2D1::IdentityMatrix());
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetWithTransformAndTintAndFlip)
    {
        DrawFixture f;

        auto width = 30.0f;
        auto height = 40.0f;
        Rect sourceRect{ 10.0f, 20.0f, width, height };
        D2D1_RECT_U s{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) };
        
        for (auto& t : GenerateFlipTestCases(s))
        {
            auto flip = t.first;
            auto srcRect = t.second;

            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetWithTransformAndTintAndFlip(f.Bitmap.Get(), float3x2::identity(), sourceRect, CanvasSpriteBatch::DEFAULT_TINT, flip));

            f.ExpectSprite(
                D2D1_RECT_F{ 0.0f, 0.0f, width, height },
                srcRect,
                D2D1_COLOR_F{ 1.0f, 1.0f, 1.0f, 1.0f},
                D2D1::IdentityMatrix());
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawAtOffsetWithTintAndTransform)
    {
        DrawFixture f;

        for (auto& t : GenerateFlipTestCases(f.FullBitmapSourceRect()))
        {
            auto flip = t.first;
            auto srcRect = t.second;

            ThrowIfFailed(f.SpriteBatch->DrawAtOffsetWithTintAndTransform(
                f.Bitmap.Get(),
                float2{ 10.0f, 10.0f },
                CanvasSpriteBatch::DEFAULT_TINT,
                float2{ f.BitmapSize.width / 2, f.BitmapSize.height / 2 },
                DirectX::XM_PI,
                float2{ 2.0f, 2.0f},
                flip));

            D2D1_MATRIX_3X2_F expectedTransform = {
                -2.0f, 0.0f,
                0.0f, -2.0f,
                110.0f, 110.0f,
            };

            f.ExpectSprite(
                f.FullBitmapDestRect(float2::zero()),
                srcRect,
                D2D1_COLOR_F{ 1, 1, 1, 1 },
                expectedTransform);
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheetAtOffsetWithTintAndTransform)
    {
        DrawFixture f;

        auto width = 30.0f;
        auto height = 40.0f;
        Rect sourceRect{ 10.0f, 20.0f, width, height };
        D2D1_RECT_U s{ 20, 40, static_cast<uint32_t>(20 + width * 2), static_cast<uint32_t>(40 + height * 2) };

        for (auto& t : GenerateFlipTestCases(s))
        {
            auto flip = t.first;
            auto srcRect = t.second;

            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetAtOffsetWithTintAndTransform(
                f.Bitmap.Get(),
                float2{ 10.0f, 10.0f },
                sourceRect,
                CanvasSpriteBatch::DEFAULT_TINT,
                float2{ f.BitmapSize.width / 2, f.BitmapSize.height / 2 },
                DirectX::XM_PI,
                float2{ 2.0f, 2.0f},
                flip));

            D2D1_MATRIX_3X2_F expectedTransform = {
                -2.0f, 0.0f,
                0.0f, -2.0f,
                110.0f, 110.0f,
            };

            f.ExpectSprite(
                D2D1_RECT_F{ 0.0f, 0.0f, sourceRect.Width, sourceRect.Height },
                srcRect,
                D2D1_COLOR_F{ 1, 1, 1, 1 },
                expectedTransform);
        }

        f.Validate();
    }

    
    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheet_NegativeCoordinatesAreClamped)
    {
        DrawFixture f;

        std::pair<Rect, D2D1_RECT_U> cases[] =
        {
            { Rect{ -100, -100,  200,  200 }, D2D1_RECT_U{   0,   0, 200, 200 } },
            { Rect{ -100, -100,   1,     1 }, D2D1_RECT_U{   0,   0,   0,   0 } },
            { Rect{  100,  100, -200, -200 }, D2D1_RECT_U{ 200, 200,   0,   0 } }
        };

        for (auto t : cases)
        {
            Rect destRect{};
            auto sourceRect = t.first;
            auto expectedSourceRect = t.second;

            // As we're confident that all the source rects are generated
            // through the same code we only test one overload.
            
            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetToRect(f.Bitmap.Get(), destRect, sourceRect));
            f.ExpectSprite(ToD2DRect(destRect), expectedSourceRect);
        }

        f.Validate();
    }


    TEST_METHOD_EX(CanvasSpriteBatch_DrawFromSpriteSheet_SourceRectRespectsUnitsMode)
    {
        // All other tests operate with the unit mode set to DIPs.  This case
        // sets the unit mode to pixels.

        DrawFixture f(D2D1_UNIT_MODE_PIXELS);

        // The unit mode is snapped when the sprite batch is created, so
        // whatever we set it to now should make no difference

        for (auto unitMode : { D2D1_UNIT_MODE_PIXELS, D2D1_UNIT_MODE_DIPS, D2D1_UNIT_MODE_PIXELS })
        {
            f.DeviceContext->GetUnitModeMethod.AllowAnyCall([=] { return unitMode; });

            Rect dipsSourceRect{ 10, 10, 200, 200 };
            Vector2 offset{ 0, 0 };
            Rect expectedDestRect{ offset.X, offset.Y, dipsSourceRect.Width, dipsSourceRect.Height };

            D2D1_RECT_U pixelsSourceRect =
                {
                    static_cast<uint32_t>(dipsSourceRect.X),
                    static_cast<uint32_t>(dipsSourceRect.Y),
                    static_cast<uint32_t>(dipsSourceRect.X + dipsSourceRect.Width),
                    static_cast<uint32_t>(dipsSourceRect.Y + dipsSourceRect.Height)
                };
        
            ThrowIfFailed(f.SpriteBatch->DrawFromSpriteSheetAtOffset(f.Bitmap.Get(), offset, dipsSourceRect));
            f.ExpectSprite(ToD2DRect(expectedDestRect), pixelsSourceRect);
        }

        f.Validate();
    }


    TEST_METHOD_EX(CanvasSpriteBatch_UnitModeIsSetCorrectlyWhenDrawSpriteBatchIsCalled_AndThenRestored)
    {
        DrawFixture f;

        Vector2 offset{};
        ThrowIfFailed(f.SpriteBatch->DrawAtOffset(f.Bitmap.Get(), offset));
        
        f.ExpectSprite(
            f.FullBitmapDestRect(offset),
            f.FullBitmapSourceRect());


        // When the sprite batch was created, it queried the unit mode and saw
        // that it was dips.  Now, we're going to say that it is pixels.
        //
        // As a result we'll expect the unit mode to be set back to dips before
        // DrawSpriteBatch is called, and then set back to pixels afterwards.
        
        f.DeviceContext->GetUnitModeMethod.SetExpectedCalls(1, [] { return D2D1_UNIT_MODE_PIXELS; });

        int calls = 0;
        f.DeviceContext->SetUnitModeMethod.SetExpectedCalls(2,
            [=] (D2D1_UNIT_MODE mode) mutable
            {
                if (calls == 0)
                    Assert::AreEqual(D2D1_UNIT_MODE_DIPS, mode);
                else
                    Assert::AreEqual(D2D1_UNIT_MODE_PIXELS, mode);

                ++calls;
            });

        f.Validate();
    }


    TEST_METHOD_EX(CanvasSpriteBatch_WhenUnitModeIsAlreadySetCorrectly_ItIsNotModified)
    {
        DrawFixture f;

        Vector2 offset{};
        ThrowIfFailed(f.SpriteBatch->DrawAtOffset(f.Bitmap.Get(), offset));
        
        f.ExpectSprite(
            f.FullBitmapDestRect(offset),
            f.FullBitmapSourceRect());


        // When the sprite batch was created, it queried the unit mode and saw
        // that it was dips.  Now, we're going to say that it is still dips.
        //
        // As a result we don't expect the unit mode to be modified.
        
        f.DeviceContext->GetUnitModeMethod.SetExpectedCalls(1, [] { return D2D1_UNIT_MODE_DIPS; });
        f.DeviceContext->SetUnitModeMethod.SetExpectedCalls(0);

        f.Validate();
    }


    //
    // Multiple bitmaps and sorting
    //

    // ExpectedSprite where we only care about a single value that's encoded
    // into the dest rect.  This is because these tests are about how
    // batching/sorting works.  Earlier tests have validated that the correct
    // values are passed in for sprites.
    struct ExpectedOffsetSprite
    {
        float Value;

        explicit ExpectedOffsetSprite(float value)
            : Value(value)
        {}

        void Validate(D2D1_RECT_F dest, D2D1_RECT_U source, D2D1_COLOR_F color, D2D1_MATRIX_3X2_F transform)
        {
            Assert::AreEqual(Value, dest.left);
            Assert::AreEqual(Value, dest.top);
        }
    };

    struct MultipleBitmapFixture
    {
        ComPtr<MockD2DDeviceContext> DeviceContext;
        ComPtr<MockD2DSpriteBatch> D2DSpriteBatch;
        ComPtr<ICanvasSpriteBatch> SpriteBatch;

        std::array<std::pair<ComPtr<StubD2DBitmap>, ComPtr<CanvasBitmap>>, 4> Bitmaps;

        AddSpritesValidator<ExpectedOffsetSprite> ExpectedSprites;
        
        MultipleBitmapFixture(CanvasSpriteSortMode sortMode = CanvasSpriteSortMode::None)
            : DeviceContext(Make<MockD2DDeviceContext>())
        {
            DeviceContext->GetUnitModeMethod.AllowAnyCall([] { return D2D1_UNIT_MODE_DIPS; });
            DeviceContext->GetAntialiasModeMethod.AllowAnyCall([] { return D2D1_ANTIALIAS_MODE_ALIASED; });
           
            D2DSpriteBatch = Make<MockD2DSpriteBatch>();
            DeviceContext->CreateSpriteBatchMethod.SetExpectedCalls(1,
                [=] (ID2D1SpriteBatch** value)
                {
                    return D2DSpriteBatch.CopyTo(value);
                });

            D2DSpriteBatch->AddSpritesMethod.SetExpectedCalls(1, ExpectedSprites.GetValidator());

            auto drawingSession = Make<CanvasDrawingSession>(DeviceContext.Get());
            ThrowIfFailed(drawingSession->CreateSpriteBatchWithSortMode(sortMode, &SpriteBatch));

            auto device = Make<MockCanvasDevice>();
            for (auto& bitmap : Bitmaps)
            {
                auto d2dBitmap = Make<StubD2DBitmap>();
                d2dBitmap->GetSizeMethod.AllowAnyCall([] { return D2D1_SIZE_F{ 100, 100 }; });
                d2dBitmap->GetPixelSizeMethod.AllowAnyCall([] { return D2D1_SIZE_U{ 100, 100 }; });
                
                bitmap = std::make_pair(d2dBitmap, Make<CanvasBitmap>(device.Get(), d2dBitmap.Get()));
            }
        }

        void AddAndExpect(std::pair<ComPtr<StubD2DBitmap>, ComPtr<CanvasBitmap>> const& bitmap, float id)
        {
            Add(bitmap, id);
            Expect(id);
        }

        void Add(std::pair<ComPtr<StubD2DBitmap>, ComPtr<CanvasBitmap>> const& bitmap, float id)
        {
            ThrowIfFailed(SpriteBatch->DrawAtOffset(bitmap.second.Get(), float2(id)));
        }

        void Expect(float id)
        {
            ExpectedSprites.Expect(id);
        }        

        struct DrawSpriteBatchEntry
        {
            std::pair<ComPtr<StubD2DBitmap>, ComPtr<CanvasBitmap>> Bitmap;
            uint32_t StartIndex;
            uint32_t SpriteCount;
        };
        
        void ExpectBatches(std::initializer_list<DrawSpriteBatchEntry> const& rawExpected)
        {
            std::vector<DrawSpriteBatchEntry> expected(rawExpected);
            size_t i = 0;
            DeviceContext->DrawSpriteBatchMethod.SetExpectedCalls(expected.size(),
                [=] (auto actualSpriteBatch, auto startIndex, auto spriteCount, auto bitmap, auto, auto) mutable
                {
                    Assert::IsTrue(i != expected.size());
                    Assert::IsTrue(IsSameInstance(D2DSpriteBatch.Get(), actualSpriteBatch));
                    Assert::AreEqual(expected[i].StartIndex, startIndex);
                    Assert::AreEqual(expected[i].SpriteCount, spriteCount);
                    Assert::IsTrue(IsSameInstance(expected[i].Bitmap.first.Get(), bitmap));
                    ++i;
                });            
        }

        void Validate()
        {
            ThrowIfFailed(As<IClosable>(SpriteBatch)->Close());        
        }
    };

    TEST_METHOD_EX(CanvasSpriteBatch_WhenNotSorted_ThereIsOneDrawSpriteCallPerBitmap_1)
    {
        MultipleBitmapFixture f;

        f.AddAndExpect(f.Bitmaps[0], 0);
        f.AddAndExpect(f.Bitmaps[1], 1);
        f.AddAndExpect(f.Bitmaps[2], 2);
        f.AddAndExpect(f.Bitmaps[3], 3);

        f.ExpectBatches(
        {
            { f.Bitmaps[0], 0, 1 },
            { f.Bitmaps[1], 1, 1 },
            { f.Bitmaps[2], 2, 1 },
            { f.Bitmaps[3], 3, 1 }
        });

        f.Validate();
    }

    TEST_METHOD_EX(CanvasSpriteBatch_WhenNotSorted_ThereIsOneDrawSpriteCallPerBitmap_2)
    {
        MultipleBitmapFixture f;

        f.AddAndExpect(f.Bitmaps[0], 0);
        f.AddAndExpect(f.Bitmaps[0], 1);
        f.AddAndExpect(f.Bitmaps[0], 2);
        f.AddAndExpect(f.Bitmaps[0], 3);
        f.AddAndExpect(f.Bitmaps[0], 4);
        f.AddAndExpect(f.Bitmaps[0], 5);

        f.AddAndExpect(f.Bitmaps[1], 6);
        f.AddAndExpect(f.Bitmaps[1], 7);
        f.AddAndExpect(f.Bitmaps[1], 8);
        f.AddAndExpect(f.Bitmaps[1], 9);
        f.AddAndExpect(f.Bitmaps[1], 10);

        f.AddAndExpect(f.Bitmaps[2], 11);
        f.AddAndExpect(f.Bitmaps[2], 12);
        f.AddAndExpect(f.Bitmaps[2], 13);
        f.AddAndExpect(f.Bitmaps[2], 14);
        f.AddAndExpect(f.Bitmaps[2], 15);

        f.AddAndExpect(f.Bitmaps[3], 16);
        f.AddAndExpect(f.Bitmaps[3], 17);
        f.AddAndExpect(f.Bitmaps[3], 18);
        f.AddAndExpect(f.Bitmaps[3], 19);
        f.AddAndExpect(f.Bitmaps[3], 20);
        f.AddAndExpect(f.Bitmaps[3], 21);

        f.AddAndExpect(f.Bitmaps[0], 22);
        f.AddAndExpect(f.Bitmaps[0], 23);
        f.AddAndExpect(f.Bitmaps[0], 24);
        f.AddAndExpect(f.Bitmaps[0], 25);
        f.AddAndExpect(f.Bitmaps[0], 26);
        f.AddAndExpect(f.Bitmaps[0], 27);

        f.ExpectBatches(
        {
            { f.Bitmaps[0],  0, 6 },
            { f.Bitmaps[1],  6, 5 },
            { f.Bitmaps[2], 11, 5 },
            { f.Bitmaps[3], 16, 6 },
            { f.Bitmaps[0], 22, 6 },
        });

        f.Validate();
    }

    TEST_METHOD_EX(CanvasSpriteBatch_WhenSorted_ThereIsOneDrawSpriteCallPerBitmap)
    {
        MultipleBitmapFixture f(CanvasSpriteSortMode::Bitmap);

        // Ensure that f.Bitmaps is sorted by the D2D bitmap pointer (this is
        // the default sort order)
        std::sort(f.Bitmaps.begin(), f.Bitmaps.end());

        f.Add(f.Bitmaps[0], 0);
        f.Add(f.Bitmaps[1], 1);
        f.Add(f.Bitmaps[2], 2);
        f.Add(f.Bitmaps[3], 3);
        f.Add(f.Bitmaps[0], 4);
        f.Add(f.Bitmaps[1], 5);
        f.Add(f.Bitmaps[2], 6);
        f.Add(f.Bitmaps[3], 7);
        f.Add(f.Bitmaps[0], 8);
        f.Add(f.Bitmaps[1], 9);

        // We expect these to be sorted by bitmap, but otherwise the order left
        // unchanged
        f.Expect(0); // 0: bitmap 0
        f.Expect(4); // 1: bitmap 0
        f.Expect(8); // 2: bitmap 0
        f.Expect(1); // 3: bitmap 1
        f.Expect(5); // 4: bitmap 1
        f.Expect(9); // 5: bitmap 1        
        f.Expect(2); // 6: bitmap 2
        f.Expect(6); // 7: bitmap 2
        f.Expect(3); // 8: bitmap 3
        f.Expect(7); // 9: bitmap 3

        f.ExpectBatches(
        {
            { f.Bitmaps[0], 0, 3 },
            { f.Bitmaps[1], 3, 3 },
            { f.Bitmaps[2], 6, 2 },
            { f.Bitmaps[3], 8, 2 }
        });

        f.Validate();
    }

    TEST_METHOD_EX(CanvasSpriteBatch_When_AntialiasingIsEnabled_ItMustBeDisabledAroundCallsToDrawSpriteBatch)
    {
        MultipleBitmapFixture f;

        for (int i = 0; i < 100; ++i)
            f.AddAndExpect(f.Bitmaps[i % 4], (float)i);

        f.DeviceContext->GetAntialiasModeMethod.SetExpectedCalls(1,
            [] { return D2D1_ANTIALIAS_MODE_PER_PRIMITIVE; });

        int callCount = 0;
        f.DeviceContext->SetAntialiasModeMethod.SetExpectedCalls(2,
            [callCount] (D2D1_ANTIALIAS_MODE mode) mutable
            {
                if (callCount == 0)
                    Assert::AreEqual(D2D1_ANTIALIAS_MODE_ALIASED, mode);
                else
                    Assert::AreEqual(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, mode);

                ++callCount;
            });

        f.DeviceContext->DrawSpriteBatchMethod.SetExpectedCalls(100);
        
        f.Validate();
    }

    TEST_METHOD_EX(CanvasSpriteBatch_When_AntialiasingIsDisabled_ThenItIsNotModified)
    {
        MultipleBitmapFixture f;

        for (int i = 0; i < 100; ++i)
            f.AddAndExpect(f.Bitmaps[i % 4], (float)i);

        f.DeviceContext->GetAntialiasModeMethod.SetExpectedCalls(1,
            [] { return D2D1_ANTIALIAS_MODE_ALIASED; });

        f.DeviceContext->SetAntialiasModeMethod.SetExpectedCalls(0);

        f.DeviceContext->DrawSpriteBatchMethod.SetExpectedCalls(100);

        f.Validate();
    }
};

#endif

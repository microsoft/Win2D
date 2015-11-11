// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/xaml/CanvasVirtualImageSource.h>

#include "mocks/MockVirtualSurfaceImageSourceFactory.h"
#include "mocks/MockVirtualSurfaceImageSource.h"

static Size anySize{1, 2};
static float anyWidth{1};
static float anyHeight{1};
static float anyDpi{123};
static CanvasAlphaMode anyAlphaMode{CanvasAlphaMode::Premultiplied};
static Color anyColor{1,2,3,4};
static Rect anyUpdateRectangle{1,2,3,4};

TEST_CLASS(CanvasVirtualImageSourceUnitTests)
{
public:
    struct Fixture
    {
        ComPtr<MockVirtualSurfaceImageSourceFactory> VsisFactory;
        std::shared_ptr<MockCanvasImageSourceDrawingSessionFactory> DrawingSessionFactory;
        ComPtr<CanvasVirtualImageSourceFactory> Factory;
        ComPtr<StubCanvasDevice> Device;
        ComPtr<StubResourceCreatorWithDpi> ResourceCreator;
        
        Fixture()
            : VsisFactory(Make<MockVirtualSurfaceImageSourceFactory>())
            , DrawingSessionFactory(std::make_shared<MockCanvasImageSourceDrawingSessionFactory>())
            , Factory(Make<CanvasVirtualImageSourceFactory>(VsisFactory.Get(), DrawingSessionFactory))
            , Device(Make<StubCanvasDevice>())
            , ResourceCreator(Make<StubResourceCreatorWithDpi>(Device.Get()))
        {
        }

        Fixture(Fixture const&) = delete;
        Fixture& operator=(Fixture const&) = delete;

        ComPtr<MockVirtualSurfaceImageSource> ExpectCreateVsis(int expectedPixelWidth = -1, int expectedPixelHeight = -1, bool* expectedIsOpaque = nullptr)
        {
            auto vsis = Make<MockVirtualSurfaceImageSource>();
            vsis->SetDeviceMethod.AllowAnyCall();

            VsisFactory->CreateInstanceWithDimensionsAndOpacityMethod.SetExpectedCalls(1,
                [=] (int32_t actualPixelWidth, int32_t actualPixelHeight, boolean actualIsOpaque, IVirtualSurfaceImageSource** value)
                {
                    if (expectedPixelWidth != -1)
                        Assert::AreEqual(expectedPixelWidth, actualPixelWidth);

                    if (expectedPixelHeight != -1)
                        Assert::AreEqual(expectedPixelHeight, actualPixelHeight);

                    if (expectedIsOpaque)
                        Assert::AreEqual(*expectedIsOpaque, !!actualIsOpaque);
                    
                    return vsis.CopyTo(value);
                });

            return vsis;
        }

        ComPtr<ICanvasVirtualImageSource> CreateCanvasVirtualImageSource(Size size = anySize, float dpi = anyDpi)
        {
            ExpectCreateVsis();

            ComPtr<ICanvasVirtualImageSource> imageSource;
            ThrowIfFailed(Factory->CreateWithWidthAndHeightAndDpi(ResourceCreator.Get(), size.Width, size.Height, dpi, &imageSource));

            return imageSource;
        }
        
        ComPtr<ICanvasVirtualImageSource> CreateCanvasVirtualImageSource(CanvasAlphaMode alphaMode)
        {
            ExpectCreateVsis();

            ComPtr<ICanvasVirtualImageSource> imageSource;
            ThrowIfFailed(Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, alphaMode, &imageSource));

            return imageSource;
        }
    };

    struct SimpleFixture : public Fixture
    {
        ComPtr<MockVirtualSurfaceImageSource> Vsis;
        ComPtr<ICanvasVirtualImageSource> ImageSource;

        SimpleFixture(Size size = anySize, float dpi = anyDpi, CanvasAlphaMode alphaMode = anyAlphaMode)
        {
            Vsis = ExpectCreateVsis();
            ThrowIfFailed(Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(ResourceCreator.Get(), size.Width, size.Height, dpi, alphaMode, &ImageSource));
        }
    };
    
    template<typename FN>
    void TestCreatesAnInstanceWrappingTheCorrectVsis(FN createImageSource)
    {
        Fixture f;

        auto vsis = f.ExpectCreateVsis();
        vsis->SetDeviceMethod.SetExpectedCalls(1,
            [&] (IUnknown* d2dDevice)
            {
                Assert::IsTrue(IsSameInstance(f.Device->GetD2DDevice().Get(), d2dDevice));
                return S_OK;
            });

        ComPtr<ICanvasVirtualImageSource> imageSource;
        createImageSource(&f, &imageSource);
        
        ComPtr<IVirtualSurfaceImageSource> retrievedVsis;
        ThrowIfFailed(imageSource->get_Source(&retrievedVsis));

        Assert::IsTrue(IsSameInstance(vsis.Get(), retrievedVsis.Get()), L"imageSource.Source returns the correct VSIS");
    }
    
    
    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithSize_CreatesAnInstanceWrappingTheCorrectVsis)
    {
        TestCreatesAnInstanceWrappingTheCorrectVsis(
            [=] (Fixture* f, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithSize(f->ResourceCreator.Get(), anySize, imageSource));
            });
    }

    
    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeight_CreatesAnInstanceWrappingTheCorrectVsis)
    {
        TestCreatesAnInstanceWrappingTheCorrectVsis(
            [=] (Fixture* f, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithWidthAndHeight(f->ResourceCreator.Get(), anyWidth, anyHeight, imageSource));
            });
    }


    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeightAndDpi_CreatesAnInstanceWrappingTheCorrectVsis)
    {
        TestCreatesAnInstanceWrappingTheCorrectVsis(
            [=] (Fixture* f, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithWidthAndHeightAndDpi(f->ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, imageSource));
            });
    }


    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeightAndDpiAndAlphaMode_CreatesAnInstanceWrappingTheCorrectVsis)
    {
        TestCreatesAnInstanceWrappingTheCorrectVsis(
            [=] (Fixture* f, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(f->ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, anyAlphaMode, imageSource));
            });
    }


    TEST_METHOD_EX(CanvasVirtualImageSource_CreateMethods_FailWhenPassedNullResourceCreator)
    {
        Fixture f;

        ComPtr<ICanvasVirtualImageSource> imageSource;
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithSize(nullptr, anySize, &imageSource));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithWidthAndHeight(nullptr, anyWidth, anyHeight, &imageSource));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithWidthAndHeightAndDpi(nullptr, anyWidth, anyHeight, anyDpi, &imageSource));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(nullptr, anyWidth, anyHeight, anyDpi, anyAlphaMode, &imageSource));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateMethods_FailWhenPassedNullOutParam)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithSize(f.ResourceCreator.Get(), anySize, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithWidthAndHeight(f.ResourceCreator.Get(), anyWidth, anyHeight, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithWidthAndHeightAndDpi(f.ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(f.ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, anyAlphaMode, nullptr));
    }


    template<typename FN>
    void TestCreatesVsisOfCorrectSize(FN fn)
    {
        struct
        {
            float Dpi;
            Size Size;
        } cases[] =
          {
              { 96.0f, { 1, 2 } },
              { 48.0f, { 1, 2 } },
              { 200.0f, { 1, 2 } },
              { 16.0f, { 1, 1 } },
          };

        for (auto& t : cases)
        {
            Fixture f;

            int expectedPixelWidth = SizeDipsToPixels(t.Size.Width, t.Dpi);
            int expectedPixelHeight = SizeDipsToPixels(t.Size.Height, t.Dpi);

            f.ExpectCreateVsis(expectedPixelWidth, expectedPixelHeight);

            ComPtr<ICanvasVirtualImageSource> imageSource;
            fn(&f, t.Dpi, t.Size, &imageSource);
        }
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithSize_CreatesVsisOfCorrectSize)
    {
        TestCreatesVsisOfCorrectSize(
            [=] (Fixture* f, float dpi, Size dipsSize, ICanvasVirtualImageSource** imageSource)
            {
                f->ResourceCreator->SetDpi(dpi);
                ThrowIfFailed(f->Factory->CreateWithSize(f->ResourceCreator.Get(), dipsSize, imageSource));
            });
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeight_CreatesVsisOfCorrectSize)
    {
        TestCreatesVsisOfCorrectSize(
            [=] (Fixture* f, float dpi, Size dipsSize, ICanvasVirtualImageSource** imageSource)
            {
                f->ResourceCreator->SetDpi(dpi);
                ThrowIfFailed(f->Factory->CreateWithWidthAndHeight(f->ResourceCreator.Get(), dipsSize.Width, dipsSize.Height, imageSource));
            });
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeightAndDpi_CreatesVsisOfCorrectSize)
    {
        TestCreatesVsisOfCorrectSize(
            [=] (Fixture* f, float dpi, Size dipsSize, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithWidthAndHeightAndDpi(f->ResourceCreator.Get(), dipsSize.Width, dipsSize.Height, dpi, imageSource));
            });
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeightAndDpiAndAlphaMode_CreatesVsisOfCorrectSize)
    {
        TestCreatesVsisOfCorrectSize(
            [=] (Fixture* f, float dpi, Size dipsSize, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(f->ResourceCreator.Get(), dipsSize.Width, dipsSize.Height, dpi, anyAlphaMode, imageSource));
            });
    }


    template<typename FN>
    void TestCreatesVsisMarkedAsTransparent(FN fn)
    {
        Fixture f;

        bool expectedIsOpaque = false;
        f.ExpectCreateVsis(-1, -1, &expectedIsOpaque);

        ComPtr<ICanvasVirtualImageSource> imageSource;
        fn(&f, &imageSource);
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithSize_CreatesVsisMarkedAsTransparent)
    {
        TestCreatesVsisMarkedAsTransparent(
            [=] (Fixture* f, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithSize(f->ResourceCreator.Get(), anySize, imageSource));
            });
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeight_CreatesVsisMarkedAsTransparent)
    {
        TestCreatesVsisMarkedAsTransparent(
            [=] (Fixture* f, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithWidthAndHeight(f->ResourceCreator.Get(), anyWidth, anyHeight, imageSource));
            });
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeightAndDpi_CreatesVsisMarkedAsTransparent)
    {
        TestCreatesVsisMarkedAsTransparent(
            [=] (Fixture* f, ICanvasVirtualImageSource** imageSource)
            {
                ThrowIfFailed(f->Factory->CreateWithWidthAndHeightAndDpi(f->ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, imageSource));
            });
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeightAndDpiAndAlphaMode_Premultiplied_CreatesVsisMarkedAsTransparent)
    {
        Fixture f;

        bool expectedIsOpaque = false;
        f.ExpectCreateVsis(-1, -1, &expectedIsOpaque);

        ComPtr<ICanvasVirtualImageSource> imageSource;
        ThrowIfFailed(f.Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(f.ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, CanvasAlphaMode::Premultiplied, &imageSource));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeightAndDpiAndAlphaMode_Ignore_CreatesVsisMarkedAsOpaque)
    {
        Fixture f;

        bool expectedIsOpaque = true;
        f.ExpectCreateVsis(-1, -1, &expectedIsOpaque);

        ComPtr<ICanvasVirtualImageSource> imageSource;
        ThrowIfFailed(f.Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(f.ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, CanvasAlphaMode::Ignore, &imageSource));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateWithWidthAndHeightAndDpiAndAlphaMode_Straight_Fails)
    {
        Fixture f;

        ComPtr<ICanvasVirtualImageSource> imageSource;
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithWidthAndHeightAndDpiAndAlphaMode(f.ResourceCreator.Get(), anyWidth, anyHeight, anyDpi, CanvasAlphaMode::Straight, &imageSource));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_Source_FailsWhenPassedNullOutParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->get_Source(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_Device_ReturnsTheCorrectDevice)
    {
        SimpleFixture f;

        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(f.ImageSource->get_Device(&device));

        Assert::IsTrue(IsSameInstance(f.Device.Get(), device.Get()), L"imageSource.Device returns the device that was passed in to the constructor.");
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_Device_FailsWhenPassedNullOutParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->get_Device(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_Dpi_ReturnsTheCorrectDpi)
    {
        float dpis[] { 1.0f, 96.0f, 120.0f };
        for (float expectedDpi : dpis)
        {
            SimpleFixture f(anySize, expectedDpi);

            float actualDpi;
            ThrowIfFailed(f.ImageSource->get_Dpi(&actualDpi));

            Assert::AreEqual(expectedDpi, actualDpi);
        }
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_Dpi_FailsWhenPassedNullOutParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->get_Dpi(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_Size_ReturnsTheCorrectSize)
    {
        Size expectedSize{ 12, 34 };

        SimpleFixture f(expectedSize);

        Size actualSize;
        ThrowIfFailed(f.ImageSource->get_Size(&actualSize));

        Assert::AreEqual(expectedSize, actualSize);
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_Size_FailsWhenPassedNullOutParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->get_Size(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_SizeInPixels_ReturnsTheCorrectSize)
    {
        Size expectedSizeInDips{ 12, 34 };

        SimpleFixture f(expectedSizeInDips);

        BitmapSize actualSize;
        ThrowIfFailed(f.ImageSource->get_SizeInPixels(&actualSize));

        Assert::AreEqual<int>(SizeDipsToPixels(expectedSizeInDips.Width, anyDpi), actualSize.Width);
        Assert::AreEqual<int>(SizeDipsToPixels(expectedSizeInDips.Height, anyDpi), actualSize.Height);
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_SizeInPixels_FailsWhenPassedNullOutParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->get_SizeInPixels(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_AlphaMode_ReturnsTheCorrectAlphaMode)
    {
        auto expectedAlphaMode = CanvasAlphaMode::Ignore;
        SimpleFixture f(anySize, anyDpi, expectedAlphaMode);

        CanvasAlphaMode actualAlphaMode;
        ThrowIfFailed(f.ImageSource->get_AlphaMode(&actualAlphaMode));

        Assert::AreEqual(expectedAlphaMode, actualAlphaMode);
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_get_AlphaMode_FailsWhenPassedNullOutParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->get_AlphaMode(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_ConvertDipsToPixels)
    {
        SimpleFixture f;

        VerifyConvertDipsToPixels(anyDpi, f.ImageSource);

        const float testValue = 100;
        float dips = 0;
        ThrowIfFailed(f.ImageSource->ConvertPixelsToDips((int)testValue, &dips));
        Assert::AreEqual(testValue * DEFAULT_DPI / anyDpi, dips);
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateDrawingSession_FailsIfPassedNullOutParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->CreateDrawingSession(anyColor, anyUpdateRectangle, nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateDrawingSession_PassesParametersThroughToFactory)
    {
        SimpleFixture f(anySize, anyDpi);

        f.DrawingSessionFactory->CreateMethod.SetExpectedCalls(1,
            [&] (ICanvasDevice* owner, ISurfaceImageSourceNativeWithD2D* sisNative, Color const& clearColor, Rect const& updateRectangleInDips, float dpi)
            {
                Assert::IsTrue(IsSameInstance(f.Device.Get(), owner));
                Assert::IsTrue(IsSameInstance(f.Vsis.Get(), sisNative));
                Assert::AreEqual(anyColor, clearColor);
                Assert::AreEqual(anyUpdateRectangle, updateRectangleInDips);               
                Assert::AreEqual(anyDpi, dpi);
                
                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.ImageSource->CreateDrawingSession(anyColor, anyUpdateRectangle, &drawingSession));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CreateDrawingSession_HandlesSurfaceContentsLost)
    {
        SimpleFixture f(anySize, anyDpi);

        bool isFirstCreateDrawingSession = true;
        bool isFirstSetDevice = true;

        f.DrawingSessionFactory->CreateMethod.SetExpectedCalls(2,
            [&] (ICanvasDevice* owner, ISurfaceImageSourceNativeWithD2D* sisNative, Color const& clearColor, Rect const& updateRectangleInDips, float dpi)
            {
                Assert::IsTrue(IsSameInstance(f.Device.Get(), owner));
                Assert::IsTrue(IsSameInstance(f.Vsis.Get(), sisNative));
                Assert::AreEqual(anyColor, clearColor);
                Assert::AreEqual(anyUpdateRectangle, updateRectangleInDips);               
                Assert::AreEqual(anyDpi, dpi);
                
                // First create call fails with surface contents lost - this should trigger a recreate.
                if (isFirstCreateDrawingSession)
                {
                    isFirstCreateDrawingSession = false;
                    ThrowHR(E_SURFACE_CONTENTS_LOST);
                }

                // Allow the second call to succeed.
                return Make<MockCanvasDrawingSession>();
            });

        // The surface contents lost error should cause the device to be set to
        // null and then back again, which triggers XAML to reallocate its surfaces.
        f.Vsis->SetDeviceMethod.SetExpectedCalls(2,
            [&](IUnknown* device)
            {
                if (isFirstSetDevice)
                {
                    isFirstSetDevice = false;
                    Assert::IsNull(device);
                }
                else
                {
                    Assert::IsTrue(IsSameInstance(f.Device->GetD2DDevice().Get(), device));
                }

                return S_OK;
            });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.ImageSource->CreateDrawingSession(anyColor, anyUpdateRectangle, &drawingSession));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_SuspendDrawingSession_CallsSuspendDrawOnTheVsis)
    {
        SimpleFixture f;

        f.DrawingSessionFactory->CreateMethod.SetExpectedCalls(1,
            [&] (ICanvasDevice* owner, ISurfaceImageSourceNativeWithD2D* sisNative, Color const& clearColor, Rect const& updateRectangleInDips, float dpi)
            {
                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.ImageSource->CreateDrawingSession(anyColor, anyUpdateRectangle, &drawingSession));

        f.Vsis->SuspendDrawMethod.SetExpectedCalls(1);
        ThrowIfFailed(f.ImageSource->SuspendDrawingSession(drawingSession.Get()));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_SuspendDrawingSession_FailsIfPassedNullParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->SuspendDrawingSession(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_ResumeDrawingSession_CallsResumeDrawOnTheVsis)
    {
        SimpleFixture f;

        f.DrawingSessionFactory->CreateMethod.SetExpectedCalls(1,
            [&] (ICanvasDevice* owner, ISurfaceImageSourceNativeWithD2D* sisNative, Color const& clearColor, Rect const& updateRectangleInDips, float dpi)
            {
                return Make<MockCanvasDrawingSession>();
            });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.ImageSource->CreateDrawingSession(anyColor, anyUpdateRectangle, &drawingSession));

        f.Vsis->ResumeDrawMethod.SetExpectedCalls(1);
        ThrowIfFailed(f.ImageSource->ResumeDrawingSession(drawingSession.Get()));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_ResumeDrawingSession_FailsIfPassedNullParam)
    {
        SimpleFixture f;
        Assert::AreEqual(E_INVALIDARG, f.ImageSource->ResumeDrawingSession(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_Invalidate_CallsInvalidateForEntireImage)
    {
        SimpleFixture f;

        BitmapSize sizeInPixels;
        ThrowIfFailed(f.ImageSource->get_SizeInPixels(&sizeInPixels));

        RECT expectedRect{ 0, 0, static_cast<int>(sizeInPixels.Width), static_cast<int>(sizeInPixels.Height) };

        f.Vsis->InvalidateMethod.SetExpectedCalls(1,
            [&] (RECT updateRect)
            {
                Assert::AreEqual(expectedRect, updateRect);
                return S_OK;
            });

        ThrowIfFailed(f.ImageSource->Invalidate());
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_InvalidateRegion_CallsInvalidateWithTheCorrectRegion)
    {
        SimpleFixture f;

        RECT expectedRect = ToRECT(anyUpdateRectangle, anyDpi);

        f.Vsis->InvalidateMethod.SetExpectedCalls(1,
            [&] (RECT updateRect)
            {
                Assert::AreEqual(expectedRect, updateRect);
                return S_OK;
            });

        ThrowIfFailed(f.ImageSource->InvalidateRegion(anyUpdateRectangle));
    }

    class CallbackFixture : public SimpleFixture
    {
        ComPtr<IVirtualSurfaceUpdatesCallbackNative> m_callback;

    public:
        void ExpectRegisterForUpdatesNeeded()
        {
            Vsis->RegisterForUpdatesNeededMethod.SetExpectedCalls(1,
                [&] (IVirtualSurfaceUpdatesCallbackNative* callback)
                {
                    m_callback = callback;
                    return S_OK;
                });
        }

        void RaiseUpdatesNeeded()
        {
            ThrowIfFailed(m_callback->UpdatesNeeded());
        }
    };

    TEST_METHOD_EX(CanvasVirtualImageSource_RegionsInvalidatedCallbackIsCalled)
    {
        CallbackFixture f;

        f.ExpectRegisterForUpdatesNeeded();

        auto onRegionsInvalidated = MockEventHandler<ImageSourceRegionsInvalidatedHandler>(L"onRegionsInvalidated");
        EventRegistrationToken token;
        ThrowIfFailed(f.ImageSource->add_RegionsInvalidated(onRegionsInvalidated.Get(), &token));

        std::vector<RECT> regions;
        regions.push_back(RECT{1,2,3,4});
        regions.push_back(RECT{5,6,7,8});

        RECT expectedVisibleBounds{9, 10, 11, 12};

        f.Vsis->GetUpdateRectCountMethod.SetExpectedCalls(2,
            [&] (DWORD* count)
            {
                *count = static_cast<DWORD>(regions.size());
                return S_OK;
            });

        f.Vsis->GetUpdateRectsMethod.SetExpectedCalls(2,

            [&] (RECT* updates, DWORD count)
            {
                for (auto i = 0U; i < count; ++i)
                {
                    updates[i] = regions[i];
                }

                return S_OK;
            });

        f.Vsis->GetVisibleBoundsMethod.SetExpectedCalls(2,
            [&] (RECT* bounds)
            {
                *bounds = expectedVisibleBounds;
                return S_OK;
            });

        onRegionsInvalidated.SetExpectedCalls(1,
            [&] (ICanvasVirtualImageSource* sender, ICanvasRegionsInvalidatedEventArgs* args)
            {
                Assert::IsTrue(IsSameInstance(f.ImageSource.Get(), sender));

                ComArray<Rect> invalidatedRegions;
                ThrowIfFailed(args->get_InvalidatedRegions(invalidatedRegions.GetAddressOfSize(), invalidatedRegions.GetAddressOfData()));

                Assert::AreEqual<size_t>(regions.size(), invalidatedRegions.GetSize());

                for (auto i = 0U; i < regions.size(); ++i)
                {
                    auto const& expectedRegionInPixels = regions[i];
                    auto const& actualRegionInDips = invalidatedRegions[i];

                    Assert::AreEqual(expectedRegionInPixels, ToRECT(actualRegionInDips, anyDpi));                    
                }

                Rect visibleRegionInDips;
                ThrowIfFailed(args->get_VisibleRegion(&visibleRegionInDips));
                Assert::AreEqual(expectedVisibleBounds, ToRECT(visibleRegionInDips, anyDpi));
                
                return S_OK;
            });

        f.RaiseUpdatesNeeded();

        ThrowIfFailed(f.ImageSource->remove_RegionsInvalidated(token));
        onRegionsInvalidated.SetExpectedCalls(0);
        f.RaiseUpdatesNeeded();
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_WhenNoRegionsAreInvalid_RegionsInvalidatedCallback_IsNotCalled)
    {
        CallbackFixture f;

        f.ExpectRegisterForUpdatesNeeded();

        auto onRegionsInvalidated = MockEventHandler<ImageSourceRegionsInvalidatedHandler>(L"onRegionsInvalidated");
        EventRegistrationToken token;
        ThrowIfFailed(f.ImageSource->add_RegionsInvalidated(onRegionsInvalidated.Get(), &token));

        f.Vsis->GetUpdateRectCountMethod.SetExpectedCalls(1,
            [&] (DWORD* count)
            {
                *count = 0;
                return S_OK;
            });

        onRegionsInvalidated.SetExpectedCalls(0);

        f.RaiseUpdatesNeeded();
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_RaiseRegionsInvalidatedIfAny_RaisesRegionsInvalidated)
    {
        CallbackFixture f;

        f.ExpectRegisterForUpdatesNeeded();

        auto onRegionsInvalidated = MockEventHandler<ImageSourceRegionsInvalidatedHandler>(L"onRegionsInvalidated");
        EventRegistrationToken token;
        ThrowIfFailed(f.ImageSource->add_RegionsInvalidated(onRegionsInvalidated.Get(), &token));

        f.Vsis->GetUpdateRectCountMethod.SetExpectedCalls(1,
            [&] (DWORD* count)
            {
                *count = 1;
                return S_OK;
            });

        f.Vsis->GetUpdateRectsMethod.SetExpectedCalls(1,
            [&] (RECT* updates, DWORD)
            {
                updates[0] = RECT{ 1, 2, 3, 4 };
                return S_OK;
            });

        f.Vsis->GetVisibleBoundsMethod.SetExpectedCalls(1,
            [&] (RECT* bounds)
            {
                *bounds = RECT{ 1, 2, 3, 4 };
                return S_OK;
            });

        onRegionsInvalidated.SetExpectedCalls(1);

        auto mockDispatcher = Make<MockDispatcher>();
        mockDispatcher->get_HasThreadAccessMethod.AllowAnyCall([] (boolean* value) { *value = TRUE; return S_OK; });
        
        f.Vsis->get_DispatcherMethod.SetExpectedCalls(1,
            [=] (ICoreDispatcher** value)
            {
                return mockDispatcher.CopyTo(value);
            });

        ThrowIfFailed(f.ImageSource->RaiseRegionsInvalidatedIfAny());
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_RaiseRegionsInvalidatedIfAny_FailsIfNotCalledFromUiThread)
    {
        CallbackFixture f;
        
        auto mockDispatcher = Make<MockDispatcher>();
        mockDispatcher->get_HasThreadAccessMethod.AllowAnyCall([] (boolean* value) { *value = FALSE; return S_OK; });
        
        f.Vsis->get_DispatcherMethod.SetExpectedCalls(1,
            [=] (ICoreDispatcher** value)
            {
                return mockDispatcher.CopyTo(value);
            });

        Assert::AreEqual(RPC_E_WRONG_THREAD, f.ImageSource->RaiseRegionsInvalidatedIfAny());
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_CanvasRegionsInvalidatedEventArgs_AccessorsFailWhenPassedNull)
    {
        auto args = Make<CanvasRegionsInvalidatedEventArgs>(std::vector<Rect>(), Rect{});

        Rect* rects;
        uint32_t count; 
       
        Assert::AreEqual(E_INVALIDARG, args->get_InvalidatedRegions(nullptr, &rects));
        Assert::AreEqual(E_INVALIDARG, args->get_InvalidatedRegions(&count, nullptr));
        Assert::AreEqual(E_INVALIDARG, args->get_VisibleRegion(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_ResizeWithSize_CallsResizeWithCorrectSize)
    {
        SimpleFixture f;

        Size anyOtherSize{50, 60};

        f.Vsis->ResizeMethod.SetExpectedCalls(1,
            [&] (int newWidth, int newHeight)
            {
                Assert::AreEqual(SizeDipsToPixels(anyOtherSize.Width, anyDpi), newWidth);
                Assert::AreEqual(SizeDipsToPixels(anyOtherSize.Height, anyDpi), newHeight);
                return S_OK;
            });

        ThrowIfFailed(f.ImageSource->ResizeWithSize(anyOtherSize));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_ResizeWithWidthAndHeight_CallsResizeWithCorrectSize)
    {
        SimpleFixture f;

        Size anyOtherSize{50, 60};

        f.Vsis->ResizeMethod.SetExpectedCalls(1,
            [&] (int newWidth, int newHeight)
            {
                Assert::AreEqual(SizeDipsToPixels(anyOtherSize.Width, anyDpi), newWidth);
                Assert::AreEqual(SizeDipsToPixels(anyOtherSize.Height, anyDpi), newHeight);
                return S_OK;
            });

        ThrowIfFailed(f.ImageSource->ResizeWithWidthAndHeight(anyOtherSize.Width, anyOtherSize.Height));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_ResizeWithWidthAndHeightAndDpi_CallsResizeWithCorrectSize)
    {
        SimpleFixture f;

        Size anyOtherSize{50, 60};
        float anyOtherDpi = anyDpi * 3;

        f.Vsis->ResizeMethod.SetExpectedCalls(1,
            [&] (int newWidth, int newHeight)
            {
                Assert::AreEqual(SizeDipsToPixels(anyOtherSize.Width, anyOtherDpi), newWidth);
                Assert::AreEqual(SizeDipsToPixels(anyOtherSize.Height, anyOtherDpi), newHeight);
                return S_OK;
            });

        ThrowIfFailed(f.ImageSource->ResizeWithWidthAndHeightAndDpi(anyOtherSize.Width, anyOtherSize.Height, anyOtherDpi));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_ResizeWithWidthAndHeightAndDpi_CausesThePropertyAccessorsToReturnUpdatedValues)
    {
        SimpleFixture f;

        Size anyOtherSize{50, 60};
        float anyOtherDpi = anyDpi * 3;

        f.Vsis->ResizeMethod.AllowAnyCall();

        ThrowIfFailed(f.ImageSource->ResizeWithWidthAndHeightAndDpi(anyOtherSize.Width, anyOtherSize.Height, anyOtherDpi));

        float actualDpi;
        ThrowIfFailed(f.ImageSource->get_Dpi(&actualDpi));
        
        Size actualSize;
        ThrowIfFailed(f.ImageSource->get_Size(&actualSize));
        
        BitmapSize actualSizeInPixels;
        ThrowIfFailed(f.ImageSource->get_SizeInPixels(&actualSizeInPixels));

        Assert::AreEqual(anyOtherDpi, actualDpi);
        Assert::AreEqual(anyOtherSize, actualSize);

        Assert::AreEqual((uint32_t)SizeDipsToPixels(anyOtherSize.Width, anyOtherDpi), actualSizeInPixels.Width);
        Assert::AreEqual((uint32_t)SizeDipsToPixels(anyOtherSize.Height, anyOtherDpi), actualSizeInPixels.Height);
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_Recreate_FailsWhenPassedNull)
    {
        SimpleFixture f;

        Assert::AreEqual(E_INVALIDARG, f.ImageSource->Recreate(nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualImageSource_Recreate_SetsDeviceToNullAndThenToTheNewDevice)
    {
        SimpleFixture f;

        auto newDevice = Make<StubCanvasDevice>();

        f.Vsis->SetDeviceMethod.SetExpectedCalls(2,
            [&] (IUnknown* device)
            {
                if (f.Vsis->SetDeviceMethod.GetCurrentCallCount() == 1)
                {
                    Assert::IsNull(device, L"expected SetDevice(nullptr)");
                }
                else
                {
                    Assert::IsTrue(IsSameInstance(newDevice->GetD2DDevice().Get(), device), L"expected SetDevice(newDevice's D2D device)");
                }
                return S_OK;
            });

        ThrowIfFailed(f.ImageSource->Recreate(newDevice.Get()));
    }
};

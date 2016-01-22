// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/xaml/CanvasVirtualControl.h>
#include <lib/xaml/CanvasVirtualImageSource.h>

#include "../stubs/StubCanvasVirtualImageSource.h"


class MockImageControl : public StubImageControl
{
public:
    CALL_COUNTER_WITH_MOCK(put_SourceMethod, HRESULT(IImageSource*));

    IFACEMETHODIMP put_Source(IImageSource* value) override
    {
        return put_SourceMethod.WasCalled(value);
    }
};


class TestVirtualControlAdapter
    : public BaseControlTestAdapter<CanvasVirtualControlTraits>
{
public:
    ComPtr<MockEventSourceUntyped> SurfaceContentsLostEventSource;
    CALL_COUNTER_WITH_MOCK(CreateCanvasVirtualImageSourceMethod, ComPtr<ICanvasVirtualImageSource>(ICanvasDevice*, float, float, float, CanvasAlphaMode));

    ComPtr<MockImageControl> Image;
    ComPtr<StubCanvasDevice> Device;

    TestVirtualControlAdapter()
        : SurfaceContentsLostEventSource(Make<MockEventSourceUntyped>(L"SurfaceContentsLost"))
        , Image(Make<MockImageControl>())
    {
        DeviceFactory->GetSharedDeviceWithForceSoftwareRendererMethod.AllowAnyCall(
            [&] (boolean, ICanvasDevice** device)
            {
                Device = Make<StubCanvasDevice>();
                return Device.CopyTo(device);
            });
    }

    virtual ComPtr<ICanvasVirtualImageSource> CreateCanvasVirtualImageSource(ICanvasDevice* device, float width, float height, float dpi, CanvasAlphaMode alphaMode) override
    {
        return CreateCanvasVirtualImageSourceMethod.WasCalled(device, width, height, dpi, alphaMode);
    }

    virtual RegisteredEvent AddSurfaceContentsLostCallback(IEventHandler<IInspectable*>* value) override
    {
        return SurfaceContentsLostEventSource->Add(value);
    }

    void RaiseSurfaceContentsLostEvent()
    {
        IInspectable* sender = nullptr;
        IInspectable* arg = nullptr;
        ThrowIfFailed(SurfaceContentsLostEventSource->InvokeAll(sender, arg));
    }

    virtual ComPtr<IImage> CreateImageControl() override
    {
        return Image;
    }
};


template<>
struct TestAdapter<CanvasVirtualControlTraits>
{
    typedef TestVirtualControlAdapter type;
    typedef TestVirtualControlAdapter injectDeviceContextType;

    static std::shared_ptr<TestVirtualControlAdapter> Create()
    {
        return std::make_shared<TestVirtualControlAdapter>();
    }

    static std::shared_ptr<TestVirtualControlAdapter> Create(MockD2DDeviceContext* deviceContext)
    {
        ThrowHR(E_NOTIMPL);
    }
};

static Rect anyRegion { 1, 2, 3, 4 };
static Color anyColor { 5, 6, 7, 8 };


TEST_CLASS(CanvasVirtualControlTests)
{
    struct Fixture : public BasicControlFixture<CanvasVirtualControlTraits>
    {
        Fixture()
        {
            CreateAdapter();
            Adapter->Image->put_SourceMethod.AllowAnyCall();

            CreateControl();
        }

        ComPtr<StubCanvasVirtualImageSource> ExpectCreateImageSource()
        {
            auto imageSource = Make<StubCanvasVirtualImageSource>();

            Adapter->CreateCanvasVirtualImageSourceMethod.SetExpectedCalls(1,
                [=] (ICanvasDevice*, float, float, float, CanvasAlphaMode)
                {
                    return imageSource;
                });

            return imageSource;
        }

        ComPtr<MockAsyncAction> AddAsyncCreateResources()
        {
            auto action = Make<MockAsyncAction>();

            auto onCreateResources = Callback<CanvasVirtualControl::createResourcesEventHandler_t>(
                [=] (IInspectable*, ICanvasCreateResourcesEventArgs* args)
                {
                    return args->TrackAsyncAction(action.Get());
                });

            AddCreateResourcesHandler(onCreateResources.Get());

            return action;
        }
    };

    TEST_METHOD_EX(CanvasVirtualControl_WhenLoadedAtZeroSize_ImageControlIsCreatedAndSourceSetToNull)
    {
        Fixture f;

        f.Adapter->Image->put_SourceMethod.SetExpectedCalls(1,
            [] (IImageSource* value)
            {
                Assert::IsNull(value);
                return S_OK;
            });

        f.Load(0,0);
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenLoaded_ImageSourceIsSetOnImageControl)
    {
        Fixture f;

        auto imageSource = f.ExpectCreateImageSource();
        ComPtr<IVirtualSurfaceImageSource> vsis;
        ThrowIfFailed(imageSource->get_Source(&vsis));

        f.Adapter->Image->put_SourceMethod.SetExpectedCalls(1,
            [=] (IImageSource* value)
            {
                Assert::IsTrue(IsSameInstance(vsis.Get(), value));
                return S_OK;
            });

        f.Load();
    }

    TEST_METHOD_EX(CanvasVirtualControl_BeforeCreateResourcesCompletes_ControlShowsTheClearColor)
    {
        Fixture f;

        f.AddAsyncCreateResources();

        ThrowIfFailed(f.Control->put_ClearColor(anyColor));

        std::vector<Rect> expectedRegions =
        {
            Rect { 1, 2, 3, 4 },
            Rect { 5, 6, 7, 8 }
        };

        auto imageSource = f.ExpectCreateImageSource();
        imageSource->CreateDrawingSessionMethod.SetExpectedCalls(static_cast<int>(expectedRegions.size()),
            [&] (Color clearColor, Rect rect, ICanvasDrawingSession** ds)
            {
                Assert::AreEqual(anyColor, clearColor);

                auto n = imageSource->CreateDrawingSessionMethod.GetCurrentCallCount() -1;

                Assert::AreEqual(expectedRegions[n], rect);

                auto newDs = Make<MockCanvasDrawingSession>();
                return newDs.CopyTo(ds);
            });

        f.Load();

        imageSource->RaiseRegionsInvalidated(expectedRegions, anyRegion);
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenCreateResourcesCompletes_ImageSourceIsNotExplicitlyInvalidatedIfItHasntBeenCleared)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();
        auto action = f.AddAsyncCreateResources();
        f.Load();

        //
        // Since imageSource hasn't raised its RegionsInvalidated yet, we don't
        // need to explicitly invalidate the image source.
        //
        imageSource->InvalidateMethod.SetExpectedCalls(0);

        action->SetResult(S_OK);
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenCreateResourcesCompletesAfterClearingToClearColor_ImageSourceIsExplicitlyInvalidated)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();
        auto action = f.AddAsyncCreateResources();
        f.Load();

        imageSource->CreateDrawingSessionMethod.AllowAnyCall(
            [] (Color, Rect, ICanvasDrawingSession** ds) { return Make<MockCanvasDrawingSession>().CopyTo(ds); });

        imageSource->RaiseRegionsInvalidated(std::vector<Rect> { anyRegion }, anyRegion);

        //
        // The image source has raised its RegionInvalidated; since resources
        // weren't completed the control responded by clearing it.  As far as
        // the image source is concerned it is now valid.  But, since resources
        // have been loaded, we need to invalidate the entire image to the
        // control a change to raise RegionsInvalidated.
        //
        imageSource->InvalidateMethod.SetExpectedCalls(1);

        action->SetResult(S_OK);
    }

    TEST_METHOD_EX(CanvasVirtualControl_RegionsInvalidatedHandler_FailsWhenPassedBadParameters)
    {
        Fixture f;

        auto onRegionsInvalidated = MockEventHandler<ControlRegionsInvalidatedHandler>(L"onRegionsInvalidated");
        EventRegistrationToken token;

        Assert::AreEqual(E_INVALIDARG, f.Control->add_RegionsInvalidated(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, f.Control->add_RegionsInvalidated(onRegionsInvalidated.Get(), nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenRegionsInvalidatedHandlerAdded_VirtualImageSourceIsInvalidated)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();

        f.Load();

        imageSource->InvalidateMethod.SetExpectedCalls(1);

        auto onRegionsInvalidated = MockEventHandler<ControlRegionsInvalidatedHandler>(L"onRegionsInvalidated");
        EventRegistrationToken token;
        ThrowIfFailed(f.Control->add_RegionsInvalidated(onRegionsInvalidated.Get(), &token));
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenCreateResourcesCompleted_RegionsInvalidatedIsCalled)
    {
        Fixture f;

        std::vector<Rect> expectedRegions =
        {
            Rect { 1, 2, 3, 4 },
            Rect { 5, 6, 7, 8 }
        };

        Rect expectedVisibleRegion { 9, 10, 11, 12 };

        auto imageSource = f.ExpectCreateImageSource();
        imageSource->CreateDrawingSessionMethod.SetExpectedCalls(0);

        auto onRegionsInvalidated = MockEventHandler<ControlRegionsInvalidatedHandler>(L"onRegionsInvalidated");
        EventRegistrationToken token;
        ThrowIfFailed(f.Control->add_RegionsInvalidated(onRegionsInvalidated.Get(), &token));

        f.Load();

        onRegionsInvalidated.SetExpectedCalls(1,
            [&] (ICanvasVirtualControl* sender, ICanvasRegionsInvalidatedEventArgs* args)
            {
                ComArray<Rect> regions;
                ThrowIfFailed(args->get_InvalidatedRegions(regions.GetAddressOfSize(), regions.GetAddressOfData()));

                Assert::AreEqual<size_t>(regions.GetSize(), expectedRegions.size());
                for (auto i = 0U; i < regions.GetSize(); ++i)
                {
                    Assert::AreEqual(expectedRegions[i], regions[i]);
                }

                Rect visibleRegion;
                ThrowIfFailed(args->get_VisibleRegion(&visibleRegion));

                Assert::AreEqual(expectedVisibleRegion, visibleRegion);

                return S_OK;
            });

        imageSource->RaiseRegionsInvalidated(expectedRegions, expectedVisibleRegion);

        // Unregistering the event should stop it getting raised
        ThrowIfFailed(f.Control->remove_RegionsInvalidated(token));
        onRegionsInvalidated.SetExpectedCalls(0);

        // When there are no RegionsInvalidated handlers the control will clear
        // itself by creating drawing sessions.
        imageSource->CreateDrawingSessionMethod.SetExpectedCalls(2,
            [&] (Color, Rect, ICanvasDrawingSession** ds)
            {
                auto newDs = Make<MockCanvasDrawingSession>();
                return newDs.CopyTo(ds);
            });


        imageSource->RaiseRegionsInvalidated(expectedRegions, expectedVisibleRegion);
    }

    TEST_METHOD_EX(CanvasVirtualControl_CreateDrawingSession_FailsWhenPassedBadParameters)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.Control->CreateDrawingSession(anyRegion, nullptr));
    }

    TEST_METHOD_EX(CanvasVirtualControl_CreateDrawingSession_FailsWhenCreateResourcesNotComplete)
    {
        Fixture f;
        f.ExpectCreateImageSource();
        f.AddAsyncCreateResources();
        f.Load();

        ComPtr<ICanvasDrawingSession> ds;

        Assert::AreEqual(E_FAIL, f.Control->CreateDrawingSession(anyRegion, &ds));
        ValidateStoredErrorState(E_FAIL, Strings::CreateDrawingSessionCalledBeforeRegionsInvalidated);
    }


    TEST_METHOD_EX(CanvasVirtualControl_CreateDrawingSessionSucceedsWhenCreateResourcesComplete)
    {
        Fixture f;

        ThrowIfFailed(f.Control->put_ClearColor(anyColor));
        auto imageSource = f.ExpectCreateImageSource();

        imageSource->CreateDrawingSessionMethod.SetExpectedCalls(1,
            [&] (Color clearColor, Rect rect, ICanvasDrawingSession** ds)
            {
                Assert::AreEqual(anyColor, clearColor);
                Assert::AreEqual(anyRegion, rect);

                auto newDs = Make<MockCanvasDrawingSession>();
                return newDs.CopyTo(ds);
            });

        f.Load();

        ComPtr<ICanvasDrawingSession> ds;
        Assert::AreEqual(S_OK, f.Control->CreateDrawingSession(anyRegion, &ds));
    }

    TEST_METHOD_EX(CanvasVirtualControl_Invalidate_IsNoOpIfSurfaceHasntBeenCreated)
    {
        Fixture f;
        ThrowIfFailed(f.Control->Invalidate());
        ThrowIfFailed(f.Control->InvalidateRegion(anyRegion));
    }

    TEST_METHOD_EX(CanvasVirtualControl_Invalidate_CallsThroughToImageSource)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();
        f.Load();

        imageSource->InvalidateMethod.SetExpectedCalls(1);
        ThrowIfFailed(f.Control->Invalidate());
    }

    TEST_METHOD_EX(CanvasVirtualControl_InvalidateRegion_CallsThroughToImageSource)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();
        f.Load();

        imageSource->InvalidateRegionMethod.SetExpectedCalls(1,
            [] (Rect region)
            {
                Assert::AreEqual(anyRegion, region);
                return S_OK;
            });

        ThrowIfFailed(f.Control->InvalidateRegion(anyRegion));
    }

    TEST_METHOD_EX(CanvasVirtualControl_SuspendDrawingSession_CallsThrough)
    {
        auto anyDrawingSession = reinterpret_cast<ICanvasDrawingSession*>(0x1234);

        Fixture f;

        // no-op if image source hasn't been created yet
        ThrowIfFailed(f.Control->SuspendDrawingSession(anyDrawingSession));

        auto imageSource = f.ExpectCreateImageSource();
        f.Load();

        imageSource->SuspendDrawingSessionMethod.SetExpectedCalls(1,
            [=] (ICanvasDrawingSession* ds)
            {
                Assert::AreEqual(anyDrawingSession, ds);
                return S_OK;
            });

        ThrowIfFailed(f.Control->SuspendDrawingSession(anyDrawingSession));
    }

    TEST_METHOD_EX(CanvasVirtualControl_ResumeDrawingSession_CallsThrough)
    {
        auto anyDrawingSession = reinterpret_cast<ICanvasDrawingSession*>(0x1234);

        Fixture f;

        // no-op if image source hasn't been created yet
        ThrowIfFailed(f.Control->ResumeDrawingSession(anyDrawingSession));

        auto imageSource = f.ExpectCreateImageSource();
        f.Load();

        imageSource->ResumeDrawingSessionMethod.SetExpectedCalls(1,
            [=] (ICanvasDrawingSession* ds)
            {
                Assert::AreEqual(anyDrawingSession, ds);
                return S_OK;
            });

        ThrowIfFailed(f.Control->ResumeDrawingSession(anyDrawingSession));
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenClearColorChanges_ImageSourceIsInvalidated)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();
        f.Load();

        imageSource->InvalidateMethod.SetExpectedCalls(1);
        ThrowIfFailed(f.Control->put_ClearColor(anyColor));
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenClearColorChangesWhileWaitingForCreateResource_ImageSourceIsInvalidated)
    {
        Fixture f;
        f.AddAsyncCreateResources();
        auto imageSource = f.ExpectCreateImageSource();
        f.Load();

        imageSource->InvalidateMethod.SetExpectedCalls(1);
        ThrowIfFailed(f.Control->put_ClearColor(anyColor));
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenDpiChanges_ImageSourceIsRecreatedAndInvalidated)
    {
        Fixture f;

        float expectedWidth = 123;
        float expectedHeight = 456;
        float expectedDpi = f.Adapter->LogicalDpi * 2;

        auto imageSource = f.ExpectCreateImageSource();
        f.Load(expectedWidth, expectedHeight);

        imageSource->ResizeWithWidthAndHeightAndDpiMethod.SetExpectedCalls(1,
            [&] (float w, float h, float dpi)
            {
                Assert::AreEqual(expectedWidth, w);
                Assert::AreEqual(expectedHeight, h);
                Assert::AreEqual(expectedDpi, dpi);
                return S_OK;
            });

        imageSource->InvalidateMethod.SetExpectedCalls(1);

        f.Adapter->LogicalDpi = f.Adapter->LogicalDpi * 2;
        f.Adapter->RaiseDpiChangedEvent();
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenResized_ImageSourceIsResized)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();
        f.Load();

        float expectedWidth = 123;
        float expectedHeight = 456;
        float expectedDpi = f.Adapter->LogicalDpi;

        imageSource->ResizeWithWidthAndHeightAndDpiMethod.SetExpectedCalls(1,
            [&] (float w, float h, float dpi)
            {
                Assert::AreEqual(expectedWidth, w);
                Assert::AreEqual(expectedHeight, h);
                Assert::AreEqual(expectedDpi, dpi);
                return S_OK;
            });

        f.UserControl->Resize(Size{ expectedWidth, expectedHeight });
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenWindowIsNotVisible_RegionsInvalidatedIsNotRaised)
    {
        Fixture f;

        auto imageSource = f.ExpectCreateImageSource();

        auto onRegionsInvalidated = MockEventHandler<ControlRegionsInvalidatedHandler>(L"onRegionsInvalidated");
        EventRegistrationToken token;
        ThrowIfFailed(f.Control->add_RegionsInvalidated(onRegionsInvalidated.Get(), &token));

        f.Load();

        onRegionsInvalidated.SetExpectedCalls(0);

        f.Adapter->GetCurrentMockWindow()->SetVisible(false);
        imageSource->RaiseRegionsInvalidated(std::vector<Rect> { anyRegion }, anyRegion);
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenWindowBecomesVisible_ForceRegionsInvalidatedIsRaised)
    {
        Fixture f;

        auto imageSource = f.ExpectCreateImageSource();

        f.Load();

        f.Adapter->GetCurrentMockWindow()->SetVisible(false);

        imageSource->RaiseRegionsInvalidatedIfAnyMethod.SetExpectedCalls(1);
        f.Adapter->GetCurrentMockWindow()->SetVisible(true);
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenDeviceLostDuringRegionsInvalidated_ImageIsRecreated)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();

        auto onRegionsInvalidated = MockEventHandler<ControlRegionsInvalidatedHandler>(L"onRegionsInvalidated");
        EventRegistrationToken token;
        ThrowIfFailed(f.Control->add_RegionsInvalidated(onRegionsInvalidated.Get(), &token));

        f.Load();

        onRegionsInvalidated.SetExpectedCalls(1,
            [&] (ICanvasVirtualControl*, ICanvasRegionsInvalidatedEventArgs*)
            {
                f.Adapter->Device->MarkAsLost();
                return DXGI_ERROR_DEVICE_REMOVED;
            });

        imageSource->RaiseRegionsInvalidated(std::vector<Rect> { anyRegion }, anyRegion);

        auto replacementImageSource = f.ExpectCreateImageSource();
        f.Adapter->TickUiThread();

        // The old image source should have had its events disconnected
        onRegionsInvalidated.SetExpectedCalls(0);
        imageSource->RaiseRegionsInvalidated(std::vector<Rect> { anyRegion }, anyRegion);

        // While the new image should should have the events connected
        onRegionsInvalidated.SetExpectedCalls(1);
        replacementImageSource->RaiseRegionsInvalidated(std::vector<Rect> { anyRegion }, anyRegion);
    }

    TEST_METHOD_EX(CanvasVirtualControl_DpiScaling_ResourceHasCorrectDpiScale)
    {
        for (auto testCase : dpiScalingTestCases)
        {
            Fixture f;

            f.Adapter->LogicalDpi = testCase.Dpi;

            f.Control->put_DpiScale(testCase.DpiScale);

            auto imageSource = Make<StubCanvasVirtualImageSource>();
            f.Adapter->CreateCanvasVirtualImageSourceMethod.SetExpectedCalls(1,
                [&](ICanvasDevice*, float, float, float dpi, CanvasAlphaMode)
                {
                    float expectedDpi = testCase.Dpi * testCase.DpiScale;
                    Assert::AreEqual(dpi, expectedDpi);
                    return imageSource;
                });

            f.Load();
        }
    }

    TEST_METHOD_EX(CanvasVirtualControl_WhenChangedIsCalledOnBackgroundThread_ChangedImplIsDeferred)
    {
        Fixture f;
        auto imageSource = f.ExpectCreateImageSource();
        f.Load();

        // Call put_ClearColor from a non-UI thread will cause Changed() to be
        // called.  ChangedImpl will eventually call Invalidate, but not until
        // we've ticked the UI thread.
        imageSource->InvalidateMethod.SetExpectedCalls(0);

        f.Adapter->SetHasUIThreadAccess(false);
        f.Control->put_ClearColor(Color{ 1, 2, 3, 4 });

        // Invalidate is now called when we tick the UI thread.
        imageSource->InvalidateMethod.SetExpectedCalls(1);

        f.Adapter->SetHasUIThreadAccess(true);
        f.Adapter->TickUiThread();
    }
};

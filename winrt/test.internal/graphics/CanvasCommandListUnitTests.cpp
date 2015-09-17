// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/images/CanvasCommandList.h>

TEST_CLASS(CanvasCommandListTests)
{
    struct Fixture
    {
        ComPtr<StubCanvasDevice> Device;
        ComPtr<CanvasCommandListFactory> Factory;
        ComPtr<ICanvasCommandList> CommandList;

        Fixture()
            : Device(Make<StubCanvasDevice>())
            , Factory(Make<CanvasCommandListFactory>())
        {
            Device->CreateCommandListMethod.AllowAnyCall(
                []
                {
                    return Make<MockD2DCommandList>();
                });

            ThrowIfFailed(Factory->Create(Device.Get(), &CommandList));
        }
    };

    TEST_METHOD_EX(CanvasCommandList_FactoryCreateReturnsCommandList)
    {
        Fixture f;
        Assert::IsNotNull(f.CommandList.Get());
    }

    TEST_METHOD_EX(CanvasCommandList_ImplementsExpectedInterfaces)
    {
        Fixture f;
        ASSERT_IMPLEMENTS_INTERFACE(f.CommandList, ICanvasCommandList);
        ASSERT_IMPLEMENTS_INTERFACE(f.CommandList, ICanvasImage);
        ASSERT_IMPLEMENTS_INTERFACE(f.CommandList, IClosable);
        ASSERT_IMPLEMENTS_INTERFACE(f.CommandList, Effects::IGraphicsEffectSource);
    }

    TEST_METHOD_EX(CanvasCommandList_GetDevice_FailsWhenCalledWithNullParameter)
    {
        Fixture f;
        Assert::AreEqual(E_INVALIDARG, f.CommandList->get_Device(nullptr));
    }

    TEST_METHOD_EX(CanvasCommandList_GetDevice_ReturnsTheDevicePassedToTheFactory)
    {
        Fixture f;

        ComPtr<ICanvasDevice> retrievedDevice;
        ThrowIfFailed(f.CommandList->get_Device(&retrievedDevice));

        Assert::IsTrue(IsSameInstance(f.Device.Get(), retrievedDevice.Get()));
    }

    TEST_METHOD_EX(CanvasCommandList_GetOrCreate_ReturnsWrapper)
    {
        auto device = Make<StubCanvasDevice>();
        auto d2dCommandList = Make<MockD2DCommandList>();

        ComPtr<ICanvasCommandList> commandList = Make<CanvasCommandList>(device.Get(), d2dCommandList.Get());

        auto wrappedD2DCommandList = GetWrappedResource<ID2D1CommandList>(commandList);
        Assert::IsTrue(IsSameInstance(d2dCommandList.Get(), wrappedD2DCommandList.Get()));

        ComPtr<ICanvasDevice> retrievedDevice;
        ThrowIfFailed(commandList->get_Device(&retrievedDevice));

        Assert::IsTrue(IsSameInstance(device.Get(), retrievedDevice.Get()));
    }

    TEST_METHOD_EX(CanvasCommandList_GetResource_ReturnsValidCommandList)
    {
        Fixture f;

        auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);

        Assert::IsNotNull(d2dCommandList.Get());

        auto mockCl = static_cast<MockD2DCommandList*>(d2dCommandList.Get());
        Assert::IsNotNull(mockCl);
    }

    TEST_METHOD_EX(CanvasCommandList_WhenClosed_D2DCommandListIsReleased)
    {
        Fixture f;

        std::weak_ptr<int> weakToken;

        {
            auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);
            auto mockCl = static_cast<MockD2DCommandList*>(d2dCommandList.Get());
            mockCl->Token = std::make_shared<int>();
            weakToken = mockCl->Token;
        }

        Assert::IsFalse(weakToken.expired());

        auto closable = As<IClosable>(f.CommandList);
        ThrowIfFailed(closable->Close());

        Assert::IsTrue(weakToken.expired());
    }

    TEST_METHOD_EX(CanvasCommandList_WhenClosed_DeviceIsReleased)
    {
        Fixture f;

        WeakRef weakDevice;
        ThrowIfFailed(AsWeak(f.Device.Get(), &weakDevice));

        f.Device.Reset();

        // weakDevice is kept alive by f.CommandList
        Assert::IsTrue(IsWeakRefValid(weakDevice));

        ThrowIfFailed(As<IClosable>(f.CommandList)->Close());

        Assert::IsFalse(IsWeakRefValid(weakDevice));
    }

    TEST_METHOD_EX(CanvasCommandList_CreateDrawingSession_FailsWhenCalledWithNullParameter)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.CommandList->CreateDrawingSession(nullptr));
    }

    TEST_METHOD_EX(CanvasCommandList_CreateDrawingSession_ReturnsContextConfiguredToRenderToD2DCommandList)
    {
        Fixture f;

        auto wrappedCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);

        auto dc = Make<MockD2DDeviceContext>();

        dc->SetTextAntialiasModeMethod.SetExpectedCalls(1,
            [](D2D1_TEXT_ANTIALIAS_MODE mode)
            {
                Assert::AreEqual(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE, mode);
            });

        f.Device->CreateDeviceContextForDrawingSessionMethod.SetExpectedCalls(1, [=] { return dc; });

        dc->SetTargetMethod.SetExpectedCalls(1, 
            [=](ID2D1Image* target)
            {
                Assert::IsTrue(IsSameInstance(wrappedCommandList.Get(), target));
                
                dc->BeginDrawMethod.SetExpectedCalls(1,
                    [=]()
                    {
                        dc->EndDrawMethod.SetExpectedCalls(1);
                    });
            });

        ComPtr<ICanvasDrawingSession> ds;
        ThrowIfFailed(f.CommandList->CreateDrawingSession(&ds));

        auto wrappedDc = GetWrappedResource<ID2D1DeviceContext1>(ds);
        Assert::IsTrue(IsSameInstance(dc.Get(), wrappedDc.Get()));

        ThrowIfFailed(As<IClosable>(ds)->Close());
    }

    TEST_METHOD_EX(CanvasCommandList_GetD2DImage_ClosesD2DCommandListOnFirstCall)
    {
        Fixture f;

        auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);
        auto mockCl = static_cast<MockD2DCommandList*>(d2dCommandList.Get());

        mockCl->CloseMethod.SetExpectedCalls(1);

        ICanvasDevice* ignoredDevice = nullptr;
        ID2D1DeviceContext* ignoredDeviceContext = nullptr;
        auto canvasImageInternal = As<ICanvasImageInternal>(f.CommandList);

        for (int i = 0; i < 10; ++i)
        {
            auto d2dImage = canvasImageInternal->GetD2DImage(ignoredDevice, ignoredDeviceContext);
            Assert::IsTrue(IsSameInstance(d2dImage.Get(), d2dCommandList.Get()));
        }
    }

    TEST_METHOD_EX(CanvasCommandList_CreateDrawingSession_FailsWhenD2DCommandListIsClosed)
    {
        Fixture f;

        auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);
        auto mockCl = static_cast<MockD2DCommandList*>(d2dCommandList.Get());
        mockCl->CloseMethod.AllowAnyCall();

        ICanvasDevice* ignoredDevice = nullptr;
        ID2D1DeviceContext* ignoredDeviceContext = nullptr;
        As<ICanvasImageInternal>(f.CommandList)->GetD2DImage(ignoredDevice, ignoredDeviceContext);

        ComPtr<ICanvasDrawingSession> ds;
        Assert::AreEqual(E_INVALIDARG, f.CommandList->CreateDrawingSession(&ds));
        ValidateStoredErrorState(E_INVALIDARG, Strings::CommandListCannotBeDrawnToAfterItHasBeenUsed);
    }

    TEST_METHOD_EX(CanvasCommandList_GetD2DImage_CanHandleCloseReturningError)
    {
        Fixture f;

        auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);
        auto mockCl = static_cast<MockD2DCommandList*>(d2dCommandList.Get());

        // This might happen if the command list was created by wrapping a
        // D2DCommandList that had already been closed.
        mockCl->CloseMethod.SetExpectedCalls(1, [] { return D2DERR_WRONG_STATE; });

        ICanvasDevice* ignoredDevice = nullptr;
        ID2D1DeviceContext* ignoredDeviceContext = nullptr;
        As<ICanvasImageInternal>(f.CommandList)->GetD2DImage(ignoredDevice, ignoredDeviceContext);
    }
};

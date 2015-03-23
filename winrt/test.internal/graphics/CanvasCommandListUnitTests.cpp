// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use these files except in compliance with the License. You may obtain
// a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

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
        auto factory = Make<CanvasCommandListFactory>();

        ComPtr<ICanvasCommandList> commandList;
        ThrowIfFailed(factory->GetOrCreate(device.Get(), d2dCommandList.Get(), &commandList));

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

        auto mockCl = dynamic_cast<MockD2DCommandList*>(d2dCommandList.Get());
        Assert::IsNotNull(mockCl);
    }

    TEST_METHOD_EX(CanvasCommandList_WhenClosed_D2DCommandListIsReleased)
    {
        Fixture f;

        std::weak_ptr<int> weakToken;

        {
            auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);
            auto mockCl = dynamic_cast<MockD2DCommandList*>(d2dCommandList.Get());
            mockCl->Token = std::make_shared<int>();
            weakToken = mockCl->Token;
        }

        Assert::IsFalse(weakToken.expired());

        auto closable = As<IClosable>(f.CommandList);
        ThrowIfFailed(closable->Close());

        Assert::IsTrue(weakToken.expired());
    }

    bool IsWeakRefValid(WeakRef weakRef)
    {
        ComPtr<IInspectable> obj;
        ThrowIfFailed(weakRef.As(&obj));        
        return static_cast<bool>(obj);
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

        f.Device->CreateDeviceContextMethod.SetExpectedCalls(1, [=] { return dc; });

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
        auto mockCl = dynamic_cast<MockD2DCommandList*>(d2dCommandList.Get());

        mockCl->CloseMethod.SetExpectedCalls(1);

        ID2D1DeviceContext* ignoredDeviceContext = nullptr;
        auto canvasImageInternal = As<ICanvasImageInternal>(f.CommandList);

        for (int i = 0; i < 10; ++i)
        {
            auto d2dImage = canvasImageInternal->GetD2DImage(ignoredDeviceContext);
            Assert::IsTrue(IsSameInstance(d2dImage.Get(), d2dCommandList.Get()));
        }
    }

    TEST_METHOD_EX(CanvasCommandList_CreateDrawingSession_FailsWhenD2DCommandListIsClosed)
    {
        Fixture f;

        auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);
        auto mockCl = dynamic_cast<MockD2DCommandList*>(d2dCommandList.Get());
        mockCl->CloseMethod.AllowAnyCall();

        ID2D1DeviceContext* ignoredDeviceContext = nullptr;
        As<ICanvasImageInternal>(f.CommandList)->GetD2DImage(ignoredDeviceContext);

        ComPtr<ICanvasDrawingSession> ds;
        Assert::AreEqual(E_INVALIDARG, f.CommandList->CreateDrawingSession(&ds));
        ValidateStoredErrorState(E_INVALIDARG, Strings::CommandListCannotBeDrawnToAfterItHasBeenUsed);
    }

    TEST_METHOD_EX(CanvasCommandList_GetD2DImage_CanHandleCloseReturningError)
    {
        Fixture f;

        auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);
        auto mockCl = dynamic_cast<MockD2DCommandList*>(d2dCommandList.Get());

        // This might happen if the command list was created by wrapping a
        // D2DCommandList that had already been closed.
        mockCl->CloseMethod.SetExpectedCalls(1, [] { return D2DERR_WRONG_STATE; });

        ID2D1DeviceContext* ignoredDeviceContext = nullptr;
        As<ICanvasImageInternal>(f.CommandList)->GetD2DImage(ignoredDeviceContext);        
    }

    TEST_METHOD_EX(CanvasCommandList_GetRealizedEffectNode_ClosesD2DCommandListOnFirstCall)
    {
        Fixture f;
        
        auto d2dCommandList = GetWrappedResource<ID2D1CommandList>(f.CommandList);
        auto mockCl = dynamic_cast<MockD2DCommandList*>(d2dCommandList.Get());

        mockCl->CloseMethod.SetExpectedCalls(1);

        ID2D1DeviceContext* ignoredDeviceContext = nullptr;
        auto canvasImageInternal = As<ICanvasImageInternal>(f.CommandList);

        for (int i = 0; i < 10; ++i)
        {
            float anyDpi = 1234;
            auto node = canvasImageInternal->GetRealizedEffectNode(ignoredDeviceContext, anyDpi);
            Assert::IsTrue(IsSameInstance(node.Image.Get(), d2dCommandList.Get()));
            Assert::AreEqual(0.0f, node.Dpi);
            Assert::AreEqual(0ULL, node.RealizationId);
        }
    }
};


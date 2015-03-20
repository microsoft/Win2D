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

#include <lib/xaml/BaseControl.h>
#include <lib/xaml/BaseControlAdapter.h>

//
// These tests explicitly exercise BaseControl.  Generally we try and avoid
// doing this and prefer to test via the derived classes.  However, in some
// cases, it becomes very complicated to try and coerce CanvasControl and
// CanvasAnimatedControl to interact with BaseControl in the same way.
//

namespace
{
    class AnyDerivedControl;
    class IAnyAdapter;
    struct IAnyControlInterface;

    class AnyRenderTarget : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IUnknown>
    {
    };

    struct AnyTraits
    {
        typedef AnyDerivedControl control_t;
        typedef IAnyControlInterface controlInterface_t;
        typedef IAnyAdapter adapter_t;
        typedef AnyRenderTarget renderTarget_t;
        typedef ITypedEventHandler<IAnyControlInterface*, IInspectable*> drawEventHandler_t;
        typedef IInspectable drawEventArgs_t;
        typedef ITypedEventHandler<IAnyControlInterface*, CanvasCreateResourcesEventArgs*> createResourcesEventHandler_t;        
    };

    [uuid(8BEBD9A8-8512-4C76-8FB7-BDF923F8D4E1)]
    struct IAnyControlInterface : public IInspectable
    {
        IFACEMETHOD(put_ClearColor)(Color) = 0;
        IFACEMETHOD(get_ClearColor)(Color*) = 0;
        IFACEMETHOD(add_CreateResources)(AnyTraits::createResourcesEventHandler_t*, EventRegistrationToken*) = 0;
        IFACEMETHOD(remove_CreateResources)(EventRegistrationToken token) = 0;
        IFACEMETHOD(add_Draw)(AnyTraits::drawEventHandler_t*, EventRegistrationToken*) = 0;
        IFACEMETHOD(remove_Draw)(EventRegistrationToken) = 0;
        IFACEMETHOD(get_ReadyToDraw)(boolean*) = 0;
        IFACEMETHOD(get_Size)(Size*) = 0;
        IFACEMETHOD(RemoveFromVisualTree)() = 0;
    };

    class IAnyAdapter : public IBaseControlAdapter<AnyTraits>
    {
    };

    class AnyAdapter : public BaseControlTestAdapter<AnyTraits>
    {
    };

    class AnyDerivedControl 
        : public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>, 
            MixIn<AnyDerivedControl, BaseControl<AnyTraits>>,
            ComposableBase<>>,
          public BaseControl<AnyTraits>
    {
        InspectableClass(L"AnyDerivedControl", BaseTrust);

    public:
        using BaseControl::RenderTarget;

        AnyDerivedControl(std::shared_ptr<IAnyAdapter> adapter)
            : BaseControl(adapter)
        {
        }

        CALL_COUNTER_WITH_MOCK(CreateOrUpdateRenderTargetMethod, void(ICanvasDevice*, CanvasBackground, float, Size, RenderTarget*));
        CALL_COUNTER_WITH_MOCK(CreateDrawEventArgsMethod, ComPtr<drawEventArgs_t>(ICanvasDrawingSession*, bool));
        CALL_COUNTER_WITH_MOCK(ChangedMethod, void());
        CALL_COUNTER_WITH_MOCK(LoadedMethod, void());
        CALL_COUNTER_WITH_MOCK(UnloadedMethod, void());
        CALL_COUNTER_WITH_MOCK(ApplicationSuspendingMethod, void(ISuspendingEventArgs*));
        CALL_COUNTER_WITH_MOCK(ApplicationResumingMethod, void());

        virtual void CreateOrUpdateRenderTarget(
            ICanvasDevice* device,
            CanvasBackground newBackgroundMode,
            float newDpi,
            Size newSize,
            RenderTarget* renderTarget) override final
        {
            return CreateOrUpdateRenderTargetMethod.WasCalled(device, newBackgroundMode, newDpi, newSize, renderTarget);
        }

        virtual ComPtr<drawEventArgs_t> CreateDrawEventArgs(
            ICanvasDrawingSession* drawingSession,
            bool isRunningSlowly) override final
        {
            return CreateDrawEventArgsMethod.WasCalled(drawingSession, isRunningSlowly);
        }

        virtual void Changed(Lock const&, ChangeReason) override final
        {
            return ChangedMethod.WasCalled();
        }

        virtual void Loaded() override final
        {
            LoadedMethod.WasCalled();
        }

        virtual void Unloaded() override final
        {
            UnloadedMethod.WasCalled();
        }

        virtual void ApplicationSuspending(ISuspendingEventArgs* args) override final
        {
            ApplicationSuspendingMethod.WasCalled(args);
        }

        virtual void ApplicationResuming() override final
        {
            ApplicationResumingMethod.WasCalled();
        }

        template<typename FN>
        void CallRunWithRenderTarget(Size const& size, FN&& fn)
        {
            CallCounter<> expectCallback(L"RunWithRenderTarget callback");
            expectCallback.SetExpectedCalls(1);

            RunWithRenderTarget(size,
                [&] (AnyRenderTarget* target, ICanvasDevice*, Color const& clearColor, bool callDrawHandlers)
                {
                    expectCallback.WasCalled();
                    fn(target, clearColor, callDrawHandlers);
                });
        }
    };
}

using Internal::AggregateType;

template<>
struct ITypedEventHandler<IAnyControlInterface*, CanvasCreateResourcesEventArgs*> 
    : ITypedEventHandler_impl<IAnyControlInterface*, AggregateType<CanvasCreateResourcesEventArgs*, ICanvasCreateResourcesEventArgs*>>
{
};

template<>
struct ITypedEventHandler<IAnyControlInterface*, IInspectable*> 
    : ITypedEventHandler_impl<IAnyControlInterface*, IInspectable*>
{
};

template<>
struct TestAdapter<AnyTraits>
{
    typedef AnyAdapter type;

    static std::shared_ptr<type> Create()
    {
        return std::make_shared<AnyAdapter>();
    }
};


TEST_CLASS(BaseControl_Interaction_With_RecreatableDeviceManager)
{
    struct Fixture : public BasicControlFixture<AnyTraits>
    {
        MockRecreatableDeviceManager<AnyTraits>* DeviceManager;
        std::function<void(ChangeReason)> ChangedCallback;

        Fixture()
            : DeviceManager(nullptr)
        {
            CreateAdapter();

            Adapter->CreateRecreatableDeviceManagerMethod.SetExpectedCalls(1,
                [=]
                {
                    Assert::IsNull(DeviceManager);
                    auto manager = std::make_unique<MockRecreatableDeviceManager<AnyTraits>>();
                    manager->SetChangedCallbackMethod.SetExpectedCalls(1,
                        [=](std::function<void(ChangeReason)> fn)
                        {
                            ChangedCallback = fn;
                        });

                    DeviceManager = manager.get();
                    return manager;
                });
            
            CreateControl();
        }

    };

    TEST_METHOD_EX(BaseControl_WhenChangedHandlerCalled_ChangedCalled)
    {
        Fixture f;
        f.Control->ChangedMethod.SetExpectedCalls(1);
        f.ChangedCallback(ChangeReason::Other);
    }

    TEST_METHOD_EX(BaseControl_WhenDeviceIsNewlyCreated_ANewRenderTargetIsCreated)
    {
        Size anySize{1,2};

        auto firstDevice = Make<StubCanvasDevice>();
        auto secondDevice = Make<StubCanvasDevice>();

        auto firstRenderTarget = Make<AnyRenderTarget>();
        auto secondRenderTarget = Make<AnyRenderTarget>();

        Fixture f;

        // First RunWithRenderTarget
        f.DeviceManager->SetDevice(firstDevice);
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::NewlyCreatedDevice, 1);

        f.Control->CreateOrUpdateRenderTargetMethod.SetExpectedCalls(1,
            [&] (ICanvasDevice* device, CanvasBackground background, float, Size size, AnyDerivedControl::RenderTarget* renderTarget)
            {
                Assert::IsTrue(IsSameInstance(firstDevice.Get(), device));
                Assert::AreEqual(anySize, size);
                Assert::IsNull(renderTarget->Target.Get());
                renderTarget->Target = firstRenderTarget;
            });

        f.Control->CallRunWithRenderTarget(anySize,
            [&] (AnyRenderTarget* target, Color const&, bool)
            {
                Assert::IsTrue(IsSameInstance(firstRenderTarget.Get(), target));
            });

        // Second RunWithRenderTarget
        f.DeviceManager->SetDevice(secondDevice);
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::NewlyCreatedDevice, 1);

        f.Control->CreateOrUpdateRenderTargetMethod.SetExpectedCalls(1,
            [&] (ICanvasDevice* device, CanvasBackground background, float, Size size, AnyDerivedControl::RenderTarget* renderTarget)
            {
                Assert::IsTrue(IsSameInstance(secondDevice.Get(), device));
                Assert::AreEqual(anySize, size);
                Assert::IsNull(renderTarget->Target.Get()); // note that the target was set to null
                renderTarget->Target = secondRenderTarget;
            });

        f.Control->CallRunWithRenderTarget(anySize,
            [&] (AnyRenderTarget* target, Color const&, bool)
            {
                Assert::IsTrue(IsSameInstance(secondRenderTarget.Get(), target));
            });
    }

    TEST_METHOD_EX(BaseControl_WhenDeviceIsNotNewlyCreated_RenderTargetIsPersisted)
    {
        Size anySize{1,2};

        auto device = Make<StubCanvasDevice>();

        auto target = Make<AnyRenderTarget>();

        Fixture f;

        // First RunWithRenderTarget
        f.DeviceManager->SetDevice(device);
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::NewlyCreatedDevice, 1);

        f.Control->CreateOrUpdateRenderTargetMethod.SetExpectedCalls(1,
            [&] (ICanvasDevice* device, CanvasBackground background, float, Size size, AnyDerivedControl::RenderTarget* renderTarget)
            {
                Assert::IsNull(renderTarget->Target.Get());
                renderTarget->Target = target;
            });

        f.Control->CallRunWithRenderTarget(anySize,
            [&] (AnyRenderTarget* passedTarget, Color const&, bool)
            {
                Assert::IsTrue(IsSameInstance(target.Get(), passedTarget));
            });

        // Second RunWithRenderTarget
        f.DeviceManager->SetRunWithDeviceFlags(RunWithDeviceFlags::None, 1);

        f.Control->CreateOrUpdateRenderTargetMethod.SetExpectedCalls(1,
            [&] (ICanvasDevice* device, CanvasBackground background, float, Size size, AnyDerivedControl::RenderTarget* renderTarget)
            {
                Assert::IsTrue(IsSameInstance(target.Get(), renderTarget->Target.Get()));
            });

        f.Control->CallRunWithRenderTarget(anySize,
            [&] (AnyRenderTarget* passedTarget, Color const&, bool)
            {
                Assert::IsTrue(IsSameInstance(target.Get(), passedTarget));
            });
    }
};

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace
{
    [uuid(92378CDA-713F-416D-99EE-EC0DFF5D238E)]
    class IDummyResource : public IUnknown
    {
    };

    class DummyResource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDummyResource>
    {
    };

    [uuid(B7E157E0-99C7-463B-8DDC-F72B10221FEC)]
    class IDummyWrapper : public IInspectable
    {
    public:
        virtual int GetId() = 0;
    };

    class DummyWrapper : RESOURCE_WRAPPER_RUNTIME_CLASS(
        IDummyResource,
        DummyWrapper,
        IDummyWrapper,
        ICanvasResourceWrapperWithDevice)
    {
        InspectableClass(L"DummyWrapper", BaseTrust);

        int m_id;
        ComPtr<ICanvasDevice> m_device;

    public:
        DummyWrapper(ICanvasDevice* device, IDummyResource* resource)
            : ResourceWrapper(resource)
            , m_device(device)
        {
            static int nextId = 1;
            m_id = nextId++;
        }

        virtual ~DummyWrapper()
        {
            Close();
        }

        virtual int GetId() override
        {
            GetResource();      // throws if closed
            return m_id;
        }

        IFACEMETHODIMP get_Device(ICanvasDevice** device) override
        {
            return m_device.CopyTo(device);
        }
    };
}


template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<IDummyWrapper>(IDummyWrapper* value)
{
    return PointerToString(L"IDummyWrapper", value);
}


TEST_CLASS(ResourceManagerUnitTests)
{
    TEST_METHOD_EX(ResourceManager_Exercise)
    {
        // Tell ResourceManager about our DummyResource/DummyWrapper test types.
        auto tryCreateDummyResource = ResourceManager::TryCreate<IDummyResource, DummyWrapper, ResourceManager::MakeWrapperWithDevice>;
        ResourceManager::RegisterType(tryCreateDummyResource);
        auto restoreTypeTable = MakeScopeWarden([&] { ResourceManager::UnregisterType(tryCreateDummyResource); });

        std::vector<ComPtr<IDummyResource>> resources;
        std::vector<ComPtr<DummyWrapper>> wrappers;

        // initial populate
        for (int i=0; i<5; ++i)
        {
            auto resource = Make<DummyResource>();
            auto wrapper = Make<DummyWrapper>(nullptr, resource.Get());

            resources.push_back(resource);
            wrappers.push_back(wrapper);
        }

        // check that GetOrCreate always returns an existing entry
        for (size_t i = 0; i < resources.size(); ++i)
        {
            auto actual = ResourceManager::GetOrCreate<IDummyWrapper>(resources[i].Get());
            Assert::AreEqual<IDummyWrapper*>(wrappers[i].Get(), actual.Get());
        }

        // destroy an entry and verify a new one is created when asked
        for (size_t i = 0; i < resources.size(); ++i)
        {
            int oldId = wrappers[i]->GetId();
            wrappers[i].Reset();
            auto actual = ResourceManager::GetOrCreate<IDummyWrapper>(resources[i].Get());
            Assert::IsTrue(actual);
            Assert::AreNotEqual(oldId, actual->GetId());
        }
    }

    TEST_METHOD_EX(ResourceManager_Close_Reused_Resource)
    {
        // Tell ResourceManager about our DummyResource/DummyWrapper test types.
        auto tryCreateDummyResource = ResourceManager::TryCreate<IDummyResource, DummyWrapper, ResourceManager::MakeWrapperWithDevice>;
        ResourceManager::RegisterType(tryCreateDummyResource);
        auto restoreTypeTable = MakeScopeWarden([&] { ResourceManager::UnregisterType(tryCreateDummyResource); });

        auto resource = Make<DummyResource>();
        auto wrapper = Make<DummyWrapper>(nullptr, resource.Get());

        // Closing a wrapper should disassociate it with the resource, so
        // GetOrCreate should create a new wrapper.
        wrapper->Close();

        auto otherWrapper = ResourceManager::GetOrCreate<IDummyWrapper>(resource.Get());

        ComPtr<IInspectable> wrapperInspectable;
        wrapper.As(&wrapperInspectable);

        ComPtr<IInspectable> otherWrapperInspectable;
        otherWrapper.As(&otherWrapperInspectable);

        Assert::AreNotEqual(wrapperInspectable.Get(), otherWrapperInspectable.Get());
    }

    TEST_METHOD_EX(ResourceManager_GetOrCreate_WithMatchingDevice_Succeeds)
    {
        auto resource = Make<DummyResource>();
        auto canvasDevice = Make<StubCanvasDevice>();
        auto expectedWrapper = Make<DummyWrapper>(canvasDevice.Get(), resource.Get());

        auto actualWrapper = ResourceManager::GetOrCreate<IDummyWrapper>(canvasDevice.Get(), resource.Get());

        Assert::AreEqual<IDummyWrapper*>(expectedWrapper.Get(), actualWrapper.Get());
    }

    TEST_METHOD_EX(ResourceManager_GetOrCreate_WithWrongDevice_Fails)
    {
        auto resource = Make<DummyResource>();
        auto canvasDevice = Make<StubCanvasDevice>();
        auto expectedWrapper = Make<DummyWrapper>(canvasDevice.Get(), resource.Get());

        auto otherCanvasDevice = Make<StubCanvasDevice>();
        ExpectHResultException(E_INVALIDARG, [&]{ ResourceManager::GetOrCreate<IDummyWrapper>(otherCanvasDevice.Get(), resource.Get()); });
    }
};


//
// Verify that ResourceManager copes with COM objects that require QI to
// IUnknown for equality comparisons.
//

namespace
{
    [uuid(52FB7C6A-20A5-4F6E-888F-F23FECF966D8)]
    struct IBaseInterface : public IUnknown
    {
    };

    [uuid(B0507E89-42F7-4087-BCE9-326F259FDA2C)]
    struct IInterface1 : public IBaseInterface
    {
    };

    [uuid(3B4BDC33-059A-4353-B870-69D42711AE24)]
    struct IInterface2 : public IBaseInterface
    {
    };

    class MultipleInterfaceResource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IInterface1, IInterface2, IBaseInterface>
    {
    };

    [uuid(5647EAB4-2EA9-4374-9D02-9C1D35E4BC43)]
    class IMultipleInterface : public IInspectable
    {
    };

    class MultipleInterfaceWrapper : RESOURCE_WRAPPER_RUNTIME_CLASS(IBaseInterface, MultipleInterfaceWrapper, IMultipleInterface)
    {
        InspectableClass(L"MultipleInterfaceWrapper", BaseTrust);

        int m_id;

    public:
        MultipleInterfaceWrapper(IBaseInterface* resource)
            : ResourceWrapper(resource)
        {
            static int nextId = 1;
            m_id = nextId++;
        }

        int GetId()
        {
            return m_id;
        }
    };
}

template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<IMultipleInterface>(IMultipleInterface* value)
{
    return PointerToString(L"IMultipleInterface", value);
}


TEST_CLASS(ResourceManagerRequiresCOMIdentity)
{
    TEST_METHOD_EX(ResourceManager_UsesCOMIdentityRules)
    {
        // Tell ResourceManager about our MultipleInterfaceResource/MultipleInterfaceWrapper test types.
        auto tryCreateDummyResource = ResourceManager::TryCreate<IBaseInterface, MultipleInterfaceWrapper, ResourceManager::MakeWrapper>;
        ResourceManager::RegisterType(tryCreateDummyResource);
        auto restoreTypeTable = MakeScopeWarden([&] { ResourceManager::UnregisterType(tryCreateDummyResource); });

        auto resource = Make<MultipleInterfaceResource>();

        ComPtr<IInterface1> resourceAsInterface1;
        ThrowIfFailed(resource.As(&resourceAsInterface1));

        ComPtr<IInterface2> resourceAsInterface2;
        ThrowIfFailed(resource.As(&resourceAsInterface2));

        Assert::IsTrue(static_cast<IUnknown*>(resourceAsInterface1.Get()) != static_cast<IUnknown*>(resourceAsInterface2.Get()));

        auto wrapper1 = ResourceManager::GetOrCreate<IMultipleInterface>(resourceAsInterface1.Get()); // implicit IInterface1->IBaseInterface cast
        auto wrapper2 = ResourceManager::GetOrCreate<IMultipleInterface>(resourceAsInterface2.Get()); // implicit IInterface2->IBaseInterface cast

        Assert::AreEqual(wrapper1.Get(), wrapper2.Get());
    }
};

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace
{
    class DummyResource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IUnknown>
    {
    };

    [uuid(B7E157E0-99C7-463B-8DDC-F72B10221FEC)]
    class IDummyWrapper : public IInspectable
    {
    public:
        virtual int GetId() = 0;
    };

    class DummyWrapper;
    class DummyManager;

    struct DummyTraits
    {
        typedef DummyResource resource_t;
        typedef DummyWrapper wrapper_t;
        typedef IDummyWrapper wrapper_interface_t;
        typedef DummyManager manager_t;
    };

    class DummyWrapper : RESOURCE_WRAPPER_RUNTIME_CLASS(DummyTraits)
    {
        InspectableClass(L"DummyWrapper", BaseTrust);

        int m_id;
        ComPtr<ICanvasDevice> m_device;

    public:
        DummyWrapper(std::shared_ptr<DummyManager> manager, ICanvasDevice* device, DummyResource* resource)
            : ResourceWrapper(manager, resource)
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

        HRESULT get_Device(ICanvasDevice** device)
        {
            return m_device.CopyTo(device);
        }
    };


    class DummyManager : public ResourceManager<DummyTraits>
    {
    public:
        ComPtr<DummyWrapper> CreateNew(DummyResource* resource)
        {
            return Make<DummyWrapper>(shared_from_this(), nullptr, resource);
        }

        ComPtr<DummyWrapper> CreateNew(ICanvasDevice* device, DummyResource* resource)
        {
            return Make<DummyWrapper>(shared_from_this(), device, resource);
        }

        ComPtr<DummyWrapper> CreateWrapper(DummyResource* resource)
        {
            return Make<DummyWrapper>(shared_from_this(), nullptr, resource);
        }

        ComPtr<DummyWrapper> CreateWrapper(ICanvasDevice* device, DummyResource* resource)
        {
            return Make<DummyWrapper>(shared_from_this(), device, resource);
        }
    };
}


template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<DummyWrapper>(DummyWrapper* value)
{
    return PointerToString(L"DummyWrapper", value);
}


TEST_CLASS(ResourceTrackerUnitTests)
{
    TEST_METHOD_EX(ResourceTracker_Exercise)
    {
        auto manager = std::make_shared<DummyManager>();

        std::vector<ComPtr<DummyResource>> resources;
        std::vector<ComPtr<DummyWrapper>> wrappers;

        // initial populate
        for (int i=0; i<5; ++i)
        {
            auto resource = Make<DummyResource>();
            auto wrapper = manager->Create(resource.Get());

            resources.push_back(resource);
            wrappers.push_back(wrapper);
        }

        // check that GetOrCreate always returns an existing entry
        for (size_t i=0; i<resources.size(); ++i)
        {
            auto actual = manager->GetOrCreate(resources[i].Get());
            Assert::AreEqual(wrappers[i].Get(), actual.Get());
        }

        // destroy an entry and verify a new one is created when asked
        for (size_t i=0; i<resources.size(); ++i)
        {
            int oldId = wrappers[i]->GetId();
            wrappers[i].Reset();
            auto actual = manager->GetOrCreate(resources[i].Get());
            Assert::IsTrue(actual);
            Assert::AreNotEqual(oldId, actual->GetId());
        }
    }

    TEST_METHOD_EX(ResourceTracker_Close_Reused_Resource)
    {
        auto manager = std::make_shared<DummyManager>();
        
        auto resource = Make<DummyResource>();

        auto wrapper = manager->Create(resource.Get());

        // Closing a wrapper should disassociate it with the resource, so
        // GetOrCreate should create a new wrapper.
        wrapper->Close();

        auto otherWrapper = manager->GetOrCreate(resource.Get());

        ComPtr<IInspectable> wrapperInspectable;
        wrapper.As(&wrapperInspectable);

        ComPtr<IInspectable> otherWrapperInspectable;
        otherWrapper.As(&otherWrapperInspectable);

        Assert::AreNotEqual(wrapperInspectable.Get(), otherWrapperInspectable.Get());
    }

    TEST_METHOD_EX(ResourceTracker_GetOrCreate_WithMatchingDevice_Succeeds)
    {
        auto manager = std::make_shared<DummyManager>();
        auto resource = Make<DummyResource>();
        auto canvasDevice = Make<StubCanvasDevice>();
        auto expectedWrapper = manager->Create(canvasDevice.Get(), resource.Get());

        auto actualWrapper = manager->GetOrCreate(canvasDevice.Get(), resource.Get());

        Assert::AreEqual(expectedWrapper.Get(), actualWrapper.Get());
    }

    TEST_METHOD_EX(ResourceTracker_GetOrCreate_WithWrongDevice_Fails)
    {
        auto manager = std::make_shared<DummyManager>();
        auto resource = Make<DummyResource>();
        auto canvasDevice = Make<StubCanvasDevice>();
        auto expectedWrapper = manager->Create(canvasDevice.Get(), resource.Get());

        auto otherCanvasDevice = Make<StubCanvasDevice>();
        ExpectHResultException(E_INVALIDARG, [&]{ manager->GetOrCreate(otherCanvasDevice.Get(), resource.Get()); });
    }
};


//
// Verify that ResourceTracker copes with COM objects that require QI to
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

    class MultipleInterfaceResource : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IInterface1, IInterface2>
    {
    };

    [uuid(5647EAB4-2EA9-4374-9D02-9C1D35E4BC43)]
    class IMultipleInterface : public IInspectable
    {
    };

    class MultipleInterfaceWrapper;
    class MultipleInterfaceManager;

    struct MultipleInterfaceTraits
    {
        typedef IBaseInterface resource_t;
        typedef MultipleInterfaceWrapper wrapper_t;
        typedef IMultipleInterface wrapper_interface_t;
        typedef MultipleInterfaceManager manager_t;
    };

    class MultipleInterfaceWrapper : RESOURCE_WRAPPER_RUNTIME_CLASS(MultipleInterfaceTraits)
    {
        InspectableClass(L"MultipleInterfaceWrapper", BaseTrust);

        int m_id;

    public:
        MultipleInterfaceWrapper(std::shared_ptr<MultipleInterfaceManager> manager, IBaseInterface* resource)
            : ResourceWrapper(manager, resource)
        {
            static int nextId = 1;
            m_id = nextId++;
        }

        int GetId()
        {
            return m_id;
        }
    };


    class MultipleInterfaceManager : public ResourceManager<MultipleInterfaceTraits>
    {
    public:
        ComPtr<MultipleInterfaceWrapper> CreateWrapper(IBaseInterface* resource)
        {
            return Make<MultipleInterfaceWrapper>(shared_from_this(), resource);
        }
    };

}

template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<MultipleInterfaceWrapper>(MultipleInterfaceWrapper* value)
{
    return PointerToString(L"MultipleInterfaceWrapper", value);
}


TEST_CLASS(ResourceTrackerRequiresCOMIdentity)
{
    TEST_METHOD_EX(ResourceTracker_UsesCOMIdentityRules)
    {
        auto manager = std::make_shared<MultipleInterfaceManager>();
        auto resource = Make<MultipleInterfaceResource>();

        ComPtr<IInterface1> resourceAsInterface1;
        ThrowIfFailed(resource.As(&resourceAsInterface1));

        ComPtr<IInterface2> resourceAsInterface2;
        ThrowIfFailed(resource.As(&resourceAsInterface2));

        Assert::IsTrue(static_cast<IUnknown*>(resourceAsInterface1.Get()) != static_cast<IUnknown*>(resourceAsInterface2.Get()));

        auto wrapper1 = manager->GetOrCreate(resourceAsInterface1.Get()); // implicit IInterface1->IBaseInterface cast
        auto wrapper2 = manager->GetOrCreate(resourceAsInterface2.Get()); // implicit IInterface2->IBaseInterface cast

        Assert::AreEqual(wrapper1.Get(), wrapper2.Get());
    }
};



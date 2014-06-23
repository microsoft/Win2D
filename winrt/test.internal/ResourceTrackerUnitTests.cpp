// Copyright (c) Microsoft Corporation.  All rights reserved

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

    public:
        DummyWrapper(std::shared_ptr<DummyManager> manager, DummyResource* resource)
            : ResourceWrapper(manager, resource)
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
    };


    class DummyManager : public ResourceManager<DummyTraits>
    {
    public:
        ComPtr<DummyWrapper> CreateNew(DummyResource* resource)
        {
            return Make<DummyWrapper>(shared_from_this(), resource);
        }

        ComPtr<DummyWrapper> CreateWrapper(DummyResource* resource)
        {
            return Make<DummyWrapper>(shared_from_this(), resource);
        }
    };
}


template<>
static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<DummyWrapper>(DummyWrapper* value)
{
    return PointerToString(L"DummyWrapper", value);
}


TEST_CLASS(ResourceTrackerUnitTests)
{
    TEST_METHOD(ResourceTracker_Exercise)
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

    TEST_METHOD(ResourceTracker_Close_Reused_Resource)
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
};


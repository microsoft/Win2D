// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

namespace
{
    class DummyResourceManager : public StoredInPropertyMap
    {
    };

    class DummyFactory : public PerApplicationManager<DummyFactory, DummyResourceManager>
    {
    public:
        DummyFactory(ABI::Windows::ApplicationModel::Core::ICoreApplication* app)
            : PerApplicationManager(app)
        {
        }
    };
}

template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<DummyResourceManager>(DummyResourceManager* value)
{
    return PointerToString(L"DummyResourceManager", value);
}


TEST_CLASS(FactoryWithResourceManagerUnitTests)
{
    //
    // FactoryWithResourceManager is intended to be used as a base class for
    // WinRT activation factories.  The life span of an activation factory does
    // not necessary match the life span of an application, so we need to cope
    // with an activation factory being destroyed and then coming back again.
    //
    // As we store state in the Manager associated with an activation factory we
    // want a newly created factory to find any existing manager instance that
    // there may be and use that one on creation.
    //
    TEST_METHOD_EX(FactoryWithResourceManager_Manager_OutLives_Factory)
    {
        //
        // We don't want to use the 'real' CoreApplication because this test
        // runs as a Win32 process and so doesn't have a core application, so we
        // use a MockCoreApplication and our own propertyset that won't outlive
        // this test method.
        //
        ComPtr<IActivationFactory> propertySetActivationFactory;
        ThrowIfFailed(GetActivationFactory(
            WinString(RuntimeClass_Windows_Foundation_Collections_PropertySet),
            &propertySetActivationFactory));

        ComPtr<IPropertySet> propertySet;
        ThrowIfFailed(propertySetActivationFactory->ActivateInstance(&propertySet));

        ComPtr<MockCoreApplication> app = Make<MockCoreApplication>();
        app->get_PropertiesMethod.AllowAnyCall(
            [&](IPropertySet** out)
            { 
                return propertySet.CopyTo(out); 
            });

        //
        // The actual test ensures that the same manager is used for two
        // different factory instances.
        //
        std::shared_ptr<DummyResourceManager> manager;

        {
            DummyFactory dummyFactory(app.Get());
            manager = dummyFactory.GetManager();
        }

        {
            DummyFactory dummyFactory(app.Get());
            Assert::AreEqual(manager.get(), dummyFactory.GetManager().get());
        }
    }
};


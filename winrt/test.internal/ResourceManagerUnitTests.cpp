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

namespace
{
    class DummyResourceManager
    {
    };

    class DummyFactory : public FactoryWithResourceManager<DummyFactory, DummyResourceManager>
    {
    public:
        DummyFactory(ABI::Windows::ApplicationModel::Core::ICoreApplication* app)
            : FactoryWithResourceManager(app)
        {
        }
    };
}

template<>
static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<DummyResourceManager>(DummyResourceManager* value)
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
    TEST_METHOD(FactoryWithResourceManager_Manager_OutLives_Factory)
    {
        //
        // We don't want to use the 'real' CoreApplication because this test
        // runs as a Win32 process and so doesn't have a core application, so we
        // use a MockCoreApplication and our own propertyset that won't outlive
        // this test method.
        //
        ComPtr<IActivationFactory> propertySetActivationFactory;
        ThrowIfFailed(GetActivationFactory(
            HStringReference(RuntimeClass_Windows_Foundation_Collections_PropertySet).Get(),
            &propertySetActivationFactory));

        ComPtr<IPropertySet> propertySet;
        ThrowIfFailed(propertySetActivationFactory->ActivateInstance(&propertySet));

        ComPtr<MockCoreApplication> app = Make<MockCoreApplication>();
        app->MockGetProperties = [&]() { return propertySet; };

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


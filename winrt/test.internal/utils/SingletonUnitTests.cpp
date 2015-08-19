// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"


class SingletonTestType : public Singleton<SingletonTestType>
{
public:
    SingletonTestType()
    {
        m_id = ++m_nextId;

        m_instanceCount++;
    }

    ~SingletonTestType()
    {
        m_instanceCount--;
    }

    int Id() { return m_id; }

    static int InstanceCount() { return m_instanceCount; }

private:
    int m_id;

    static int m_nextId;
    static int m_instanceCount;
};


int SingletonTestType::m_nextId = 0;
int SingletonTestType::m_instanceCount = 0;


template<>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<SingletonTestType>(SingletonTestType* value)
{
    return PointerToString(L"SingletonTestType", value);
}


TEST_CLASS(SingletonTests)
{
    TEST_METHOD_EX(Singleton_GetInstance_ReturnsSharedInstance)
    {
        auto reference1 = SingletonTestType::GetInstance();
        auto reference2 = SingletonTestType::GetInstance();

        Assert::AreEqual(reference1.get(), reference2.get());
        Assert::AreEqual(1, SingletonTestType::InstanceCount());
    }


    TEST_METHOD_EX(Singleton_AfterAllReferencesReleased_SingletonIsRecreated)
    {
        // Query the singleton.
        auto originalReference = SingletonTestType::GetInstance();
        auto originalId = originalReference->Id();
        Assert::AreEqual(1, SingletonTestType::InstanceCount());

        // Releasing our reference to it should destroy the object.
        originalReference.reset();
        Assert::AreEqual(0, SingletonTestType::InstanceCount());

        // Querying again should return a newly created instance.
        auto newReference = SingletonTestType::GetInstance();
        Assert::AreNotEqual(originalId, newReference->Id());
        Assert::AreEqual(1, SingletonTestType::InstanceCount());
    }


    TEST_METHOD_EX(Singleton_SetInstance)
    {
        auto myInstance = std::make_shared<SingletonTestType>();

        // Setting our instance and then reading it back should return the one we set.
        SingletonTestType::SetInstance(myInstance);

        Assert::AreEqual(myInstance.get(), SingletonTestType::GetInstance().get());

        // Releasing our reference should automatically unset it.
        auto myId = myInstance->Id();

        myInstance.reset();

        Assert::AreNotEqual(myId, SingletonTestType::GetInstance()->Id());
    }
};

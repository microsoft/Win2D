// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once


// Manages a singleton instance of T, which will be kept alive as long as any client maintains a shared_ptr to it.
template<typename T, typename DefaultImplementation = T>
class Singleton
{
public:
    // Demand-creates the singleton instance.
    static std::shared_ptr<T> GetInstance()
    {
        std::lock_guard<std::mutex> lock(Mutex());

        // Do we have an existing instance?
        auto instance = CurrentInstance().lock();

        // If not, make one now.
        if (!instance)
        {
            instance = std::make_shared<DefaultImplementation>();

            CurrentInstance() = instance;
        }

        return instance;
    }


    // Explicitly specifies the active instance, overriding the normal demand-create behavior.
    // This is used by unit tests to inject custom adapters.
    static void SetInstance(std::shared_ptr<T> const& instance)
    {
        std::lock_guard<std::mutex> lock(Mutex());

        assert(CurrentInstance().expired());

        CurrentInstance() = instance;
    }


private:
    // Weak pointer to the current singleton instance.
    static std::weak_ptr<T>& CurrentInstance()
    {
        static std::weak_ptr<T> instance;

        return instance;
    }


    static std::mutex& Mutex()
    {
        static std::mutex mutex;

        return mutex;
    }
};

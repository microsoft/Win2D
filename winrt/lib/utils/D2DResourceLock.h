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

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class D2DResourceLock
    {
        //
        // D2D has two options for factory initialization- 'single'
        // or 'multi' threaded. 
        //
        // 'Multi' threaded will cause D2D to protect cross-thread access of
        // resources.
        //
        // 'Single' threaded will not do that- affording a large degree of 
        // CPU scaling.
        //
        // Only for factories designated as 'Multi', D2D goes a step further
        // and exposes synchronization methods through ID2D1Multithread.
        // Apps using D2D with D3D/DXGI interop can use these methods to
        // prevent concurrent access- from within D2D, or anywhere- so long
        // as the ID2D1Multithread Enter/Leave methods are used consistently.
        //
        // Win2D's own factories are created as 'Multi' to allow for maximum
        // safety of D2D operations across threads. This does not protect 
        // D3D/DXGI behavior, though. Operations such as mapping a D3D resource,
        // then, should be protected using the ID2D1Multithread lock.
        //
        // It is possible to interop an object which was created by the 
        // app using the 'Single' option. For these, Win2D does not perform
        // synchronization- by choosing the 'Single' multithreaded option,
        // the app is making a conscious choice to not have protection
        // across threads.
        //
        ComPtr<ID2D1Multithread> m_d2dMultithread;

    public:
        D2DResourceLock(ID2D1Resource* d2dResource)
        {
            ComPtr<ID2D1Factory> d2dFactory;
            d2dResource->GetFactory(&d2dFactory);

            m_d2dMultithread = As<ID2D1Multithread>(d2dFactory);

            if (m_d2dMultithread->GetMultithreadProtected())
            {
                m_d2dMultithread->Enter();
            }
        }

        ~D2DResourceLock()
        {
            if (m_d2dMultithread->GetMultithreadProtected())
            {
                m_d2dMultithread->Leave();
            }
        }
    };

} } } }
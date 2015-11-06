// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{

    class CanvasLock : public RuntimeClass<ICanvasLock, IClosable>
                     , private LifespanTracker<CanvasLock>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasLock, BaseTrust);

        ComPtr<ID2D1Multithread> m_multithread;

    public:
        CanvasLock(ID2D1Multithread* multithread)
            : m_multithread(multithread)
        {
            m_multithread->Enter();
        }

        virtual ~CanvasLock()
        {
            (void)Close();
        }

        IFACEMETHODIMP Close() override
        {
            if (m_multithread)
            {
                m_multithread->Leave();
                m_multithread.Reset();
            }

            return S_OK;
        }
    };
    
}}}}

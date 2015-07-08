// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    class CanvasActiveLayer : public RuntimeClass<ICanvasActiveLayer, IClosable>,
                              private LifespanTracker<CanvasActiveLayer>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasActiveLayer, BaseTrust);

        std::function<void()> m_closeAction;

    public:
        CanvasActiveLayer(std::function<void()>&& closeAction)
            : m_closeAction(std::move(closeAction))
        {
        }

        IFACEMETHODIMP Close()
        {
            return ExceptionBoundary(
                [&]
                {
                    if (m_closeAction)
                    {
                        m_closeAction();
                        m_closeAction = nullptr;
                    }
                });
        }
    };
}}}}

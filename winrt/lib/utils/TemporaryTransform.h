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
    // Helper object whose job is to apply a temporary transform to a
    // D2D device context, then undo it at the end of a drawing operation.
    class TemporaryTransform
    {
        // Make ourselves non-copyable.
        TemporaryTransform(TemporaryTransform const&) = delete;
        TemporaryTransform& operator=(TemporaryTransform const&) = delete;

        // Capture by reference because we should only exist on the stack within a single
        // CanvasDrawingSession method, so there's no need to be messing with AddRef.
        ComPtr<ID2D1DeviceContext1> const& m_deviceContext;

        D2D1::Matrix3x2F m_previousTransform;

    public:
        TemporaryTransform(ComPtr<ID2D1DeviceContext1> const& deviceContext, Vector2 const& offset)
            : m_deviceContext(deviceContext)
        {
            auto translation = D2D1::Matrix3x2F::Translation(offset.X, offset.Y);

            m_deviceContext->GetTransform(&m_previousTransform);
            m_deviceContext->SetTransform(translation * m_previousTransform);
        }

        ~TemporaryTransform()
        {
            m_deviceContext->SetTransform(&m_previousTransform);
        }
    };
}}}}

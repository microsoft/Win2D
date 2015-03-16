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
    // Helper object whose job is to apply a temporary transform to a D2D
    // device context or brush, then undo it at the end of a drawing operation.
    template<typename T>
    class TemporaryTransform
    {
        // Make ourselves non-copyable.
        TemporaryTransform(TemporaryTransform const&) = delete;
        TemporaryTransform& operator=(TemporaryTransform const&) = delete;

        // Not a ComPtr because we should only exist on the stack within a single
        // CanvasDrawingSession method, so there's no need to be messing with AddRef.
        T* m_target;

        D2D1::Matrix3x2F m_previousTransform;

    public:
        TemporaryTransform(T* target, Vector2 const& offset, bool postMultiply = false)
            : m_target(target)
        {
            if (m_target)
            {
                auto translation = D2D1::Matrix3x2F::Translation(offset.X, offset.Y);

                m_target->GetTransform(&m_previousTransform);

                m_target->SetTransform(postMultiply ? m_previousTransform * translation
                                                    : translation * m_previousTransform);
            }
        }

        ~TemporaryTransform()
        {
            if (m_target)
            {
                m_target->SetTransform(&m_previousTransform);
            }
        }
    };
}}}}

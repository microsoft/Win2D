// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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
                Apply(D2D1::Matrix3x2F::Translation(offset.X, offset.Y), postMultiply);
            }
        }

        TemporaryTransform(T* target, Vector2 const& offset, Vector2 const& scale, bool postMultiply = false)
            : m_target(target)
        {
            if (m_target)
            {
                auto translationMatrix = D2D1::Matrix3x2F::Translation(offset.X, offset.Y);
                auto scaleMatrix = D2D1::Matrix3x2F::Scale(scale.X, scale.Y);
                Apply(scaleMatrix * translationMatrix, postMultiply);
            }
        }

        ~TemporaryTransform()
        {
            if (m_target)
            {
                m_target->SetTransform(&m_previousTransform);
            }
        }

    private:
        void Apply(D2D1::Matrix3x2F const& transform, bool postMultiply)
        {
            assert(m_target);

            m_target->GetTransform(&m_previousTransform);
            
            m_target->SetTransform(postMultiply ? m_previousTransform * transform
                                                : transform * m_previousTransform);
        }
    };
}}}}

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    inline int ClampedOffset(int value, int offset)
    {
        auto sum = static_cast<int64_t>(value) + offset;

        auto clamped = std::min<int64_t>(std::max<int64_t>(INT_MIN, sum), INT_MAX);

        return static_cast<int>(clamped);
    }


    inline D2D1_RECT_L ExpandRectangle(D2D1_RECT_L const& rect, int offset)
    {
        return D2D1_RECT_L
        {
            ClampedOffset(rect.left,  -offset),
            ClampedOffset(rect.top,   -offset),
            ClampedOffset(rect.right,  offset),
            ClampedOffset(rect.bottom, offset),
        };
    }


    inline D2D1_RECT_L RectangleUnion(D2D1_RECT_L const& rect1, D2D1_RECT_L const& rect2)
    {
        return D2D1_RECT_L
        {
            std::min(rect1.left,   rect2.left),
            std::min(rect1.top,    rect2.top),
            std::max(rect1.right,  rect2.right),
            std::max(rect1.bottom, rect2.bottom),
        };
    }


    inline Numerics::Matrix3x2 const& Identity3x2()
    {
        static Numerics::Matrix3x2 identity{ 1, 0, 0, 1, 0, 0 };
        return identity;
    }

}}}}

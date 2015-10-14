// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"


TEST_CLASS(MathUtilitiesTests)
{
    TEST_METHOD_EX(ExpandRectangle_PositiveOffset)
    {
        D2D1_RECT_L rect{ 100, 200, 300, 400 };

        auto result = ExpandRectangle(rect, 23);

        Assert::AreEqual(D2D1_RECT_L{ 77, 177, 323, 423 }, result);
    }


    TEST_METHOD_EX(ExpandRectangle_NegativeOffset)
    {
        D2D1_RECT_L rect{ 100, 200, 300, 400 };

        auto result = ExpandRectangle(rect, -23);

        Assert::AreEqual(D2D1_RECT_L{ 123, 223, 277, 377 }, result);
    }


    TEST_METHOD_EX(ExpandRectangle_Overflow)
    {
        D2D1_RECT_L rect{ INT_MIN, INT_MIN, INT_MAX, INT_MAX };

        auto result1 = ExpandRectangle(rect, 1);
        auto result2 = ExpandRectangle(rect, INT_MAX);

        Assert::AreEqual(rect, result1);
        Assert::AreEqual(rect, result2);
    }


    TEST_METHOD_EX(RectangleUnion_Overlapping)
    {
        D2D1_RECT_L rect1{ 10, 20, 30, 40 };
        D2D1_RECT_L rect2{ 17, 5, 51, 31 };

        D2D1_RECT_L expected{ 10, 5, 51, 40 };

        auto result1 = RectangleUnion(rect1, rect2);
        auto result2 = RectangleUnion(rect2, rect1);

        Assert::AreEqual(expected, result1);
        Assert::AreEqual(expected, result2);
    }


    TEST_METHOD_EX(RectangleUnion_Disjoint)
    {
        D2D1_RECT_L rect1{ 1, 2, 3, 4 };
        D2D1_RECT_L rect2{ 5, 6, 7, 8 };

        D2D1_RECT_L expected{ 1, 2, 7, 8 };

        auto result1 = RectangleUnion(rect1, rect2);
        auto result2 = RectangleUnion(rect2, rect1);

        Assert::AreEqual(expected, result1);
        Assert::AreEqual(expected, result2);
    }
};

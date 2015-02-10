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

#include "pch.h"
#include "MockD2DRectangleGeometry.h"
#include "MockD2DEllipseGeometry.h"
#include "MockD2DRoundedRectangleGeometry.h"

TEST_CLASS(CanvasGeometryTests)
{
public: 

    struct Fixture
    {
        ComPtr<StubCanvasDevice> Device;
        std::shared_ptr<CanvasGeometryManager> Manager;

        Fixture()
            : Device(Make<StubCanvasDevice>())
            , Manager(std::make_shared<CanvasGeometryManager>())
        {
            Device->CreateRectangleGeometryMethod.AllowAnyCall(
                [](D2D1_RECT_F const&)
                {
                    return Make<MockD2DRectangleGeometry>();
                });

        }
    };

    TEST_METHOD_EX(CanvasGeometry_ImplementsExpectedInterfaces)
    {
        Fixture f;

        auto canvasGeometry = f.Manager->Create(f.Device.Get(), Rect{});

        ASSERT_IMPLEMENTS_INTERFACE(canvasGeometry, ICanvasGeometry);
        ASSERT_IMPLEMENTS_INTERFACE(canvasGeometry, ABI::Windows::Foundation::IClosable);
    }

    TEST_METHOD_EX(CanvasGeometry_CreatedWithCorrectD2DRectangleResource)
    {
        Fixture f;

        Rect expectedRect{ 1, 2, 3, 4 };
        const D2D1_RECT_F expectedD2DRect = 
            D2D1::RectF(expectedRect.X, expectedRect.Y, expectedRect.X + expectedRect.Width, expectedRect.Y + expectedRect.Height);
        
        f.Device->CreateRectangleGeometryMethod.SetExpectedCalls(1, 
            [expectedD2DRect](D2D1_RECT_F const& rect)
            {
                Assert::AreEqual(expectedD2DRect, rect);
                return Make<MockD2DRectangleGeometry>();
            });

        f.Manager->Create(f.Device.Get(), expectedRect);
    }

    TEST_METHOD_EX(CanvasGeometry_CreatedWithCorrectD2DEllipseResource)
    {
        Fixture f;

        D2D1_ELLIPSE testEllipse = D2D1::Ellipse(D2D1::Point2F(1, 2), 3, 4);

        f.Device->CreateEllipseGeometryMethod.SetExpectedCalls(1,
            [testEllipse](D2D1_ELLIPSE const& ellipse)
            {
                Assert::AreEqual(testEllipse, ellipse);
                return Make<MockD2DEllipseGeometry>();
            });
            
        f.Manager->Create(f.Device.Get(), Vector2{ testEllipse.point.x, testEllipse.point.y }, testEllipse.radiusX, testEllipse.radiusY);
    }

    TEST_METHOD_EX(CanvasGeometry_CreatedWithCorrectD2DRoundedRectangleResource)
    {
        Fixture f;

        Rect expectedRect{ 1, 2, 3, 4 };
        const D2D1_RECT_F expectedD2DRect =
            D2D1::RectF(expectedRect.X, expectedRect.Y, expectedRect.X + expectedRect.Width, expectedRect.Y + expectedRect.Height);
        float expectedRadiusX = 5;
        float expectedRadiusY = 6;

        f.Device->CreateRoundedRectangleGeometryMethod.SetExpectedCalls(1,
            [expectedD2DRect, expectedRadiusX, expectedRadiusY](D2D1_ROUNDED_RECT const& roundedRect)
        {
            Assert::AreEqual(D2D1::RoundedRect(expectedD2DRect, expectedRadiusX, expectedRadiusY), roundedRect);
            return Make<MockD2DRoundedRectangleGeometry>();
        });

        f.Manager->Create(f.Device.Get(), expectedRect, expectedRadiusX, expectedRadiusY);
    }

};

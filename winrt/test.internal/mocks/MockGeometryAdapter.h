// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class MockGeometryAdapter : public GeometryAdapter
    {
    public:        
        CALL_COUNTER_WITH_MOCK(CreateRectangleGeometryMethod, ComPtr<ID2D1RectangleGeometry>(D2D1_RECT_F const&));
        CALL_COUNTER_WITH_MOCK(CreateEllipseGeometryMethod, ComPtr<ID2D1EllipseGeometry>(D2D1_ELLIPSE const&));
        CALL_COUNTER_WITH_MOCK(CreateRoundedRectangleGeometryMethod, ComPtr<ID2D1RoundedRectangleGeometry>(D2D1_ROUNDED_RECT const&));
        CALL_COUNTER_WITH_MOCK(CreatePathGeometryMethod, ComPtr<ID2D1PathGeometry1>());
        CALL_COUNTER_WITH_MOCK(CreateGeometryGroupMethod, ComPtr<ID2D1GeometryGroup>(D2D1_FILL_MODE, ID2D1Geometry**, UINT32));
        CALL_COUNTER_WITH_MOCK(CreateTransformedGeometryMethod, ComPtr<ID2D1TransformedGeometry>(ID2D1Geometry*, D2D1_MATRIX_3X2_F*));

        virtual ComPtr<ID2D1RectangleGeometry> CreateRectangleGeometry(GeometryDevicePtr const&, D2D1_RECT_F const& rect) override
        {
            return CreateRectangleGeometryMethod.WasCalled(rect);
        }

        virtual ComPtr<ID2D1EllipseGeometry> CreateEllipseGeometry(GeometryDevicePtr const&, D2D1_ELLIPSE const& ellipse) override
        {
            return CreateEllipseGeometryMethod.WasCalled(ellipse);
        }

        virtual ComPtr<ID2D1RoundedRectangleGeometry> CreateRoundedRectangleGeometry(GeometryDevicePtr const&, D2D1_ROUNDED_RECT const& roundedRect) override
        {
            return CreateRoundedRectangleGeometryMethod.WasCalled(roundedRect);
        }

        virtual ComPtr<ID2D1PathGeometry1> CreatePathGeometry(GeometryDevicePtr const&) override
        {
            return CreatePathGeometryMethod.WasCalled();
        }

        virtual ComPtr<ID2D1GeometryGroup> CreateGeometryGroup(GeometryDevicePtr const&, D2D1_FILL_MODE fillMode, ID2D1Geometry** d2dGeometries, uint32_t geometryCount) override
        {
            return CreateGeometryGroupMethod.WasCalled(fillMode, d2dGeometries, geometryCount);
        }

        virtual ComPtr<ID2D1TransformedGeometry> CreateTransformedGeometry(GeometryDevicePtr const&, ID2D1Geometry* d2dGeometry, D2D1_MATRIX_3X2_F* transform) override
        {
            return CreateTransformedGeometryMethod.WasCalled(d2dGeometry, transform);
        }
    };
}


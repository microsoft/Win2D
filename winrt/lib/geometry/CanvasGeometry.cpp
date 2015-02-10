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
#include "CanvasGeometry.h"
#include "CanvasDevice.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{

    IFACEMETHODIMP CanvasGeometryFactory::CreateRectangle(
        ICanvasResourceCreator* resourceCreator,
        Rect rect,
        ICanvasGeometry** geometry)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(geometry);

                auto newCanvasGeometry = GetManager()->Create(resourceCreator, rect);

                ThrowIfFailed(newCanvasGeometry.CopyTo(geometry));
            });
    }

    IFACEMETHODIMP CanvasGeometryFactory::CreateRectangleAtCoords(
        ICanvasResourceCreator* resourceCreator,
        float x,
        float y,
        float w,
        float h,
        ICanvasGeometry** geometry)
    {
        return CreateRectangle(resourceCreator, Rect{ x, y, w, h }, geometry);
    }

    IFACEMETHODIMP CanvasGeometryFactory::CreateRoundedRectangle(
        ICanvasResourceCreator* resourceCreator,
        Rect rect,
        float xRadius,
        float yRadius,
        ICanvasGeometry** geometry)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(geometry);

                auto newCanvasGeometry = GetManager()->Create(resourceCreator, rect, xRadius, yRadius);

                ThrowIfFailed(newCanvasGeometry.CopyTo(geometry));
            });
    }

    IFACEMETHODIMP CanvasGeometryFactory::CreateRoundedRectangleAtCoords(
        ICanvasResourceCreator* resourceCreator,
        float x,
        float y,
        float w,
        float h,
        float xRadius,
        float yRadius,
        ICanvasGeometry** geometry)
    {
        return CreateRoundedRectangle(resourceCreator, Rect{ x, y, w, h }, xRadius, yRadius, geometry);
    }

    IFACEMETHODIMP CanvasGeometryFactory::CreateEllipse(
        ICanvasResourceCreator* resourceCreator,
        Numerics::Vector2 center,
        float xRadius,
        float yRadius,
        ICanvasGeometry** geometry)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(geometry);

                auto newCanvasGeometry = GetManager()->Create(resourceCreator, center, xRadius, yRadius);

                ThrowIfFailed(newCanvasGeometry.CopyTo(geometry));
            });
    }

    IFACEMETHODIMP CanvasGeometryFactory::CreateEllipseAtCoords(
        ICanvasResourceCreator* resourceCreator,
        float centerX,
        float centerY,
        float xRadius,
        float yRadius,
        ICanvasGeometry** geometry)
    {
        return CreateEllipse(resourceCreator, Vector2{ centerX, centerY }, xRadius, yRadius, geometry);
    }

    IFACEMETHODIMP CanvasGeometryFactory::CreateCircle(
        ICanvasResourceCreator* resourceCreator,
        Numerics::Vector2 center,
        float radius,
        ICanvasGeometry** geometry)
    {
        return CreateEllipse(resourceCreator, center, radius, radius, geometry);
    }

    IFACEMETHODIMP CanvasGeometryFactory::CreateCircleAtCoords(
        ICanvasResourceCreator* resourceCreator,
        float centerX,
        float centerY,
        float radius,
        ICanvasGeometry** geometry)
    {
        return CreateEllipse(resourceCreator, Vector2{ centerX, centerY }, radius, radius, geometry);
    }

    CanvasGeometry::CanvasGeometry(
        std::shared_ptr<CanvasGeometryManager> manager,
        ID2D1Geometry* d2dGeometry,
        ICanvasDevice* canvasDevice)
        : ResourceWrapper(manager, d2dGeometry)
        , m_canvasDevice(canvasDevice)
    {
    }

    IFACEMETHODIMP CanvasGeometry::Close()
    {
        m_canvasDevice.Reset();
        return ResourceWrapper::Close();
    }

    IFACEMETHODIMP CanvasGeometry::get_Device(ICanvasDevice** device)
    {
        return m_canvasDevice.CopyTo(device);
    }

    ComPtr<CanvasGeometry> CanvasGeometryManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        Rect rect)
    {
        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(resourceCreator->get_Device(&device));

        auto deviceInternal = As<ICanvasDeviceInternal>(device);

        auto d2dGeometry = deviceInternal->CreateRectangleGeometry(ToD2DRect(rect));

        auto canvasGeometry = Make<CanvasGeometry>(
            shared_from_this(),
            d2dGeometry.Get(),
            device.Get());
        CheckMakeResult(canvasGeometry);

        return canvasGeometry;
    }

    ComPtr<CanvasGeometry> CanvasGeometryManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        Vector2 center,
        float radiusX,
        float radiusY)
    {
        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(resourceCreator->get_Device(&device));

        auto deviceInternal = As<ICanvasDeviceInternal>(device);

        auto d2dGeometry = deviceInternal->CreateEllipseGeometry(D2D1::Ellipse(ToD2DPoint(center), radiusX, radiusY));

        auto canvasGeometry = Make<CanvasGeometry>(
            shared_from_this(),
            d2dGeometry.Get(),
            device.Get());
        CheckMakeResult(canvasGeometry);

        return canvasGeometry;
    }

    ComPtr<CanvasGeometry> CanvasGeometryManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        Rect rect,
        float radiusX,
        float radiusY)
    {
        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(resourceCreator->get_Device(&device));

        auto deviceInternal = As<ICanvasDeviceInternal>(device);

        auto d2dGeometry = deviceInternal->CreateRoundedRectangleGeometry(D2D1::RoundedRect(ToD2DRect(rect), radiusX, radiusY));

        auto canvasGeometry = Make<CanvasGeometry>(
            shared_from_this(),
            d2dGeometry.Get(),
            device.Get());
        CheckMakeResult(canvasGeometry);

        return canvasGeometry;
    }

    ComPtr<CanvasGeometry> CanvasGeometryManager::CreateWrapper(
        ICanvasDevice* device,
        ID2D1Geometry* geometry)
    {
        auto canvasGeometry = Make<CanvasGeometry>(
            shared_from_this(),
            geometry,
            device);
        CheckMakeResult(canvasGeometry);

        return canvasGeometry;
    }

    ActivatableClassWithFactory(CanvasGeometry, CanvasGeometryFactory);

}}}}

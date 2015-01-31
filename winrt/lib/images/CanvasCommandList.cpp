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

#include "CanvasCommandList.h"
#include "CanvasDevice.h"
#include "CanvasDrawingSession.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    //
    // CanvasCommandListFactory
    //


    IFACEMETHODIMP CanvasCommandListFactory::Create(
        ICanvasResourceCreator* resourceCreator,
        ICanvasCommandList** commandList)
    {
        return ExceptionBoundary(
            [&]
            {
                auto cl = GetManager()->Create(resourceCreator);
                ThrowIfFailed(cl.CopyTo(commandList));
            });
    }


    IFACEMETHODIMP CanvasCommandListFactory::GetOrCreate(
        ICanvasDevice* device,
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(device);
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                auto cl = GetManager()->GetOrCreate(device, As<ID2D1CommandList>(resource).Get());
                ThrowIfFailed(cl.CopyTo(wrapper));
            });
    }


    //
    // CanvasCommandListManager
    //


    ComPtr<CanvasCommandList> CanvasCommandListManager::CreateNew(
        ICanvasResourceCreator* resourceCreator)
    {
        ComPtr<ICanvasDevice> device;
        ThrowIfFailed(resourceCreator->get_Device(&device));
        
        auto d2dCommandList = As<ICanvasDeviceInternal>(device)->CreateCommandList();

        auto cl = Make<CanvasCommandList>(shared_from_this(), device.Get(), d2dCommandList.Get());
        CheckMakeResult(cl);
        return cl;
    }


    ComPtr<CanvasCommandList> CanvasCommandListManager::CreateWrapper(
        ICanvasDevice* device,
        ID2D1CommandList* resource)
    {
        auto cl = Make<CanvasCommandList>(shared_from_this(), device, resource);
        CheckMakeResult(cl);
        return cl;
    }


    //
    // CanvasCommandList
    //


    CanvasCommandList::CanvasCommandList(
        std::shared_ptr<CanvasCommandListManager> manager, 
        ICanvasDevice* device,
        ID2D1CommandList* d2dCommandList)
        : ResourceWrapper(manager, d2dCommandList)
        , m_device(device)
        , m_d2dCommandListIsClosed(false)
    {
    }


    IFACEMETHODIMP CanvasCommandList::CreateDrawingSession(
        ICanvasDrawingSession** drawingSession)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(drawingSession);

                if (m_d2dCommandListIsClosed)
                    ThrowHR(E_INVALIDARG, HStringReference(Strings::CommandListCannotBeDrawnToAfterItHasBeenUsed).Get());

                auto& d2dCommandList = GetResource();
                auto& device = m_device.EnsureNotClosed();

                auto deviceContext = As<ICanvasDeviceInternal>(device)->CreateDeviceContext();
                deviceContext->SetTarget(d2dCommandList.Get());

                auto drawingSessionManager = CanvasDrawingSessionFactory::GetOrCreateManager();
                auto adapter = std::make_shared<SimpleCanvasDrawingSessionAdapter>(deviceContext.Get());

                auto ds = drawingSessionManager->Create(device.Get(), deviceContext.Get(), adapter);

                ThrowIfFailed(ds.CopyTo(drawingSession));
            });
    }


    IFACEMETHODIMP CanvasCommandList::get_Device(ICanvasDevice** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);
                
                auto& device = m_device.EnsureNotClosed();
                ThrowIfFailed(device.CopyTo(value));
            });
    }


    IFACEMETHODIMP CanvasCommandList::Close()
    {
        m_device.Close();
        return __super::Close();
    }


    IFACEMETHODIMP CanvasCommandList::GetBounds(
        ICanvasDrawingSession* drawingSession,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, drawingSession, nullptr, bounds);
    }


    IFACEMETHODIMP CanvasCommandList::GetBoundsWithTransform(
        ICanvasDrawingSession* drawingSession,
        Numerics::Matrix3x2 transform,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, drawingSession, &transform, bounds);
    }


    ComPtr<ID2D1Image> CanvasCommandList::GetD2DImage(
        ID2D1DeviceContext*)
    {
        auto& commandList = GetResource();
        if (!m_d2dCommandListIsClosed)
        {
            HRESULT hr = commandList->Close();

            // D2DERR_WRONG_STATE means that this CL was already closed.  This
            // might happen if we were interopping with an existing
            // D2D1CommandList.  We ignore this error.
            if (hr != D2DERR_WRONG_STATE && FAILED(hr))
                ThrowHR(hr);

            m_d2dCommandListIsClosed = true;
        }
        return commandList;
    }


    ICanvasImageInternal::RealizedEffectNode CanvasCommandList::GetRealizedEffectNode(
        ID2D1DeviceContext* deviceContext,
        float targetDpi)
    {
        UNREFERENCED_PARAMETER(targetDpi);

        return RealizedEffectNode{ GetD2DImage(deviceContext), 0, 0 };
    }

    ActivatableClassWithFactory(CanvasCommandList, CanvasCommandListFactory);

}}}}

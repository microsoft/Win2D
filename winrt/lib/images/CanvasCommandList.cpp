// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "CanvasCommandList.h"

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
                CheckInPointer(resourceCreator);
                
                ComPtr<ICanvasDevice> device;
                ThrowIfFailed(resourceCreator->get_Device(&device));
                
                auto cl = CanvasCommandList::CreateNew(device.Get());
                ThrowIfFailed(cl.CopyTo(commandList));
            });
    }


    //
    // CanvasCommandList
    //


    ComPtr<CanvasCommandList> CanvasCommandList::CreateNew(
        ICanvasDevice* device)
    {
        auto d2dCommandList = As<ICanvasDeviceInternal>(device)->CreateCommandList();

        auto cl = Make<CanvasCommandList>(device, d2dCommandList.Get(), false);
        CheckMakeResult(cl);
        return cl;
    }


    CanvasCommandList::CanvasCommandList(
        ICanvasDevice* device,
        ID2D1CommandList* d2dCommandList,
        bool hasInteropBeenUsed)
        : ResourceWrapper(d2dCommandList)
        , m_device(device)
        , m_d2dCommandListIsClosed(false)
        , m_hasInteropBeenUsed(hasInteropBeenUsed)
        , m_hasActiveDrawingSession(std::make_shared<bool>())
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
                    ThrowHR(E_INVALIDARG, Strings::CommandListCannotBeDrawnToAfterItHasBeenUsed);

                if (*m_hasActiveDrawingSession)
                    ThrowHR(E_FAIL, Strings::CannotCreateDrawingSessionUntilPreviousOneClosed);

                auto& d2dCommandList = GetResource();
                auto& device = m_device.EnsureNotClosed();

                auto deviceContext = As<ICanvasDeviceInternal>(device)->CreateDeviceContextForDrawingSession();
                deviceContext->SetTarget(d2dCommandList.Get());

                auto adapter = std::make_shared<SimpleCanvasDrawingSessionAdapter>(deviceContext.Get());

                auto ds = CanvasDrawingSession::CreateNew(deviceContext.Get(), adapter, device.Get(), m_hasActiveDrawingSession);

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
        ICanvasResourceCreator* resourceCreator,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, resourceCreator, nullptr, bounds);
    }


    IFACEMETHODIMP CanvasCommandList::GetBoundsWithTransform(
        ICanvasResourceCreator* resourceCreator,
        Numerics::Matrix3x2 transform,
        Rect* bounds)
    {
        return GetImageBoundsImpl(this, resourceCreator, &transform, bounds);
    }

    bool IsD2DCommandListClosed(
        ID2D1CommandList* d2dCommandList,
        ICanvasDevice* device,
        ID2D1DeviceContext* d2dDeviceContext)
    {
        D2D1_RECT_F unusedBounds;
        
        // If the caller did not provide a device context, get one from the CanvasDevice.
        DeviceContextLease lease;

        if (!d2dDeviceContext)
        {
            lease = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();
            d2dDeviceContext = lease.Get();
        }

        return d2dDeviceContext->GetImageLocalBounds(d2dCommandList, &unusedBounds) != D2DERR_WRONG_STATE;
    }

    ComPtr<ID2D1Image> CanvasCommandList::GetD2DImage(
        ICanvasDevice* device,
        ID2D1DeviceContext* deviceContext,
        GetD2DImageFlags,
        float /*targetDpi*/,
        float* realizedDpi)
    {
        auto& commandList = GetResource();

        if (!m_d2dCommandListIsClosed)
        {
            // This command list might have been interopped, and could actually 
            // already be closed but we haven't 'realized' it yet.
            // Unfortunately, the act of calling Close on it again will mess up the 
            // state of any device context we're trying to draw this
            // command list to.
            // Therefore, we do a check before closing it.
            //
            if (!m_hasInteropBeenUsed || !IsD2DCommandListClosed(commandList.Get(), device, deviceContext))
            {
                ThrowIfFailed(commandList->Close());
            }

            m_d2dCommandListIsClosed = true;
        }

        if (realizedDpi)
            *realizedDpi = 0;

        return commandList;
    }

    IFACEMETHODIMP CanvasCommandList::GetNativeResource(ICanvasDevice* device, float dpi, REFIID iid, void** outResource)
    {
        m_hasInteropBeenUsed = true;

        return __super::GetNativeResource(device, dpi, iid, outResource);
    }


    ActivatableClassWithFactory(CanvasCommandList, CanvasCommandListFactory);

}}}}

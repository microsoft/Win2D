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

using ABI::Windows::Graphics::Display::DisplayInformation;

class CanvasControlTestAdapter : public ICanvasControlAdapter
{
    EventSource<IEventHandler<IInspectable*>> m_compositionRenderingEventList;

    EventSource<ITypedEventHandler<DisplayInformation*, IInspectable*>> m_dpiChangedEventList;

public:
    virtual std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>> CreateUserControl(IInspectable* canvasControl) override
    {
        auto control = Make<StubUserControl>();

        ComPtr<IInspectable> inspectableControl;
        ThrowIfFailed(control.As(&inspectableControl));

        return std::pair<ComPtr<IInspectable>, ComPtr<IUserControl>>(inspectableControl, control);
    }

    virtual ComPtr<ICanvasDevice> CreateCanvasDevice() override
    {
        return Make<StubCanvasDevice>();
    }

    virtual EventRegistrationToken AddCompositionRenderingCallback(IEventHandler<IInspectable*>* value) override
    {
        EventRegistrationToken token;
        ThrowIfFailed(m_compositionRenderingEventList.Add(value, &token));
        return token;
    }

    virtual void RemoveCompositionRenderingCallback(EventRegistrationToken token) override
    {
        ThrowIfFailed(m_compositionRenderingEventList.Remove(token));
    }

    void FireCompositionRenderingEvent(IInspectable* sender)
    {
        IInspectable* arg = nullptr;
        ThrowIfFailed(m_compositionRenderingEventList.InvokeAll(sender, arg));
    }

    virtual ComPtr<ICanvasImageSource> CreateCanvasImageSource(ICanvasDevice* device, int width, int height) override
    {
        using canvas::CanvasImageSource;

        auto sisFactory = Make<MockSurfaceImageSourceFactory>();
        sisFactory->MockCreateInstanceWithDimensionsAndOpacity =
            [&](int32_t actualWidth, int32_t actualHeight, bool isOpaque, IInspectable* outer)
        {
            return Make<StubSurfaceImageSource>();
        };

        auto dsFactory = std::make_shared<MockCanvasImageSourceDrawingSessionFactory>();
        dsFactory->MockCreate =
            [&](ISurfaceImageSourceNativeWithD2D* sisNative, const Rect& updateRect, float dpi)
        {
            return Make<MockCanvasDrawingSession>();
        };

        ComPtr<ICanvasResourceCreator> resourceCreator;
        ThrowIfFailed(device->QueryInterface(resourceCreator.GetAddressOf()));

        return Make<CanvasImageSource>(
            resourceCreator.Get(),
            width,
            height,
            CanvasBackground::Transparent,
            sisFactory.Get(),
            dsFactory);
    }

    virtual ComPtr<IImage> CreateImageControl() override
    {
        return Make<StubImageControl>();
    }

    virtual float GetLogicalDpi() override
    {
        return DEFAULT_DPI;
    }

    virtual EventRegistrationToken AddDpiChangedCallback(ITypedEventHandler<DisplayInformation*, IInspectable*>* handler) override
    {
        EventRegistrationToken token;
        ThrowIfFailed(m_dpiChangedEventList.Add(handler, &token));
        return token;
    }

    virtual void FireDpiChangedEvent()
    {
        ThrowIfFailed(m_dpiChangedEventList.InvokeAll(nullptr, nullptr));
    }
};
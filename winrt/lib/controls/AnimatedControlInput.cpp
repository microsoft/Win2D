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
#include "AnimatedControlInput.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::UI::Xaml::Controls;

    AnimatedControlInput::AnimatedControlInput(ComPtr<ISwapChainPanel> const& swapChainPanel)
        : m_swapChainPanel(swapChainPanel)
    {}

    IFACEMETHODIMP AnimatedControlInput::ReleasePointerCapture()
    {
        return ExceptionBoundary(
            [&]
            {
                CheckHasSource();

                ThrowIfFailed(m_source->ReleasePointerCapture());
            });
    }

    IFACEMETHODIMP AnimatedControlInput::SetPointerCapture()
    {
        return ExceptionBoundary(
            [&]
            {
                CheckHasSource();

                ThrowIfFailed(m_source->SetPointerCapture());
            });
    }

    IFACEMETHODIMP AnimatedControlInput::get_HasCapture(boolean* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckHasSource();

                ThrowIfFailed(m_source->get_HasCapture(value));
            });
    }

    IFACEMETHODIMP AnimatedControlInput::get_PointerPosition(Point* value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckHasSource();

                ThrowIfFailed(m_source->get_PointerPosition(value));
            });
    }

    IFACEMETHODIMP AnimatedControlInput::get_PointerCursor(ICoreCursor** value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckHasSource();

                ThrowIfFailed(m_source->get_PointerCursor(value));
            });
    }

    IFACEMETHODIMP AnimatedControlInput::put_PointerCursor(ICoreCursor* value)
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // There is no null check here, because null is a valid argument.
                //

                CheckHasSource();

                ThrowIfFailed(m_source->put_PointerCursor(value));
            });
    }

    IFACEMETHODIMP AnimatedControlInput::add_PointerCaptureLost(
        EventHandlerWithPointerArgs* value,
        EventRegistrationToken* token)
    {
        return m_pointerCaptureLostEventList.Add(value, token);
    }

    IFACEMETHODIMP AnimatedControlInput::remove_PointerCaptureLost(
        EventRegistrationToken token)
    {
        return m_pointerCaptureLostEventList.Remove(token);
    }

    IFACEMETHODIMP AnimatedControlInput::add_PointerEntered(
        EventHandlerWithPointerArgs* value,
        EventRegistrationToken* token)
    {
        return m_pointerEnteredEventList.Add(value, token);
    }

    IFACEMETHODIMP AnimatedControlInput::remove_PointerEntered(
        EventRegistrationToken token)
    {
        return m_pointerEnteredEventList.Remove(token);
    }

    IFACEMETHODIMP AnimatedControlInput::add_PointerExited(
        EventHandlerWithPointerArgs* value,
        EventRegistrationToken* token)
    {
        return m_pointerExitedEventList.Add(value, token);
    }

    IFACEMETHODIMP AnimatedControlInput::remove_PointerExited(
        EventRegistrationToken token)
    {
        return m_pointerExitedEventList.Remove(token);
    }

    IFACEMETHODIMP AnimatedControlInput::add_PointerMoved(
        EventHandlerWithPointerArgs* value,
        EventRegistrationToken* token)
    {
        return m_pointerMovedEventList.Add(value, token);
    }
    IFACEMETHODIMP AnimatedControlInput::remove_PointerMoved(
        EventRegistrationToken token)
    {
        return m_pointerMovedEventList.Remove(token);
    }

    IFACEMETHODIMP AnimatedControlInput::add_PointerPressed(
        EventHandlerWithPointerArgs* value,
        EventRegistrationToken* token)
    {
        return m_pointerPressedEventList.Add(value, token);
    }

    IFACEMETHODIMP AnimatedControlInput::remove_PointerPressed(
        EventRegistrationToken token)
    {
        return m_pointerPressedEventList.Remove(token);
    }

    IFACEMETHODIMP AnimatedControlInput::add_PointerReleased(
        EventHandlerWithPointerArgs* value,
        EventRegistrationToken* token)
    {
        return m_pointerReleasedEventList.Add(value, token);
    }

    IFACEMETHODIMP AnimatedControlInput::remove_PointerReleased(
        EventRegistrationToken token)
    {
        return m_pointerReleasedEventList.Remove(token);
    }

    IFACEMETHODIMP AnimatedControlInput::add_PointerWheelChanged(
        EventHandlerWithPointerArgs* value,
        EventRegistrationToken* token)
    {
        return m_pointerWheelChangedEventList.Add(value, token);
    }

    IFACEMETHODIMP AnimatedControlInput::remove_PointerWheelChanged(
        EventRegistrationToken token)
    {
        return m_pointerWheelChangedEventList.Remove(token);
    }


    HRESULT AnimatedControlInput::OnPointerCaptureLost(IInspectable* sender, IPointerEventArgs* args)
    {
        return m_pointerCaptureLostEventList.InvokeAll(sender, args);
    }

    HRESULT AnimatedControlInput::OnPointerEntered(IInspectable* sender, IPointerEventArgs* args)
    {
        return m_pointerEnteredEventList.InvokeAll(sender, args);
    }

    HRESULT AnimatedControlInput::OnPointerExited(IInspectable* sender, IPointerEventArgs* args)
    {
        return m_pointerExitedEventList.InvokeAll(sender, args);
    }

    HRESULT AnimatedControlInput::OnPointerMoved(IInspectable* sender, IPointerEventArgs* args)
    {
        return m_pointerMovedEventList.InvokeAll(sender, args);
    }

    HRESULT AnimatedControlInput::OnPointerPressed(IInspectable* sender, IPointerEventArgs* args)
    {
        return m_pointerPressedEventList.InvokeAll(sender, args);
    }

    HRESULT AnimatedControlInput::OnPointerReleased(IInspectable* sender, IPointerEventArgs* args)
    {
        return m_pointerReleasedEventList.InvokeAll(sender, args);
    }

    HRESULT AnimatedControlInput::OnPointerWheelChanged(IInspectable* sender, IPointerEventArgs* args)
    {
        return m_pointerWheelChangedEventList.InvokeAll(sender, args);
    }
    void AnimatedControlInput::SetSource()
    {
        assert(!m_source);

        ComPtr<ICoreInputSourceBase> inputSourceBase;

        ThrowIfFailed(m_swapChainPanel->CreateCoreIndependentInputSource(
            CoreInputDeviceTypes_Touch | CoreInputDeviceTypes_Pen | CoreInputDeviceTypes_Mouse, 
            &inputSourceBase));

        m_source = As<ICorePointerInputSource>(inputSourceBase);

#define DECLARE_REGISTERED_EVENT(NAME) \
        m_pointer##NAME##Event = RegisteredEvent(\
            m_source.Get(),\
            &ICorePointerInputSource::add_Pointer##NAME##,\
            &ICorePointerInputSource::remove_Pointer##NAME##,\
            Callback<EventHandlerWithPointerArgs>(this, &AnimatedControlInput::OnPointer##NAME##).Get());

        DECLARE_REGISTERED_EVENT(CaptureLost);
        DECLARE_REGISTERED_EVENT(Entered);
        DECLARE_REGISTERED_EVENT(Exited);
        DECLARE_REGISTERED_EVENT(Moved);
        DECLARE_REGISTERED_EVENT(Pressed);
        DECLARE_REGISTERED_EVENT(Released);
        DECLARE_REGISTERED_EVENT(WheelChanged);

#undef DECLARE_REGISTERED_EVENT
    }

    void AnimatedControlInput::RemoveSource()
    {
        assert(m_source);

        m_source.Reset();
    }

    void AnimatedControlInput::ProcessEvents()
    {
        //
        // This may only be called on the render thread.
        //

        auto baseSource = As<ICoreInputSourceBase>(m_source);

        ComPtr<ICoreDispatcher> dispatcher;
        ThrowIfFailed(baseSource->get_Dispatcher(&dispatcher));

        ThrowIfFailed(dispatcher->ProcessEvents(CoreProcessEventsOption_ProcessAllIfPresent));
    }

    void AnimatedControlInput::CheckHasSource()
    {
        //
        // The design of this object, is to have CanvasAnimatedControl call EnsureSource from its
        // render thread whenever it begins. Then it calls RemoveSource when the render thread
        // exits. m_source indicates whether the render thread is alive.
        //
        // As for the properties which are routed to m_source, they already perform a check to
        // verify they're called on the correct (non-UI) worker thread, and return RPC_E_WRONG_THREAD otherwise. 
        //
        // If m_source is null and this worker thread doesn't exist, it is consistent to also
        // return RPC_E_WRONG_THREAD.
        // 
        if (!m_source)
        {
            ThrowHR(RPC_E_WRONG_THREAD);
        }
    }
}}}}

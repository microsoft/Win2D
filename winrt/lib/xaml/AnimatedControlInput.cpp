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

using namespace ABI::Microsoft::Graphics::Canvas::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;

AnimatedControlInput::AnimatedControlInput(ComPtr<ISwapChainPanel> const& swapChainPanel)
    : m_weakSwapChainPanel(AsWeak(swapChainPanel.Get()))
{
}

IFACEMETHODIMP AnimatedControlInput::ReleasePointerCapture()
{
    // 
    // Capture-related methods are not available for this control.
    //
    return E_NOTIMPL;
}

IFACEMETHODIMP AnimatedControlInput::SetPointerCapture()
{
    return E_NOTIMPL;
}

IFACEMETHODIMP AnimatedControlInput::get_HasCapture(boolean*)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP AnimatedControlInput::get_PointerPosition(Point* value)
{
    return ExceptionBoundary(
        [&]
        {
            Lock lock(m_mutex);

            CheckHasSource();

            ThrowIfFailed(m_source->get_PointerPosition(value));
        });
}

IFACEMETHODIMP AnimatedControlInput::get_PointerCursor(ICoreCursor** value)
{
    return ExceptionBoundary(
        [&]
        {
            Lock lock(m_mutex);

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
            Lock lock(m_mutex);

            CheckHasSource();

            ThrowIfFailed(m_source->put_PointerCursor(value));
        });
}

IFACEMETHODIMP AnimatedControlInput::add_PointerCaptureLost(
    EventHandlerWithPointerArgs*,
    EventRegistrationToken*)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP AnimatedControlInput::remove_PointerCaptureLost(
    EventRegistrationToken)
{
    return E_NOTIMPL;
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
    Lock lock(m_mutex);

    assert(!m_source);

    ComPtr<ICoreInputSourceBase> inputSourceBase;

    auto swapChainPanel = LockWeakRef<ISwapChainPanel>(m_weakSwapChainPanel);
    if (!swapChainPanel)
        return;

    ThrowIfFailed(swapChainPanel->CreateCoreIndependentInputSource(
        CoreInputDeviceTypes_Touch | CoreInputDeviceTypes_Pen | CoreInputDeviceTypes_Mouse, 
        &inputSourceBase));

    m_source = As<ICorePointerInputSource>(inputSourceBase);

#define DECLARE_REGISTERED_EVENT(NAME) \
    m_pointer##NAME##Event = RegisteredEvent(\
        m_source.Get(),\
        &ICorePointerInputSource::add_Pointer##NAME##,\
        &ICorePointerInputSource::remove_Pointer##NAME##,\
        Callback<EventHandlerWithPointerArgs>(this, &AnimatedControlInput::OnPointer##NAME##).Get());

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
    Lock lock(m_mutex);

    assert(m_source);

    m_pointerEnteredEvent.Release();
    m_pointerExitedEvent.Release();
    m_pointerMovedEvent.Release();
    m_pointerPressedEvent.Release();
    m_pointerReleasedEvent.Release();
    m_pointerWheelChangedEvent.Release();

    m_source.Reset();

    //
    // Let the swap chain panel know that we're really done with the input
    // source we just released.  We do this by creating a new input source
    // that's associated with no CoreInputDeviceTypes.  This disassociates
    // the previous input.
    //
    // This is a workaround for MS:2294504.
    //
    auto swapChainPanel = LockWeakRef<ISwapChainPanel>(m_weakSwapChainPanel);
    if (!swapChainPanel)
        return;

    ComPtr<ICoreInputSourceBase> dummyIgnored;
    ThrowIfFailed(swapChainPanel->CreateCoreIndependentInputSource(CoreInputDeviceTypes_None, &dummyIgnored));
}

void AnimatedControlInput::ProcessEvents()
{
    //
    // Because this may only be called on the render thread,
    // and m_source is only ever set up or torn down on the
    // render thread, it is not necessary to take out the lock
    // for this.
    //

    ThrowIfFailed(GetDispatcher()->ProcessEvents(CoreProcessEventsOption_ProcessAllIfPresent));
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

boolean AnimatedControlInput::GetHasThreadAccess()
{
    Lock lock(m_mutex);

    if (!m_source)
    {
        return false;
    }
    else
    {
        auto dispatcher = GetDispatcher();

        boolean hasAccess;
        ThrowIfFailed(dispatcher->get_HasThreadAccess(&hasAccess));
        return hasAccess;
    }
}

ComPtr<ICoreDispatcher> AnimatedControlInput::GetDispatcher()
{
    assert(m_source);

    auto baseSource = As<ICoreInputSourceBase>(m_source);

    ComPtr<ICoreDispatcher> dispatcher;
    ThrowIfFailed(baseSource->get_Dispatcher(&dispatcher));

    return dispatcher;
}

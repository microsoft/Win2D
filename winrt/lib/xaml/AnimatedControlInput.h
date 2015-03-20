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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::UI::Core;
    using namespace ABI::Windows::UI::Xaml::Controls;

    typedef ITypedEventHandler<IInspectable*, PointerEventArgs*> EventHandlerWithPointerArgs;

    class AnimatedControlInput : public RuntimeClass<ICorePointerInputSource>,
                                 private LifespanTracker<AnimatedControlInput>
    {
        InspectableClass(InterfaceName_Windows_UI_Core_ICorePointerInputSource, BaseTrust);

        ComPtr<ICorePointerInputSource> m_source;
        WeakRef m_weakSwapChainPanel;

        EventSource<EventHandlerWithPointerArgs, InvokeModeOptions<StopOnFirstError>> m_pointerCaptureLostEventList;
        EventSource<EventHandlerWithPointerArgs, InvokeModeOptions<StopOnFirstError>> m_pointerEnteredEventList;
        EventSource<EventHandlerWithPointerArgs, InvokeModeOptions<StopOnFirstError>> m_pointerExitedEventList;
        EventSource<EventHandlerWithPointerArgs, InvokeModeOptions<StopOnFirstError>> m_pointerMovedEventList;
        EventSource<EventHandlerWithPointerArgs, InvokeModeOptions<StopOnFirstError>> m_pointerPressedEventList;
        EventSource<EventHandlerWithPointerArgs, InvokeModeOptions<StopOnFirstError>> m_pointerReleasedEventList;
        EventSource<EventHandlerWithPointerArgs, InvokeModeOptions<StopOnFirstError>> m_pointerWheelChangedEventList;

        RegisteredEvent m_pointerCaptureLostEvent;
        RegisteredEvent m_pointerEnteredEvent;
        RegisteredEvent m_pointerExitedEvent;
        RegisteredEvent m_pointerMovedEvent;
        RegisteredEvent m_pointerPressedEvent;
        RegisteredEvent m_pointerReleasedEvent;
        RegisteredEvent m_pointerWheelChangedEvent;

    public:
        AnimatedControlInput(ComPtr<ISwapChainPanel> const& swapChainPanel);

        //
        // ICorePointerInputSource members
        //

        IFACEMETHODIMP ReleasePointerCapture(void) override;

        IFACEMETHODIMP SetPointerCapture(void) override;

        IFACEMETHODIMP get_HasCapture(boolean*) override;

        IFACEMETHODIMP get_PointerPosition(Point*) override;

        IFACEMETHODIMP get_PointerCursor(ICoreCursor**) override;

        IFACEMETHODIMP put_PointerCursor(ICoreCursor*) override;

        IFACEMETHODIMP add_PointerCaptureLost(
            EventHandlerWithPointerArgs*,
            EventRegistrationToken*) override;

        IFACEMETHODIMP remove_PointerCaptureLost(
            EventRegistrationToken) override;

        IFACEMETHODIMP add_PointerEntered(
            EventHandlerWithPointerArgs*,
            EventRegistrationToken*) override;

        IFACEMETHODIMP remove_PointerEntered(
            EventRegistrationToken) override;

        IFACEMETHODIMP add_PointerExited(
            EventHandlerWithPointerArgs*,
            EventRegistrationToken*) override;

        IFACEMETHODIMP remove_PointerExited(
            EventRegistrationToken) override;

        IFACEMETHODIMP add_PointerMoved(
            EventHandlerWithPointerArgs*,
            EventRegistrationToken*) override;

        IFACEMETHODIMP remove_PointerMoved(
            EventRegistrationToken) override;

        IFACEMETHODIMP add_PointerPressed(
            EventHandlerWithPointerArgs*,
            EventRegistrationToken*) override;

        IFACEMETHODIMP remove_PointerPressed(
            EventRegistrationToken) override;

        IFACEMETHODIMP add_PointerReleased(
            EventHandlerWithPointerArgs*,
            EventRegistrationToken*) override;

        IFACEMETHODIMP remove_PointerReleased(
            EventRegistrationToken) override;

        IFACEMETHODIMP add_PointerWheelChanged(
            EventHandlerWithPointerArgs*,
            EventRegistrationToken*) override;

        IFACEMETHODIMP remove_PointerWheelChanged(
            EventRegistrationToken) override;

        //
        // Internal methods
        //

        void SetSource();

        void RemoveSource();

        void ProcessEvents();

    private:

        HRESULT OnPointerCaptureLost(IInspectable*, IPointerEventArgs*);

        HRESULT OnPointerEntered(IInspectable*, IPointerEventArgs*);

        HRESULT OnPointerExited(IInspectable*, IPointerEventArgs*);

        HRESULT OnPointerMoved(IInspectable*, IPointerEventArgs*);

        HRESULT OnPointerPressed(IInspectable*, IPointerEventArgs*);

        HRESULT OnPointerReleased(IInspectable*, IPointerEventArgs*);

        HRESULT OnPointerWheelChanged(IInspectable*, IPointerEventArgs*);

        void CheckHasSource();
    };

}}}}

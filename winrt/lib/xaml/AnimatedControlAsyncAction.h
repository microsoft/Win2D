// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    class AnimatedControlAsyncAction : public RuntimeClass<
        AsyncBase<IAsyncActionCompletedHandler, ::Microsoft::WRL::Details::Nil, SingleResult, AsyncOptions<PropagateDelegateError>>, IAsyncAction>,
        private LifespanTracker<AnimatedControlAsyncAction>
    {
        InspectableClass(InterfaceName_Windows_Foundation_IAsyncAction, BaseTrust);

        ComPtr<IDispatchedHandler> m_dispatchedHandler;

    public:
        AnimatedControlAsyncAction(IDispatchedHandler* callback)
            : m_dispatchedHandler(callback)
        {
            Start();
        }

        struct InvocationResult
        {
            HRESULT ActionResult;
            HRESULT CompletedHandlerResult;
        };
        InvocationResult InvokeAndFireCompletion()
        {
            InvocationResult result{ S_OK, S_OK };

            //
            // Only actually invoke the handler if we're in a non-terminal
            // state (eg we haven't been canceled already).
            //
            if (!IsTerminalState())
                result.ActionResult = m_dispatchedHandler->Invoke();

            //
            // Even though the CanvasControl will handle device lost,
            // the completed handler should still be notified there 
            // was an error and that the async handler wasn't actually
            // called.
            //
            if (FAILED(result.ActionResult))
            {
                TryTransitionToError(result.ActionResult);
            }
            
            //
            // FireCompletion is a method inherited from AsyncBase
            // that transitions the action into a completed state.
            //
            // Errors from it are expected to be handled by the calling
            // CanvasAnimatedControl. 
            //
            // In particular, CanvasAnimatedControl should re-thow 
            // device lost so that the device can be re-created.
            //
            // Non-device-lost errors should result in the cancellation of
            // subsequent async actions.
            //
            result.CompletedHandlerResult = FireCompletion();
            return result;
        }

        void CancelAndFireCompletion()
        {
            (void)Cancel();
            (void)FireCompletion();
        }

        IFACEMETHODIMP put_Completed(IAsyncActionCompletedHandler* value) override
        {
            return PutOnComplete(value);
        }

        IFACEMETHODIMP get_Completed(IAsyncActionCompletedHandler** value) override
        {
            return GetOnComplete(value);
        }

        IFACEMETHODIMP GetResults() override
        {
            return CheckValidStateForResultsCall();
        }

    protected:
        virtual HRESULT OnStart() override
        {
            return S_OK;
        }

        virtual void OnClose() override
        {
        }

        virtual void OnCancel() override
        {
        }
    };
}}}}}}

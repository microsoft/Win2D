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
            InvocationResult result;
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
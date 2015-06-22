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

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::System::Threading;
using namespace ABI::Windows::UI::Core;
using namespace ABI::Windows::UI::Xaml::Controls;
using namespace Microsoft::WRL;

class IGameLoopThread
{
public:
    virtual ~IGameLoopThread() = default;
    virtual void StartDispatcher() = 0;
    virtual void StopDispatcher() = 0;
    virtual ComPtr<IAsyncAction> RunAsync(IDispatchedHandler* handler) = 0;
    virtual bool HasThreadAccess() = 0;
};

class ICanvasGameLoopClient;

std::unique_ptr<IGameLoopThread> CreateGameLoopThread(ISwapChainPanel* swapChainPanel, ICanvasGameLoopClient* client);

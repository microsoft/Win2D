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

namespace canvas
{
    class MockStorageFileStatics : public RuntimeClass<IStorageFileStatics>
    {
    public:
        CALL_COUNTER_WITH_MOCK(GetFileFromApplicationUriAsyncMethod, HRESULT(IUriRuntimeClass*, IAsyncOperation<StorageFile*>**));
        
        IFACEMETHODIMP GetFileFromPathAsync(
            HSTRING, 
            IAsyncOperation<StorageFile*>**) override
        {
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetFileFromApplicationUriAsync(
            IUriRuntimeClass* uri, 
            IAsyncOperation<StorageFile*>** operation) override
        {
            return GetFileFromApplicationUriAsyncMethod.WasCalled(uri, operation);
        }

        IFACEMETHODIMP CreateStreamedFileAsync(
            HSTRING,
            IStreamedFileDataRequestedHandler*,
            IRandomAccessStreamReference*,
            IAsyncOperation<StorageFile*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP ReplaceWithStreamedFileAsync( 
            IStorageFile*,
            IStreamedFileDataRequestedHandler*,
            IRandomAccessStreamReference*,
            IAsyncOperation<StorageFile*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP CreateStreamedFileFromUriAsync( 
            HSTRING,
            IUriRuntimeClass*,
            IRandomAccessStreamReference*,
            IAsyncOperation<StorageFile*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP ReplaceWithStreamedFileFromUriAsync( 
            IStorageFile*,
            IUriRuntimeClass*,
            IRandomAccessStreamReference*,
            IAsyncOperation<StorageFile*>**) override
        {
            return E_NOTIMPL;
        }
    };
}

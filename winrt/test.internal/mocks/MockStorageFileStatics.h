// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

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

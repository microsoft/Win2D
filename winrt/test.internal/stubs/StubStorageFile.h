// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace canvas
{
    class StubStorageFile : public RuntimeClass<IStorageFile, IStorageItem>
    {
        WinString m_path;

    public:
        StubStorageFile(std::wstring const& path)
            : m_path(path)
        {
        }

        //
        // IStorageFile
        //

        IFACEMETHODIMP get_FileType( 
            HSTRING*) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP get_ContentType( 
            HSTRING*) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP OpenAsync( 
            FileAccessMode,
            IAsyncOperation<IRandomAccessStream*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP OpenTransactedWriteAsync( 
            IAsyncOperation<StorageStreamTransaction*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP CopyOverloadDefaultNameAndOptions( 
            IStorageFolder*,
            IAsyncOperation<StorageFile*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP CopyOverloadDefaultOptions( 
            IStorageFolder*,
            HSTRING,
            IAsyncOperation<StorageFile*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP CopyOverload( 
            IStorageFolder*,
            HSTRING,
            NameCollisionOption,
            IAsyncOperation<StorageFile*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP CopyAndReplaceAsync( 
            IStorageFile*,
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP MoveOverloadDefaultNameAndOptions( 
            IStorageFolder*,
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP MoveOverloadDefaultOptions( 
            IStorageFolder*,
            HSTRING,
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP MoveOverload( 
            IStorageFolder*,
            HSTRING,
            NameCollisionOption,
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP MoveAndReplaceAsync( 
            IStorageFile*,
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }


        //
        // IStorageItem
        //


        IFACEMETHODIMP RenameAsyncOverloadDefaultOptions( 
            HSTRING,
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP RenameAsync( 
            HSTRING,
            NameCollisionOption,
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP DeleteAsyncOverloadDefaultOptions( 
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP DeleteAsync( 
            StorageDeleteOption,
            IAsyncAction**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP GetBasicPropertiesAsync( 
            IAsyncOperation<ABI::Windows::Storage::FileProperties::BasicProperties*>**) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP get_Name( 
            HSTRING*) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP get_Path( 
            HSTRING* value) override
        {
            return ExceptionBoundary(
                [=]
                {
                    m_path.CopyTo(value);
                });
        }
                    
        IFACEMETHODIMP get_Attributes( 
            FileAttributes*) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP get_DateCreated( 
            DateTime*) override
        {
            return E_NOTIMPL;
        }
                    
        IFACEMETHODIMP IsOfType( 
            StorageItemTypes,
            boolean*) override
        {
            return E_NOTIMPL;
        }
    };
}

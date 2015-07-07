// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "mocks/MockStorageFileStatics.h"
#include "stubs/StubStorageFile.h"

namespace canvas
{
    //
    // Helper operation used by StubStorageFileStatics below.
    //
    // This was inline in the method, but this caused an ICE.
    //
    class StorageFileOperation : public RuntimeClass<
        AsyncBase<IAsyncOperationCompletedHandler<StorageFile*>>,
        IAsyncOperation<StorageFile* >>
    {
        ComPtr<StubStorageFile> m_storageFile;

    public:
        StorageFileOperation(std::wstring const& path)
            : m_storageFile(Make<StubStorageFile>(path))
        {
            ThrowIfFailed(Start());
        }

        IFACEMETHODIMP GetResults(IStorageFile** value) override
        {
            HRESULT hr = CheckValidStateForResultsCall();
            if (FAILED(hr))
                return hr;

            return m_storageFile.CopyTo(value);
        }

        IFACEMETHODIMP put_Completed(IAsyncOperationCompletedHandler<StorageFile*>* value) override
        {
            return PutOnComplete(value);
        }

        IFACEMETHODIMP get_Completed(IAsyncOperationCompletedHandler<StorageFile*>** value) override
        {
            return GetOnComplete(value);
        }

        virtual HRESULT OnStart() override { return S_OK; }
        virtual void OnClose() override {}
        virtual void OnCancel() override {}
    };

    class StubStorageFileStatics : public MockStorageFileStatics
    {
    public:
        static std::wstring GetFakePath(WinString const& uri)
        {
            std::wstring path(L"pathof(");
            path += static_cast<std::wstring>(uri);
            path += L")";
            return path;
        }

        StubStorageFileStatics()
        {
            GetFileFromApplicationUriAsyncMethod.AllowAnyCall(
                [] (IUriRuntimeClass* uri, IAsyncOperation<StorageFile*>** operation)
                {
                    return ExceptionBoundary(
                        [=]
                        {
                            WinString canonicalUri;
                            ThrowIfFailed(As<IUriRuntimeClassWithAbsoluteCanonicalUri>(uri)->get_AbsoluteCanonicalUri(canonicalUri.GetAddressOf()));

                            auto path = GetFakePath(canonicalUri);

                            auto op = Make<StorageFileOperation>(path);
                            ThrowIfFailed(op->FireCompletion());
                            ThrowIfFailed(op.CopyTo(operation));
                        });
                });                    
        }
    };
}

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "pch.h"

#include "CustomFontManager.h"

using namespace ABI::Microsoft::Graphics::Canvas::Text;


ComPtr<IDWriteFactory> DefaultCustomFontManagerAdapter::CreateDWriteFactory(DWRITE_FACTORY_TYPE type)
{
    ComPtr<IDWriteFactory> factory;
    ThrowIfFailed(DWriteCreateFactory(type, __uuidof(factory), &factory));
    return factory;
}


IStorageFileStatics* DefaultCustomFontManagerAdapter::GetStorageFileStatics()
{
    if (!m_storageFileStatics)
    {
        ThrowIfFailed(GetActivationFactory(
            HStringReference(RuntimeClass_Windows_Storage_StorageFile).Get(),
            &m_storageFileStatics));
    }

    return m_storageFileStatics.Get();
}


class CustomFontFileEnumerator 
    : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDWriteFontFileEnumerator>
    , private LifespanTracker<CustomFontFileEnumerator>
{
    ComPtr<IDWriteFactory> m_factory;
    std::wstring m_filename;
    ComPtr<IDWriteFontFile> m_theFile;

public:
    CustomFontFileEnumerator(IDWriteFactory* factory, void const* collectionKey, uint32_t collectionKeySize)
        : m_factory(factory)
        , m_filename(static_cast<wchar_t const*>(collectionKey), collectionKeySize / 2)
    {
    }

    IFACEMETHODIMP MoveNext(BOOL* hasCurrentFile) override
    {
        if (m_theFile)
        {
            *hasCurrentFile = FALSE;
        }
        else if (SUCCEEDED(m_factory->CreateFontFileReference(m_filename.c_str(), nullptr, &m_theFile)))
        {
            *hasCurrentFile = TRUE;
        }
        else
        {
            *hasCurrentFile = FALSE;
        }

        return S_OK;
    }

    IFACEMETHODIMP GetCurrentFontFile(IDWriteFontFile** fontFile) override
    {
        return m_theFile.CopyTo(fontFile);
    }
};


class CustomFontLoader
    : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDWriteFontCollectionLoader>
    , private LifespanTracker<CustomFontLoader>
{
public:
    IFACEMETHODIMP CreateEnumeratorFromKey(
        IDWriteFactory* factory,
        void const* collectionKey,
        uint32_t collectionKeySize,
        IDWriteFontFileEnumerator** fontFileEnumerator) override
    {
        return ExceptionBoundary(
            [=]
        {
            auto enumerator = Make<CustomFontFileEnumerator>(factory, collectionKey, collectionKeySize);
            CheckMakeResult(enumerator);
            ThrowIfFailed(enumerator.CopyTo(fontFileEnumerator));
        });
    }
};


CustomFontManager::CustomFontManager()
    : m_adapter(CustomFontManagerAdapter::GetInstance())
{
    ThrowIfFailed(GetActivationFactory(
        HStringReference(RuntimeClass_Windows_Foundation_Uri).Get(),
        &m_uriFactory));
}

WinString CustomFontManager::GetAbsolutePathFromUri(WinString const& uriString)
{
    ComPtr<IUriRuntimeClass> uri;
    ThrowIfFailed(m_uriFactory->CreateWithRelativeUri(WinString(L"ms-appx://"), uriString, &uri));

    auto storageFileStatics = m_adapter->GetStorageFileStatics();
    ComPtr<IAsyncOperation<StorageFile*>> operation;

    HRESULT hr = storageFileStatics->GetFileFromApplicationUriAsync(uri.Get(), &operation);
    if (FAILED(hr))
    {
        ThrowHR(hr, HStringReference(Strings::InvalidFontFamilyUri).Get());
    }

    Event operationCompleted(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
    auto handler = Callback<AddFtmBase<IAsyncOperationCompletedHandler<StorageFile*>>::Type>(
        [&](IAsyncOperation<StorageFile*>*, AsyncStatus)
        {
            SetEvent(operationCompleted.Get());
            return S_OK;
        });
    CheckMakeResult(handler);

    ThrowIfFailed(operation->put_Completed(handler.Get()));

    auto res = WaitForSingleObjectEx(operationCompleted.Get(), INFINITE, false);
    if (res != WAIT_OBJECT_0)
        ThrowHR(E_UNEXPECTED);

    ComPtr<IStorageFile> storageFile;
    ThrowIfFailed(operation->GetResults(&storageFile));

    WinString path;
    ThrowIfFailed(As<IStorageItem>(storageFile)->get_Path(path.GetAddressOf()));

    return path;
}

ComPtr<IDWriteFontCollection> CustomFontManager::GetFontCollectionFromUri(WinString const& uri)
{
    //
    // No URI means no custom font collection - ie use the system font
    // collection.
    //
    if (uri == WinString())
    {
        return nullptr;
    }

    auto path = GetAbsolutePathFromUri(uri);

    auto pathBegin = begin(path);
    auto pathEnd = end(path);

    assert(pathBegin && pathEnd);

    void const* key = pathBegin;
    uint32_t keySize = static_cast<uint32_t>(std::distance(pathBegin, pathEnd) * sizeof(wchar_t));

    ComPtr<IDWriteFontCollection> collection;

    auto factory = GetIsolatedFactory();
    ThrowIfFailed(factory->CreateCustomFontCollection(m_customLoader.Get(), key, keySize, &collection));

    return collection;
}

ComPtr<IDWriteFactory> const& CustomFontManager::GetIsolatedFactory()
{
    if (!m_isolatedFactory)
    {
        m_isolatedFactory = m_adapter->CreateDWriteFactory(DWRITE_FACTORY_TYPE_ISOLATED);
        m_customLoader = Make<CustomFontLoader>();
        ThrowIfFailed(m_isolatedFactory->RegisterFontCollectionLoader(m_customLoader.Get()));
    }

    return m_isolatedFactory;
}

ComPtr<IDWriteFactory> const& CustomFontManager::GetSharedFactory()
{
    if (!m_sharedFactory)
    {
        m_sharedFactory = m_adapter->CreateDWriteFactory(DWRITE_FACTORY_TYPE_SHARED);
    }

    return m_sharedFactory;
}

void CustomFontManager::ValidateUri(WinString const& uriString)
{
    if (uriString == WinString())
        return;

    ComPtr<IUriRuntimeClass> uri;
    ThrowIfFailed(m_uriFactory->CreateWithRelativeUri(WinString(L"ms-appx://"), uriString, &uri));

    WinString schemeName;
    ThrowIfFailed(uri->get_SchemeName(schemeName.GetAddressOf()));

    if (!schemeName.Equals(HStringReference(L"ms-appx").Get()) &&
        !schemeName.Equals(HStringReference(L"ms-appdata").Get()))
    {
        ThrowHR(E_INVALIDARG, HStringReference(Strings::InvalidFontFamilyUriScheme).Get());
    }
}

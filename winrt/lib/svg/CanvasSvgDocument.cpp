// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "CanvasSvgDocument.h"

using namespace ABI::Microsoft::Graphics::Canvas::Svg;
using namespace Microsoft::WRL::Wrappers;

ActivatableStaticOnlyFactory(CanvasSvgDocumentStatics);

// static
ComPtr<CanvasSvgDocument> CanvasSvgDocument::CreateNew(ICanvasResourceCreator* resourceCreator, IStream* stream)
{
    ComPtr<ICanvasDevice> device;
    ThrowIfFailed(resourceCreator->get_Device(&device));

    auto deviceInternal = As<ICanvasDeviceInternal>(device);

    ComPtr<ID2D1SvgDocument> d2dSvgDocument = deviceInternal->CreateSvgDocument(stream);

    auto canvasSvgDocument = Make<CanvasSvgDocument>(
        device.Get(),
        d2dSvgDocument.Get());
    CheckMakeResult(canvasSvgDocument);

    return canvasSvgDocument;
}

CanvasSvgDocument::CanvasSvgDocument(
    ICanvasDevice* canvasDevice,
    ID2D1SvgDocument* d2dSvgDocument)
    : ResourceWrapper(d2dSvgDocument)
    , m_canvasDevice(canvasDevice)
{
}

IFACEMETHODIMP CanvasSvgDocument::Close()
{
    m_canvasDevice.Close();
    return ResourceWrapper::Close();
}

IFACEMETHODIMP CanvasSvgDocument::get_Device(ICanvasDevice** device)
{
    return ExceptionBoundary(
        [&]
        {
            CheckAndClearOutPointer(device);

            ThrowIfFailed(m_canvasDevice.EnsureNotClosed().CopyTo(device));
        });
}


IFACEMETHODIMP CanvasSvgDocument::GetXml(HSTRING* result)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(result);

            auto& resource = GetResource();

            ComPtr<IStream> outputStream;
            ThrowIfFailed(CreateStreamOnHGlobal(nullptr, TRUE, &outputStream));

            ThrowIfFailed(resource->Serialize(outputStream.Get()));
            
            HGLOBAL globalHandle;
            ThrowIfFailed(GetHGlobalFromStream(outputStream.Get(), &globalHandle));

            void* lockedBytes = GlobalLock(globalHandle);
            if (!lockedBytes)
            {
                ThrowHR(E_INVALIDARG);
            }

            auto unlockBytes = MakeScopeWarden([&] { GlobalUnlock(globalHandle); });

            auto lockedBytesLength = GlobalSize(globalHandle);
            if (lockedBytesLength == 0)
            {
                ThrowHR(E_INVALIDARG);
            }
            
            // The string is returned from D2D in a multi-byte UTF-8 format. We need to convert it to a wide
            // character format to return it through the platform.

            // Note that the string pointed at by lockedBytes is not null terminated. Therefore, we need to 
            // pass the locked size to the function below.
            
            assert(lockedBytesLength <= INT_MAX);
            int wideCharacterCount = MultiByteToWideChar(
                CP_UTF8,
                0, // Default flags
                reinterpret_cast<char* const>(lockedBytes),
                static_cast<int32_t>(lockedBytesLength),
                nullptr,
                0); // Write nothing, and retrieve size

            WinStringBuilder stringBuilder;
            wideCharacterCount++; // Account for null terminator
            auto buffer = stringBuilder.Allocate(wideCharacterCount);

            if (MultiByteToWideChar(
                CP_UTF8,
                0, // Default flags
                reinterpret_cast<char* const>(lockedBytes),
                static_cast<int32_t>(lockedBytesLength),
                buffer,
                wideCharacterCount) == 0)
            {
                ThrowHR(E_INVALIDARG);
            }

            // MultiByteToWideChar does not null-terminate the string, so we do it ourselves here.
            buffer[wideCharacterCount - 1] = 0;

            stringBuilder.Get().CopyTo(result);
        });
}

IFACEMETHODIMP CanvasSvgDocument::SaveAsync(IRandomAccessStream* rawStream, IAsyncAction** resultAsyncAction)
{
    return ExceptionBoundary(
        [=]
        {
            CheckInPointer(rawStream);
            CheckAndClearOutPointer(resultAsyncAction);

            auto& resource = GetResource();

            ComPtr<IRandomAccessStream> randomAccessStream = rawStream;

            auto asyncAction = Make<AsyncAction>(
                [=]
                {
                    ComPtr<IStream> stream;
                    ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream.Get(), IID_PPV_ARGS(&stream)));

                    ThrowIfFailed(resource->Serialize(stream.Get()));
                });

            CheckMakeResult(asyncAction);
            ThrowIfFailed(asyncAction.CopyTo(resultAsyncAction));
        });
}

// This allows us to wrap an IStream around an existing, arbitrary buffer which has not necessarily been allocated 
// using GlobalAlloc. It is set up eliminate the need for copying anything.
class BufferStreamWrapper : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IStream>
    , private LifespanTracker<BufferStreamWrapper>
{
    byte const* m_buffer;
    size_t m_bufferSizeInBytes;
    size_t m_seekLocation;

public:
    BufferStreamWrapper(byte const* buffer, size_t bufferSizeInBytes)
        : m_buffer(buffer)
        , m_bufferSizeInBytes(bufferSizeInBytes)
        , m_seekLocation(0)
    {}

    // ISequentialStream Interface

    virtual HRESULT STDMETHODCALLTYPE Read(void* dest, ULONG numberOfBytesRequested, ULONG* outputNumberOfBytesRead)
    {
        if (!dest)
            return STG_E_INVALIDPOINTER;

        if (numberOfBytesRequested == 0)
            return S_OK; // Nothing to do

        if (m_seekLocation >= m_bufferSizeInBytes)
        {
            // We've reached the end of the stream.
            if (outputNumberOfBytesRead)
                *outputNumberOfBytesRead = 0;

            return S_FALSE;
        }

        size_t bytesLeft = m_bufferSizeInBytes - m_seekLocation;
        size_t numberOfBytesToRead;
        bool partialRead;

        if (numberOfBytesRequested <= bytesLeft)
        {
            numberOfBytesToRead = numberOfBytesRequested;
            partialRead = false;
        }
        else
        {
            numberOfBytesToRead = bytesLeft;
            partialRead = true;
        }

        if (memcpy_s(dest, numberOfBytesToRead, &m_buffer[m_seekLocation], numberOfBytesToRead) != 0)
        {
            return E_OUTOFMEMORY;
        }

        if (outputNumberOfBytesRead)
        {
            assert(numberOfBytesToRead <= ULONG_MAX);
            *outputNumberOfBytesRead = static_cast<ULONG>(numberOfBytesToRead);
        }

        m_seekLocation += numberOfBytesRequested;

        if (partialRead)
            return S_FALSE;

        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Write(void const*, ULONG, ULONG*)
    {
        return E_NOTIMPL; // This stream supports read only.
    }

    // IStream Interface  

    virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER)
    {
        return E_NOTIMPL; // This stream is not resizable.
    }

    virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*,
        ULARGE_INTEGER*)
    {
        return E_NOTIMPL; // Copying to other streams is not supported
    }

    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD)
    {
        return S_OK; // Nothing is transacted (e.g., requires flushing), so this has no effect.
    }

    virtual HRESULT STDMETHODCALLTYPE Revert(void)
    {
        return S_OK; // Nothing is transacted, so this has no effect.
    }

    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
    {
        return E_NOTIMPL; // Region locking is not supported
    }

    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
    {
        return E_NOTIMPL; // Region locking is not supported
    }

    virtual HRESULT STDMETHODCALLTYPE Clone(IStream **)
    {
        return E_NOTIMPL; // Nothing should be cloning this stream.
    }

    virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin,
        ULARGE_INTEGER* newSeekLocation)
    {
        return ExceptionBoundary(
            [&]
            {
                if (dwOrigin == STREAM_SEEK_SET)
                {
                    m_seekLocation = liDistanceToMove.LowPart;
                }
                else if (dwOrigin == STREAM_SEEK_CUR)
                {
                    m_seekLocation += liDistanceToMove.LowPart;
                }
                else if (dwOrigin == STREAM_SEEK_END)
                {
                    m_seekLocation = m_bufferSizeInBytes + liDistanceToMove.LowPart;
                }
                else
                {
                    ThrowHR(E_INVALIDARG);
                }

                if (newSeekLocation)
                {
                    newSeekLocation->HighPart = 0;

                    assert(m_seekLocation < DWORD_MAX);
                    newSeekLocation->LowPart = static_cast<DWORD>(m_seekLocation);
                }
            });
    }

    virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG*, DWORD)
    {
        return E_NOTIMPL; // Not supported
    }
};

IFACEMETHODIMP CanvasSvgDocumentStatics::Load(
    ICanvasResourceCreator* resourceCreator,
    HSTRING xmlString,
    ICanvasSvgDocument** svgDocument)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckAndClearOutPointer(svgDocument);

            uint32_t textLength;
            auto textBuffer = WindowsGetStringRawBuffer(xmlString, &textLength);
            ThrowIfNullPointer(textBuffer, E_INVALIDARG);

            if (textLength == 0)
                ThrowHR(E_INVALIDARG, Strings::SvgTextShouldHaveNonZeroLength);

            // Wrap the input text into an IStream.
            ComPtr<BufferStreamWrapper> inputXmlStream = Make<BufferStreamWrapper>(reinterpret_cast<byte const*>(textBuffer), textLength * sizeof(wchar_t));

            ComPtr<CanvasSvgDocument> newDocument = CanvasSvgDocument::CreateNew(resourceCreator, inputXmlStream.Get());
            ThrowIfFailed(newDocument.CopyTo(svgDocument));
        });
}

IFACEMETHODIMP CanvasSvgDocumentStatics::LoadAsync(
    ICanvasResourceCreator* resourceCreator,
    IRandomAccessStream* randomAccessStream,
    IAsyncOperation<CanvasSvgDocument*>** svgDocument)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(resourceCreator);
            CheckInPointer(randomAccessStream);
            CheckAndClearOutPointer(svgDocument);

            ComPtr<IStream> stream;
            ThrowIfFailed(CreateStreamOverRandomAccessStream(randomAccessStream, IID_PPV_ARGS(&stream)));
            
            auto asyncOperation = Make<AsyncOperation<CanvasSvgDocument>>(
                [=]
                {
                    auto svgDocument = CanvasSvgDocument::CreateNew(resourceCreator, stream.Get());
                    CheckMakeResult(svgDocument);

                    return svgDocument;
                });

            CheckMakeResult(asyncOperation);
            ThrowIfFailed(asyncOperation.CopyTo(svgDocument));
        });
}

IFACEMETHODIMP CanvasSvgDocumentStatics::IsSupported(ICanvasDevice* device, boolean* isSupported)
{
    return ExceptionBoundary(
        [&]
        {
            CheckInPointer(device);
            CheckInPointer(isSupported);
        
            auto lease = As<ICanvasDeviceInternal>(device)->GetResourceCreationDeviceContext();
            auto deviceContext5 = MaybeAs<ID2D1DeviceContext5>(lease.Get());
        
            *isSupported = static_cast<bool>(deviceContext5);
        });
}


#endif
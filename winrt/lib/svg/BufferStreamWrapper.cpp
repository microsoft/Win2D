// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include "BufferStreamWrapper.h"

using namespace ABI::Microsoft::Graphics::Canvas::Svg;

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{
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

    ComPtr<IStream> WrapSvgStringInStream(HSTRING sourceString)
    {
        uint32_t textLength;
        auto textBuffer = WindowsGetStringRawBuffer(sourceString, &textLength);
        ThrowIfNullPointer(textBuffer, E_INVALIDARG);

        if (textLength == 0)
            ThrowHR(E_INVALIDARG, Strings::SvgTextShouldHaveNonZeroLength);

        auto stream = Make<BufferStreamWrapper>(reinterpret_cast<byte const*>(textBuffer), textLength * sizeof(wchar_t));
        CheckMakeResult(stream);

        return stream;
    }

}}}}}

#endif
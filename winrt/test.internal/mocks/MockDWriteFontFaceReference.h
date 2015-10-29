// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE
namespace canvas
{
    class MockDWriteFontFaceReference
        : public RuntimeClass<
        RuntimeClassFlags<ClassicCom>,
        IDWriteFontFaceReference>
    {
    public:
        CALL_COUNTER_WITH_MOCK(CreateFontFaceMethod, HRESULT(IDWriteFontFace3**));
        CALL_COUNTER_WITH_MOCK(CreateFontFaceWithSimulationsMethod, HRESULT(DWRITE_FONT_SIMULATIONS, IDWriteFontFace3**));
        CALL_COUNTER_WITH_MOCK(EqualsMethod, HRESULT(IDWriteFontFaceReference*));
        CALL_COUNTER_WITH_MOCK(GetFontFaceIndexMethod, UINT32());
        CALL_COUNTER_WITH_MOCK(GetSimulationsMethod, DWRITE_FONT_SIMULATIONS());
        CALL_COUNTER_WITH_MOCK(GetFontFileMethod, HRESULT(IDWriteFontFile**));
        CALL_COUNTER_WITH_MOCK(GetLocalFileSizeMethod, UINT64());
        CALL_COUNTER_WITH_MOCK(GetFileSizeMethod, UINT64());
        CALL_COUNTER_WITH_MOCK(GetFileTimeMethod, HRESULT(FILETIME*));
        CALL_COUNTER_WITH_MOCK(GetLocalityMethod, DWRITE_LOCALITY());
        CALL_COUNTER_WITH_MOCK(EnqueueCharacterDownloadRequestMethod, HRESULT(WCHAR const*, UINT32));
        CALL_COUNTER_WITH_MOCK(EnqueueFontDownloadRequestMethod, HRESULT());
        CALL_COUNTER_WITH_MOCK(EnqueueGlyphDownloadRequestMethod, HRESULT(UINT16 const*, UINT32));
        CALL_COUNTER_WITH_MOCK(EnqueueFileFragmentDownloadRequestMethod, HRESULT(UINT64, UINT64));

        IFACEMETHODIMP CreateFontFace(
            IDWriteFontFace3** fontFace)
        {
            return CreateFontFaceMethod.WasCalled(fontFace);
        }

        IFACEMETHODIMP CreateFontFaceWithSimulations(
            DWRITE_FONT_SIMULATIONS fontFaceSimulationFlags, 
            IDWriteFontFace3** fontFace)
        {
            return CreateFontFaceWithSimulationsMethod.WasCalled(fontFaceSimulationFlags, fontFace);
        }

        IFACEMETHODIMP_(BOOL) Equals(IDWriteFontFaceReference* reference)
        {
            return EqualsMethod.WasCalled(reference);
        }

        IFACEMETHODIMP_(UINT32) GetFontFaceIndex()
        {
            return GetFontFaceIndexMethod.WasCalled();
        }

        IFACEMETHODIMP_(DWRITE_FONT_SIMULATIONS) GetSimulations()
        {
            return GetSimulationsMethod.WasCalled();
        }

        IFACEMETHODIMP GetFontFile(IDWriteFontFile** fontFile)
        {
            return GetFontFileMethod.WasCalled(fontFile);
        }

        IFACEMETHODIMP_(UINT64) GetLocalFileSize()
        {
            return GetLocalFileSizeMethod.WasCalled();
        }

        IFACEMETHODIMP_(UINT64) GetFileSize()
        {
            return GetFileSizeMethod.WasCalled();
        }

        IFACEMETHODIMP GetFileTime(FILETIME* fileTime)
        {
            return GetFileTimeMethod.WasCalled(fileTime);
        }

        IFACEMETHODIMP_(DWRITE_LOCALITY) GetLocality()
        {
            return GetLocalityMethod.WasCalled();
        }

        IFACEMETHODIMP EnqueueCharacterDownloadRequest(
            WCHAR const* characters, 
            UINT32 characterCount)
        {
            return EnqueueCharacterDownloadRequestMethod.WasCalled(characters, characterCount);
        }

        IFACEMETHODIMP EnqueueFontDownloadRequest()
        {
            return EnqueueFontDownloadRequestMethod.WasCalled();
        }

        IFACEMETHODIMP EnqueueGlyphDownloadRequest(
            _In_reads_(glyphCount) UINT16 const* glyphIndices,
            UINT32 glyphCount)
        {
            return EnqueueGlyphDownloadRequestMethod.WasCalled(glyphIndices, glyphCount);
        }

        IFACEMETHODIMP EnqueueFileFragmentDownloadRequest(
            UINT64 fileOffset,
            UINT64 fragmentSize)
        {
            return EnqueueFileFragmentDownloadRequestMethod.WasCalled(fileOffset, fragmentSize);
        }

    };
}

#endif
// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace Microsoft 
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template<>
            static inline std::wstring ToString<CanvasBackground>(const CanvasBackground& value)
            {
                switch (value)
                {
                case CanvasBackground::Transparent: return L"CanvasBackground::Transparent";
                case CanvasBackground::Opaque: return L"CanvasBackground::Opaque";
                default: return L"<<invalid CanvasBackground>>";
                }
            }

            static inline std::wstring PointerToString(const wchar_t* name, void* value)
            {
                wchar_t buf[64];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"%s(%x)",
                    name,
                    value));
                return std::wstring(buf);
            }

            template<>
            static inline std::wstring ToString<IUnknown>(IUnknown* value)
            {
                return PointerToString(L"IUnknown", value);
            }

            template<>
            static inline std::wstring ToString<IInspectable>(IInspectable* value)
            {
                return PointerToString(L"IInspectable", value);
            }

            template<>
            static inline std::wstring ToString<ICanvasDevice>(ICanvasDevice* value)
            {
                return PointerToString(L"ICanvasDevice", value);
            }
        }
    }
}


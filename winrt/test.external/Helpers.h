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

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
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

#define TO_STRING(T)                                            \
            template<>                                          \
            static inline std::wstring ToString<T>(T* value)    \
            {                                                   \
                return PointerToString(L#T, value);             \
            }

            TO_STRING(ID2D1Device1);
            TO_STRING(ID2D1DeviceContext1);
            TO_STRING(ID2D1SolidColorBrush);
            TO_STRING(ID3D11Device);
            TO_STRING(ID3D11Texture2D);
            TO_STRING(IDXGIDevice);
            TO_STRING(IDXGISurface);
            TO_STRING(ID2D1StrokeStyle1);
            TO_STRING(IDWriteFactory2);

#undef TO_STRING

            template<>
            static inline std::wstring ToString<Windows::UI::Color>(Windows::UI::Color* value)
            {
                return value->ToString()->Data();
            }

            template<>
            static inline std::wstring ToString<Windows::UI::Color>(const Windows::UI::Color& value)
            {
                return L"Color";
            }

            template<>
            static inline std::wstring ToString<Microsoft::Graphics::Canvas::Numerics::Matrix3x2>(const Microsoft::Graphics::Canvas::Numerics::Matrix3x2& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Numerics.Matrix{M11=%f,M12=%f,M21=%f,M22=%f,M31=%f,M32=%f}",
                    value.M11, value.M12,
                    value.M21, value.M22,
                    value.M31, value.M32));

                return buf;
            }

            // TODO: Consider sharing these definitions somehow between internal and external tests.
#define CX_VALUE_TO_STRING(T)                                       \
            template<>                                              \
            static inline std::wstring ToString<T>(const T& value)  \
            {                                                       \
                return value.ToString()->Data();                    \
            }
            
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasHardwareAcceleration);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasCapStyle);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasLineJoin);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasDashStyle);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasStrokeTransformBehavior);

#undef CX_VALUE_TO_STRING

            inline bool operator==(const Windows::UI::Color& a, const Windows::UI::Color& b)
            {
                return a.A == b.A &&
                    a.R == b.R &&
                    a.G == b.G &&
                    a.B == b.B;
            }

            inline bool operator==(const Microsoft::Graphics::Canvas::Numerics::Matrix3x2& a, const Microsoft::Graphics::Canvas::Numerics::Matrix3x2& b)
            {
                return
                    a.M11 == b.M11 && a.M12 == b.M12 &&
                    a.M21 == b.M21 && a.M22 == b.M22 &&
                    a.M31 == b.M31 && a.M32 == b.M32;
            }

            inline bool operator==(const Windows::UI::Text::FontWeight& a, const Windows::UI::Text::FontWeight& b)
            {
                return a.Weight == b.Weight;
            }
        }
    }
}


inline ComPtr<ID3D11Device> CreateD3DDevice()
{
    ComPtr<ID3D11Device> d3dDevice;
    D3D_FEATURE_LEVEL featureLevel;
    ComPtr<ID3D11DeviceContext> immediateContext;

    ThrowIfFailed(D3D11CreateDevice(
        nullptr,            // adapter
        D3D_DRIVER_TYPE_WARP,
        nullptr,            // software
        D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr,            // feature levels
        0,                  // feature levels count
        D3D11_SDK_VERSION,
        &d3dDevice,
        &featureLevel,
        &immediateContext));

    return d3dDevice;
}

template<typename ACTION_ON_CLOSED_OBJECT>
void ExpectObjectClosed(ACTION_ON_CLOSED_OBJECT&& fn)
{
    Assert::ExpectException<Platform::ObjectDisposedException^>(
        [&]()
        {
            fn();
        });
}

//
// Executes the given code on the UI thread.  Any C++ exception that is thrown
// by this code is captured and re-thrown from the CppUnitTestFramework.Executor
// thread so that it gets picked up by the test framework.
//
// NOTE: unfortunately, it seems that Assert::FailImpl uses SEH rather than C++
// exceptions, so these don't get caught and transported.  This means that if
// 'code' contains unit test Asserts that fail then the only way to find out
// more is to attach a debugger.
//
template<typename CODE>
void RunOnUIThread(CODE&& code)
{
    using namespace Microsoft::WRL::Wrappers;
    using namespace Windows::ApplicationModel::Core;
    using namespace Windows::UI::Core;

    // 'code' will be executed on the UI thread.  This event is used to notify
    // this thread when the UI thread has completed executing 'code'.
    Event event(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
    if (!event.IsValid())
        throw std::bad_alloc();

    // If 'code' throws then we capture the exception here and rethrow.
    std::exception_ptr exceptionDuringTest = nullptr;

    // Dispatch 'code' to run on the UI thread
    CoreWindow^ wnd = CoreApplication::MainView->CoreWindow;
    CoreDispatcher^ dispatcher = wnd->Dispatcher;
    dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [&event, code, &exceptionDuringTest]() 
            {
                try
                {
                    code();
                }
                catch (...)
                {
                    exceptionDuringTest = std::current_exception();
                }

                SetEvent(event.Get());
            }));

    // Wait for the UI thread to run 'code', but don't wait forever.
    auto timeout = 1000 * 5;
    auto waitResult = WaitForSingleObjectEx(event.Get(), timeout, FALSE);
    Assert::AreEqual(WAIT_OBJECT_0, waitResult);

    // Rethrow any exception we caught
    if (exceptionDuringTest)
        std::rethrow_exception(exceptionDuringTest);
}

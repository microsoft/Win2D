// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <ppl.h>
#include <ppltasks.h>

using namespace concurrency;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Microsoft::Graphics::Canvas::Geometry;
using namespace Microsoft::Graphics::Canvas::Svg;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;
using namespace Microsoft::Graphics::Canvas::UI;
using namespace Microsoft::Graphics::Canvas;
using namespace WinRTDirectX;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::UI::Core;

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

#define TO_STRING(T)                                    \
            template<>                                  \
            inline std::wstring ToString<T>(T* value)   \
            {                                           \
                return PointerToString(L#T, value);     \
            }

#define TO_STRING_CX(T)                                                 \
            template<>                                                  \
            inline std::wstring ToString<T^>(T^ const& value)           \
            {                                                           \
                return PointerToString(L#T, reinterpret_cast<IInspectable*>(value)); \
            }

            TO_STRING(ID2D1Bitmap);
            TO_STRING(ID2D1Bitmap1);
            TO_STRING(ID2D1BitmapBrush);
            TO_STRING(ID2D1Device1);
            TO_STRING(ID2D1DeviceContext1);
            TO_STRING(ID2D1ImageBrush);
            TO_STRING(ID2D1SolidColorBrush);
            TO_STRING(ID3D11Device);
            TO_STRING(ID3D11Texture2D);
            TO_STRING(IDXGIDevice);
            TO_STRING(IDXGISurface);
            TO_STRING(ID2D1StrokeStyle1);
            TO_STRING(IDWriteFactory2);
            TO_STRING(ID2D1Image);
            TO_STRING(ID2D1LinearGradientBrush);
            TO_STRING(ID2D1RadialGradientBrush);
            TO_STRING_CX(ICanvasImage);
            TO_STRING(IDXGISwapChain2);
            TO_STRING(ID2D1RectangleGeometry);
            TO_STRING(ID2D1EllipseGeometry);
            TO_STRING(ID2D1RoundedRectangleGeometry);
            TO_STRING(ID2D1PathGeometry);
            TO_STRING(ID2D1GeometryRealization);
            TO_STRING(ID2D1Factory);
            TO_STRING_CX(ICanvasSvgElement);

#undef TO_STRING
#undef TO_STRING_CX

            template<>
            inline std::wstring ToString<Windows::UI::Color>(Windows::UI::Color* value)
            {
                return value->ToString()->Data();
            }

            template<>
            inline std::wstring ToString<Windows::UI::Color>(Windows::UI::Color const& value)
            {
                return L"Color";
            }

            template<>
            inline std::wstring ToString<Windows::Foundation::Size>(Windows::Foundation::Size const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Size{%f,%f}", value.Width, value.Height));
                return buf;
            }

            template<>
            inline std::wstring ToString<BitmapSize>(BitmapSize const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"BitmapSize{%u,%u}", value.Width, value.Height));
                return buf;
            }

            template<>
            inline std::wstring ToString<Windows::Foundation::Rect>(Windows::Foundation::Rect const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Rect{%f,%f,%f,%f}", value.X, value.Y, value.Width, value.Height));
                return buf;
            }

            template<>
            inline std::wstring ToString<D2D1_RECT_F>(D2D1_RECT_F const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"D2D_RECT_F{%f,%f,%f,%f}",
                    value.left,
                    value.top,
                    value.right,
                    value.bottom));
                return buf;
            }

            template<>
            inline std::wstring ToString<float2>(float2 const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Numerics.float2{%f,%f}",
                    value.x, value.y));

                return buf;
            }

            template<>
            inline std::wstring ToString<float3>(float3 const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Numerics.float3{%f,%f,%f}",
                    value.x, value.y, value.z));

                return buf;
            }

            template<>
            inline std::wstring ToString<float4>(float4 const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Numerics.float4{%f,%f,%f,%f}",
                    value.x, value.y, value.z, value.w));

                return buf;
            }

            template<>
            inline std::wstring ToString<float3x2>(float3x2 const& value)
            {
                wchar_t buf[256];
                ThrowIfFailed(StringCchPrintf(
                    buf,
                    _countof(buf),
                    L"Numerics.float3x2{m11=%f,m12=%f,m21=%f,m22=%f,m31=%f,m32=%f}",
                    value.m11, value.m12,
                    value.m21, value.m22,
                    value.m31, value.m32));

                return buf;
            }

#define CX_VALUE_TO_STRING(T)                                       \
            template<>                                              \
            inline std::wstring ToString<T>(T const& value)  \
            {                                                       \
                return value.ToString()->Data();                    \
            }
            
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Geometry::CanvasCapStyle);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Geometry::CanvasLineJoin);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Geometry::CanvasDashStyle);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Geometry::CanvasStrokeTransformBehavior);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasAntialiasing);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasBlend);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Text::CanvasTextAntialiasing);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasUnits);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasAlphaMode);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasImageInterpolation);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasBufferPrecision);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::CanvasColorSpace);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesReason);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Text::CanvasGlyphOrientation);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Text::CanvasTextDirection);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Text::CanvasTextMeasuringMode);
            CX_VALUE_TO_STRING(DirectXPixelFormat);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Effects::ColorManagementProfileType);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Effects::ColorManagementGamma);
            CX_VALUE_TO_STRING(Microsoft::Graphics::Canvas::Effects::ExtendedColorSpace);

#undef CX_VALUE_TO_STRING

            template<>
            inline std::wstring ToString<D2D1_ANTIALIAS_MODE>(D2D1_ANTIALIAS_MODE const& value)
            {
                switch (value)
                {
                    case D2D1_ANTIALIAS_MODE_PER_PRIMITIVE: return L"D2D1_ANTIALIAS_MODE_PER_PRIMITIVE";
                    case D2D1_ANTIALIAS_MODE_ALIASED: return L"D2D1_ANTIALIAS_MODE_ALIASED";
                    default: assert(false); return L"<unknown D2D1_ANTIALIAS_MODE>";
                }
            }

            template<>
            inline std::wstring ToString<D2D1_PRIMITIVE_BLEND>(D2D1_PRIMITIVE_BLEND const& value)
            {
                switch (value)
                {
                    case D2D1_PRIMITIVE_BLEND_SOURCE_OVER: return L"D2D1_PRIMITIVE_BLEND_SOURCE_OVER";
                    case D2D1_PRIMITIVE_BLEND_COPY: return L"D2D1_PRIMITIVE_BLEND_COPY";
                    case D2D1_PRIMITIVE_BLEND_MIN: return L"D2D1_PRIMITIVE_BLEND_MIN";
                    case D2D1_PRIMITIVE_BLEND_ADD: return L"D2D1_PRIMITIVE_BLEND_ADD";
                    default: assert(false); return L"<unknown D2D1_PRIMITIVE_BLEND>";
                }
            }

            template<>
            inline std::wstring ToString<D2D1_TEXT_ANTIALIAS_MODE>(D2D1_TEXT_ANTIALIAS_MODE const& value)
            {
                switch (value)
                {
                    case D2D1_TEXT_ANTIALIAS_MODE_DEFAULT: return L"D2D1_TEXT_ANTIALIAS_MODE_DEFAULT";
                    case D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE: return L"D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE";
                    case D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE: return L"D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE";
                    case D2D1_TEXT_ANTIALIAS_MODE_ALIASED: return L"D2D1_TEXT_ANTIALIAS_MODE_ALIASED";
                    default: assert(false); return L"<unknown D2D1_TEXT_ANTIALIAS_MODE>";
                }
            }

            template<>
            inline std::wstring ToString<D2D1_UNIT_MODE>(D2D1_UNIT_MODE const& value)
            {
                switch (value)
                {
                    case D2D1_UNIT_MODE_DIPS: return L"D2D1_UNIT_MODE_DIPS";
                    case D2D1_UNIT_MODE_PIXELS: return L"D2D1_UNIT_MODE_PIXELS";
                    default: assert(false); return L"<unknown D2D1_UNIT_MODE>";
                }
            }

            template<>
            inline std::wstring ToString<D2D1_ALPHA_MODE>(D2D1_ALPHA_MODE const& value)
            {
                switch (value)
                {
                    case D2D1_ALPHA_MODE_UNKNOWN: return L"D2D1_ALPHA_MODE_UNKNOWN";
                    case D2D1_ALPHA_MODE_PREMULTIPLIED: return L"D2D1_ALPHA_MODE_PREMULTIPLIED";
                    case D2D1_ALPHA_MODE_STRAIGHT: return L"D2D1_ALPHA_MODE_STRAIGHT";
                    case D2D1_ALPHA_MODE_IGNORE: return L"D2D1_ALPHA_MODE_IGNORE";
                    default: assert(false); return L"<unknown D2D1_ALPHA_MODE>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasSwapChainRotation>(CanvasSwapChainRotation const& value)
            {
                switch (value)
                {
                    case CanvasSwapChainRotation::None: return L"CanvasSwapChainRotation::None";
                    case CanvasSwapChainRotation::Rotate90: return L"CanvasSwapChainRotation::Rotate90";
                    case CanvasSwapChainRotation::Rotate180: return L"CanvasSwapChainRotation::Rotate180";
                    case CanvasSwapChainRotation::Rotate270: return L"CanvasSwapChainRotation::Rotate270";
                    default: assert(false); return L"<unknown CanvasSwapChainRotation>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasDebugLevel>(CanvasDebugLevel const& value)
            {
                switch (value)
                {
                    case CanvasDebugLevel::None: return L"CanvasDebugLevel::None";
                    case CanvasDebugLevel::Error: return L"CanvasDebugLevel::Error";
                    case CanvasDebugLevel::Warning: return L"CanvasDebugLevel::Warning";
                    case CanvasDebugLevel::Information: return L"CanvasDebugLevel::Information";
                    default: assert(false); return L"<unknown CanvasDebugLevel>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasTextRenderingMode>(CanvasTextRenderingMode const& value)
            {
                switch (value)
                {
                    case CanvasTextRenderingMode::Default: return L"CanvasTextRenderingMode::Default";
                    case CanvasTextRenderingMode::Aliased: return L"CanvasTextRenderingMode::Aliased";
                    case CanvasTextRenderingMode::GdiClassic: return L"CanvasTextRenderingMode::GdiClassic";
                    case CanvasTextRenderingMode::GdiNatural: return L"CanvasTextRenderingMode::GdiNatural";
                    case CanvasTextRenderingMode::NaturalSymmetric: return L"CanvasTextRenderingMode::NaturalSymmetric";
                    case CanvasTextRenderingMode::Outline: return L"CanvasTextRenderingMode::Outline";
                    case CanvasTextRenderingMode::NaturalSymmetricDownsampled: return L"CanvasTextRenderingMode::NaturalSymmetricDownsampled";
                    default: assert(false); return L"<unknown CanvasTextRenderingMode>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasTextGridFit>(CanvasTextGridFit const& value)
            {
                switch (value)
                {
                    case CanvasTextGridFit::Default: return L"CanvasTextGridFit::Default";
                    case CanvasTextGridFit::Disable: return L"CanvasTextGridFit::Disable";
                    case CanvasTextGridFit::Enable: return L"CanvasTextGridFit::Enable";
                    default: assert(false); return L"<unknown CanvasTextGridFit>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasTextTrimmingGranularity>(CanvasTextTrimmingGranularity const& value)
            {
                switch (value)
                {
                    case CanvasTextTrimmingGranularity::None: return L"CanvasDebugLevel::None";
                    case CanvasTextTrimmingGranularity::Character: return L"CanvasDebugLevel::Character";
                    case CanvasTextTrimmingGranularity::Word: return L"CanvasDebugLevel::Word";
                    default: assert(false); return L"<unknown CanvasTextTrimmingGranularity>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasTrimmingSign>(CanvasTrimmingSign const& value)
            {
                switch (value)
                {
                    case CanvasTrimmingSign::None: return L"CanvasTrimmingSign::None";
                    case CanvasTrimmingSign::Ellipsis: return L"CanvasTrimmingSign::Ellipsis";
                    default: assert(false); return L"<unknown CanvasTrimmingSign>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasLineBreakCondition>(CanvasLineBreakCondition const& value)
            {
                switch (value)
                {
                    case CanvasLineBreakCondition::Neutral: return L"CanvasLineBreakCondition::Neutral";
                    case CanvasLineBreakCondition::CanBreak: return L"CanvasLineBreakCondition::CanBreak";
                    case CanvasLineBreakCondition::CannotBreak: return L"CanvasLineBreakCondition::CannotBreak";
                    case CanvasLineBreakCondition::MustBreak: return L"CanvasLineBreakCondition::MustBreak";
                    default: assert(false); return L"<unknown CanvasLineBreakCondition>";
                }
            }

            template<>
            inline std::wstring ToString<D2D1_BUFFER_PRECISION>(D2D1_BUFFER_PRECISION const& value)
            {
                switch (value)
                {
                    case D2D1_BUFFER_PRECISION_UNKNOWN: return L"D2D1_BUFFER_PRECISION_UNKNOWN";
                    case D2D1_BUFFER_PRECISION_8BPC_UNORM: return L"D2D1_BUFFER_PRECISION_8BPC_UNORM";
                    case D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB: return L"D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB";
                    case D2D1_BUFFER_PRECISION_16BPC_UNORM: return L"D2D1_BUFFER_PRECISION_16BPC_UNORM";
                    case D2D1_BUFFER_PRECISION_16BPC_FLOAT: return L"D2D1_BUFFER_PRECISION_16BPC_FLOAT";
                    case D2D1_BUFFER_PRECISION_32BPC_FLOAT: return L"D2D1_BUFFER_PRECISION_32BPC_FLOAT";
                    default: assert(false); return L"<unknown D2D1_BUFFER_PRECISION>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasSvgPathCommand>(CanvasSvgPathCommand const& value)
            {
                switch (value)
                {
                    case CanvasSvgPathCommand::ClosePath: return L"CanvasSvgPathCommand::ClosePath";
                    case CanvasSvgPathCommand::MoveAbsolute: return L"CanvasSvgPathCommand::MoveAbsolute";
                    case CanvasSvgPathCommand::MoveRelative: return L"CanvasSvgPathCommand::MoveRelative";
                    case CanvasSvgPathCommand::LineAbsolute: return L"CanvasSvgPathCommand::LineAbsolute";
                    case CanvasSvgPathCommand::LineRelative: return L"CanvasSvgPathCommand::LineRelative";
                    case CanvasSvgPathCommand::CubicAbsolute: return L"CanvasSvgPathCommand::CubicAbsolute";
                    case CanvasSvgPathCommand::CubicRelative: return L"CanvasSvgPathCommand::CubicRelative";
                    case CanvasSvgPathCommand::QuadraticAbsolute: return L"CanvasSvgPathCommand::QuadraticAbsolute";
                    case CanvasSvgPathCommand::QuadraticRelative: return L"CanvasSvgPathCommand::QuadraticRelative";
                    case CanvasSvgPathCommand::ArcAbsolute: return L"CanvasSvgPathCommand::ArcAbsolute";
                    case CanvasSvgPathCommand::ArcRelative: return L"CanvasSvgPathCommand::ArcRelative";
                    case CanvasSvgPathCommand::HorizontalAbsolute: return L"CanvasSvgPathCommand::HorizontalAbsolute";
                    case CanvasSvgPathCommand::HorizontalRelative: return L"CanvasSvgPathCommand::HorizontalRelative";
                    case CanvasSvgPathCommand::VerticalAbsolute: return L"CanvasSvgPathCommand::VerticalAbsolute";
                    case CanvasSvgPathCommand::VerticalRelative: return L"CanvasSvgPathCommand::VerticalRelative";
                    case CanvasSvgPathCommand::CubicSmoothAbsolute: return L"CanvasSvgPathCommand::CubicSmoothAbsolute";
                    case CanvasSvgPathCommand::CubicSmoothRelative: return L"CanvasSvgPathCommand::CubicSmoothRelative";
                    case CanvasSvgPathCommand::QuadraticSmoothAbsolute: return L"CanvasSvgPathCommand::QuadraticSmoothAbsolute";
                    case CanvasSvgPathCommand::QuadraticSmoothRelative: return L"CanvasSvgPathCommand::QuadraticSmoothRelative";
                    default: assert(false); return L"<unknown CanvasSvgPathCommand>";
                }
            }

            template<>
            inline std::wstring ToString<CanvasSvgLengthUnits>(CanvasSvgLengthUnits const& value)
            {
                switch (value)
                {
                    case CanvasSvgLengthUnits::Number: return L"CanvasSvgLengthUnits::Number";
                    case CanvasSvgLengthUnits::Percentage: return L"CanvasSvgLengthUnits::Percentage";
                    default: assert(false); return L"<unknown CanvasSvgLengthUnits>";
                }
            }

            template<>
            inline std::wstring ToString<Platform::Guid>(Platform::Guid const& value)
            {
                Platform::Guid copy = value;
                return copy.ToString()->Data();
            }

            inline bool operator==(Windows::UI::Color const& a, Windows::UI::Color const& b)
            {
                return a.A == b.A &&
                       a.R == b.R &&
                       a.G == b.G &&
                       a.B == b.B;
            }

            inline bool operator==(BitmapSize const& a, BitmapSize const& b)
            {
                return a.Width == b.Width &&
                       a.Height == b.Height;
            }

            inline bool operator==(Windows::UI::Text::FontWeight const& a, Windows::UI::Text::FontWeight const& b)
            {
                return a.Weight == b.Weight;
            }

            inline bool operator==(D2D1_RECT_F const& a, D2D1_RECT_F const& b)
            {
                return a.left == b.left &&
                       a.top == b.top &&
                       a.right == b.right &&
                       a.bottom == b.bottom;
            }
        }
    }
}


inline ComPtr<ID3D11Device> CreateD3DDevice()
{
    ComPtr<ID3D11Device> d3dDevice;
    D3D_FEATURE_LEVEL featureLevel;
    ComPtr<ID3D11DeviceContext> immediateContext;

    if (FAILED(D3D11CreateDevice(
        nullptr,            // adapter
        D3D_DRIVER_TYPE_WARP,
        nullptr,            // software
        D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr,            // feature levels
        0,                  // feature levels count
        D3D11_SDK_VERSION,
        &d3dDevice,
        &featureLevel,
        &immediateContext)))
    {
        // Avoid taking hard dependency on the D3D debug layer.
        ThrowIfFailed(D3D11CreateDevice(
            nullptr,            // adapter
            D3D_DRIVER_TYPE_WARP,
            nullptr,            // software
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            nullptr,            // feature levels
            0,                  // feature levels count
            D3D11_SDK_VERSION,
            &d3dDevice,
            &featureLevel,
            &immediateContext));
    }

    return d3dDevice;
}

template<typename ACTION_ON_CLOSED_OBJECT>
inline void ExpectObjectClosed(ACTION_ON_CLOSED_OBJECT&& fn)
{
    Assert::ExpectException<Platform::ObjectDisposedException^>(
        [&]
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
inline void RunOnUIThread(CODE&& code)
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
            [&event, code, &exceptionDuringTest]
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

template<typename T>
inline T WaitExecution(IAsyncOperation<T>^ asyncOperation)
{
    using namespace Microsoft::WRL::Wrappers;

    Event emptyEvent(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
    if (!emptyEvent.IsValid())
        throw std::bad_alloc();

    task_options options;
    options.set_continuation_context(task_continuation_context::use_arbitrary());

    task<T> asyncTask(asyncOperation);

    asyncTask.then([&](task<T>)
    {
        SetEvent(emptyEvent.Get());
    }, options);

    // waiting before event executed
    auto timeout = 1000 * 5;
    auto waitResult = WaitForSingleObjectEx(emptyEvent.Get(), timeout, true);
    Assert::AreEqual(WAIT_OBJECT_0, waitResult, L"WaitExecution: WaitForSingleObject timed out.");

    return asyncTask.get();
};

inline void WaitExecution(IAsyncAction^ ayncAction)
{
    using namespace Microsoft::WRL::Wrappers;

    Event emptyEvent(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
    if (!emptyEvent.IsValid())
        throw std::bad_alloc();

    task_options options;
    options.set_continuation_context(task_continuation_context::use_arbitrary());

    task<void> asyncTask(ayncAction);

    asyncTask.then([&](task<void>)
    {
        SetEvent(emptyEvent.Get());
    }, options);

    // waiting before event executed
    auto timeout = 1000 * 5;
    auto waitResult = WaitForSingleObjectEx(emptyEvent.Get(), timeout, true);
    Assert::AreEqual(WAIT_OBJECT_0, waitResult);

    asyncTask.get();
};

template<typename T, typename U>
inline void AssertTypeName(U^ obj)
{
    Assert::AreEqual(T::typeid->FullName, obj->GetType()->FullName);
}

template<typename T, typename U>
inline ComPtr<T> GetDXGIInterface(U^ obj)
{
    ComPtr<T> dxgi;
    ThrowIfFailed(GetDXGIInterface<T>(obj, &dxgi));
    return dxgi;
}

ComPtr<ID2D1DeviceContext1> CreateTestD2DDeviceContext(CanvasDevice^ device = nullptr);

ComPtr<ID2D1Bitmap1> CreateTestD2DBitmap(D2D1_BITMAP_OPTIONS options, ComPtr<ID2D1DeviceContext1> deviceContext = nullptr);

void VerifyDpiAndAlpha(ComPtr<ID2D1Bitmap1> const& d2dBitmap, float expectedDpi, D2D1_ALPHA_MODE expectedAlphaMode);

bool GpuMatchesDescription(CanvasDevice^ canvasDevice, wchar_t const* description);

int NextValueRepresentableAsFloat(int value);

struct WicBitmapTestFixture
{
    ComPtr<ID2D1DeviceContext1> RenderTarget;
    ComPtr<ID2D1Bitmap1> Bitmap;
};
WicBitmapTestFixture CreateWicBitmapTestFixture();

template<typename T>
inline void ExpectCOMException(HRESULT expectedHR, T&& lambda)
{
    try
    {
        lambda();
        Assert::Fail(L"Expected this to throw.");
    }
    catch (Platform::COMException^ e)
    {
        Assert::AreEqual<HRESULT>(expectedHR, e->HResult);
    }
}

template<typename T>
inline void ExpectCOMException(HRESULT expectedHR, wchar_t const* expectedExceptionText, T&& lambda)
{
    try
    {
        lambda();
        Assert::Fail(L"Expected this to throw.");
    }
    catch (Platform::COMException^ e)
    {
        Assert::AreEqual<HRESULT>(expectedHR, e->HResult);

        if (expectedExceptionText)
        {
            std::wstring msg(e->Message->Data());
            Assert::IsTrue(msg.find(expectedExceptionText) != std::wstring::npos,
                (std::wstring(L"Expected message: '") + expectedExceptionText + L"' actual: '" + msg + L"'").c_str());
        }
    }
}


ref class StubResourceCreatorWithDpi sealed : public ICanvasResourceCreatorWithDpi
{
    CanvasDevice^ m_device;
    float m_dpi;

public:
    StubResourceCreatorWithDpi(CanvasDevice^ device, float dpi = DEFAULT_DPI)
        : m_device(device)
        , m_dpi(dpi)
    { }

    property CanvasDevice^ Device
    {
        virtual CanvasDevice^ get() { return m_device; }
    }

    property float Dpi
    {
        virtual float get() { return m_dpi; }
    }

    virtual float ConvertPixelsToDips(int pixels) { Assert::Fail(L"Not implemented"); }
    virtual int   ConvertDipsToPixels(float dips, CanvasDpiRounding roundingBehavior) { Assert::Fail(L"Not implemented"); }
};


inline int GetRefCount(IUnknown* unknown)
{
    unknown->AddRef();
    return (int)unknown->Release();
}


template<typename T>
inline int GetRefCount(T^ const& hatPointer)
{
    return GetRefCount(reinterpret_cast<IInspectable*>(hatPointer));
}

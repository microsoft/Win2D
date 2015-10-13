// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

#include <lib/composition/CanvasComposition.h>

using namespace ABI::Microsoft::Graphics::Canvas::UI::Composition;

static int const AnyOffsetX = 123;
static int const AnyOffsetY = -456;
static HRESULT const AnyErrorResult = 0x81234567;


static ComPtr<ICanvasCompositionStatics> GetCompositionStatics()
{
    ComPtr<ICanvasCompositionStatics> composition;
    ThrowIfFailed(MakeAndInitialize<CanvasCompositionStatics>(&composition));
    return composition;
}


TEST_CLASS(CanvasCompositionUnitTests)
{
public:
    struct Fixture
    {
        std::shared_ptr<ApiInformationTestAdapter> ApiInformation = ApiInformationTestAdapter::Create();
        ComPtr<ICanvasCompositionStatics> Composition;

        Fixture()
        {
            ApiInformation->AddContract(UNIVERSAL_API_CONTRACT, 2);
            Composition = GetCompositionStatics();
        }

        Fixture(Fixture const&) = delete;
        Fixture& operator=(Fixture const&) = delete;
    };

    TEST_METHOD_EX(CanvasComposition_CreateCompositionGraphicsDevice_FailsWhenPassedNullParameters)
    {
        Fixture f;
        
        auto compositor = Make<MockCompositor>();
        auto canvasDevice = Make<MockCanvasDevice>();
        ComPtr<ICompositionGraphicsDevice> graphicsDevice;
        
        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateCompositionGraphicsDevice(nullptr,          canvasDevice.Get(), &graphicsDevice));
        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateCompositionGraphicsDevice(compositor.Get(), nullptr,            &graphicsDevice));
        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateCompositionGraphicsDevice(compositor.Get(), canvasDevice.Get(), nullptr));
    }

    TEST_METHOD_EX(CanvasComposition_CreateCompositionGraphicsDevice)
    {
        Fixture f;

        auto compositor = Make<MockCompositor>();
        auto d2dDevice = Make<StubD2DDevice>();
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());
        auto graphicsDevice = Make<MockCompositionGraphicsDevice>();

        compositor->CreateGraphicsDeviceMethod.SetExpectedCalls(1,
            [&] (IUnknown* renderingDevice, ICompositionGraphicsDevice** value)
            {
                Assert::IsTrue(IsSameInstance(d2dDevice.Get(), renderingDevice));
                return graphicsDevice.CopyTo(value);
            });

        ComPtr<ICompositionGraphicsDevice> actualGraphicsDevice;
        ThrowIfFailed(f.Composition->CreateCompositionGraphicsDevice(compositor.Get(), canvasDevice.Get(), &actualGraphicsDevice));

        Assert::IsTrue(IsSameInstance(graphicsDevice.Get(), actualGraphicsDevice.Get()));
    }

    TEST_METHOD_EX(CanvasComposition_CreateCompositionSurfaceForSwapChain_FailsWhenPassedNullParameters)
    {
        Fixture f;
        
        auto compositor = Make<MockCompositor>();
        auto swapChain = Make<MockCanvasSwapChain>();
        ComPtr<ICompositionSurface> compositionSurface;

        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateCompositionSurfaceForSwapChain(nullptr,          swapChain.Get(), &compositionSurface));
        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateCompositionSurfaceForSwapChain(compositor.Get(), nullptr,         &compositionSurface));
        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateCompositionSurfaceForSwapChain(compositor.Get(), swapChain.Get(), nullptr));
    }

    TEST_METHOD_EX(CanvasComposition_CreateCompositionSurfaceForSwapChain)
    {
        Fixture f;

        auto compositor = Make<MockCompositor>();
        auto dxgiSwapChain = Make<MockDxgiSwapChain>();
        auto canvasDevice = Make<MockCanvasDevice>();
        auto canvasSwapChain = Make<CanvasSwapChain>(canvasDevice.Get(), dxgiSwapChain.Get(), DEFAULT_DPI, false);
        auto compositionSurface = Make<RuntimeClass<ICompositionSurface>>();

        compositor->CreateCompositionSurfaceForSwapChainMethod.SetExpectedCalls(1,
            [&] (IUnknown* swapChain, ICompositionSurface** value)
            {
                Assert::IsTrue(IsSameInstance(dxgiSwapChain.Get(), swapChain));
                return compositionSurface.CopyTo(value);
            });

        ComPtr<ICompositionSurface> actualCompositionSurface;
        ThrowIfFailed(f.Composition->CreateCompositionSurfaceForSwapChain(compositor.Get(), canvasSwapChain.Get(), &actualCompositionSurface));

        Assert::IsTrue(IsSameInstance(compositionSurface.Get(), actualCompositionSurface.Get()));
    }

    TEST_METHOD_EX(CanvasComposition_GetCanvasDevice_FailsWhenPassedNullParameters)
    {
        Fixture f;

        auto graphicsDevice = Make<MockCompositionGraphicsDevice>();
        ComPtr<ICanvasDevice> canvasDevice;

        Assert::AreEqual(E_INVALIDARG, f.Composition->GetCanvasDevice(nullptr,              &canvasDevice));
        Assert::AreEqual(E_INVALIDARG, f.Composition->GetCanvasDevice(graphicsDevice.Get(), nullptr));
    }

    TEST_METHOD_EX(CanvasComposition_GetCanvasDevice)
    {
        Fixture f;

        auto graphicsDevice = Make<MockCompositionGraphicsDevice>();
        auto d2dDevice = Make<StubD2DDevice>();
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());

        graphicsDevice->GetRenderingDeviceMethod.SetExpectedCalls(1,
            [&] (IUnknown** value)
            {
                return d2dDevice.CopyTo(value);
            });

        ComPtr<ICanvasDevice> actualCanvasDevice;
        ThrowIfFailed(f.Composition->GetCanvasDevice(graphicsDevice.Get(), &actualCanvasDevice));

        Assert::IsTrue(IsSameInstance(canvasDevice.Get(), actualCanvasDevice.Get()));
    }

    TEST_METHOD_EX(CanvasComposition_SetCanvasDevice_FailsWhenPassedNullParameters)
    {
        Fixture f;

        auto graphicsDevice = Make<MockCompositionGraphicsDevice>();
        auto canvasDevice = Make<MockCanvasDevice>();

        Assert::AreEqual(E_INVALIDARG, f.Composition->SetCanvasDevice(nullptr,              canvasDevice.Get()));
        Assert::AreEqual(E_INVALIDARG, f.Composition->SetCanvasDevice(graphicsDevice.Get(), nullptr));            
    }

    TEST_METHOD_EX(CanvasComposition_SetCanvasDevice)
    {
        Fixture f;

        auto graphicsDevice = Make<MockCompositionGraphicsDevice>();
        auto d2dDevice = Make<StubD2DDevice>();
        auto canvasDevice = Make<CanvasDevice>(d2dDevice.Get());

        graphicsDevice->SetRenderingDeviceMethod.SetExpectedCalls(1,
            [&] (IUnknown* value)
            {
                Assert::IsTrue(IsSameInstance(d2dDevice.Get(), value));
                return S_OK;
            });

        ThrowIfFailed(f.Composition->SetCanvasDevice(graphicsDevice.Get(), canvasDevice.Get()));
    }

    TEST_METHOD_EX(CanvasComposition_CreateDrawingSession_FailsWhenPassedNullParameters)
    {
        Fixture f;

        auto drawingSurface = Make<MockCompositionDrawingSurface>();
        ComPtr<ICanvasDrawingSession> drawingSession;

        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateDrawingSession(nullptr,              &drawingSession));
        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateDrawingSession(drawingSurface.Get(), nullptr));

        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateDrawingSessionWithUpdateRect(nullptr,              Rect{}, &drawingSession));
        Assert::AreEqual(E_INVALIDARG, f.Composition->CreateDrawingSessionWithUpdateRect(drawingSurface.Get(), Rect{}, nullptr));
    }

    struct DrawingSurfaceFixture : public Fixture
    {
        ComPtr<MockCompositionDrawingSurface> DrawingSurface = Make<MockCompositionDrawingSurface>();
    };

    struct DrawingSessionFixture : public DrawingSurfaceFixture
    {
        ComPtr<StubD2DDeviceContext> D2DDeviceContext = Make<StubD2DDeviceContext>();

        DrawingSessionFixture()
        {
            D2DDeviceContext->SetTransformMethod.SetExpectedCalls(1,
                [&] (D2D1_MATRIX_3X2_F const* m)
                {
                    Assert::AreEqual(1.0f, m->_11);
                    Assert::AreEqual(0.0f, m->_12);
                    Assert::AreEqual(0.0f, m->_21);
                    Assert::AreEqual(1.0f, m->_22);
                    Assert::AreEqual((float)AnyOffsetX, m->_31);
                    Assert::AreEqual((float)AnyOffsetY, m->_32);                
                });
        }

        HRESULT DoBeginDraw(REFIID iid, void** updateObject, POINT* updateOffset)
        {
            Assert::AreEqual(__uuidof(ID2D1DeviceContext), iid);
            ThrowIfFailed(D2DDeviceContext.CopyTo(iid, updateObject));
            
            *updateOffset = POINT{ AnyOffsetX, AnyOffsetY };
            
            return S_OK;
        }

        void Check(ComPtr<ICanvasDrawingSession> drawingSession)
        {
            auto actualDeviceContext = GetWrappedResource<ID2D1DeviceContext1>(drawingSession);
            Assert::IsTrue(IsSameInstance(D2DDeviceContext.Get(), actualDeviceContext.Get()));
            
            // the return value from EndDraw should be passed back through Close()
            DrawingSurface->EndDrawMethod.SetExpectedCalls(1,
                [&]
                {
                    return AnyErrorResult;
                });
            Assert::AreEqual(AnyErrorResult, As<IClosable>(drawingSession)->Close());
        }
    };

    TEST_METHOD_EX(CanvasComposition_CreateDrawingSession)
    {
        DrawingSessionFixture f;

        f.DrawingSurface->BeginDrawMethod.SetExpectedCalls(1,
            [&] (const RECT* updateRect, REFIID iid, void** updateObject, POINT* updateOffset)
            {
                Assert::IsNull(updateRect);
                return f.DoBeginDraw(iid, updateObject, updateOffset);
            });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.Composition->CreateDrawingSession(f.DrawingSurface.Get(), &drawingSession));

        f.Check(drawingSession);
    }

    TEST_METHOD_EX(CanvasComposition_CreateDrawingSessionWithUpdateRect)
    {
        DrawingSessionFixture f;

        Rect anyRect{};
        anyRect.X = 1.1f;
        anyRect.Y = 2.1f;
        anyRect.Width = 3.1f;
        anyRect.Height = 4.4f;

        RECT anyRECT;
        anyRECT.left = 1;
        anyRECT.right = 4;      // (1.1 + 3.1) = (4.2) - Composition rounds, so this becomes 4
        anyRECT.top = 2;
        anyRECT.bottom = 7;     // (2.1 + 4.4) = (6.5) - Composition rounds, so this becomes 7.

        f.DrawingSurface->BeginDrawMethod.SetExpectedCalls(1,
            [&] (const RECT* updateRect, REFIID iid, void** updateObject, POINT* updateOffset)
            {
                Assert::IsNotNull(updateRect);
                Assert::AreEqual(anyRECT, *updateRect);
                return f.DoBeginDraw(iid, updateObject, updateOffset);
            });

        ComPtr<ICanvasDrawingSession> drawingSession;
        ThrowIfFailed(f.Composition->CreateDrawingSessionWithUpdateRect(f.DrawingSurface.Get(), anyRect, &drawingSession));

        f.Check(drawingSession);
    }

    TEST_METHOD_EX(CanvasComposition_SuspendDrawing_FailsWhenPassedNullParameters)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.Composition->SuspendDrawing(nullptr));
    }

    TEST_METHOD_EX(CanvasComposition_SuspendDrawing)
    {
        DrawingSurfaceFixture f;

        f.DrawingSurface->SuspendDrawMethod.SetExpectedCalls(1,
            []
            {
                return AnyErrorResult;
            });
        Assert::AreEqual(AnyErrorResult, f.Composition->SuspendDrawing(f.DrawingSurface.Get()));
    }

    TEST_METHOD_EX(CanvasComposition_ResumeDrawing_FailsWhenPassedNullParameters)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.Composition->ResumeDrawing(nullptr));
    }

    TEST_METHOD_EX(CanvasComposition_ResumeDrawing)
    {
        DrawingSurfaceFixture f;

        f.DrawingSurface->ResumeDrawMethod.SetExpectedCalls(1,
            []
            {
                return AnyErrorResult;
            });
        Assert::AreEqual(AnyErrorResult, f.Composition->ResumeDrawing(f.DrawingSurface.Get()));
    }

    TEST_METHOD_EX(CanvasComposition_Resize_FailsWhenPassedNullParameters)
    {
        DrawingSurfaceFixture f;

        Assert::AreEqual(E_INVALIDARG, f.Composition->Resize(nullptr, Size{}));
    }

    TEST_METHOD_EX(CanvasComposition_Resize)
    {
        DrawingSurfaceFixture f;

        Size anySize{ 12.49f, 12.50f };
        SIZE expectedSIZE{ 12, 13 }; // Compositor rounds rather than truncates

        f.DrawingSurface->ResizeMethod.SetExpectedCalls(1,
            [&] (SIZE size)
            {
                Assert::AreEqual(expectedSIZE.cx, size.cx);
                Assert::AreEqual(expectedSIZE.cy, size.cy);
                return AnyErrorResult;
            });
        Assert::AreEqual(AnyErrorResult, f.Composition->Resize(f.DrawingSurface.Get(), anySize));
    }

    TEST_METHOD_EX(CanvasComposition_Scroll_FailsWhenPassedNullParameters)
    {
        DrawingSurfaceFixture f;

        IReference<Rect>* nullScrollRectIsValid = nullptr;
        IReference<Rect>* nullClipRectIsValid = nullptr;

        Assert::AreEqual(E_INVALIDARG, f.Composition->Scroll(nullptr, nullScrollRectIsValid, nullClipRectIsValid, Point{}));
    }

    struct ScrollFixture : public DrawingSurfaceFixture
    {
        Rect ScrollRectValue{ 1.1f, 1.4f, 2.3f, 2.1f };
        Rect ClipRectValue{ 3.4f, 3.4f, 4.1f, 4.0f };

        RECT ExpectedScrollRect{ 1, 1, 3, 4 };
        RECT ExpectedClipRect{ 3, 3, 8, 7 };
        
        ComPtr<IReference<Rect>> ScrollRect = Make<Nullable<Rect>>(ScrollRectValue);
        ComPtr<IReference<Rect>> ClipRect = Make<Nullable<Rect>>(ClipRectValue);

        void Test(bool scroll, bool clip)
        {
            Point offset{ -1.5f, 1.5f };
            int expectedOffsetX = -2;
            int expectedOffsetY = 2;
            
            DrawingSurface->ScrollMethod.SetExpectedCalls(1,
                [=] (RECT const* scrollRect, RECT const* clipRect, int offsetX, int offsetY)
                {
                    if (scroll)
                        Assert::AreEqual(ExpectedScrollRect, *scrollRect);
                    else
                        Assert::IsNull(scrollRect);

                    if (clip)
                        Assert::AreEqual(ExpectedClipRect, *clipRect);
                    else
                        Assert::IsNull(clipRect);

                    Assert::AreEqual(expectedOffsetX, offsetX);
                    Assert::AreEqual(expectedOffsetY, offsetY);

                    return AnyErrorResult;
                });

            Assert::AreEqual(AnyErrorResult, Composition->Scroll(
                DrawingSurface.Get(),
                scroll ? ScrollRect.Get() : nullptr,
                clip   ? ClipRect.Get()   : nullptr,
                offset));
        }
    };

    TEST_METHOD_EX(CanvasComposition_Scroll_NoRects)
    {
        ScrollFixture f;
        f.Test(false, false);
    }

    TEST_METHOD_EX(CanvasComposition_Scroll_ScrollRect)
    {
        ScrollFixture f;
        f.Test(true, false);
    }

    TEST_METHOD_EX(CanvasComposition_Scroll_ClipRect)
    {
        ScrollFixture f;
        f.Test(false, true);
    }

    TEST_METHOD_EX(CanvasComposition_Scroll_ScrollRectAndClipRect)
    {
        ScrollFixture f;
        f.Test(true, true);
    }

    TEST_METHOD_EX(CanvasComposition_When_CompositionApiNotPresent_CannotActivate)
    {
        auto apiInformation = ApiInformationTestAdapter::Create();

        ExpectHResultException(E_NOTIMPL, [] { GetCompositionStatics(); });
    }
};

#endif
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include <lib/printing/CanvasPreviewEventArgs.h>
#include <lib/printing/CanvasPrintDocument.h>
#include <lib/printing/CanvasPrintDocumentAdapter.h>
#include <lib/printing/CanvasPrintEventArgs.h>
#include <lib/printing/CanvasPrintTaskOptionsChangedEventArgs.h>

#include "../mocks/MockPrintControl.h"
#include "../mocks/MockPrintDocumentPackageTarget.h"
#include "../mocks/MockPrintPreviewDxgiPackageTarget.h"
#include "../mocks/MockPrintTaskOptions.h"

using namespace ABI::Microsoft::Graphics::Canvas::Printing;


static uint32_t const AnyPageNumber = 123;
static float const AnyWidth = 12.0f;
static float const AnyHeight = 34.0f;
static float const AnyDpi = 120.0f;

class TestPrintDocumentAdapter : public CanvasPrintDocumentAdapter
{
public:
    ComPtr<StubDispatcher> Dispatcher;
    ComPtr<MockCanvasDevice> SharedDevice;
    float Dpi;
    
    TestPrintDocumentAdapter()
        : Dispatcher(Make<StubDispatcher>())
        , SharedDevice(Make<MockCanvasDevice>())
        , Dpi(AnyDpi)
    {
        SharedDevice->CreateRenderTargetBitmapMethod.AllowAnyCall(
            [&] (float, float, float, DirectXPixelFormat, CanvasAlphaMode)
            {
                return Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);
            });

        SharedDevice->CreateDeviceContextForDrawingSessionMethod.AllowAnyCall(
            [=]
            {
                return Make<StubD2DDeviceContext>(nullptr);
            });
    }

    virtual ComPtr<ICanvasDevice> GetSharedDevice() override
    {
        return SharedDevice;
    }

    virtual ComPtr<ICoreDispatcher> GetDispatcherForCurrentThread() override
    {
        return Dispatcher;
    }

    virtual float GetLogicalDpi() override
    {
        return Dpi;
    }

    virtual bool ShouldWaitForUIThread() override
    {
        // In product code, Paginate, MakePage and MakeDocument block until the
        // UI thread has completed.
        //
        // This makes it hard to test, so we disable this behavior in these
        // tests.
        return false;
    }

    void RunNextAction()
    {
        Dispatcher->Tick();
    }
};


TEST_CLASS(CanvasPrintDocumentUnitTests)
{
    struct Fixture
    {
        std::shared_ptr<TestPrintDocumentAdapter> Adapter;
        ComPtr<CanvasPrintDocumentFactory> Factory;

        ComPtr<MockPrintTaskOptions> AnyPrintTaskOptions;

        Fixture()
            : AnyPrintTaskOptions(Make<MockPrintTaskOptions>())
        {
            Adapter = std::make_shared<TestPrintDocumentAdapter>();
            CanvasPrintDocumentAdapter::SetInstance(Adapter);

            Factory = Make<CanvasPrintDocumentFactory>();

            AnyPrintTaskOptions->GetPageDescriptionMethod.AllowAnyCall();
        }

        ComPtr<ICanvasPrintDocument> Create()
        {
            ComPtr<IInspectable> docInsp;
            ThrowIfFailed(Factory->ActivateInstance(&docInsp));
            
            return As<ICanvasPrintDocument>(docInsp);
        }

        Fixture(Fixture const&) = delete;
        Fixture& operator=(Fixture const&) = delete;
    };

    TEST_METHOD_EX(CanvasPrintDocument_DefaultActivation_FailsWhenNoDispatcher)
    {
        Fixture f;
        f.Adapter->Dispatcher.Reset();

        ComPtr<IInspectable> docInsp;
        Assert::AreEqual(RPC_E_WRONG_THREAD, f.Factory->ActivateInstance(&docInsp));
        ValidateStoredErrorState(RPC_E_WRONG_THREAD, Strings::CanvasPrintDocumentMustBeConstructedOnUIThread);
    }

    TEST_METHOD_EX(CanvasPrintDocument_DefaultActivation_UsesSharedDevice)
    {
        Fixture f;

        auto doc = As<ICanvasResourceCreator>(f.Create());

        ComPtr<ICanvasDevice> retrievedDevice;
        ThrowIfFailed(doc->get_Device(&retrievedDevice));

        Assert::IsTrue(IsSameInstance(f.Adapter->SharedDevice.Get(), retrievedDevice.Get()));        
    }

    TEST_METHOD_EX(CanvasPrintDocument_CreateWithDevice_UsesProvidedDevice)
    {
        Fixture f;

        auto device = Make<MockCanvasDevice>();

        ComPtr<ICanvasPrintDocument> doc;
        ThrowIfFailed(f.Factory->CreateWithDevice(device.Get(), &doc));

        ComPtr<ICanvasDevice> retrievedDevice;
        ThrowIfFailed(As<ICanvasResourceCreator>(doc)->get_Device(&retrievedDevice));

        Assert::IsTrue(IsSameInstance(device.Get(), retrievedDevice.Get()));
    }

    TEST_METHOD_EX(CanvasPrintDocument_CreateWithDevice_FailsIfPassedInvalidParams)        
    {
        Fixture f;
        
        auto device = Make<MockCanvasDevice>();
        ComPtr<ICanvasPrintDocument> doc;

        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithDevice(nullptr, &doc));
        Assert::AreEqual(E_INVALIDARG, f.Factory->CreateWithDevice(device.Get(), nullptr));
    }

    TEST_METHOD_EX(CanvasPrintDocument_GetPreviewPageCollection_FailsIfPassedInvalidParams)
    {
        Fixture f;

        auto doc = As<IPrintDocumentPageSource>(f.Create());

        ComPtr<IPrintPreviewPageCollection> c;

        Assert::AreEqual(E_INVALIDARG, doc->GetPreviewPageCollection(nullptr, &c));
        Assert::AreEqual(E_INVALIDARG, doc->GetPreviewPageCollection(Make<MockPrintDocumentPackageTarget>().Get(), nullptr));
    }

    struct PrintPreviewFixture : public Fixture
    {
        ComPtr<MockPrintPreviewDxgiPackageTarget> PreviewTarget;

        ComPtr<ICanvasPrintDocument> Doc;
        ComPtr<IPrintPreviewPageCollection> PageCollection;

        MockEventHandler<CanvasPrintTaskOptionsChangedHandler> PrintTaskOptionsChangedHandler;
        MockEventHandler<CanvasPreviewHandler> PreviewHandler;
        
        PrintPreviewFixture()
            : Doc(Create())
            , PreviewTarget(Make<MockPrintPreviewDxgiPackageTarget>())
            , PrintTaskOptionsChangedHandler(L"PrintTaskOptionsChangedHandler")
            , PreviewHandler(L"PreviewHandler")
        {            
            PreviewTarget->DrawPageMethod.AllowAnyCall();

            auto target = Make<MockPrintDocumentPackageTarget>();
            target->GetPackageTargetMethod.SetExpectedCalls(1,
                [=] (const GUID& type, const IID& iid, void** ppv)
                {
                    Assert::AreEqual(ID_PREVIEWPACKAGETARGET_DXGI, type);
                    return PreviewTarget.CopyTo(iid, ppv);
                });

            ThrowIfFailed(As<IPrintDocumentPageSource>(Doc)->GetPreviewPageCollection(target.Get(), &PageCollection));
        }

        EventRegistrationToken RegisterPrintTaskOptionsChanged()
        {
            EventRegistrationToken token;
            ThrowIfFailed(Doc->add_PrintTaskOptionsChanged(PrintTaskOptionsChangedHandler.Get(), &token));
            return token;
        }

        EventRegistrationToken RegisterPreview()
        {
            EventRegistrationToken token;
            ThrowIfFailed(Doc->add_Preview(PreviewHandler.Get(), &token));
            return token;
        }
    };

    TEST_METHOD_EX(CanvasPrintDocument_GetPreviewPageCollection_ReturnsCollection)
    {
        PrintPreviewFixture f;
        Assert::IsTrue(f.PageCollection);
    }

    TEST_METHOD_EX(CanvasPrintDocument_When_InvalidatePreview_CalledBeforePreviewing_ItIsNoOp)
    {
        Fixture f;
        auto doc = f.Create();

        ThrowIfFailed(doc->InvalidatePreview());
    }

    TEST_METHOD_EX(CanvasPrintDocument_InvalidatePreview_ForwardsToPreviewTarget)
    {
        PrintPreviewFixture f;

        f.PreviewTarget->InvalidatePreviewMethod.SetExpectedCalls(1);
        ThrowIfFailed(f.Doc->InvalidatePreview());
    }

    TEST_METHOD_EX(CanvasPrintDocument_When_SetPageCount_CalledBeforePreviewing_ItFails)
    {
        Fixture f;
        auto doc = f.Create();

        Assert::AreEqual(E_FAIL, doc->SetPageCount(1));
        ValidateStoredErrorState(E_FAIL, Strings::SetPageCountCalledBeforePreviewing);
    }

    TEST_METHOD_EX(CanvasPrintDocument_When_SetIntermediatePageCount_CalledBeforePreviewing_ItFails)
    {
        Fixture f;
        auto doc = f.Create();

        Assert::AreEqual(E_FAIL, doc->SetIntermediatePageCount(1));
        ValidateStoredErrorState(E_FAIL, Strings::SetPageCountCalledBeforePreviewing);
    }

    TEST_METHOD_EX(CanvasPrintDocument_SetPageCount_ForwardsToPreviewTarget)
    {
        PrintPreviewFixture f;

        f.PreviewTarget->SetJobPageCountMethod.SetExpectedCalls(1,
            [&] (PageCountType t, UINT32 c)
            {
                Assert::AreEqual<int>(PageCountType::FinalPageCount, t);
                Assert::AreEqual(c, (uint32_t)AnyPageNumber);
                return S_OK;
            });
        ThrowIfFailed(f.Doc->SetPageCount(AnyPageNumber));
    }

    TEST_METHOD_EX(CanvasPrintDocument_SetIntermediatePageCount_ForwardsToPreviewTarget)
    {
        PrintPreviewFixture f;

        f.PreviewTarget->SetJobPageCountMethod.SetExpectedCalls(1,
            [&] (PageCountType t, UINT32 c)
            {
                Assert::AreEqual<int>(PageCountType::IntermediatePageCount, t);
                Assert::AreEqual(c, (uint32_t)AnyPageNumber);
                return S_OK;
            });
        ThrowIfFailed(f.Doc->SetIntermediatePageCount(AnyPageNumber));
    }

    TEST_METHOD_EX(CanvasPrintDocument_WhenPaginateCalled_PrintTaskOptionsChangedIsRaised)
    {
        PrintPreviewFixture f;

        f.RegisterPrintTaskOptionsChanged();

        // The call to Paginate is meant to just queue up the work, so we don't
        // expect the event to be raised until we call RunNextAction.
        ThrowIfFailed(f.PageCollection->Paginate(static_cast<uint32_t>(AnyPageNumber), f.AnyPrintTaskOptions.Get()));

        f.PrintTaskOptionsChangedHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument* sender, ICanvasPrintTaskOptionsChangedEventArgs* args)
            {
                Assert::IsTrue(IsSameInstance(sender, f.Doc.Get()));
                Assert::IsNotNull(args);

                uint32_t currentPreviewPageNumber = 0;
                ThrowIfFailed(args->get_CurrentPreviewPageNumber(&currentPreviewPageNumber));
                Assert::AreEqual(AnyPageNumber, currentPreviewPageNumber);

                uint32_t newPreviewPageNumber = 0;
                ThrowIfFailed(args->get_NewPreviewPageNumber(&newPreviewPageNumber));
                Assert::AreEqual(1U, newPreviewPageNumber);

                ComPtr<IPrintTaskOptionsCore> retrievedPrintTaskOptions;
                ThrowIfFailed(args->get_PrintTaskOptions(&retrievedPrintTaskOptions));
                Assert::IsTrue(IsSameInstance(f.AnyPrintTaskOptions.Get(), retrievedPrintTaskOptions.Get()));
                
                return S_OK;
            });

        f.Adapter->RunNextAction();
    }

    TEST_METHOD_EX(CanvasPrintDocument_WhenPaginateCalled_WithPageSetToMinusOne_PrintTaskOptionsIsRaisedWithCurrentPageSetToOne)
    {
        PrintPreviewFixture f;
        
        f.RegisterPrintTaskOptionsChanged();

        //
        // The print system will call Paginate with JOB_PAGE_APPLICATION_DEFINED
        // if this is the first time it has been called (and so no page is
        // currently displayed).
        //
        // Win2D massages this to be '1' in this case.
        //
        ThrowIfFailed(f.PageCollection->Paginate(JOB_PAGE_APPLICATION_DEFINED, f.AnyPrintTaskOptions.Get()));

        f.PrintTaskOptionsChangedHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument* sender, ICanvasPrintTaskOptionsChangedEventArgs* args)
            {
                uint32_t currentPreviewPageNumber = 0;
                ThrowIfFailed(args->get_CurrentPreviewPageNumber(&currentPreviewPageNumber));
                Assert::AreEqual(1U, currentPreviewPageNumber);
                return S_OK;
            });

        f.Adapter->RunNextAction();
    }

    TEST_METHOD_EX(CanvasPrintDocument_WhenPrintTaskOptionsChangedIsUnregistered_ItIsNotCalled)
    {
        PrintPreviewFixture f;

        auto token = f.RegisterPrintTaskOptionsChanged();
        ThrowIfFailed(f.Doc->remove_PrintTaskOptionsChanged(token));

        ThrowIfFailed(f.PageCollection->Paginate(AnyPageNumber, f.AnyPrintTaskOptions.Get()));
        f.Adapter->RunNextAction();        
    }

    TEST_METHOD_EX(CanvasPrintDocument_add_PrintTaskOptionsChanged_FailsWithBadParams)
    {
        PrintPreviewFixture f;

        EventRegistrationToken token;
        Assert::AreEqual(E_INVALIDARG, f.Doc->add_PrintTaskOptionsChanged(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, f.Doc->add_PrintTaskOptionsChanged(f.PrintTaskOptionsChangedHandler.Get(), nullptr));
    }

    TEST_METHOD_EX(CanvasPrintDocument_WhenMakePageCalled_PreviewIsRaised)
    {
        PrintPreviewFixture f;

        f.RegisterPreview();

        // The system will always call Paginate before MakePage.
        ThrowIfFailed(f.PageCollection->Paginate(AnyPageNumber, f.AnyPrintTaskOptions.Get()));
        f.Adapter->RunNextAction();
        
        ThrowIfFailed(f.PageCollection->MakePage(AnyPageNumber, AnyWidth, AnyHeight));

        f.PreviewHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument* sender, ICanvasPreviewEventArgs* args)
            {
                Assert::IsTrue(IsSameInstance(f.Doc.Get(), sender));
                Assert::IsNotNull(args);

                uint32_t pageNumber;
                ThrowIfFailed(args->get_PageNumber(&pageNumber));
                Assert::AreEqual(AnyPageNumber, pageNumber);

                ComPtr<IPrintTaskOptionsCore> retrievedPrintTaskOptions;
                ThrowIfFailed(args->get_PrintTaskOptions(&retrievedPrintTaskOptions));
                Assert::IsTrue(IsSameInstance(f.AnyPrintTaskOptions.Get(), retrievedPrintTaskOptions.Get()));

                return S_OK;
            });

        f.Adapter->RunNextAction();
    }

    TEST_METHOD_EX(CanvasPrintDocument_WhenMakePageCalled_WithJobPageAppDefined_NewPreviewPageNumberUsed)
    {
        PrintPreviewFixture f;

        f.RegisterPrintTaskOptionsChanged();
        f.RegisterPreview();
        
        ThrowIfFailed(f.PageCollection->Paginate(JOB_PAGE_APPLICATION_DEFINED, f.AnyPrintTaskOptions.Get()));

        f.PrintTaskOptionsChangedHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument* sender, ICanvasPrintTaskOptionsChangedEventArgs* args)
            {
                ThrowIfFailed(args->put_NewPreviewPageNumber(AnyPageNumber));
                return S_OK;
            });

        f.Adapter->RunNextAction();
        
        ThrowIfFailed(f.PageCollection->MakePage(JOB_PAGE_APPLICATION_DEFINED, AnyWidth, AnyHeight));

        f.PreviewHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument* sender, ICanvasPreviewEventArgs* args)
            {
                uint32_t pageNumber;
                ThrowIfFailed(args->get_PageNumber(&pageNumber));
                Assert::AreEqual(AnyPageNumber, pageNumber);
                return S_OK;
            });

        f.Adapter->RunNextAction();        
    }

    struct MakePageFixture : public PrintPreviewFixture
    {
        float ExpectedBitmapDpi;
        ComPtr<StubD2DBitmap> D2DBitmap;

        MakePageFixture()
        {
            RegisterPreview();
            
            float pageWidth = 100;
            float pageHeight = 200;

            auto printPageDescription = PrintPageDescription{
                Size{ pageWidth, pageHeight },
                Rect{ 0, 0, pageWidth, pageHeight }, // ImageableRect
                (uint32_t)AnyDpi, (uint32_t)AnyDpi   // DPI X, Y
            };

            auto printTaskOptions = Make<MockPrintTaskOptions>();
            printTaskOptions->GetPageDescriptionMethod.SetExpectedCalls(1,
                [=] (uint32_t page, PrintPageDescription* outDesc)
                {
                    Assert::AreEqual(AnyPageNumber, page);
                    *outDesc = printPageDescription;
                    return S_OK;
                });

            ThrowIfFailed(PageCollection->Paginate(static_cast<uint32_t>(AnyPageNumber), printTaskOptions.Get()));
            Adapter->RunNextAction();

            float previewScale = 0.5f;

            float displayWidth = pageWidth * previewScale;
            float displayHeight = pageHeight * previewScale;

            ThrowIfFailed(PageCollection->MakePage(AnyPageNumber, displayWidth, displayHeight));

            ExpectedBitmapDpi = Adapter->Dpi * previewScale;

            D2DBitmap = Make<StubD2DBitmap>(D2D1_BITMAP_OPTIONS_TARGET);

            Adapter->SharedDevice->CreateRenderTargetBitmapMethod.SetExpectedCalls(1,
                [=] (float width, float height, float dpi, DirectXPixelFormat format, CanvasAlphaMode alpha)
                {
                    // The width/height of the RT should be the same as the
                    // width/height of the page (since we've adjusted the DPI so
                    // that pageSize * DPI = previewSizeInPixels.
                    Assert::AreEqual(pageWidth, width);
                    Assert::AreEqual(pageHeight, height);

                    Assert::AreEqual(ExpectedBitmapDpi, dpi);

                    Assert::AreEqual(PIXEL_FORMAT(B8G8R8A8UIntNormalized), format);
                    Assert::AreEqual(CanvasAlphaMode::Premultiplied, alpha);

                    return D2DBitmap;
                });
        }

        void ExpectDrawPage()
        {
            PreviewTarget->DrawPageMethod.SetExpectedCalls(1,
                [=] (uint32_t pageNumber, IDXGISurface* dxgiSurface, float dpiX, float dpiY)
                {
                    Assert::AreEqual<int>(AnyPageNumber, pageNumber);
                    Assert::AreEqual(ExpectedBitmapDpi, dpiX);
                    Assert::AreEqual(ExpectedBitmapDpi, dpiY);

                    ComPtr<IDXGISurface> expectedDxgiSurface;
                    ThrowIfFailed(D2DBitmap->GetSurface(&expectedDxgiSurface));
                    Assert::IsTrue(IsSameInstance(expectedDxgiSurface.Get(), dxgiSurface));

                    return S_OK;
                });
        }
    };

    TEST_METHOD_EX(CanvasPrintDocument_WhenMakePageCalled_PreviewIsDrawn)
    {
        // To draw the preview:
        //
        // - a DXGI surface of the correct size must be created
        //
        // - preview handler must be called with a drawing session,
        //   appropriately configured
        //
        // - IPrintPreviewDxgiPackageTarget::DrawPage() must be called with the
        //   DXGI surface and the correct DPI values

        MakePageFixture f;
        
        f.PreviewHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument*, ICanvasPreviewEventArgs* args)
            {
                ComPtr<ICanvasDrawingSession> ds;
                ThrowIfFailed(args->get_DrawingSession(&ds));

                // This drawing session should be pointing at the render target
                // that was created
                auto deviceContext = GetWrappedResource<ID2D1DeviceContext>(ds);
                ComPtr<ID2D1Image> currentTarget;
                deviceContext->GetTarget(&currentTarget);
                Assert::IsTrue(IsSameInstance(f.D2DBitmap.Get(), currentTarget.Get()));
                
                return S_OK;
            });

        f.ExpectDrawPage();

        f.Adapter->RunNextAction();        
    }

    TEST_METHOD_EX(CanvasPrintDocument_WhenMakePageIsCalled_AndPreviewHandlerIsDeferred_PreviewIsDrawnWhenDeferralCompletes)
    {
        MakePageFixture f;

        ComPtr<ICanvasPrintDeferral> deferral;

        f.PreviewHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument*, ICanvasPreviewEventArgs* args)
            {
                ThrowIfFailed(args->GetDeferral(&deferral));
                return S_OK;
            });

        f.Adapter->RunNextAction();

        ThrowIfFailed(deferral->Complete());

        f.ExpectDrawPage();
        f.Adapter->RunNextAction();        
    }

    TEST_METHOD_EX(CanvasPrintDocument_WhenPreviewIsUnregistered_ItIsNotCalled)
    {
        PrintPreviewFixture f;

        auto token = f.RegisterPreview();
        ThrowIfFailed(f.Doc->remove_Preview(token));

        ThrowIfFailed(f.PageCollection->Paginate(AnyPageNumber, f.AnyPrintTaskOptions.Get()));
        f.Adapter->RunNextAction();

        ThrowIfFailed(f.PageCollection->MakePage(AnyPageNumber, AnyWidth, AnyHeight));
        f.Adapter->RunNextAction();
    }

    TEST_METHOD_EX(CanvasPrintDocument_add_Preview_FailsWithBadParams)
    {
        PrintPreviewFixture f;

        EventRegistrationToken token;
        Assert::AreEqual(E_INVALIDARG, f.Doc->add_Preview(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, f.Doc->add_Preview(f.PreviewHandler.Get(), nullptr));
    }

    TEST_METHOD_EX(CanvasPrintDocument_When_PrintTaskOptionsChanged_IsDeferred_Preview_IsNotRaised_UntilItCompletes)
    {
        PrintPreviewFixture f;

        f.RegisterPrintTaskOptionsChanged();
        f.RegisterPreview();

        ComPtr<ICanvasPrintDeferral> deferral;
        ComPtr<ICanvasPrintTaskOptionsChangedEventArgs> optionsChangedEventArgs;
        f.PrintTaskOptionsChangedHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument*, ICanvasPrintTaskOptionsChangedEventArgs* args)
            {
                ThrowIfFailed(args->GetDeferral(&deferral));
                optionsChangedEventArgs = args;

                ThrowIfFailed(args->put_NewPreviewPageNumber(AnyPageNumber));

                return S_OK;
            });

        ThrowIfFailed(f.PageCollection->Paginate(static_cast<uint32_t>(AnyPageNumber), f.AnyPrintTaskOptions.Get()));
        f.Adapter->RunNextAction();

        f.PrintTaskOptionsChangedHandler.SetExpectedCalls(0);

        // The call to MakePage would normally raise the Preview event, but
        // because we've taken out the deferral it will wait until that has
        // completed.

        ThrowIfFailed(f.PageCollection->MakePage(JOB_PAGE_APPLICATION_DEFINED, AnyWidth, AnyHeight));
        f.Adapter->RunNextAction();
        f.Adapter->RunNextAction();
        f.Adapter->RunNextAction();

        // Set the new page number, outside of the event handler.  This should
        // only get picked up when the deferral completes.
        ThrowIfFailed(optionsChangedEventArgs->put_NewPreviewPageNumber(AnyPageNumber + 1));

        // Now complete the deferral.  This just queues something to the
        // dispatcher so we don't expect anything to happen immediately.
        ThrowIfFailed(deferral->Complete());
        f.Adapter->RunNextAction();

        // The pending call to the preview handler should now execute.
        f.PreviewHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument*, ICanvasPreviewEventArgs* args)
            {
                uint32_t pageNumber;
                ThrowIfFailed(args->get_PageNumber(&pageNumber));

                // The page number should match the one most recently set before
                // the deferral was completed
                Assert::AreEqual(AnyPageNumber + 1, pageNumber);

                return S_OK;
            });

        f.Adapter->RunNextAction();
        f.PreviewHandler.SetExpectedCalls(0);

        // We don't expect any other events to be raised after this
        f.Adapter->RunNextAction();
    }

    struct PrintFixture : public Fixture
    {
        ComPtr<ICanvasPrintDocument> Doc;
        
        MockEventHandler<CanvasPrintHandler> PrintHandler;

        ComPtr<MockPrintDocumentPackageTarget> AnyTarget;
        ComPtr<MockPrintControl> PrintControl;

        PrintFixture()
            : Doc(Create())
            , PrintHandler(L"PrintHandler")
            , AnyTarget(Make<MockPrintDocumentPackageTarget>())
            , PrintControl(Make<MockPrintControl>())
        {
            Adapter->SharedDevice->CreateCommandListMethod.AllowAnyCall(
                [=]
                {
                    auto cl = Make<MockD2DCommandList>();
                    cl->CloseMethod.SetExpectedCalls(1);
                    return cl;
                });
        }

        EventRegistrationToken RegisterPrint()
        {
            EventRegistrationToken token;
            ThrowIfFailed(Doc->add_Print(PrintHandler.Get(), &token));
            return token;
        }
    };

    TEST_METHOD_EX(CanvasPrintDocument_WhenMakeDocumentCalled_PrintEventIsRaised)
    {
        PrintFixture f;
        f.RegisterPrint();

        ThrowIfFailed(As<IPrintDocumentPageSource>(f.Doc)->MakeDocument(
            f.AnyPrintTaskOptions.Get(),
            f.AnyTarget.Get()));

        f.PrintHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument* doc, ICanvasPrintEventArgs* args)
            {
                Assert::IsTrue(IsSameInstance(f.Doc.Get(), doc));

                ComPtr<IPrintTaskOptionsCore> retrievedPrintTaskOptions;
                ThrowIfFailed(args->get_PrintTaskOptions(&retrievedPrintTaskOptions));
                Assert::IsTrue(IsSameInstance(f.AnyPrintTaskOptions.Get(), retrievedPrintTaskOptions.Get()));
                
                return S_OK;
            });

        f.Adapter->RunNextAction();        
    }

    TEST_METHOD_EX(CanvasPrintDocument_PrintEvent_InitialDpiValueMatchesFirstPageDpi)
    {
        PrintFixture f;
        f.RegisterPrint();

        PrintPageDescription printPageDescription {
            Size{ AnyWidth, AnyHeight },
            Rect{ 0, 0, AnyWidth, AnyHeight }, // ImageableRect
            (uint32_t)AnyDpi, (uint32_t)AnyDpi // DPI X, Y
        };

        auto printTaskOptions = Make<MockPrintTaskOptions>();
        printTaskOptions->GetPageDescriptionMethod.SetExpectedCalls(1,
            [&] (uint32_t page, PrintPageDescription* outDesc)
            {
                Assert::AreEqual(1U, page);
                *outDesc = printPageDescription;
                return S_OK;
            });

        ThrowIfFailed(As<IPrintDocumentPageSource>(f.Doc)->MakeDocument(
            printTaskOptions.Get(),
            f.AnyTarget.Get()));

        f.PrintHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument*, ICanvasPrintEventArgs* args)
            {
                float dpi;
                ThrowIfFailed(args->get_Dpi(&dpi));

                Assert::AreEqual(AnyDpi, dpi);
                return S_OK;
            });

        f.Adapter->RunNextAction();        
    }

    TEST_METHOD_EX(CanvasPrintDocument_PrintEvent_CreateDrawingSession_CreatesPrintControlAndClosesItWhenDone)
    {
        //
        // This test verifies that the CanvasPrintEventArgs is hooked up to the
        // right CanvasDevice / IPrintDocumentPackageTarget.  The
        // CanvasPrintEventArgsUnitTests below exercise more of the interactions
        // with these.
        //
        
        PrintFixture f;
        f.RegisterPrint();
        
        ThrowIfFailed(As<IPrintDocumentPageSource>(f.Doc)->MakeDocument(
            f.AnyPrintTaskOptions.Get(),
            f.AnyTarget.Get()));

        f.PrintHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument*, ICanvasPrintEventArgs* args)
            {
                f.Adapter->SharedDevice->CreatePrintControlMethod.SetExpectedCalls(1,
                    [&] (IPrintDocumentPackageTarget* target, float)
                    {
                        Assert::IsTrue(IsSameInstance(f.AnyTarget.Get(), target));
                        return f.PrintControl;
                    });

                ComPtr<ICanvasDrawingSession> ds;
                ThrowIfFailed(args->CreateDrawingSession(&ds));

                f.PrintControl->AddPageMethod.SetExpectedCalls(1);
                f.PrintControl->CloseMethod.SetExpectedCalls(1);

                return S_OK;
            });

        f.Adapter->RunNextAction();        
    }

    TEST_METHOD_EX(CanvasPrintDocument_PrintEvent_CanBeDeferred)
    {        
        PrintFixture f;
        f.RegisterPrint();
        
        ThrowIfFailed(As<IPrintDocumentPageSource>(f.Doc)->MakeDocument(
            f.AnyPrintTaskOptions.Get(),
            f.AnyTarget.Get()));

        ComPtr<ICanvasPrintDeferral> deferral;
        ComPtr<ICanvasDrawingSession> ds;

        f.PrintHandler.SetExpectedCalls(1,
            [&] (ICanvasPrintDocument*, ICanvasPrintEventArgs* args)
            {
                f.Adapter->SharedDevice->CreatePrintControlMethod.SetExpectedCalls(1,
                    [&] (IPrintDocumentPackageTarget* target, float)
                    {
                        Assert::IsTrue(IsSameInstance(f.AnyTarget.Get(), target));
                        return f.PrintControl;
                    });

                ThrowIfFailed(args->CreateDrawingSession(&ds));

                ThrowIfFailed(args->GetDeferral(&deferral));

                return S_OK;
            });

        f.Adapter->RunNextAction();        

        // Closing the drawing session will immediately AddPage
        f.PrintControl->AddPageMethod.SetExpectedCalls(1);
        ds.Reset();

        // But the print control won't be closed until the deferral completes

        f.PrintControl->CloseMethod.SetExpectedCalls(1);
        ThrowIfFailed(deferral->Complete());
        f.Adapter->RunNextAction();        
    }

    TEST_METHOD_EX(CanvasPrintDocument_PrintIsUnregistered_ItIsNotCalled)
    {
        PrintFixture f;

        auto token = f.RegisterPrint();
        ThrowIfFailed(f.Doc->remove_Print(token));

        ThrowIfFailed(As<IPrintDocumentPageSource>(f.Doc)->MakeDocument(
            f.AnyPrintTaskOptions.Get(),
            f.AnyTarget.Get()));
        
        f.Adapter->RunNextAction();        
    }

    TEST_METHOD_EX(CanvasPrintDocument_add_Print_FailsWithBadParams)
    {
        PrintFixture f;

        EventRegistrationToken token;
        Assert::AreEqual(E_INVALIDARG, f.Doc->add_Print(nullptr, &token));
        Assert::AreEqual(E_INVALIDARG, f.Doc->add_Print(f.PrintHandler.Get(), nullptr));
    }
};


struct DeferrableFixture
{
    ComPtr<MockDispatcher> Dispatcher;
    DeferrableTaskScheduler Scheduler;
    std::unique_ptr<DeferrableTask> Task;

    DeferrableFixture()
        : Dispatcher(Make<MockDispatcher>())
        , Scheduler(Dispatcher)
    {
        Task = Scheduler.CreateTask(
            [=] (DeferrableTask* task)
            {
            });
    }

    template<typename ARGS>
    void VerifySecondCallToGetDeferralFails(ComPtr<ARGS> const& args)
    {
        ComPtr<ICanvasPrintDeferral> deferral[2];

        ThrowIfFailed(args->GetDeferral(&deferral[0]));

        Assert::AreEqual(E_FAIL, args->GetDeferral(&deferral[1]));
        ValidateStoredErrorState(E_FAIL, Strings::CanvasPrintDocumentGetDeferralMayOnlyBeCalledOnce);        
    }


    template<typename ARGS>
    void VerifySecondCallToDeferralCompleteFails(ComPtr<ARGS> const& args)
    {
        // The scheduler needs to think that it is running this task
        Dispatcher->RunAsyncMethod.SetExpectedCalls(1);
        Scheduler.Schedule(std::move(Task));
        
        ComPtr<ICanvasPrintDeferral> deferral;
        ThrowIfFailed(args->GetDeferral(&deferral));

        Dispatcher->RunAsyncMethod.SetExpectedCalls(1);
        ThrowIfFailed(deferral->Complete());

        Assert::AreEqual(E_FAIL, deferral->Complete());
        ValidateStoredErrorState(E_FAIL, Strings::CanvasPrintDocumentDeferralCompleteMayOnlyBeCalledOnce);
    }
};


TEST_CLASS(CanvasPrintTaskOptionsChangedEventArgsUnitTests)
{
    struct Fixture : public DeferrableFixture
    {
        ComPtr<MockPrintTaskOptions> AnyPrintTaskOptions;
        ComPtr<CanvasPrintTaskOptionsChangedEventArgs> Args;

        Fixture()
            : AnyPrintTaskOptions(Make<MockPrintTaskOptions>())
            , Args(Make<CanvasPrintTaskOptionsChangedEventArgs>(Task.get(), AnyPageNumber, AnyPrintTaskOptions))
        {
        }
    };

    TEST_METHOD_EX(CanvasPrintTaskOptionsChangedEventArgs_Getters_FailWithBadParams)
    {
        Fixture f;
        Assert::AreEqual(E_INVALIDARG, f.Args->get_CurrentPreviewPageNumber(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->get_NewPreviewPageNumber(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->GetDeferral(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->get_PrintTaskOptions(nullptr));
    }

    TEST_METHOD_EX(CanvasPrintTaskOptionsChangedEventArgs_NewPreviewPageNumber_MustBeGreaterThanOrEqualToOne)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.Args->put_NewPreviewPageNumber(0));
        Assert::AreEqual(S_OK, f.Args->put_NewPreviewPageNumber(1));
        Assert::AreEqual(S_OK, f.Args->put_NewPreviewPageNumber(10));
    }

    TEST_METHOD_EX(CanvasPrintTaskOptionsChangedEventArgs_When_GetDeferralCalledMultipleTimes_SecondCallFails)
    {
        Fixture f;
        f.VerifySecondCallToGetDeferralFails(f.Args);
    }

    TEST_METHOD_EX(CanvasPrintTaskOptionsChangedEventArgs_When_DeferralCompleteCalledMultipleTimes_SecondCallFails)
    {
        Fixture f;
        f.VerifySecondCallToDeferralCompleteFails(f.Args);
    }
};


TEST_CLASS(CanvasPreviewEventArgsUnitTests)
{
    struct Fixture : public DeferrableFixture
    {
        ComPtr<MockPrintTaskOptions> AnyPrintTaskOptions;
        ComPtr<MockCanvasDrawingSession> AnyDrawingSession;

        ComPtr<CanvasPreviewEventArgs> Args;

        Fixture()
            : AnyPrintTaskOptions(Make<MockPrintTaskOptions>())
            , AnyDrawingSession(Make<MockCanvasDrawingSession>())
            , Args(Make<CanvasPreviewEventArgs>(Task.get(), AnyPageNumber, AnyPrintTaskOptions, AnyDrawingSession))
        {
        }
    };

    TEST_METHOD_EX(CanvasPreviewEventArgs_Getters_FailWithBadParams)
    {
        Fixture f;
        
        Assert::AreEqual(E_INVALIDARG, f.Args->get_PageNumber(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->get_PrintTaskOptions(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->GetDeferral(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->get_DrawingSession(nullptr));
    }

    TEST_METHOD_EX(CanvasPreviewEventArgs_When_GetDeferralCalledMultipleTimes_SecondCallFails)
    {
        Fixture f;
        f.VerifySecondCallToGetDeferralFails(f.Args);
    }

    TEST_METHOD_EX(CanvasPreviewEventArgs_When_DeferralCompleteCalledMultipleTimes_SecondCallFails)
    {
        Fixture f;
        f.VerifySecondCallToDeferralCompleteFails(f.Args);
    }
};


TEST_CLASS(CanvasPrintEventArgsUnitTests)
{
    struct Fixture : public DeferrableFixture
    {
        ComPtr<MockCanvasDevice> Device;
        ComPtr<MockPrintTaskOptions> PrintTaskOptions;
        ComPtr<MockPrintDocumentPackageTarget> AnyTarget;
        ComPtr<MockPrintControl> PrintControl;

        ComPtr<CanvasPrintEventArgs> Args;

        Fixture()
            : Device(Make<MockCanvasDevice>())
            , PrintTaskOptions(Make<MockPrintTaskOptions>())
            , AnyTarget(Make<MockPrintDocumentPackageTarget>())
            , PrintControl(Make<MockPrintControl>())
            , Args(Make<CanvasPrintEventArgs>(Task.get(), Device, AnyTarget, PrintTaskOptions, AnyDpi))
        {
            Device->CreatePrintControlMethod.AllowAnyCall(
                [=] (IPrintDocumentPackageTarget*, float)
                {
                    return PrintControl;
                });

            Device->CreateCommandListMethod.AllowAnyCall(
                [=]
                {
                    auto cl = Make<MockD2DCommandList>();
                    cl->CloseMethod.SetExpectedCalls(1);
                    return cl;
                });

            Device->CreateDeviceContextForDrawingSessionMethod.AllowAnyCall(
                [=]
                {
                    return Make<StubD2DDeviceContext>(nullptr);
                });

            PrintTaskOptions->GetPageDescriptionMethod.AllowAnyCall();
            PrintControl->AddPageMethod.AllowAnyCall();
        }

    };

    TEST_METHOD_EX(CanvasPrintEventArgs_Getters_FailWithBadParams)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.Args->get_PrintTaskOptions(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->get_Dpi(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->GetDeferral(nullptr));
        Assert::AreEqual(E_INVALIDARG, f.Args->CreateDrawingSession(nullptr));
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_DpiCanBeModified)
    {
        Fixture f;

        float expectedDpi = AnyDpi * 2;

        ThrowIfFailed(f.Args->put_Dpi(expectedDpi));

        float retrievedDpi;
        ThrowIfFailed(f.Args->get_Dpi(&retrievedDpi));

        Assert::AreEqual(expectedDpi, retrievedDpi);
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_DpiMustBeGreaterThanZero)
    {
        Fixture f;

        Assert::AreEqual(E_INVALIDARG, f.Args->put_Dpi(0.0f));
        Assert::AreEqual(E_INVALIDARG, f.Args->put_Dpi(-FLT_EPSILON));
        Assert::AreEqual(E_INVALIDARG, f.Args->put_Dpi(-1000.0f));
        Assert::AreEqual(S_OK, f.Args->put_Dpi(FLT_EPSILON));
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_CreateDrawingSession_CreatesPrintControlWithTheCorrectDpi)
    {
        Fixture f;

        float expectedDpi = AnyDpi * 2;
        ThrowIfFailed(f.Args->put_Dpi(expectedDpi));

        f.Device->CreatePrintControlMethod.SetExpectedCalls(1,
            [&] (IPrintDocumentPackageTarget*, float dpi)
            {
                Assert::AreEqual(expectedDpi, dpi);
                return f.PrintControl;
            });

        ComPtr<ICanvasDrawingSession> ds;
        ThrowIfFailed(f.Args->CreateDrawingSession(&ds));
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_AfterFirstCreateDrawingSession_PutDpiFails)
    {
        Fixture f;

        ComPtr<ICanvasDrawingSession> ds;
        ThrowIfFailed(f.Args->CreateDrawingSession(&ds));

        Assert::AreEqual(E_FAIL, f.Args->put_Dpi(AnyDpi));
        ValidateStoredErrorState(E_FAIL, Strings::CanvasPrintEventArgsDpiCannotBeChangedAfterCreateDrawingSession);
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_CreateDrawingSession_ReturnsDrawingSessionWithCorrectDpi)
    {
        Fixture f;

        ThrowIfFailed(f.Args->put_Dpi(AnyDpi));

        ComPtr<ICanvasDrawingSession> ds;
        ThrowIfFailed(f.Args->CreateDrawingSession(&ds));

        float dpi;
        ThrowIfFailed(As<ICanvasResourceCreatorWithDpi>(ds)->get_Dpi(&dpi));

        Assert::AreEqual(AnyDpi, dpi);
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_WhenDrawingSessionIsClosed_CommandListPassedToPrintControl)
    {
        Fixture f;

        for (auto pageNumber = 1U; pageNumber < 10U; ++pageNumber)
        {
            float pageWidth = 100.0f * (float)pageNumber;
            float pageHeight = 200.0f * (float)pageNumber;
            
            auto printPageDescription = PrintPageDescription{
                Size{ pageWidth, pageHeight },
                Rect{ 0, 0, pageWidth, pageHeight }, // ImageableRect
                (uint32_t)AnyDpi, (uint32_t)AnyDpi   // DPI X, Y
            };

            f.PrintTaskOptions->GetPageDescriptionMethod.AllowAnyCall(
                [&] (uint32_t page, PrintPageDescription* outDesc)
                {
                    Assert::AreEqual(pageNumber, page);
                    *outDesc = printPageDescription;
                    return S_OK;
                });

            ComPtr<ICanvasDrawingSession> ds;
            ThrowIfFailed(f.Args->CreateDrawingSession(&ds));
            
            ComPtr<ID2D1Image> d2dTarget;
            GetWrappedResource<ID2D1DeviceContext>(ds)->GetTarget(&d2dTarget);
            
            f.PrintControl->AddPageMethod.SetExpectedCalls(1,
                [&] (ID2D1CommandList* commandList, D2D_SIZE_F pageSize, IStream* pagePrintTicketStream, D2D1_TAG* tag1, D2D1_TAG* tag2)
                {
                    Assert::IsTrue(IsSameInstance(d2dTarget.Get(), commandList));
                    Assert::AreEqual(D2D_SIZE_F{ pageWidth, pageHeight }, pageSize);
                    Assert::IsNull(pagePrintTicketStream);
                    Assert::IsNull(tag1);
                    Assert::IsNull(tag2);
                    return S_OK;
                });
            
            ThrowIfFailed(As<IClosable>(ds)->Close());
        }
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_WhenCreateDrawingSession_IsCalledBeforeLastDrawingSessionClose_ItFails)
    {
        Fixture f;

        ComPtr<ICanvasDrawingSession> ds[2];
        ThrowIfFailed(f.Args->CreateDrawingSession(&ds[0]));

        Assert::AreEqual(E_FAIL, f.Args->CreateDrawingSession(&ds[1]));
        ValidateStoredErrorState(E_FAIL, Strings::CanvasPrintEventArgsCannotCreateDrawingSessionUntilPreviousOneClosed);
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_When_GetDeferralCalledMultipleTimes_SecondCallFails)
    {
        Fixture f;
        f.VerifySecondCallToGetDeferralFails(f.Args);
    }

    TEST_METHOD_EX(CanvasPrintEventArgs_When_DeferralCompleteCalledMultipleTimes_SecondCallFails)
    {
        Fixture f;
        f.VerifySecondCallToDeferralCompleteFails(f.Args);
    }
};


static auto Timeout = std::chrono::seconds(5);
static HRESULT AnyHR = 0x87654321;

TEST_CLASS(CanvasPrintDeferrableTaskUnitTests)
{
    struct Fixture
    {
        ComPtr<StubDispatcher> Dispatcher;
        DeferrableTaskScheduler Scheduler;

        Fixture()
            : Dispatcher(Make<StubDispatcher>())
            , Scheduler(Dispatcher)
        {
        }

        template<typename FN>
        std::future<void> Schedule(FN&& fn)
        {
            auto task = Scheduler.CreateTask(fn);
            auto future = task->GetFuture();
            Scheduler.Schedule(std::move(task));
            return future;
        }

        void TickDispatcherOnOtherThreadUntilDone()
        {
            auto future = std::async(std::launch::async,
                [=]
                {
                    while (Dispatcher->HasPendingActions())
                    {
                        Dispatcher->TickAll();
                    }
                });
            future.wait();
        }
    };

    TEST_METHOD_EX(CanvasPrint_DeferrableTask_SetsFutureOnCompletion)
    {
        Fixture f;

        std::promise<void> completionFnCalled;

        auto future = f.Schedule(
            [&] (DeferrableTask* task)
            {
                task->SetCompletionFn([&] { completionFnCalled.set_value(); });
                task->NonDeferredComplete();
            });

        f.TickDispatcherOnOtherThreadUntilDone();

        auto completionFnFuture = completionFnCalled.get_future();
        Assert::IsTrue(completionFnFuture.wait_for(Timeout) == std::future_status::ready);
        completionFnFuture.get();

        Assert::IsTrue(future.wait_for(Timeout) == std::future_status::ready);
        future.get();
    }

    TEST_METHOD_EX(CanvasPrint_DeferrableTask_FailureInMainTask_MarshaledBackViaFuture)
    {
        Fixture f;

        auto future = f.Schedule(
            [&] (DeferrableTask* task)
            {
                ThrowHR(AnyHR);
            });

        f.TickDispatcherOnOtherThreadUntilDone();

        Assert::IsTrue(future.wait_for(Timeout) == std::future_status::ready);
        ExpectHResultException(AnyHR, [&] { future.get(); });
    }

    TEST_METHOD_EX(CanvasPrint_DeferrableTask_FailureInMaskTask_DoesNotBlockFutureTasks)
    {
        Fixture f;

        f.Schedule(
            [&] (DeferrableTask* task)
            {
                ThrowHR(AnyHR);
            });

        auto future = f.Schedule(
            [&] (DeferrableTask* task)
            {
                task->SetCompletionFn([&] {});
                task->NonDeferredComplete();
            });

        f.TickDispatcherOnOtherThreadUntilDone();

        Assert::IsTrue(future.wait_for(Timeout) == std::future_status::ready);
        future.get();
    }

    TEST_METHOD_EX(CanvasPrint_DeferrableTask_FailureInCompletionFn_MarshaledBackViaFuture)
    {
        Fixture f;

        auto future = f.Schedule(
            [&] (DeferrableTask* task)
            {
                task->SetCompletionFn([] { ThrowHR(AnyHR); });
                task->NonDeferredComplete();
            });

        f.TickDispatcherOnOtherThreadUntilDone();

        Assert::IsTrue(future.wait_for(Timeout) == std::future_status::ready);
        ExpectHResultException(AnyHR, [&] { future.get(); });
    }

    TEST_METHOD_EX(CanvasPrint_DeferrableTask_FailureInCompletoinFn_DoesNotBlockFutureTasks)
    {
        Fixture f;

        f.Schedule(
            [&] (DeferrableTask* task)
            {
                task->SetCompletionFn([] { ThrowHR(AnyHR); });
                task->NonDeferredComplete();
            });

        auto future = f.Schedule(
            [&] (DeferrableTask* task)
            {
                task->SetCompletionFn([&] {});
                task->NonDeferredComplete();
            });

        f.TickDispatcherOnOtherThreadUntilDone();

        Assert::IsTrue(future.wait_for(Timeout) == std::future_status::ready);
        future.get();
    }
};




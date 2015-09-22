// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "CanvasPrintDocumentAdapter.h"
#include "DeferrableTaskScheduler.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Printing
{
    using namespace ABI::Windows::Graphics::Printing;

    class CanvasPrintDocumentFactory
        : public ActivationFactory<ICanvasPrintDocumentFactory>
        , private LifespanTracker<CanvasPrintDocumentFactory>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Printing_CanvasPrintDocument, BaseTrust);

    public:
        //
        // ActivationFactory
        //
        IFACEMETHODIMP ActivateInstance(IInspectable**) override;

        //
        // ICanvasPrintDocumentFactory
        //
        IFACEMETHODIMP CreateWithDevice(ICanvasDevice*, ICanvasPrintDocument**) override;
    };

    typedef ITypedEventHandler<CanvasPrintDocument*, CanvasPrintTaskOptionsChangedEventArgs*> CanvasPrintTaskOptionsChangedHandler;
    typedef ITypedEventHandler<CanvasPrintDocument*, CanvasPreviewEventArgs*> CanvasPreviewHandler;
    typedef ITypedEventHandler<CanvasPrintDocument*, CanvasPrintEventArgs*> CanvasPrintHandler;

    class CanvasPrintDocument
        : public RuntimeClass<
            RuntimeClassFlags<WinRtClassicComMix>,
            ICanvasPrintDocument,
            IClosable,
            ICanvasResourceCreator,
            IPrintDocumentSource,
            IPrintPreviewPageCollection,
            IPrintDocumentPageSource>
        , private LifespanTracker<CanvasPrintDocument>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Printing_CanvasPrintDocument, BaseTrust);

        // These members are only modified on construction
        DeferrableTaskScheduler m_scheduler;
        float const m_displayDpi;
        bool const m_waitForUIThread;

        // These members are not threadsafe and may be accessed from any thread
        // and so are protected by m_mutex
        std::mutex m_mutex;
        ClosablePtr<ICanvasDevice> m_device;
        ComPtr<IPrintPreviewDxgiPackageTarget> m_previewTarget;

        // Event sources are stored in a shared_ptr so we can destroy them when
        // Close() is called.  Although the event sources are threadsafe, we
        // hold the mutex when accessing m_eventSources.
        struct EventSources
        {
            EventSource<CanvasPrintTaskOptionsChangedHandler, InvokeModeOptions<StopOnFirstError>> PrintTaskOptionsChanged;
            EventSource<CanvasPreviewHandler, InvokeModeOptions<StopOnFirstError>> Preview;
            EventSource<CanvasPrintHandler, InvokeModeOptions<StopOnFirstError>> Print;
        };

        std::shared_ptr<EventSources> m_eventSources;
        
        // These members are only accessed from the UI thread
        ComPtr<IPrintTaskOptionsCore> m_printTaskOptions;
        int m_newPreviewPageNumber;

        
    public:
        CanvasPrintDocument(
            std::shared_ptr<CanvasPrintDocumentAdapter> adapter,
            ComPtr<ICanvasDevice> const& device);
        
        //
        // ICanvasPrintDocument
        //
        IFACEMETHODIMP add_PrintTaskOptionsChanged(CanvasPrintTaskOptionsChangedHandler*, EventRegistrationToken*) override;
        IFACEMETHODIMP remove_PrintTaskOptionsChanged(EventRegistrationToken) override;
        IFACEMETHODIMP add_Preview(CanvasPreviewHandler*, EventRegistrationToken*) override;
        IFACEMETHODIMP remove_Preview(EventRegistrationToken) override;
        IFACEMETHODIMP add_Print(CanvasPrintHandler*, EventRegistrationToken*) override;
        IFACEMETHODIMP remove_Print(EventRegistrationToken) override;
        IFACEMETHODIMP InvalidatePreview() override;
        IFACEMETHODIMP SetPageCount(uint32_t) override;
        IFACEMETHODIMP SetIntermediatePageCount(uint32_t) override;
      
        //
        // IClosable
        //
        IFACEMETHODIMP Close() override;

        //
        // ICanvasResourceCreator
        //
        IFACEMETHODIMP get_Device(ICanvasDevice**) override;

        //
        // IPrintDocumentSource has no methods
        //

        //
        // IPrintPreviewPageCollection
        //
        IFACEMETHODIMP Paginate(uint32_t, IInspectable*) override;
        IFACEMETHODIMP MakePage(uint32_t, float, float) override;

        //
        // IPrintDocumentPageSource
        //
        IFACEMETHODIMP GetPreviewPageCollection(IPrintDocumentPackageTarget*, IPrintPreviewPageCollection**) override;
        IFACEMETHODIMP MakeDocument(IInspectable*, IPrintDocumentPackageTarget*) override;

    private:
        ComPtr<IPrintPreviewDxgiPackageTarget> GetPreviewTarget();
        void SetPreviewTarget(ComPtr<IPrintPreviewDxgiPackageTarget>&& previewTarget);

        std::shared_ptr<EventSources> GetEventSources();

        HRESULT SetJobPageCount(PageCountType type, uint32_t count);

        void RunOnUIThread(std::function<void(CanvasPrintDocument*, DeferrableTask*)>&& fn);

        void PaginateImpl(
            DeferrableTask* task,
            uint32_t currentPreviewPageNumber,
            ComPtr<IPrintTaskOptionsCore> const& printTaskOptions);

        void MakePageImpl(
            DeferrableTask* task,
            uint32_t pageNumber,
            float width,
            float height);

        float CalculateDpiForPreviewBitmap(Size previewSize, Size pageSize) const;

        void MakeDocumentImpl(
            DeferrableTask* task,
            ComPtr<IPrintTaskOptionsCore> const& printTaskOptions,
            ComPtr<IPrintDocumentPackageTarget> const& target);
    };
    
} } } } }

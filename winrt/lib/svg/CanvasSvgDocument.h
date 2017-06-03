// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#if WINVER > _WIN32_WINNT_WINBLUE

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Svg
{
    class CanvasSvgDocument : RESOURCE_WRAPPER_RUNTIME_CLASS(
        ID2D1SvgDocument,
        CanvasSvgDocument,
        ICanvasSvgDocument,
        CloakedIid<ICanvasResourceWrapperWithDevice>)
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgDocument, BaseTrust);

        ClosablePtr<ICanvasDevice> m_canvasDevice;

    public:
        
        static ComPtr<CanvasSvgDocument> CreateNew(ICanvasResourceCreator* resourceCreator, IStream* stream);

        CanvasSvgDocument(
            ICanvasDevice* canvasDevice,
            ID2D1SvgDocument* d2dSvgDocument);        

        IFACEMETHOD(Close)() override;

        IFACEMETHOD(get_Device)(ICanvasDevice** device) override;

        IFACEMETHOD(GetXml)(HSTRING* string) override;

        IFACEMETHOD(SaveAsync(IRandomAccessStream* stream, IAsyncAction** asyncAction)) override;

        IFACEMETHOD(put_Root)(ICanvasSvgElement* root) override;

        IFACEMETHOD(get_Root)(ICanvasSvgElement** root) override;

        IFACEMETHOD(LoadElementFromXml)(
            HSTRING xmlString,
            ICanvasSvgElement** svgElement) override;

        IFACEMETHOD(LoadElementAsync)(
            IRandomAccessStream *stream,
            IAsyncOperation<CanvasSvgElement*>** svgElement) override;

        // No exception boundary
        ComPtr<ICanvasDevice> GetDevice() { return m_canvasDevice.EnsureNotClosed(); }
    };

    class CanvasSvgDocumentStatics
        : public AgileActivationFactory<ICanvasSvgDocumentFactory, ICanvasSvgDocumentStatics>
    {
        InspectableClassStatic(RuntimeClass_Microsoft_Graphics_Canvas_Svg_CanvasSvgDocument, BaseTrust);

    public:
        IFACEMETHODIMP CreateEmpty(
            ICanvasResourceCreator *resourceCreator,
            ICanvasSvgDocument **canvasSvgDocument) override;

        IFACEMETHODIMP LoadFromXml(
            ICanvasResourceCreator* resourceCreator, 
            HSTRING xmlString, 
            ICanvasSvgDocument** svgDocument) override;

        IFACEMETHODIMP LoadAsync(
            ICanvasResourceCreator* resourceCreator, 
            IRandomAccessStream *stream,
            IAsyncOperation<CanvasSvgDocument*>** svgDocument) override;

        IFACEMETHODIMP IsSupported(ICanvasDevice* device, boolean* isSupported) override;
    };

    // Sets and un-sets a viewport size to an SVG document.
    class TemporaryViewportSize
    {
        // Non-copyable.
        TemporaryViewportSize(TemporaryViewportSize const&) = delete;
        TemporaryViewportSize& operator=(TemporaryViewportSize const&) = delete;

        // A weak pointer will work; this object will only ever live in the stack frame of a CanvasDrawingSession's 
        // DrawSvg* methods, where there is guaranteed to be a reference to the ID2D1SvgDocument.
        ID2D1SvgDocument* m_svgDocument;

        D2D1_SIZE_F m_previousViewportSize;

    public:
        TemporaryViewportSize(ID2D1SvgDocument* svgDocument, Size newSize)
            : m_svgDocument(svgDocument)
        {
            if (m_svgDocument)
            {
                m_previousViewportSize = m_svgDocument->GetViewportSize();

                m_svgDocument->SetViewportSize(D2D1::SizeF(newSize.Width, newSize.Height));
            }
        }

        ~TemporaryViewportSize()
        {
            if (m_svgDocument)
            {
                if (FAILED(m_svgDocument->SetViewportSize(m_previousViewportSize)))
                {
                    assert(false); // This should never fail, since the previous viewport size should have been valid.
                }
            }
        }
    };

    // Helper method

    template<typename IMPLEMENTATION_TYPE>
    void VerifyDeviceBoundary(
        ClosablePtr<ICanvasDevice> thisDeviceClosable,
        IMPLEMENTATION_TYPE* otherObjectImplementationType);
}}}}}

#endif
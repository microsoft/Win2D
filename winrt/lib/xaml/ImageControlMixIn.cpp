// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#include "ImageControlMixIn.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace UI { namespace Xaml
{
    ImageControlMixIn::ImageControlMixIn(IUserControl* userControl, IImageControlMixInAdapter* adapter)
        : m_composableBase(userControl)
    {
        m_imageControl = adapter->CreateImageControl();

        //
        // Set the stretch mode to Fill. This will ensure that on high DPI, the
        // layout will confine the control to the correct area even when the
        // backing image has a different physical size from the control's
        // device-independent size.
        //
        // The logic in EnsureSizeDependentResources ensures that the Source
        // assigned to the Image control matches the CanvasImageSource extents.
        //
        ThrowIfFailed(m_imageControl->put_Stretch(Stretch_Fill));

        //
        // Set the image control as the content of this control.
        //
        ComPtr<IUIElement> imageAsUIElement;
        ThrowIfFailed(m_imageControl.As(&imageAsUIElement));

        m_composableBase->put_Content(imageAsUIElement.Get());        
    }

    
    void ImageControlMixIn::UnregisterEventHandlers()
    {
        m_surfaceContentsLostEventRegistration.Release();
    }

    
    void ImageControlMixIn::SetImageSource(IImageSource* imageSource)
    {
        ThrowIfFailed(m_imageControl->put_Source(imageSource));
    }

    
    IFACEMETHODIMP ImageControlMixIn::MeasureOverride(
        Size availableSize, 
        Size* returnValue)
    {
        UNREFERENCED_PARAMETER(availableSize);
        
        return ExceptionBoundary(
            [&]
            {
                Size zeroSize{ 0, 0 };
                
                //
                // Call Measure on our children (in this case just the image control).
                //
                ThrowIfFailed(As<IUIElement>(m_imageControl)->Measure(zeroSize));
                
                //
                // Reply that we're happy to be sized however the layout engine wants to size us.
                //
                *returnValue = zeroSize;
            });
    }

    
    IFACEMETHODIMP ImageControlMixIn::ArrangeOverride(
        Size finalSize, 
        Size* returnValue)
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // Call Arrange on our children (in this case just the image control).
                //
                ThrowIfFailed(As<IUIElement>(m_imageControl)->Arrange(Rect{ 0, 0, finalSize.Width, finalSize.Height }));
                
                //
                // Reply that we're happy to accept the size chosen by the layout engine.
                //
                *returnValue = finalSize;
            });
    }

    
    IFACEMETHODIMP ImageControlMixIn::OnApplyTemplate()
    {
        return ExceptionBoundary(
            [&]
            {
                //
                // Allow base class to handle this
                //
                As<IFrameworkElementOverrides>(m_composableBase)->OnApplyTemplate();
            });
    }

}}}}}}

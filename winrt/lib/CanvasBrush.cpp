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

#include "pch.h"
#include "CanvasBrush.h"
#include "CanvasDevice.h"
#include "CanvasImage.h"
#include "CanvasBitmap.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using ABI::Windows::UI::Color;
    using namespace ABI::Windows::Foundation;
    using namespace ::Microsoft::WRL::Wrappers;

    IFACEMETHODIMP CanvasBrush::get_Opacity(_Out_ float *value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                *value = GetD2DBrush()->GetOpacity();
            });
    }

    IFACEMETHODIMP CanvasBrush::put_Opacity(_In_ float value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetD2DBrush()->SetOpacity(value);
            });
    }


    IFACEMETHODIMP CanvasBrush::get_Transform(_Out_ Numerics::Matrix3x2 *value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);

                GetD2DBrush()->GetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F*>(value));
            });
    }

    IFACEMETHODIMP CanvasBrush::put_Transform(_In_ Numerics::Matrix3x2 value)
    {
        return ExceptionBoundary(
            [&]()
            {
                GetD2DBrush()->SetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F*>(&value));
            });
    }

    ComPtr<CanvasSolidColorBrush> CanvasSolidColorBrushManager::CreateNew(
        ICanvasResourceCreator* resourceCreator,
        Color color)
    {
        ComPtr<ICanvasDevice> device;
        resourceCreator->get_Device(&device);

        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;
        ThrowIfFailed(device.As(&canvasDeviceInternal));

        auto& d2dBrush = canvasDeviceInternal->CreateSolidColorBrush(ToD2DColor(color));

        auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
            shared_from_this(),
            d2dBrush.Get());
        CheckMakeResult(canvasSolidColorBrush);
        
        return canvasSolidColorBrush;
    };


    ComPtr<CanvasSolidColorBrush> CanvasSolidColorBrushManager::CreateWrapper(
        ID2D1SolidColorBrush* brush)
    {
        auto canvasSolidColorBrush = Make<CanvasSolidColorBrush>(
            shared_from_this(),
            brush);
        CheckMakeResult(canvasSolidColorBrush);

        return canvasSolidColorBrush;
    }

    IFACEMETHODIMP CanvasSolidColorBrushFactory::Create(
        ICanvasResourceCreator* resourceCreator,
        ABI::Windows::UI::Color color,
        ICanvasSolidColorBrush** canvasSolidColorBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceCreator);
                CheckAndClearOutPointer(canvasSolidColorBrush);

                auto newSolidColorBrush = GetManager()->Create(
                    resourceCreator,
                    color);

                ThrowIfFailed(newSolidColorBrush.CopyTo(canvasSolidColorBrush));
            });
    }


    IFACEMETHODIMP CanvasSolidColorBrushFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);

                ComPtr<ID2D1SolidColorBrush> d2dBrush;
                ThrowIfFailed(resource->QueryInterface(d2dBrush.GetAddressOf()));

                auto newBrush = GetManager()->GetOrCreate(d2dBrush.Get());

                ThrowIfFailed(newBrush.CopyTo(wrapper));
            });
    }


    CanvasSolidColorBrush::CanvasSolidColorBrush(
        std::shared_ptr<CanvasSolidColorBrushManager> manager,
        ID2D1SolidColorBrush* brush)
        : ResourceWrapper(manager, brush)
    {
    }

    IFACEMETHODIMP CanvasSolidColorBrush::get_Color(_Out_ Color *value)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);
                *value = ToWindowsColor(GetResource()->GetColor());
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::put_Color(_In_ Color value)
    {
        return ExceptionBoundary(
            [&]
            {                
                GetResource()->SetColor(ToD2DColor(value));
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::Close()
    {
        return ResourceWrapper::Close();
    }

    ComPtr<ID2D1Brush> CanvasSolidColorBrush::GetD2DBrush()
    {
        return GetResource();
    }

    ComPtr<ID2D1SolidColorBrush> CanvasSolidColorBrush::GetD2DSolidColorBrush()
    {
        return GetResource();
    }

    //
    // CanvasImageBrush
    //

    class CanvasImageBrushAdapter : public ICanvasImageBrushAdapter
    {
        ComPtr<IPropertyValueStatics> m_propertyValueStatics;
    public:
        CanvasImageBrushAdapter()
        {
            ThrowIfFailed(GetActivationFactory(
                HStringReference(RuntimeClass_Windows_Foundation_PropertyValue).Get(),
                &m_propertyValueStatics));
        }

        ComPtr<IReference<Rect>> CreateRectReference(const D2D1_RECT_F& d2dRect) override
        {
            const float width = d2dRect.right - d2dRect.left;
            const float height = d2dRect.bottom - d2dRect.top;
            Rect rect = { d2dRect.left, d2dRect.top, width, height };

            ComPtr<IReference<Rect>> rectReference;
            ThrowIfFailed(m_propertyValueStatics->CreateRect(rect, &rectReference));

            return rectReference;
        }
    };

    CanvasImageBrushFactory::CanvasImageBrushFactory()
        : m_adapter(std::make_shared<CanvasImageBrushAdapter>()) {}

    IFACEMETHODIMP CanvasImageBrushFactory::Create(
        ICanvasResourceCreator* resourceAllocator,
        ICanvasImageBrush** canvasImageBrush)
    {
        return CreateWithImage(
            resourceAllocator,
            nullptr,
            canvasImageBrush);
    }

    IFACEMETHODIMP CanvasImageBrushFactory::CreateWithImage(
        ICanvasResourceCreator* resourceAllocator,
        ICanvasImage* image,
        ICanvasImageBrush** canvasImageBrush)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(resourceAllocator);
                CheckAndClearOutPointer(canvasImageBrush);

                ComPtr<ICanvasDevice> device;
                resourceAllocator->get_Device(&device);

                auto newImageBrush = Make<CanvasImageBrush>(
                    device.Get(), 
                    image,
                    m_adapter);

                ThrowIfFailed(newImageBrush.CopyTo(canvasImageBrush));
            });
    }

    IFACEMETHODIMP CanvasImageBrushFactory::GetOrCreate(
        IUnknown* resource,
        IInspectable** wrapper)
    {
        // TODO #2237: Implement image brush interop.
        // Waiting on implementation of bitmap interop,
        // plus effects/command list.
        return E_NOTIMPL;
    }

    CanvasImageBrush::CanvasImageBrush(
        ICanvasDevice* device,
        ICanvasImage* image,
        std::shared_ptr<ICanvasImageBrushAdapter> adapter)
        : m_isClosed(false)
        , m_useBitmapBrush(true)
        , m_adapter(adapter)
    {
        ThrowIfFailed(device->QueryInterface(m_deviceInternal.GetAddressOf()));

        m_d2dBitmapBrush = m_deviceInternal->CreateBitmapBrush(NULL);
        m_d2dImageBrush = m_deviceInternal->CreateImageBrush(NULL);

        SetImage(image);
    }

    void CanvasImageBrush::SetImage(ICanvasImage* image)
    {
        if (image == nullptr)
        {
            if (m_useBitmapBrush) m_d2dBitmapBrush->SetBitmap(nullptr);
            else m_d2dImageBrush->SetImage(nullptr);
            return;
        }

        // Can replace the current bitmap brush's bitmap
        ComPtr<ICanvasBitmap> bitmap;
        if (SUCCEEDED(image->QueryInterface(bitmap.GetAddressOf())) && m_useBitmapBrush)
        {
            ComPtr<ICanvasBitmapInternal> bitmapInternal;
            ThrowIfFailed(bitmap.As(&bitmapInternal));

            ComPtr<ID2D1Bitmap1> d2dBitmap = bitmapInternal->GetD2DBitmap();

            m_d2dBitmapBrush->SetBitmap(d2dBitmap.Get());
        }
        // If they specified an image, but this is currently backed by a bitmap brush,
        // then we need to switch to using an image brush.
        // And in all other cases, we stick with image brush.
        else
        {
            if (m_useBitmapBrush) SwitchFromBitmapBrushToImageBrush();

            m_d2dImageBrush->SetImage(m_deviceInternal->GetD2DImage(image).Get());
        }
    }

    IFACEMETHODIMP CanvasImageBrush::get_Image(ICanvasImage** value)
	{        
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);
                ThrowIfClosed();

                // TODO #2237: Requires implementation of interop path for CanvasBitmap,
                // CanvasEffect and command list.
                ThrowHR(E_NOTIMPL);
            });
	}

    IFACEMETHODIMP CanvasImageBrush::put_Image(ICanvasImage* value)
	{
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                
                SetImage(value);
            });
	}

    IFACEMETHODIMP CanvasImageBrush::get_ExtendX(CanvasEdgeBehavior* value)
	{
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);
                ThrowIfClosed();
                D2D1_EXTEND_MODE extendMode;

                if (m_useBitmapBrush)
                    extendMode = m_d2dBitmapBrush->GetExtendModeX();
                else 
                    extendMode = m_d2dImageBrush->GetExtendModeX();

                *value = static_cast<CanvasEdgeBehavior>(extendMode);
            });
	}

    IFACEMETHODIMP CanvasImageBrush::put_ExtendX(CanvasEdgeBehavior value)
	{
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                if (m_useBitmapBrush)
                    m_d2dBitmapBrush->SetExtendModeX(static_cast<D2D1_EXTEND_MODE>(value));
                else 
                    m_d2dImageBrush->SetExtendModeX(static_cast<D2D1_EXTEND_MODE>(value));
            });
	}

    IFACEMETHODIMP CanvasImageBrush::get_ExtendY(CanvasEdgeBehavior* value)
	{
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                ThrowIfClosed();
                D2D1_EXTEND_MODE extendMode;

                if (m_useBitmapBrush)
                    extendMode = m_d2dBitmapBrush->GetExtendModeY();
                else 
                    extendMode = m_d2dImageBrush->GetExtendModeY();

                *value = static_cast<CanvasEdgeBehavior>(extendMode);
            });
	}

    IFACEMETHODIMP CanvasImageBrush::put_ExtendY(CanvasEdgeBehavior value)
	{
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();
                if (m_useBitmapBrush)
                    m_d2dBitmapBrush->SetExtendModeY(static_cast<D2D1_EXTEND_MODE>(value));
                else 
                    m_d2dImageBrush->SetExtendModeY(static_cast<D2D1_EXTEND_MODE>(value));
            });
	}

    IFACEMETHODIMP CanvasImageBrush::get_SourceRectangle(ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>** value)
	{
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(value);
                ThrowIfClosed();

                ComPtr<IReference<Rect>> rectReference;

                if (!m_useBitmapBrush)
                {
                    D2D1_RECT_F sourceRectangle;
                    m_d2dImageBrush->GetSourceRectangle(&sourceRectangle);

                    rectReference = m_adapter->CreateRectReference(sourceRectangle);
                }

                *value = rectReference.Detach(); // returns a NULL rect for m_useBitmapBrush.
            });
	}

    //static
    D2D1_RECT_F CanvasImageBrush::GetD2DRectFromRectReference(ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>* value)
    {
        Rect rect;
        ThrowIfFailed(value->get_Value(&rect));
        return ToD2DRect(rect);
    }

    IFACEMETHODIMP CanvasImageBrush::put_SourceRectangle(ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>* value)
	{
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();

                if (m_useBitmapBrush)
                {
                    if (value)
                    {
                        SwitchFromBitmapBrushToImageBrush();

                        D2D1_RECT_F d2dRect = GetD2DRectFromRectReference(value);

                        m_d2dImageBrush->SetSourceRectangle(&d2dRect);
                    }
                    // If value is null, do nothing.
                }
                else
                {
                    if (value)
                    {
                        D2D1_RECT_F d2dRect = GetD2DRectFromRectReference(value);

                        m_d2dImageBrush->SetSourceRectangle(&d2dRect);
                    }
                    else
                    {
                        // Source rect is null. Check the backing image.
                        // If it's a bitmap, we can switch to bitmap brush.

                        ComPtr<ID2D1Image> backingD2DImage;
                        m_d2dImageBrush->GetImage(&backingD2DImage);

                        ComPtr<ID2D1Bitmap1> backingD2DBitmap;

                        if (!backingD2DImage || SUCCEEDED(backingD2DImage.As(&backingD2DBitmap)))
                        {
                            // Switch to bitmap brush.
                            SwitchFromImageBrushToBitmapBrush();
                        }
                        else
                        {
                            // Error to set a NULL source rect if the backing image
                            // isn't CanvasBitmap.
                            ThrowHR(E_INVALIDARG);
                        }
                    }
                }
            });
	}

    IFACEMETHODIMP CanvasImageBrush::get_Interpolation(CanvasImageInterpolation* value)
	{
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(value);
                ThrowIfClosed();
                D2D1_INTERPOLATION_MODE interpolationMode;

                if (m_useBitmapBrush)
                    interpolationMode = m_d2dBitmapBrush->GetInterpolationMode1();
                else 
                    interpolationMode = m_d2dImageBrush->GetInterpolationMode();

                *value = static_cast<CanvasImageInterpolation>(interpolationMode);
            });
	}

    IFACEMETHODIMP CanvasImageBrush::put_Interpolation(CanvasImageInterpolation value)
	{
        return ExceptionBoundary(
            [&]()
            {
                ThrowIfClosed();

                if (m_useBitmapBrush)
                    m_d2dBitmapBrush->SetInterpolationMode1(static_cast<D2D1_INTERPOLATION_MODE>(value));
                else 
                    m_d2dImageBrush->SetInterpolationMode(static_cast<D2D1_INTERPOLATION_MODE>(value));
            });
    }

    IFACEMETHODIMP CanvasImageBrush::Close()
    {
        m_d2dBitmapBrush.Reset();
        m_d2dImageBrush.Reset();
        m_isClosed = true;
        return S_OK;
    }

    ComPtr<ID2D1Brush> CanvasImageBrush::GetD2DBrush()
    {
        ThrowIfClosed();

        if (m_useBitmapBrush) return m_d2dBitmapBrush;
        else return m_d2dImageBrush;
    }

    ComPtr<ID2D1BitmapBrush> CanvasImageBrush::GetD2DBitmapBrush()
    {
        ThrowIfClosed();

        return m_d2dBitmapBrush;
    }

    ComPtr<ID2D1ImageBrush> CanvasImageBrush::GetD2DImageBrush()
    {
        ThrowIfClosed();

        return m_d2dImageBrush;
    }

    void CanvasImageBrush::ThrowIfClosed()
    {
        if (m_isClosed)
        {
            ThrowHR(RO_E_CLOSED);
        }
    }

    void CanvasImageBrush::SwitchFromBitmapBrushToImageBrush()
    {
        assert(m_useBitmapBrush);

        m_useBitmapBrush = false;

        m_d2dImageBrush->SetExtendModeX(m_d2dBitmapBrush->GetExtendModeX());
        m_d2dImageBrush->SetExtendModeY(m_d2dBitmapBrush->GetExtendModeY());
        m_d2dImageBrush->SetInterpolationMode(m_d2dBitmapBrush->GetInterpolationMode1());
        m_d2dImageBrush->SetOpacity(m_d2dBitmapBrush->GetOpacity());
        D2D1_MATRIX_3X2_F transform;
        m_d2dBitmapBrush->GetTransform(&transform);
        m_d2dImageBrush->SetTransform(transform);

        ComPtr<ID2D1Bitmap> targetImage;
        m_d2dBitmapBrush->GetBitmap(&targetImage);
        m_d2dBitmapBrush->SetBitmap(nullptr);
        m_d2dImageBrush->SetImage(targetImage.Get());
    }

    void CanvasImageBrush::SwitchFromImageBrushToBitmapBrush()
    {
        assert(!m_useBitmapBrush);

        m_useBitmapBrush = true;

        m_d2dBitmapBrush->SetExtendModeX(m_d2dImageBrush->GetExtendModeX());
        m_d2dBitmapBrush->SetExtendModeY(m_d2dImageBrush->GetExtendModeY());
        m_d2dBitmapBrush->SetInterpolationMode1(m_d2dImageBrush->GetInterpolationMode());
        m_d2dBitmapBrush->SetOpacity(m_d2dImageBrush->GetOpacity());
        D2D1_MATRIX_3X2_F transform;
        m_d2dImageBrush->GetTransform(&transform);
        m_d2dBitmapBrush->SetTransform(transform);

        ComPtr<ID2D1Image> targetImage;
        m_d2dImageBrush->GetImage(&targetImage);
        ComPtr<ID2D1Bitmap> targetBitmap;
        if (targetImage) ThrowIfFailed(targetImage.As(&targetBitmap));
        m_d2dImageBrush->SetImage(nullptr);
        m_d2dBitmapBrush->SetBitmap(targetBitmap.Get());
    }

    ActivatableClassWithFactory(CanvasSolidColorBrush, CanvasSolidColorBrushFactory);
    ActivatableClassWithFactory(CanvasImageBrush, CanvasImageBrushFactory);
} } } }

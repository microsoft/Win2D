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
#include "CanvasImageBrush.h"
#include "CanvasDevice.h"
#include "CanvasImage.h"
#include "CanvasBitmap.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using ABI::Windows::UI::Color;
    using namespace ABI::Windows::Foundation;
    using namespace ::Microsoft::WRL::Wrappers;


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
                ThrowIfFailed(resourceAllocator->get_Device(&device));

                auto newImageBrush = Make<CanvasImageBrush>(
                    device.Get());
                CheckMakeResult(newImageBrush);

                newImageBrush->SetImage(image);

                ThrowIfFailed(newImageBrush.CopyTo(canvasImageBrush));
            });
    }

    IFACEMETHODIMP CanvasImageBrushFactory::GetOrCreate(
        ICanvasDevice* device,
        IUnknown* resource,
        IInspectable** wrapper)
    {
        return ExceptionBoundary(
            [&]
            {
                CheckInPointer(device);
                CheckInPointer(resource);
                CheckAndClearOutPointer(wrapper);
                
                auto deviceInternal = As<ICanvasDeviceInternal>(device);

                auto d2dBitmapBrush = MaybeAs<ID2D1BitmapBrush1>(resource);
                auto d2dImageBrush = MaybeAs<ID2D1ImageBrush>(resource);

                if (!d2dBitmapBrush && !d2dImageBrush)
                    ThrowHR(E_NOINTERFACE);

                auto newImageBrush = Make<CanvasImageBrush>(
                    device,
                    d2dBitmapBrush.Get(),
                    d2dImageBrush.Get());
                CheckMakeResult(newImageBrush);
                ThrowIfFailed(newImageBrush.CopyTo(wrapper));
            });
    }


    CanvasImageBrush::CanvasImageBrush(
        ICanvasDevice* device,
        ID2D1BitmapBrush1* bitmapBrush,
        ID2D1ImageBrush* imageBrush)
        : m_device(device)
        , m_d2dBitmapBrush(bitmapBrush)
        , m_d2dImageBrush(imageBrush)
        , m_useBitmapBrush(true)
        , m_isSourceRectSet(false)
    {
        auto deviceInternal = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed());

        // We always use the bitmap brush, unless an image brush was explicitly
        // specified
        if (m_d2dImageBrush)
        {
            m_useBitmapBrush = false;
            m_isSourceRectSet = true;
        }

        // Create missing brushes
        if (!m_d2dBitmapBrush)
            m_d2dBitmapBrush = deviceInternal->CreateBitmapBrush(nullptr);

        if (!m_d2dImageBrush)
            m_d2dImageBrush = deviceInternal->CreateImageBrush(nullptr);
    }

    void CanvasImageBrush::SetImage(ICanvasImage* image)
    {
        m_effectNeedingDpiFixup.Reset();

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

            auto& d2dBitmap = bitmapInternal->GetD2DBitmap();

            m_d2dBitmapBrush->SetBitmap(d2dBitmap.Get());
        }
        // If they specified an image, but this is currently backed by a bitmap brush,
        // then we need to switch to using an image brush.
        // And in all other cases, we stick with image brush.
        else
        {
            if (m_useBitmapBrush) 
                SwitchFromBitmapBrushToImageBrush();

            auto deviceInternal = As<ICanvasDeviceInternal>(m_device.EnsureNotClosed());
            m_d2dImageBrush->SetImage(deviceInternal->GetD2DImage(image).Get());

            // Effects need to be reconfigured depending on the DPI of the device context they
            // are drawn onto. We don't know target DPI at this point, so if the image is an
            // effect, we store that away for use by a later fixup inside GetD2DBrush.
            ComPtr<IEffect> effect;
            if (SUCCEEDED(image->QueryInterface(effect.GetAddressOf())))
            {
                m_effectNeedingDpiFixup = As<ICanvasImageInternal>(effect);
            }
        }
    }

    IFACEMETHODIMP CanvasImageBrush::get_Image(ICanvasImage** value)
	{        
        return ExceptionBoundary(
            [&]
            {
                CheckAndClearOutPointer(value);
                auto& device = m_device.EnsureNotClosed();

                auto d2dBitmap = GetD2DBitmap();

                if (!d2dBitmap)
                    return;

                auto bitmapManager = PerApplicationPolymorphicBitmapManager::GetOrCreateManager();
                auto bitmap = bitmapManager->GetOrCreateBitmap(device.Get(), d2dBitmap.Get());
                ThrowIfFailed(bitmap.CopyTo(value));
            });
	}


    ComPtr<ID2D1Bitmap1> CanvasImageBrush::GetD2DBitmap() const
    {
        if (m_useBitmapBrush)
        {
            ComPtr<ID2D1Bitmap> bm;
            m_d2dBitmapBrush->GetBitmap(&bm);

            if (!bm)
                return nullptr;

            return As<ID2D1Bitmap1>(bm);
        }
        else
        {
            ComPtr<ID2D1Image> image;
            m_d2dImageBrush->GetImage(&image);

            ComPtr<ID2D1Bitmap1> d2dBitmap; 
            HRESULT hr = image.As(&d2dBitmap);
            if (hr == E_NOINTERFACE)
            {
                // TODO #2630: polymorphic interop for all ICanvasImage
                // types.  For now, this only works for CanvasBitmap.
                ThrowHR(E_NOTIMPL);
            }
            ThrowIfFailed(hr);

            return d2dBitmap;
        }
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

                if (!m_useBitmapBrush && m_isSourceRectSet)
                {
                    D2D1_RECT_F sourceRectangle;
                    m_d2dImageBrush->GetSourceRectangle(&sourceRectangle);

                    rectReference = Make<Nullable<Rect>>(FromD2DRect(sourceRectangle));
                    CheckMakeResult(rectReference);
                }
                
                *value = rectReference.Detach(); // returns a NULL rect for m_useBitmapBrush.
            });
	}

    //static
    D2D1_RECT_F CanvasImageBrush::GetD2DRectFromRectReference(ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>* value)
    {
        Rect rect{};
        ThrowIfFailed(value->get_Value(&rect));
        return ToD2DRect(rect);
    }

    IFACEMETHODIMP CanvasImageBrush::put_SourceRectangle(ABI::Windows::Foundation::IReference<ABI::Windows::Foundation::Rect>* value)
	{
        return ExceptionBoundary(
            [&]
            {
                ThrowIfClosed();

                if (m_useBitmapBrush)
                {
                    assert(!m_isSourceRectSet);
                    if (value)
                    {
                        SwitchFromBitmapBrushToImageBrush();

                        D2D1_RECT_F d2dRect = GetD2DRectFromRectReference(value);

                        m_d2dImageBrush->SetSourceRectangle(&d2dRect);
                        m_isSourceRectSet = true;
                    }
                    // If value is null, do nothing.
                }
                else
                {
                    if (value)
                    {
                        D2D1_RECT_F d2dRect = GetD2DRectFromRectReference(value);

                        m_d2dImageBrush->SetSourceRectangle(&d2dRect);
                        m_isSourceRectSet = true;
                    }
                    else
                    {
                        D2D1_RECT_F defaultRect{};
                        m_d2dImageBrush->SetSourceRectangle(&defaultRect);

                        // Source rect is null. We might be able to switch to
                        // bitmap brush.
                        TrySwitchFromImageBrushToBitmapBrush();
                        m_isSourceRectSet = false;
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
        m_device.Close();
        m_d2dBitmapBrush.Reset();
        m_d2dImageBrush.Reset();
        return S_OK;
    }

    ComPtr<ID2D1Brush> CanvasImageBrush::GetD2DBrush(ID2D1DeviceContext* deviceContext)
    {
        ThrowIfClosed();

        if (m_useBitmapBrush) 
        {
            return m_d2dBitmapBrush;
        }
        else 
        {
            if (!m_isSourceRectSet) 
            {
                ThrowHR(E_INVALIDARG, HStringReference(Strings::ImageBrushRequiresSourceRectangle).Get());
            }

            // If our input image is an effect graph, make sure it is fully configured to match the target DPI.
            if (m_effectNeedingDpiFixup && deviceContext)
            {
                float targetDpi = GetDpi(deviceContext);

                m_effectNeedingDpiFixup->GetRealizedEffectNode(deviceContext, targetDpi);
            }

            return m_d2dImageBrush;
        }
    }

    ComPtr<ID2D1Brush> CanvasImageBrush::GetD2DBrushNoValidation()
    {
        ThrowIfClosed();

        if (m_useBitmapBrush) 
            return m_d2dBitmapBrush;
        else
            return m_d2dImageBrush;
    }

    IFACEMETHODIMP CanvasImageBrush::GetResource(REFIID iid, void** resource)
    {
        return ExceptionBoundary(
            [=]
            {
                ThrowIfClosed();

                if (m_useBitmapBrush)
                    ThrowIfFailed(m_d2dBitmapBrush.CopyTo(iid, resource));
                else
                    ThrowIfFailed(m_d2dImageBrush.CopyTo(iid, resource));
            });
    }

    void CanvasImageBrush::ThrowIfClosed()
    {
        m_device.EnsureNotClosed();
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

    void CanvasImageBrush::TrySwitchFromImageBrushToBitmapBrush()
    {
        assert(!m_useBitmapBrush);

        ComPtr<ID2D1Image> targetImage;
        m_d2dImageBrush->GetImage(&targetImage);

        ComPtr<ID2D1Bitmap> targetBitmap;
        if (targetImage)
        {
            HRESULT hr = targetImage.As(&targetBitmap);

            if (FAILED(hr))
            {
                // The image brush's image isn't a bitmap, so we can't switch to
                // bitmap brush.
                return;
            }
        }

        m_useBitmapBrush = true;

        m_d2dBitmapBrush->SetExtendModeX(m_d2dImageBrush->GetExtendModeX());
        m_d2dBitmapBrush->SetExtendModeY(m_d2dImageBrush->GetExtendModeY());
        m_d2dBitmapBrush->SetInterpolationMode1(m_d2dImageBrush->GetInterpolationMode());
        m_d2dBitmapBrush->SetOpacity(m_d2dImageBrush->GetOpacity());
        D2D1_MATRIX_3X2_F transform;
        m_d2dImageBrush->GetTransform(&transform);
        m_d2dBitmapBrush->SetTransform(transform);

        m_d2dImageBrush->SetImage(nullptr);
        m_d2dBitmapBrush->SetBitmap(targetBitmap.Get());
    }

    ActivatableClassWithFactory(CanvasImageBrush, CanvasImageBrushFactory);
} } } }

// Copyright (c) Microsoft Corporation.  All rights reserved

#include "pch.h"
#include "CanvasBrush.h"
#include "CanvasDevice.h"

namespace canvas
{
    using ABI::Windows::UI::Color;

    class DefaultCanvasSolidColorBrushResourceCreationAdapter: public ICanvasSolidColorBrushResourceCreationAdapter
    {
    public:
        ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(ID2D1DeviceContext* deviceContext, Color color) override
        {
            ComPtr<ID2D1SolidColorBrush> solidColorBrush;
            
            ThrowIfFailed(deviceContext->CreateSolidColorBrush(
                ToD2DColor(color),
                &solidColorBrush
                ));

            return solidColorBrush;            
        }
    };

    CanvasSolidColorBrushFactory::CanvasSolidColorBrushFactory()
        : m_resourceCreationAdapter(std::make_shared<DefaultCanvasSolidColorBrushResourceCreationAdapter>())
    {

    }

    IFACEMETHODIMP CanvasSolidColorBrushFactory::Create(
        ICanvasDevice* canvasDevice,
        ABI::Windows::UI::Color color,
        ICanvasSolidColorBrush** canvasSolidColorBrush)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(canvasDevice);
                CheckAndClearOutPointer(canvasSolidColorBrush);

                auto newSolidColorBrush = Make<CanvasSolidColorBrush>(
                    canvasDevice, 
                    color,
                    m_resourceCreationAdapter
                    );

                CheckMakeResult(newSolidColorBrush);

                ThrowIfFailed(newSolidColorBrush.CopyTo(canvasSolidColorBrush));
            });
    }

    CanvasSolidColorBrush::CanvasSolidColorBrush(
        ICanvasDevice* canvasDevice,
        ABI::Windows::UI::Color color,
        std::shared_ptr<ICanvasSolidColorBrushResourceCreationAdapter> resourceCreationAdapter
        ) 
        : m_device(canvasDevice)
    {
        ComPtr<ICanvasDeviceInternal> canvasDeviceInternal;

        ThrowIfFailed(canvasDevice->QueryInterface(canvasDeviceInternal.GetAddressOf()));

        ComPtr<ID2D1SolidColorBrush> d2dSolidColorBrush = resourceCreationAdapter->CreateSolidColorBrush(
            canvasDeviceInternal->GetD2DResourceCreationDeviceContext().Get(),
            color
            );

        // This assignment is necessary because m_d2dSolidColorBrush is IClosablePtr.
        m_d2dSolidColorBrush = d2dSolidColorBrush; 
    }

    IFACEMETHODIMP CanvasSolidColorBrush::get_Color(_Out_ Color *value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);
                
                auto& brush = m_d2dSolidColorBrush.EnsureNotClosed();

                D2D1_COLOR_F d2dColor = brush->GetColor();

                *value = ToWindowsColor(d2dColor);
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::put_Color(_In_ Color value)
    {
        return ExceptionBoundary(
            [&]()
            {                
                auto& brush = m_d2dSolidColorBrush.EnsureNotClosed();

                brush->SetColor(ToD2DColor(value));
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::get_Opacity(_Out_ float *value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);

                auto& brush = m_d2dSolidColorBrush.EnsureNotClosed();

                *value = brush->GetOpacity();
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::put_Opacity(_In_ float value)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& brush = m_d2dSolidColorBrush.EnsureNotClosed();

                brush->SetOpacity(value);
            });
    }


    IFACEMETHODIMP CanvasSolidColorBrush::get_Transform(_Out_ Math::Matrix3x2 *value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckInPointer(value);

                auto& brush = m_d2dSolidColorBrush.EnsureNotClosed();

                D2D1_MATRIX_3X2_F d2dTransformMatrix;
                brush->GetTransform(&d2dTransformMatrix);

                *value = ToMathMatrix3x2(d2dTransformMatrix);
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::put_Transform(_In_ Math::Matrix3x2 value)
    {
        return ExceptionBoundary(
            [&]()
            {
                auto& brush = m_d2dSolidColorBrush.EnsureNotClosed();

                brush->SetTransform(ToD2DMatrix3x2(value));
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::get_Device(_In_ ICanvasDevice** value)
    {
        return ExceptionBoundary(
            [&]()
            {
                CheckAndClearOutPointer(value);

                auto& device = m_device.EnsureNotClosed();

                device.CopyTo(value);
            });
    }

    IFACEMETHODIMP CanvasSolidColorBrush::Close()
    {
        m_d2dSolidColorBrush.Close();

        m_device.Close();

        return S_OK;
    }

    ComPtr<ID2D1Brush> CanvasSolidColorBrush::GetD2DBrush()
    {
        ComPtr<ID2D1SolidColorBrush> brush = m_d2dSolidColorBrush.EnsureNotClosed();

        return brush;
    }

    ActivatableClassWithFactory(CanvasSolidColorBrush, CanvasSolidColorBrushFactory);
}
// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

namespace canvas
{
    using namespace ABI::Microsoft::Graphics::Canvas;

    class MockCanvasDrawingSession : public RuntimeClass<
        ICanvasDrawingSession>
    {
    public:
        IFACEMETHODIMP Clear(
            ABI::Windows::UI::Color color) override
        {
            Assert::Fail(L"Unexpected call to Clear");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawLine(
            ABI::Windows::Foundation::Point point0,
            ABI::Windows::Foundation::Point point1,
            ICanvasBrush* brush) override
        {
            Assert::Fail(L"Unexpected call to DrawLine");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawLineWithStrokeWidth(
            ABI::Windows::Foundation::Point point0,
            ABI::Windows::Foundation::Point point1,
            ICanvasBrush* brush,
            float strokeWidth) override
        {
            Assert::Fail(L"Unexpected call to DrawLineWithStrokeWidth");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawRectangle(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush) override
        {
            Assert::Fail(L"Unexpected call to DrawRectangle");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawRectangleWithStrokeWidth(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush,
            float strokeWidth) override
        {
            Assert::Fail(L"Unexpected call to DrawRectangleWithStrokeWidth");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP FillRectangle(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush) override
        {
            Assert::Fail(L"Unexpected call to FillRectangle");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawRoundedRectangle(
            CanvasRoundedRectangle roundedRectangle,
            ICanvasBrush* brush) override
        {
            Assert::Fail(L"Unexpected call to DrawRoundedRectangle");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawRoundedRectangleWithStrokeWidth(
            CanvasRoundedRectangle roundedRectangle,
            ICanvasBrush* brush,
            float strokeWidth) override
        {
            Assert::Fail(L"Unexpected call to DrawRoundedRectangleWithStrokeWidth");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP FillRoundedRectangle(
            CanvasRoundedRectangle roundedRectangle,
            ICanvasBrush* brush) override
        {
            Assert::Fail(L"Unexpected call to FillRoundedRectangle");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawEllipse(
            CanvasEllipse ellipse,
            ICanvasBrush* brush) override
        {
            Assert::Fail(L"Unexpected call to DrawEllipse");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawEllipseWithStrokeWidth(
            CanvasEllipse ellipse,
            ICanvasBrush* brush,
            float strokeWidth) override
        {
            Assert::Fail(L"Unexpected call to DrawEllipseWithStrokeWidth");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP FillEllipse(
            CanvasEllipse ellipse,
            ICanvasBrush* brush) override
        {
            Assert::Fail(L"Unexpected call to FillEllipse");
            return E_NOTIMPL;
        }
    };
}

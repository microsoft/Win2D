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

        IFACEMETHODIMP DrawLineWithStrokeWidthAndStrokeStyle(
            ABI::Windows::Foundation::Point point0,
            ABI::Windows::Foundation::Point point1,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override
        {
            Assert::Fail(L"Unexpected call to DrawLineWithStrokeWidthAndStrokeStyle");
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

        IFACEMETHODIMP DrawRectangleWithStrokeWidthAndStrokeStyle(
            ABI::Windows::Foundation::Rect rect,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override
        {
            Assert::Fail(L"Unexpected call to DrawRectangleWithStrokeWidthAndStrokeStyle");
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

        IFACEMETHODIMP DrawRoundedRectangleWithStrokeWidthAndStrokeStyle(
            CanvasRoundedRectangle roundedRectangle,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override
        {
            Assert::Fail(L"Unexpected call to DrawRoundedRectangleWithStrokeWidthAndStrokeStyle");
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

        IFACEMETHODIMP DrawEllipseWithStrokeWidthAndStrokeStyle(
            CanvasEllipse ellipse,
            ICanvasBrush* brush,
            float strokeWidth,
            ICanvasStrokeStyle* strokeStyle) override
        {
            Assert::Fail(L"Unexpected call to DrawEllipseWithStrokeWidthAndStrokeStyle");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP FillEllipse(
            CanvasEllipse ellipse,
            ICanvasBrush* brush) override
        {
            Assert::Fail(L"Unexpected call to FillEllipse");
            return E_NOTIMPL;
        }
        
        IFACEMETHODIMP DrawTextAtPoint(
            HSTRING,
            ABI::Windows::Foundation::Point,
            ICanvasBrush*) override
        {
            Assert::Fail(L"Unexpected call to DrawTextAtPoint");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawTextAtPointWithFormat(
            HSTRING,
            ABI::Windows::Foundation::Point,
            ICanvasBrush*,
            ICanvasTextFormat*) override
        {
            Assert::Fail(L"Unexpected call to DrawTextAtPointWithFormat");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawText(
            HSTRING,
            ABI::Windows::Foundation::Rect,
            ICanvasBrush*) override
        {
            Assert::Fail(L"Unexpected call to DrawText");
            return E_NOTIMPL;
        }

        IFACEMETHODIMP DrawTextWithFormat(
            HSTRING,
            ABI::Windows::Foundation::Rect,
            ICanvasBrush*,
            ICanvasTextFormat*) override
        {
            Assert::Fail(L"Unexpected call to DrawTextWithFormat");
            return E_NOTIMPL;
        }
    };
}

using Microsoft.Graphics.Canvas;
using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI;
using Windows.Foundation;
using System.Diagnostics;

namespace CsConsumer
{
    public enum TestSceneRenderingType { Default, Randomized, Wireframe }

    // Used to abstract out a wireframe view.
    class TestSceneRenderer
    {
        public TestSceneRenderer(CanvasDrawingSession drawingSession, TestSceneRenderingType renderingType)
        {
            m_drawingSession = drawingSession;
            m_renderingType = renderingType;
            Debug.Assert(m_renderingType == TestSceneRenderingType.Default || m_renderingType == TestSceneRenderingType.Wireframe);
        }

        public void Clear(Color color)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.Clear(color);
            }
            else
            {
                m_drawingSession.Clear(Colors.White);
            }
        }

        public void FillRectangle(Rect rect, CanvasSolidColorBrush brush)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.FillRectangle(rect, brush);
            }
            else
            {
                brush.Color = Colors.Black;
                m_drawingSession.DrawRectangle(rect, brush);
            }
        }

        public void DrawLine(Point p1, Point p2, CanvasSolidColorBrush brush, float strokeWidth)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawLine(p1, p2, brush, strokeWidth);
            }
            else
            {
                brush.Color = Colors.Black;
                m_drawingSession.DrawLine(p1, p2, brush);
            }
        }

        public void DrawLine(Point p1, Point p2, CanvasSolidColorBrush brush, float strokeWidth, CanvasStrokeStyle strokeStyle)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawLine(p1, p2, brush, strokeWidth, strokeStyle);
            }
            else
            {
                brush.Color = Colors.Black;
                m_drawingSession.DrawLine(p1, p2, brush);
            }
        }

        public void FillEllipse(CanvasEllipse ellipse, CanvasSolidColorBrush brush)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.FillEllipse(ellipse, brush);
            }
            else
            {
                brush.Color = Colors.Black;
                m_drawingSession.DrawEllipse(ellipse, brush);
            }
        }

        public void DrawEllipse(CanvasEllipse ellipse, CanvasSolidColorBrush brush, float strokeWidth)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawEllipse(ellipse, brush, strokeWidth);
            }
            else
            {
                brush.Color = Colors.Black;
                m_drawingSession.DrawEllipse(ellipse, brush);
            }
        }

        public void DrawEllipse(CanvasEllipse ellipse, CanvasSolidColorBrush brush, float strokeWidth, CanvasStrokeStyle strokeStyle)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawEllipse(ellipse, brush, strokeWidth, strokeStyle);
            }
            else
            {
                brush.Color = Colors.Black;
                m_drawingSession.DrawEllipse(ellipse, brush);
            }
        }

        CanvasDrawingSession m_drawingSession;
        TestSceneRenderingType m_renderingType;
    }
}
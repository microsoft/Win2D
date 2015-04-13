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

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Geometry;
using System.Diagnostics;
using System.Numerics;
using Windows.Foundation;
using Windows.UI;

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

        public void FillRectangle(Rect rect, Color color)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.FillRectangle(rect, color);
            }
            else
            {
                m_drawingSession.DrawRectangle(rect, Colors.Black);
            }
        }

        public void DrawLine(Vector2 p1, Vector2 p2, Color color, float strokeWidth)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawLine(p1, p2, color, strokeWidth);
            }
            else
            {
                m_drawingSession.DrawLine(p1, p2, Colors.Black);
            }
        }

        public void DrawLine(Vector2 p1, Vector2 p2, Color color, float strokeWidth, CanvasStrokeStyle strokeStyle)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawLine(p1, p2, color, strokeWidth, strokeStyle);
            }
            else
            {
                m_drawingSession.DrawLine(p1, p2, Colors.Black);
            }
        }

        public void FillEllipse(Vector2 center, float radiusX, float radiusY, Color color)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.FillEllipse(center, radiusX, radiusY, color);
            }
            else
            {
                m_drawingSession.DrawEllipse(center, radiusX, radiusY, Colors.Black);
            }
        }

        public void DrawEllipse(Vector2 center, float radiusX, float radiusY, Color color, float strokeWidth)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawEllipse(center, radiusX, radiusY, color, strokeWidth);
            }
            else
            {
                m_drawingSession.DrawEllipse(center, radiusX, radiusY, Colors.Black);
            }
        }

        public void DrawEllipse(Vector2 center, float radiusX, float radiusY, Color color, float strokeWidth, CanvasStrokeStyle strokeStyle)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawEllipse(center, radiusX, radiusY, color, strokeWidth, strokeStyle);
            }
            else
            {
                m_drawingSession.DrawEllipse(center, radiusX, radiusY, Colors.Black);
            }
        }

        public void FillCircle(Vector2 centerPoint, float radius, Color color)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.FillCircle(centerPoint, radius, color);
            }
            else
            {
                m_drawingSession.DrawCircle(centerPoint, radius, Colors.Black);
            }
        }

        public void DrawCircle(Vector2 centerPoint, float radius, Color color, float strokeWidth)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawCircle(centerPoint, radius, color, strokeWidth);
            }
            else
            {
                m_drawingSession.DrawCircle(centerPoint, radius, Colors.Black);
            }
        }

        public void DrawCircle(Vector2 centerPoint, float radius, Color color, float strokeWidth, CanvasStrokeStyle strokeStyle)
        {
            if (m_renderingType == TestSceneRenderingType.Default)
            {
                m_drawingSession.DrawCircle(centerPoint, radius, color, strokeWidth, strokeStyle);
            }
            else
            {
                m_drawingSession.DrawCircle(centerPoint, radius, Colors.Black);
            }
        }

        CanvasDrawingSession m_drawingSession;
        TestSceneRenderingType m_renderingType;
    }
}

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas { namespace Geometry
{
    class TessellationSink : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ID2D1TessellationSink>,
                             private LifespanTracker<TessellationSink>
    {
        std::vector<CanvasTriangleVertices> m_triangles;
        HRESULT m_result;

    public:
        TessellationSink()
            : m_result(S_OK)
        { }

        IFACEMETHODIMP_(void) AddTriangles(D2D1_TRIANGLE const* triangles, UINT32 trianglesCount)
        {
            if (FAILED(m_result))
                return;

            m_result = ExceptionBoundary([&]
            {
                auto canvasTriangles = ReinterpretAs<CanvasTriangleVertices const*>(triangles);

                m_triangles.insert(m_triangles.end(), canvasTriangles, canvasTriangles + trianglesCount);
            });
        }

        IFACEMETHODIMP Close()
        {
            return m_result;
        }

        ComArray<CanvasTriangleVertices> GetTriangles()
        {
            ThrowIfFailed(m_result);

            return ComArray<CanvasTriangleVertices>(m_triangles.begin(), m_triangles.end());
        }
    };
}}}}}

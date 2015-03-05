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

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
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
}}}}

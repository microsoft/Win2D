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

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Numerics;

namespace CoreWindowExample
{
    class ConvexHull
    {
        public static List<Vector2> Create(IEnumerable<Vector2> sourceVerts)
        {
            var builder = new ConvexHull(sourceVerts);
            return builder.hull;
        }

        private List<Vector2> verts;
        private List<Vector2> hull;

        private Vector2 lastVert;
        private Vector2 lastEdgeDirection;

        private ConvexHull(IEnumerable<Vector2> sourceVerts)
        {
            verts = new List<Vector2>(sourceVerts);
            hull = new List<Vector2>(verts.Count);

            if (verts.Count == 0)
                return;

            int firstIndex = GetIndexOfLeftMostVertex();
            Debug.Assert(firstIndex != -1);

            AddVertToHull(-Vector2.UnitY, firstIndex);

            while (true)
            {
                float bestAngle;
                Vector2 bestEdgeDirection;
                int bestIndex = FindBestNextVert(out bestAngle, out bestEdgeDirection);

                if (bestIndex == -1)
                    break;

                // Make sure that the next best vert is indeed better than just closing the hull back to the first vert
                if (hull.Count > 1)
                {
                    var edgeDirectionToFirstVert = Vector2.Normalize(lastVert - hull[0]);
                    float angleToFirstVert = CalculateAngleFromLastEdge(edgeDirectionToFirstVert);
                    if (angleToFirstVert < bestAngle)
                        break;
                }

                AddVertToHull(bestEdgeDirection, bestIndex);
            }
        }

        private void AddVertToHull(Vector2 lineDirection, int index)
        {
            var nextVert = verts[index];
            hull.Add(nextVert);
            verts.RemoveAt(index);

            lastVert = nextVert;
            lastEdgeDirection = lineDirection;
        }

        private int FindBestNextVert(out float bestAngle, out Vector2 bestLine)
        {
            int bestIndex = -1;
            bestAngle = float.MaxValue;
            bestLine = lastEdgeDirection;

            for (int i = 0; i < verts.Count; ++i)
            {
                Vector2 line = Vector2.Normalize(lastVert - verts[i]);
                float angle = CalculateAngleFromLastEdge(line);
                if (angle < bestAngle)
                {
                    bestIndex = i;
                    bestAngle = angle;
                    bestLine = line;
                }
            }

            return bestIndex;
        }

        private float CalculateAngleFromLastEdge(Vector2 line)
        {
            return CalculateAngle(lastEdgeDirection, line);
        }

        private int GetIndexOfLeftMostVertex()
        {
            int firstIndex = -1;
            float minX = float.MaxValue;

            for (int i = 0; i < verts.Count; ++i)
            {
                if (verts[i].X < minX)
                {
                    firstIndex = i;
                    minX = verts[i].X;
                }
            }

            return firstIndex;
        }

        private static float CalculateAngle(Vector2 a, Vector2 b)
        {
            var angle = Math.Atan2(b.Y, b.X) - Math.Atan2(a.Y, a.X);
            if (angle < 0)
                angle += (Math.PI * 2);

            return (float)angle;
        }
    }
}

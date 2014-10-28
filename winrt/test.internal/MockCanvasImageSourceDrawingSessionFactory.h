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
    class MockCanvasImageSourceDrawingSessionFactory : public ICanvasImageSourceDrawingSessionFactory
    {
    public:
        CALL_COUNTER_WITH_MOCK(CreateMethod, ComPtr<ICanvasDrawingSession>(ICanvasDevice*, ISurfaceImageSourceNativeWithD2D*, Rect const&, float));

        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            ISurfaceImageSourceNativeWithD2D* sisNative,
            Rect const& updateRect,
            float dpi) const override
        {
            return CreateMethod.WasCalled(owner, sisNative, updateRect, dpi);
        }
    };
}

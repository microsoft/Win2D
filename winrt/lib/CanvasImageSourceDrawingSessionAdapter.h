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

#include "CanvasDrawingSession.h"

namespace canvas
{
    using namespace ABI::Windows::UI::Xaml::Media::Imaging;

    class CanvasImageSourceDrawingSessionAdapter : public ICanvasDrawingSessionAdapter
    {
        ComPtr<ISurfaceImageSourceNativeWithD2D> m_sisNative;

        D2D1_POINT_2F m_renderingSurfaceOffset;

    public:
        static std::shared_ptr<CanvasImageSourceDrawingSessionAdapter> Create(
            ISurfaceImageSourceNativeWithD2D* sisNative,
            const RECT& updateRect,
            float dpi,
            ID2D1DeviceContext1** outDeviceContext);

        CanvasImageSourceDrawingSessionAdapter(
            ISurfaceImageSourceNativeWithD2D* sisNative,
            const D2D1_POINT_2F& renderingSurfaceOffset);

        virtual ~CanvasImageSourceDrawingSessionAdapter() = default;

        virtual void EndDraw() override;

        virtual D2D1_POINT_2F GetRenderingSurfaceOffset() override;
    };
}

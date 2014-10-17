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

#include "pch.h"
#include "CanvasImage.h"
#include "ScopeWarden.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ABI::Windows::Foundation;

    Rect GetImageBoundsImpl(
        ICanvasImageInternal* imageInternal,
        ICanvasDrawingSession *drawingSession,
        Numerics::Matrix3x2 transform)
    {
        auto drawingSessionResourceWrapper = As<ICanvasResourceWrapperNative>(drawingSession);

        ComPtr<ID2D1DeviceContext1> d2dDeviceContext;
        ThrowIfFailed(drawingSessionResourceWrapper->GetResource(IID_PPV_ARGS(&d2dDeviceContext)));

        auto d2dImage = imageInternal->GetD2DImage(d2dDeviceContext.Get());

        D2D1_RECT_F d2dBounds;
        
        D2D1_MATRIX_3X2_F previousTransform;
        d2dDeviceContext->GetTransform(&previousTransform);

        auto restoreTransformWarden = MakeScopeWarden([&] { d2dDeviceContext->SetTransform(previousTransform); });

        d2dDeviceContext->SetTransform(ReinterpretAs<D2D1_MATRIX_3X2_F*>(&transform));

        ThrowIfFailed(d2dDeviceContext->GetImageWorldBounds(d2dImage.Get(), &d2dBounds));

        return FromD2DRect(d2dBounds);
    }
}}}}

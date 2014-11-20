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
    using namespace ::Microsoft::WRL;
    using namespace ABI::Windows::Foundation;

    [uuid(2F434224-053C-4978-87C4-CFAAFA2F4FAC)]
    class ICanvasImageInternal : public IUnknown
    {
    public:
        virtual ComPtr<ID2D1Image> GetD2DImage(ID2D1DeviceContext* deviceContext) = 0;
      
        // GetRealizedEffectNode is a fancier version of GetD2DImage, which propagates the
        // extra information needed to realize an effect graph where input nodes may be
        // either bitmaps or other effects.
        //
        // targetDpi passes in the DPI of the target device context. This is used to
        // determine when a D2D1DpiCompensation effect needs to be inserted. If zero,
        // DPI has already been handled and no further compensation is required.
        //
        // result.Dpi returns the DPI of a source bitmap, or zero if the image does not
        // have a fixed DPI. A D2D1DpiCompensation effect will be inserted whenever
        // targetDpi and imageDpi are different and both non-zero.

        // result.RealizationId is used to efficiently detect when the ID2D1Image returned by an
        // ICanvasImage has changed. Effects are virtualized and may need to re-realize their
        // underlying D2D objects for various reasons. To avoid having to hang on to COM
        // interface pointers (which introduces lifespan complexities) we use a simple integer
        // ID to detect these cases. The contract is that whenever an ICanvasImage implementation 
        // changes its ID2D1Image, it must also report a new realizationId (eg. by incrementing it).

        struct RealizedEffectNode
        {
            ComPtr<ID2D1Image> Image;
            float Dpi;
            uint64_t RealizationId;
        };

        virtual RealizedEffectNode GetRealizedEffectNode(ID2D1DeviceContext* deviceContext, float targetDpi) = 0;
    };

    Rect GetImageBoundsImpl(
        ICanvasImageInternal* imageInternal,
        ICanvasDrawingSession *drawingSession,
        Numerics::Matrix3x2 transform);
}}}}

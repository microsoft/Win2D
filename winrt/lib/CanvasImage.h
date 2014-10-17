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
        // Some image types (effects) are virtualized and may need to re-realize 
        // their underlying D2D object for various reasons. To avoid having to hang 
        // on to COM interface pointers (which introduces lifespan complexities) we 
        // use a simple integer ID to detect these cases. The contract is that 
        // whenever an ICanvasImage implementation changes its GetD2DImage return 
        // value, it must also report a new realizationId (eg. by incrementing it).

        virtual ComPtr<ID2D1Image> GetD2DImage(ID2D1DeviceContext* deviceContext, uint64_t* realizationId = nullptr) = 0;
    };

    Rect GetImageBoundsImpl(
        ICanvasImageInternal* imageInternal,
        ICanvasDrawingSession *drawingSession,
        Numerics::Matrix3x2 transform);
}}}}

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

#include "ResourceManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace DirectX::Direct3D11;
    using namespace ::Microsoft::WRL;
    using namespace ABI::Microsoft::Graphics::Canvas::DirectX;
    using namespace ABI::Windows::UI;

    class CanvasBitmapManager;
    class CanvasRenderTargetManager;
    class ICanvasBitmapResourceCreationAdapter;

    inline bool IsRenderTargetBitmap(ID2D1Bitmap1* d2dBitmap)
    {
        auto options = d2dBitmap->GetOptions();
        return (options & D2D1_BITMAP_OPTIONS_TARGET) != 0;
    }

    //
    // Keeps track of Win2D objects that wrap ID2D1Bitmaps.  Depending on their
    // properties, ID2D1Bitmaps can be wrapped by either CanvasBitmap or
    // CanvasRenderTarget.  For this reason the standard pattern of one-manager
    // per activation factory is insufficient.
    //
    // PolymorphicBitmapManager holds a CanvasBitmapManager and
    // CanvasRenderTargetManager.  The non-wrapping constructors, Create*, are
    // delegated directly to the appropriate manager.  The GetOrCreate*
    // overloads first examines the properties on the bitmap to determine which
    // manager to delegate to.
    //
    class PolymorphicBitmapManager
    {
        std::shared_ptr<CanvasBitmapManager> m_bitmapManager;
        std::shared_ptr<CanvasRenderTargetManager> m_renderTargetManager;

    public:
        PolymorphicBitmapManager(std::shared_ptr<ICanvasBitmapResourceCreationAdapter> adapter);

        template<typename... Arguments>
        ComPtr<CanvasBitmap> CreateBitmap(Arguments&&... args)
        {
            return m_bitmapManager->Create(args...);
        }

        template<typename... Arguments>
        ComPtr<CanvasRenderTarget> CreateRenderTarget(Arguments&&... args)
        {
            return m_renderTargetManager->Create(args...);
        }

        ComPtr<ICanvasBitmap> CreateBitmapFromSurface(ICanvasDevice* device, IDirect3DSurface* surface, CanvasAlphaBehavior alpha, float dpi);
        ComPtr<CanvasRenderTarget> CreateRenderTargetFromSurface(ICanvasDevice* device, IDirect3DSurface* surface, CanvasAlphaBehavior alpha, float dpi);

        ComPtr<ICanvasBitmap> CreateBitmapFromBytes(
            ICanvasDevice* device,
            uint32_t byteCount,
            BYTE* bytes,
            float width,
            float height,
            DirectXPixelFormat format,
            CanvasAlphaBehavior alpha,
            float dpi);

        ComPtr<ICanvasBitmap> CreateBitmapFromColors(
            ICanvasDevice* device,
            uint32_t colorCount,
            Color* colors,
            float width,
            float height,
            CanvasAlphaBehavior alpha,
            float dpi);

        //
        // Returns a wrapper around the given d2dBitmap.  Depending on the
        // bitmap properties a CanvasBitmap or CanvasRenderTarget instance will
        // be returned.
        //
        ComPtr<ICanvasBitmap> GetOrCreateBitmap(ICanvasDevice* device, ID2D1Bitmap1* d2dBitmap);

        //
        // Returns a wrapper around the given d2dBitmap.  This will only return
        // a CanvasRenderTarget and will fail if the d2dBitmap's properties are
        // incompatible with CanvasRenderTarget.
        //
        ComPtr<CanvasRenderTarget> GetOrCreateRenderTarget(ICanvasDevice* device, ID2D1Bitmap1* d2dBitmap);

    private:

        ComPtr<ICanvasBitmap> CreateBitmapFromBytesImpl(
            ICanvasDevice* device,
            float rawWidth,
            float rawHeight,
            uint32_t byteCount,
            BYTE* bytes,
            DirectXPixelFormat format,
            CanvasAlphaBehavior alpha,
            float dpi);
    };


    //
    // CanvasBitmapFactory and CanvasRenderTargetFactory derive from this to
    // ensure that they both share the same instance of
    // PolymorphicBitmapManager.
    //
    class PerApplicationPolymorphicBitmapManager : public PerApplicationManager<PerApplicationPolymorphicBitmapManager, PolymorphicBitmapManager>
    {
    public:
        // See PerApplicationManager's public interface

        // Called by PerApplicationManager
        static std::shared_ptr<PolymorphicBitmapManager> CreateManager();
    };

}}}}

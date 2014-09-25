// Copyright (c) Microsoft Corporation.  All rights reserved

#pragma once

#include "ResourceManager.h"

namespace ABI { namespace Microsoft { namespace Graphics { namespace Canvas
{
    using namespace ::Microsoft::WRL;

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

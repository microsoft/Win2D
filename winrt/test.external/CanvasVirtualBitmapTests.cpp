// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

TEST_CLASS(CanvasVirtualBitmapTests)
{
    CanvasDevice^ m_device;

public:    
    CanvasVirtualBitmapTests()
        : m_device(ref new CanvasDevice())
    {        
    }
    
    TEST_METHOD(CanvasVirtualBitmap_Load_FileNotFound)
    {
        auto async = CanvasVirtualBitmap::LoadAsync(m_device, "FileThatDoesNotExist.jpg");
        ExpectCOMException(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), [&] { WaitExecution(async); });

        async = CanvasVirtualBitmap::LoadAsync(m_device, ref new Uri("ms-appx:///doesnotexist"));
        ExpectCOMException(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), [&] { WaitExecution(async); });
    }

    TEST_METHOD(CanvasVirtualBitmap_Load)
    {
        auto fromFileName = CanvasVirtualBitmap::LoadAsync(m_device, "Assets/HighDpiGrid.png");
        auto bitmapFromFileName = WaitExecution(fromFileName);
        Assert::AreEqual(4.0f, bitmapFromFileName->Size.Width);
        Assert::AreEqual(4.0f, bitmapFromFileName->Size.Height);
        
        auto uri = ref new Uri("ms-appx:///Assets/HighDpiGrid.png");

        auto fromUri = CanvasVirtualBitmap::LoadAsync(m_device, uri);
        auto bitmapFromUri = WaitExecution(fromUri);
        Assert::AreEqual(4.0f, bitmapFromUri->Size.Width);
        Assert::AreEqual(4.0f, bitmapFromUri->Size.Height);

            
        auto storageFile = WaitExecution(Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri));
        auto stream = WaitExecution(storageFile->OpenReadAsync());

        auto fromStream = CanvasVirtualBitmap::LoadAsync(m_device, stream);
        auto bitmapFromStream = WaitExecution(fromStream);
        Assert::AreEqual(4.0f, bitmapFromStream->Size.Width);
        Assert::AreEqual(4.0f, bitmapFromStream->Size.Height);
    }

    TEST_METHOD(CanvasVirtualBitmap_GetBounds)
    {
        auto virtualBitmap = WaitExecution(CanvasVirtualBitmap::LoadAsync(m_device, "Assets/HighDpiGrid.png"));
        
        auto rt = ref new CanvasRenderTarget(m_device, 128, 128, 96);
        auto ds = rt->CreateDrawingSession();

        auto bounds = virtualBitmap->GetBounds(ds);

        Assert::AreEqual(0.0f, bounds.X);
        Assert::AreEqual(0.0f, bounds.Y);
        Assert::AreEqual(4.0f, bounds.Width);
        Assert::AreEqual(4.0f, bounds.Height);
    }
};

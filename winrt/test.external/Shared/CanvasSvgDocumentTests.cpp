// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

#if WINVER > _WIN32_WINNT_WINBLUE

using namespace Microsoft::Graphics::Canvas::Svg;
using namespace Windows::Storage::Streams;

TEST_CLASS(CanvasSvgDocumentTests)
{
    CanvasDevice^ m_device;

public:
    CanvasSvgDocumentTests()
    {
        // Win2D activity which causes a D2D error will be bubbled up as a "WinRT originate error" if the
        // debug layer is turned on.
        CanvasDevice::DebugLevel = CanvasDebugLevel::None;
        m_device = ref new CanvasDevice();
    }

    TEST_METHOD(CanvasSvgDocument_IsSupported)
    {
        Assert::IsTrue(CanvasSvgDocument::IsSupported(m_device));
    }

    TEST_METHOD(CanvasSvgDocument_InteropFailAfterClosure)
    {
        auto canvasSvgDocument = CanvasSvgDocument::Load(m_device, "<svg/>");

        delete canvasSvgDocument;

        ExpectObjectClosed([&]{ GetWrappedResource<ID2D1SvgDocument>(canvasSvgDocument); });
    }

    TEST_METHOD(CanvasSvgDocument_NativeInterop)
    {
        auto d2dDeviceContext = As<ID2D1DeviceContext5>(CreateTestD2DDeviceContext(m_device));
        D2D1_GRADIENT_MESH_PATCH patch = {};

        ComPtr<ID2D1SvgDocument> originalSvgDocument;
        ThrowIfFailed(d2dDeviceContext->CreateSvgDocument(nullptr, D2D1::SizeF(1, 1), &originalSvgDocument));

        auto canvasSvgDocument = GetOrCreate<CanvasSvgDocument>(m_device, originalSvgDocument.Get());

        auto retrievedSvgDocument = GetWrappedResource<ID2D1SvgDocument>(canvasSvgDocument);

        Assert::IsTrue(IsSameInstance(originalSvgDocument.Get(), retrievedSvgDocument.Get()));
    }

    TEST_METHOD(CanvasSvgDocument_NullParameters)
    {
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasSvgDocument::Load(nullptr, "<svg/>");
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasSvgDocument::Load(m_device, nullptr);
            });
    }

    TEST_METHOD(CanvasSvgDocument_StringIsNotValidSvg)
    {
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasSvgDocument::Load(m_device, "");
            });
        
        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasSvgDocument::Load(m_device, "not SVG");
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasSvgDocument::Load(m_device, L"<svg>");
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasSvgDocument::Load(m_device, L"这个文字不是SVG");
            });

        Assert::ExpectException<Platform::InvalidArgumentException^>(
            [&]
            {
                CanvasSvgDocument::Load(m_device, L"🌄🌄🌄🌄🌄🌄");
            });
    }

    struct LoadingAndSavingFixture
    {
        CanvasDevice^ m_device;
        CanvasSvgDocument^ m_document;

        LoadingAndSavingFixture(CanvasDevice^ device)
            : m_device(device)
        {
        }

        void LoadDocument(int loadMethod, Platform::String^ input)
        {
            if (loadMethod == 0)
            {
                m_document = CanvasSvgDocument::Load(m_device, input);
            }
            else
            {
                InMemoryRandomAccessStream^ memoryStream = ref new InMemoryRandomAccessStream();
                DataWriter^ dataWriter = ref new DataWriter(memoryStream->GetOutputStreamAt(0));
                dataWriter->WriteString(input);
                WaitExecution(dataWriter->StoreAsync());

                m_document = WaitExecution(CanvasSvgDocument::LoadAsync(m_device, memoryStream));
            }
        }

        Platform::String^ SaveDocument(int saveMethod)
        {
            Platform::String^ readbackString;

            if (saveMethod == 0)
            {
                readbackString = m_document->GetXml();
            }
            else
            {
                InMemoryRandomAccessStream^ memoryStream = ref new InMemoryRandomAccessStream();
                WaitExecution(m_document->SaveAsync(memoryStream));

                auto streamSize = static_cast<uint32_t>(memoryStream->Size);

                DataReader^ dataReader = ref new DataReader(memoryStream->GetInputStreamAt(0));
                WaitExecution(dataReader->LoadAsync(streamSize));

                readbackString = dataReader->ReadString(streamSize);
            }

            return readbackString;
        }
    };

    TEST_METHOD(CanvasSvgDocument_LoadingAndSaving)
    {
        struct TestCase
        {
            Platform::String^ Input;
            Platform::String^ Expected;
        } testCases[] =
        {
            { 
                // ASCII
                "<svg/>", 
                "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\r\n</svg>" 
            },
            { 
                // Non-ASCII
                LR"SVG(<svg><g id="テスト・ストリング"/></svg>)SVG",
                L"<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\r\n  <g id=\"テスト・ストリング\">\r\n  </g>\r\n</svg>"
            },
            { 
                // Instances where single characters have multiple UTF-16 codepoints
                LR"SVG(<svg><g id="☀🌤🌥🌦"/></svg>)SVG", 
                L"<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\r\n  <g id=\"☀🌤🌥🌦\">\r\n  </g>\r\n</svg>" 
            }
        };

        for (int loadMethod = 0; loadMethod < 2; loadMethod++)
        {
            for (int saveMethod = 0; saveMethod < 2; saveMethod++)
            {
                for (auto const& testCase : testCases)
                {
                    LoadingAndSavingFixture f(m_device);

                    f.LoadDocument(loadMethod, testCase.Input);

                    Platform::String^ readbackString = f.SaveDocument(saveMethod);

                    Assert::AreEqual(testCase.Expected, readbackString);
                }
            }
        }
    }
};

#endif
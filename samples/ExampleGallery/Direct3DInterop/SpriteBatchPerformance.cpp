// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "pch.h"

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::DirectX::Direct3D11;

namespace ExampleGallery
{
    namespace Direct3DInterop
    {
        namespace SpriteBatchPerformance
        {
            class D2DLock
            {
                ComPtr<ID2D1Multithread> m_d2dMultithread;
            public:
                D2DLock(ComPtr<ID2D1Multithread> const& multiThread)
                    : m_d2dMultithread(multiThread)
                {
                    if (m_d2dMultithread->GetMultithreadProtected())
                    {
                        m_d2dMultithread->Enter();
                    }
                    else
                    {
                        m_d2dMultithread.Reset();
                    }
                }

                ~D2DLock()
                {
                    Unlock();
                }

                void Unlock()
                {
                    if (m_d2dMultithread)
                    {
                        m_d2dMultithread->Leave();
                        m_d2dMultithread.Reset();
                    }
                }
            };
            
            //
            // This records how long something takes to execute on the GPU.
            //
            public ref class GpuStopWatch sealed
            {
                ComPtr<ID2D1Multithread> m_d2dMultithread;

                ComPtr<ID3D11Device> m_d3dDevice;
                ComPtr<ID3D11DeviceContext> m_deviceContext;
                
                ComPtr<ID3D11Query> m_disjointQuery;
                ComPtr<ID3D11Query> m_timestampStartQuery;
                ComPtr<ID3D11Query> m_timestampEndQuery;

                enum class State
                {
                    New, Started, Stopped, Error
                };

                State m_state = State::New;
                
            public:
                GpuStopWatch(CanvasDevice^ canvasDevice)
                {
                    // Get the ID2D1Multithread for the device
                    auto d2dDevice = GetWrappedResource<ID2D1Device>(canvasDevice);

                    ComPtr<ID2D1Factory> factory;
                    d2dDevice->GetFactory(&factory);
                    
                    __abi_ThrowIfFailed(factory.As(&m_d2dMultithread));

                    // Get the D3D interfaces
                    __abi_ThrowIfFailed(GetDXGIInterface(canvasDevice, m_d3dDevice.ReleaseAndGetAddressOf()));
                    m_d3dDevice->GetImmediateContext(&m_deviceContext);

                    // Create the queries
                    D2DLock lock(m_d2dMultithread);
                    
                    D3D11_QUERY_DESC desc{};
                    desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

                    HRESULT hr = m_d3dDevice->CreateQuery(&desc, &m_disjointQuery);

                    if (SUCCEEDED(hr))
                    {
                        desc.Query = D3D11_QUERY_TIMESTAMP;
                        hr = m_d3dDevice->CreateQuery(&desc, &m_timestampStartQuery);
                    }

                    if (SUCCEEDED(hr))
                    {
                        hr = m_d3dDevice->CreateQuery(&desc, &m_timestampEndQuery);
                    }

                    if (FAILED(hr))
                    {
                        m_state = State::Error;
                    }
                }

                virtual ~GpuStopWatch()
                {
                    // having a virtual destructor allows us to Dispose() this object.
                }
                
                void Start()
                {
                    if (m_state == State::Error)
                        return;
                    
                    if (m_state != State::New)
                        throw ref new Platform::FailureException();
                    
                    D2DLock lock(m_d2dMultithread);
                    m_deviceContext->Begin(m_disjointQuery.Get());
                    m_deviceContext->End(m_timestampStartQuery.Get());

                    m_state = State::Started;
                }

                void Stop()
                {
                    if (m_state == State::Error)
                        return;
                    
                    if (m_state != State::Started)
                        throw ref new Platform::FailureException();
                    
                    D2DLock lock(m_d2dMultithread);
                    m_deviceContext->End(m_timestampEndQuery.Get());
                    m_deviceContext->End(m_disjointQuery.Get());

                    m_state = State::Stopped;
                }

                double GetGpuTimeInMs()
                {
                    if (m_state == State::Error)
                        return 0;
                    
                    if (m_state != State::Stopped)
                        throw ref new Platform::FailureException();

                    uint64_t startTime;
                    uint64_t endTime;
                    D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;

                    D2DLock lock(m_d2dMultithread);
                    BlockAndGetQueryData(m_timestampStartQuery, &startTime);
                    BlockAndGetQueryData(m_timestampEndQuery, &endTime);
                    BlockAndGetQueryData(m_disjointQuery, &disjointData);

                    if (disjointData.Disjoint)
                    {
                        // this data isn't reliable - we return a negative
                        // number to indicate this
                        return -1.0;
                    }

                    auto delta = endTime - startTime;
                    auto frequency = static_cast<double>(disjointData.Frequency);

                    return (delta / frequency) * 1000.0;
                }

            private:
                template<typename T>
                void BlockAndGetQueryData(ComPtr<ID3D11Query> const& query, T* data)
                {
                    HRESULT hr = S_FALSE;
                    
                    do
                    {
                        hr = m_deviceContext->GetData(query.Get(), data, sizeof(T), 0);
                        __abi_ThrowIfFailed(hr);

                        // GetData returns S_FALSE to indicate that the data
                        // isn't ready yet.
                    } while (hr != S_OK);
                }
            };
            
            //
            // The sprite batch performance tests need to be able to run
            // multiple scenarios against the same set of sprites.
            //
            // However, we don't want to pay the price for passing this data
            // into the scenario runner as part of the scenario, so we call
            // AddSprite before we start timing to populate the runner.
            //
            // The runners do as little work as possible in AddSprite since the
            // time taken to process AddSprite is not counted towards the
            // scenario time.
            //
            public interface class IScenarioRunner
            {
                virtual void AddSprite(CanvasBitmap^ bitmap, float4 tint, float2 position, float rotation) = 0;
                virtual void RunScenario(CanvasDrawingSession^ drawingSession, CanvasSpriteSortMode sortMode) = 0;
            };

            struct Sprite
            {
                CanvasBitmap^ Bitmap;
                float4 Tint;
                float2 Position;
                float Rotation;
            };

            //
            // This runner uses Win2D via C++/CX to draw the sprites.
            //
            public ref class CppWin2DScenarioRunner sealed : public IScenarioRunner
            {
                std::vector<Sprite> m_sprites;

            public:
                virtual void AddSprite(CanvasBitmap^ bitmap, float4 tint, float2 position, float rotation)
                {
                    m_sprites.push_back(Sprite{ bitmap, tint, position, rotation });
                }

                virtual void RunScenario(CanvasDrawingSession^ drawingSession, CanvasSpriteSortMode sortMode)
                {
                    auto spriteBatch = drawingSession->CreateSpriteBatch(sortMode);
                    for (auto const& sprite : m_sprites)
                    {
                        spriteBatch->Draw(sprite.Bitmap, sprite.Position, sprite.Tint, float2::zero(), sprite.Rotation, float2::one(), CanvasSpriteFlip::None);
                    }
                    delete spriteBatch;
                }
            };

            
            //
            // This runner calls Direct2D's SpriteBatch APIs directly.
            //
            public ref class Direct2DScenarioRunner sealed : public IScenarioRunner
            {
                std::vector<Sprite> m_sprites;
                
            public:
                virtual void AddSprite(CanvasBitmap^ bitmap, float4 tint, float2 position, float rotation)
                {
                    m_sprites.push_back(Sprite{ bitmap, tint, position, rotation });
                }

                virtual void RunScenario(CanvasDrawingSession^ drawingSession, CanvasSpriteSortMode sortMode)
                {
                    UNREFERENCED_PARAMETER(sortMode); // sort mode is ignored

                    std::vector<ID2D1Bitmap*> bitmaps;
                    std::vector<D2D1_RECT_F> destRects;
                    std::vector<D2D1_COLOR_F> colors;
                    std::vector<D2D1_MATRIX_3X2_F> transforms;

                    for (auto const& sprite : m_sprites)
                    {
                        bitmaps.push_back(GetWrappedResource<ID2D1Bitmap>(sprite.Bitmap).Get());

                        auto size = sprite.Bitmap->Size;
                        destRects.push_back(D2D1_RECT_F{ 0, 0, size.Width, size.Height });
                        colors.push_back(D2D1_COLOR_F{ sprite.Tint.x, sprite.Tint.y, sprite.Tint.z, sprite.Tint.w });
                        transforms.push_back(D2D1::Matrix3x2F::Rotation(DirectX::XMConvertToDegrees(sprite.Rotation)) * D2D1::Matrix3x2F::Translation(sprite.Position.x, sprite.Position.y));
                    }

                    if (destRects.empty())
                        return;

                    auto deviceContext = GetWrappedResource<ID2D1DeviceContext3>(drawingSession);

                    auto oldAntialiasMode = deviceContext->GetAntialiasMode();
                    deviceContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

                    ComPtr<ID2D1SpriteBatch> spriteBatch;
                    __abi_ThrowIfFailed(deviceContext->CreateSpriteBatch(&spriteBatch));

                    __abi_ThrowIfFailed(spriteBatch->AddSprites(
                        static_cast<uint32_t>(destRects.size()),
                        destRects.data(),
                        nullptr, // source rectangles
                        colors.data(),
                        transforms.data()));

                    uint32_t startIndex = 0;
                    uint32_t endIndex = 0;

                    ID2D1Bitmap* lastBitmap = bitmaps.front();

                    for (auto bitmap : bitmaps)
                    {
                        if (bitmap != lastBitmap)
                        {
                            deviceContext->DrawSpriteBatch(
                                spriteBatch.Get(),
                                startIndex,
                                endIndex - startIndex,
                                lastBitmap);

                            lastBitmap = bitmap;
                            startIndex = endIndex;
                        }

                        ++endIndex;
                    }

                    if (endIndex > startIndex)
                    {
                        deviceContext->DrawSpriteBatch(
                            spriteBatch.Get(),
                            startIndex,
                            endIndex - startIndex,
                            lastBitmap);
                    }

                    deviceContext->SetAntialiasMode(oldAntialiasMode);
                }
            };
        }
    }
}

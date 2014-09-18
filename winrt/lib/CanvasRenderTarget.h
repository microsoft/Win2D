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

    class ICanvasRenderTargetDrawingSessionFactory
    {
    public:
        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            ID2D1Bitmap1* targetBitmap) const = 0;
    };    

    class CanvasRenderTargetFactory : public ActivationFactory<
        ICanvasRenderTargetFactory>
    {
        std::shared_ptr<ICanvasRenderTargetDrawingSessionFactory> m_drawingSessionFactory;
    public:

        CanvasRenderTargetFactory();
    
        IFACEMETHOD(Create)(
            ICanvasResourceCreator* resourceCreator,
            ABI::Windows::Foundation::Size size,
            ICanvasRenderTarget** renderTarget);
    };

    class CanvasRenderTarget : public RuntimeClass<
        ICanvasRenderTarget,
        ComposableBase<ICanvasBitmapFactory>>
    {
        InspectableClass(RuntimeClass_Microsoft_Graphics_Canvas_CanvasRenderTarget, BaseTrust);

        ComPtr<ICanvasDevice> m_device;

        std::shared_ptr<ICanvasRenderTargetDrawingSessionFactory> m_drawingSessionFactory;

    public:
        
        CanvasRenderTarget(
            ICanvasDevice* canvasDevice,
            ABI::Windows::Foundation::Size size,
            std::shared_ptr<ICanvasRenderTargetDrawingSessionFactory> drawingSessionFactory);

        IFACEMETHOD(CreateDrawingSession)(
            _COM_Outptr_ ICanvasDrawingSession** drawingSession) override;
    };

    class CanvasDrawingSessionManager;

    class CanvasRenderTargetDrawingSessionFactory : public ICanvasRenderTargetDrawingSessionFactory
    {
        std::shared_ptr<CanvasDrawingSessionManager> m_drawingSessionManager;

    public:
        CanvasRenderTargetDrawingSessionFactory();

        virtual ComPtr<ICanvasDrawingSession> Create(
            ICanvasDevice* owner,
            ID2D1Bitmap1* targetBitmap) const override;
    };

}}}}

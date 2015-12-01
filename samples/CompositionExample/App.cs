// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI.Composition;
using System;
using System.Numerics;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.UI.Composition;
using Windows.UI.Core;
using Windows.UI.Popups;

namespace CompositionExample
{
    class App : IFrameworkView
    {
        CoreWindow window;
        Compositor compositor;
        CanvasDevice device;
        CompositionGraphicsDevice compositionGraphicsDevice;

        DrawingSurfaceRenderer drawingSurfaceRenderer;
        SwapChainRenderer swapChainRenderer;

        CancellationTokenSource cancellationTokenSource = new CancellationTokenSource();
        Random rnd = new Random();
        
        public void Initialize(CoreApplicationView applicationView)
        {
            applicationView.Activated += applicationView_Activated;
        }

        public void Uninitialize()
        {
            swapChainRenderer?.Dispose();
            cancellationTokenSource.Cancel();
        }

        void applicationView_Activated(CoreApplicationView sender, IActivatedEventArgs args)
        {
            CoreWindow.GetForCurrentThread().Activate();
        }

        public void Load(string entryPoint)
        {
        }

        public void Run()
        {
            CoreWindow.GetForCurrentThread().Dispatcher.ProcessEvents(CoreProcessEventsOption.ProcessUntilQuit);
        }

        public async void SetWindow(CoreWindow window)
        {
            this.window = window;

            if (!Windows.Foundation.Metadata.ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 2))
            {
                var dialog = new MessageDialog("This version of Windows does not support the Composition APIs.");
                await dialog.ShowAsync();
                CoreApplication.Exit();
                return;
            }

            window.PointerPressed += Window_PointerPressed;

            CoreApplication.Suspending += CoreApplication_Suspending;

            compositor = new Compositor();

            CreateDevice();

            drawingSurfaceRenderer = new DrawingSurfaceRenderer(compositor, compositionGraphicsDevice);

            swapChainRenderer = new SwapChainRenderer(compositor);
            swapChainRenderer.SetDevice(device, new Size(window.Bounds.Width, window.Bounds.Height));

            drawingSurfaceRenderer.Visual.Offset = new Vector3(-drawingSurfaceRenderer.Size.ToVector2(), 0);
            swapChainRenderer.Visual.Offset = new Vector3((float)window.Bounds.Width, (float)window.Bounds.Height, 0);
            
            var rootVisual = compositor.CreateContainerVisual();
            rootVisual.Children.InsertAtTop(swapChainRenderer.Visual);
            rootVisual.Children.InsertAtTop(drawingSurfaceRenderer.Visual);

            compositor.CreateTargetForCurrentView().Root = rootVisual;

            var ignoredTask = UpdateVisualsLoop();
        }

        async Task UpdateVisualsLoop()
        {
            var token = cancellationTokenSource.Token;

            while (!token.IsCancellationRequested)
            {
                UpdateVisual(swapChainRenderer.Visual, swapChainRenderer.Size);
                UpdateVisual(drawingSurfaceRenderer.Visual, drawingSurfaceRenderer.Size);

                await Task.Delay(TimeSpan.FromSeconds(2));
            }
        }

        void UpdateVisual(Visual visual, Size size)
        {
            UpdateVisualPosition(visual, size);
            UpdateVisualOpacity(visual);
        }

        void UpdateVisualPosition(Visual visual, Size size)
        {
            var oldOffset = visual.Offset;
            var newOffset = new Vector3(
                (float)(rnd.NextDouble() * (window.Bounds.Width - size.Width)),
                (float)(rnd.NextDouble() * (window.Bounds.Height - size.Height)),
                0);

            visual.Offset = newOffset;
            visual.Size = size.ToVector2();

            AnimateOffset(visual, oldOffset, newOffset);
        }

        void UpdateVisualOpacity(Visual visual)
        {
            var oldOpacity = visual.Opacity;
            var newOpacity = (float)rnd.NextDouble();

            var animation = compositor.CreateScalarKeyFrameAnimation();
            animation.InsertKeyFrame(0, oldOpacity);
            animation.InsertKeyFrame(1, newOpacity);

            visual.Opacity = newOpacity;
            visual.StartAnimation("Opacity", animation);
        }

        void AnimateOffset(Visual visual, Vector3 oldOffset, Vector3 newOffset)
        {
            var animation = compositor.CreateVector3KeyFrameAnimation();
            animation.InsertKeyFrame(0, oldOffset);
            animation.InsertKeyFrame(1, newOffset);
            animation.Duration = TimeSpan.FromSeconds(1);

            visual.StartAnimation("Offset", animation);
        }

        void Window_PointerPressed(CoreWindow sender, PointerEventArgs args)
        {
            swapChainRenderer.Paused = !swapChainRenderer.Paused;
        }

        void CoreApplication_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs args)
        {
            try
            {
                device.Trim();
            }
            catch (Exception e) when (device.IsDeviceLost(e.HResult))
            {
                device.RaiseDeviceLost();
            }
        }

        void CreateDevice()
        {
            device = CanvasDevice.GetSharedDevice();
            device.DeviceLost += Device_DeviceLost;

            if (compositionGraphicsDevice == null)
            {
                compositionGraphicsDevice = CanvasComposition.CreateCompositionGraphicsDevice(compositor, device);
            }
            else
            {
                CanvasComposition.SetCanvasDevice(compositionGraphicsDevice, device);
            }

            if (swapChainRenderer != null)
                swapChainRenderer.SetDevice(device, new Size(window.Bounds.Width, window.Bounds.Height));
        }

        void Device_DeviceLost(CanvasDevice sender, object args)
        {
            device.DeviceLost -= Device_DeviceLost;
            var unwaitedTask = window.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => CreateDevice());
        }
    }

    class ViewSource : IFrameworkViewSource
    {
        public IFrameworkView CreateView()
        {
            return new App();
        }

        static void Main(string[] args)
        {
            CoreApplication.Run(new ViewSource());
        }
    }
}

// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;

namespace CoreWindowExample
{
    class App : IFrameworkView
    {
        Renderer renderer;

        public void Initialize(CoreApplicationView applicationView)
        {
            applicationView.Activated += applicationView_Activated;
            CoreApplication.Suspending += CoreApplication_Suspending;
        }

        public void Load(string entryPoint)
        {
        }

        public void Run()
        {
            while (true)
            {
                CoreWindow.GetForCurrentThread().Dispatcher.ProcessEvents(CoreProcessEventsOption.ProcessAllIfPresent);
                renderer.Render();
            }
        }

        public void SetWindow(CoreWindow window)
        {
            renderer = new Renderer(window);
        }

        private void CoreApplication_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            renderer.Trim();
        }

        private void applicationView_Activated(CoreApplicationView sender, IActivatedEventArgs args)
        {
            CoreWindow.GetForCurrentThread().Activate();
        }

        public void Uninitialize()
        {
        }

        static void Main(string[] args)
        {
            CoreApplication.Run(new ViewSource());
        }
    }

    class ViewSource : IFrameworkViewSource
    {
        public IFrameworkView CreateView()
        {
            return new App();
        }
    }
}

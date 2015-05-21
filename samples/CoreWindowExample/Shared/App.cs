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

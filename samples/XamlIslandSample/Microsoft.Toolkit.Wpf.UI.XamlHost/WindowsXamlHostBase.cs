// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.Runtime.InteropServices;
using System.Windows.Interop;
using Microsoft.Toolkit.Win32.UI.XamlHost;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using windows = Windows;

namespace Microsoft.Toolkit.Wpf.UI.XamlHost
{
    /// <summary>
    /// WindowsXamlHost control hosts UWP XAML content inside the Windows Presentation Foundation
    /// </summary>
    public abstract partial class WindowsXamlHostBase : HwndHost
    {
        /// <summary>
        /// UWP XAML Application instance and root UWP XamlMetadataProvider.  Custom implementation required to
        /// probe at runtime for custom UWP XAML type information.  This must be created before
        /// creating any DesktopWindowXamlSource instances if custom UWP XAML types are required.
        /// </summary>
        private readonly windows.UI.Xaml.Application _application;

        /// <summary>
        /// UWP XAML DesktopWindowXamlSource instance that hosts XAML content in a win32 application
        /// </summary>
        private readonly windows.UI.Xaml.Hosting.DesktopWindowXamlSource _xamlSource;

        /// <summary>
        /// A reference count on the UWP XAML framework is tied to WindowsXamlManager's
        /// lifetime.  UWP XAML is spun up on the first WindowsXamlManager creation and
        /// deinitialized when the last instance of WindowsXamlManager is destroyed.
        /// </summary>
        private readonly windows.UI.Xaml.Hosting.WindowsXamlManager _windowsXamlManager;

        /// <summary>
        /// Private field that backs ChildInternal property.
        /// </summary>
        private UIElement _childInternal;

        /// <summary>
        ///     Fired when WindowsXamlHost root UWP XAML content has been updated
        /// </summary>
        public event EventHandler ChildChanged;

        /// <summary>
        ///     When a window containing Xaml content moves, Xaml closes all open popups. We need the same behavior for Xaml
        ///     content in the DesktopWindowXamlSource. Since the DesktopWindowXamlSource itself is not notified when the WPF
        ///     Window moves, we attach handlers to the WPF Window's SizeChanged and LocationChanged events and use the Xaml
        ///     VisualTreeHelper API to close all open popups in an event handler. The WPF Window is not reachable until after
        ///     this control is created. This field tracks the WPF Window so we can detach the event handlers during cleanup.
        /// </summary>
        private System.Windows.Window _window;

        /// <summary>
        /// Initializes a new instance of the <see cref="WindowsXamlHostBase"/> class.
        /// </summary>
        /// <remarks>
        /// Default constructor is required for use in WPF markup. When the default constructor is called,
        /// object properties have not been set. Put WPF logic in OnInitialized.
        /// </remarks>
        public WindowsXamlHostBase()
        {
            // Windows.UI.Xaml.Application object is required for loading custom control metadata.  If a custom
            // Application object is not provided by the application, the host control will create one (XamlApplication).
            // Instantiation of the application object must occur before creating the DesktopWindowXamlSource instance.
            // If no Application object is created before DesktopWindowXamlSource is created, DesktopWindowXamlSource
            // will create a generic Application object unable to load custom UWP XAML metadata.
            Microsoft.Toolkit.Win32.UI.XamlHost.XamlApplication.GetOrCreateXamlApplicationInstance(ref _application);

            // Create an instance of the WindowsXamlManager. This initializes and holds a
            // reference on the UWP XAML DXamlCore and must be explicitly created before
            // any UWP XAML types are programmatically created.  If WindowsXamlManager has
            // not been created before creating DesktopWindowXamlSource, DesktopWindowXaml source
            // will create an instance of WindowsXamlManager internally.  (Creation is explicit
            // here to illustrate how to initialize UWP XAML before initializing the DesktopWindowXamlSource.)
            _windowsXamlManager = windows.UI.Xaml.Hosting.WindowsXamlManager.InitializeForCurrentThread();

            // Create DesktopWindowXamlSource, host for UWP XAML content
            _xamlSource = new windows.UI.Xaml.Hosting.DesktopWindowXamlSource();

            // Hook DesktopWindowXamlSource OnTakeFocus event for Focus processing
            _xamlSource.TakeFocusRequested += OnTakeFocusRequested;

            Loaded += WindowsXamlHostBase_Loaded;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="WindowsXamlHostBase"/> class.
        /// </summary>
        /// <remarks>
        /// Constructor is required for use in WPF markup. When the default constructor is called,
        /// object properties have not been set. Put WPF logic in OnInitialized.
        /// </remarks>
        /// <param name="typeName">UWP XAML Type name</param>
        public WindowsXamlHostBase(string typeName)
            : this()
        {
            ChildInternal = UWPTypeFactory.CreateXamlContentByType(typeName);
            ChildInternal.SetWrapper(this);
        }

        /// <summary>
        /// Attaches an event handler to Window.LocationChanged to close all popups opened by the
        /// Xaml content inside the DesktopWindowXamlSource (Only if <code>Windows.UI.Xaml.XamlRoot</code> is present,
        /// because this type is needed in the event handler).
        /// </summary>
        private void WindowsXamlHostBase_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            if (_window == null && ApiInformation.IsTypePresent("Windows.UI.Xaml.XamlRoot"))
            {
                _window = System.Windows.Window.GetWindow(this);
                _window.LocationChanged += OnWindowLocationChanged;
            }
        }

        /// <summary>
        /// Close all popups opened by the Xaml content inside the DesktopWindowXamlSource.
        /// </summary>
        private void OnWindowLocationChanged(object sender, EventArgs e)
        {
#pragma warning disable 8305    // Experimental API
            XamlRoot xamlRoot = _childInternal.XamlRoot;
            var openPopups = VisualTreeHelper.GetOpenPopupsForXamlRoot(xamlRoot);
            foreach (windows.UI.Xaml.Controls.Primitives.Popup popup in openPopups)
            {
                // Toggle the CompositeMode property, which will force all windowed Popups
                // to reposition themselves relative to the new position of the host window.
                var compositeMode = popup.CompositeMode;

                // Set CompositeMode to some value it currently isn't set to.
                if (compositeMode == ElementCompositeMode.SourceOver)
                {
                    popup.CompositeMode = ElementCompositeMode.MinBlend;
                }
                else
                {
                    popup.CompositeMode = ElementCompositeMode.SourceOver;
                }

                // Restore CompositeMode to whatever it was originally set to.
                popup.CompositeMode = compositeMode;
            }
        }

        /// <summary>
        /// Binds this wrapper object's exposed WPF DependencyProperty with the wrapped UWP object's DependencyProperty
        /// for what effectively works as a regular one- or two-way binding.
        /// </summary>
        /// <param name="propertyName">the registered name of the dependency property</param>
        /// <param name="wpfProperty">the DependencyProperty of the wrapper</param>
        /// <param name="uwpProperty">the related DependencyProperty of the UWP control</param>
        /// <param name="converter">a converter, if one's needed</param>
        /// <param name="direction">indicates that the binding should be one or two directional.  If one way, the Uwp control is only updated from the wrapper.</param>
        public void Bind(string propertyName, System.Windows.DependencyProperty wpfProperty, windows.UI.Xaml.DependencyProperty uwpProperty, object converter = null, System.ComponentModel.BindingDirection direction = System.ComponentModel.BindingDirection.TwoWay)
        {
            if (direction == System.ComponentModel.BindingDirection.TwoWay)
            {
                var binder = new windows.UI.Xaml.Data.Binding()
                {
                    Source = this,
                    Path = new windows.UI.Xaml.PropertyPath(propertyName),
                    Converter = (windows.UI.Xaml.Data.IValueConverter)converter
                };
                windows.UI.Xaml.Data.BindingOperations.SetBinding(ChildInternal, uwpProperty, binder);
            }

            var rebinder = new System.Windows.Data.Binding()
            {
                Source = ChildInternal,
                Path = new System.Windows.PropertyPath(propertyName),
                Converter = (System.Windows.Data.IValueConverter)converter
            };
            System.Windows.Data.BindingOperations.SetBinding(this, wpfProperty, rebinder);
        }

        /// <inheritdoc />
        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            if (_childInternal != null)
            {
                SetContent();
            }
        }

        /// <summary>
        /// Gets or sets the root UWP XAML element displayed in the WPF control instance.
        /// </summary>
        /// <value>The <see cref="windows.UI.Xaml.UIElement"/> child.</value>
        /// <remarks>This UWP XAML element is the root element of the wrapped <see cref="windows.UI.Xaml.Hosting.DesktopWindowXamlSource" />.</remarks>
        protected windows.UI.Xaml.UIElement ChildInternal
        {
            get
            {
                return _childInternal;
            }

            set
            {
                if (value == ChildInternal)
                {
                    return;
                }

                var currentRoot = (windows.UI.Xaml.FrameworkElement)ChildInternal;
                if (currentRoot != null)
                {
                    currentRoot.SizeChanged -= XamlContentSizeChanged;
                }

                _childInternal = value;
                SetContent();

                var frameworkElement = ChildInternal as windows.UI.Xaml.FrameworkElement;
                if (frameworkElement != null)
                {
                    // If XAML content has changed, check XAML size
                    // to determine if WindowsXamlHost needs to re-run layout.
                    frameworkElement.SizeChanged += XamlContentSizeChanged;

                    // WindowsXamlHost DataContext should flow through to UWP XAML content
                    frameworkElement.DataContext = DataContext;
                }

                // Fire updated event
                ChildChanged?.Invoke(this, new EventArgs());
            }
        }

        /// <summary>
        /// Exposes ChildInternal without exposing its actual Type.
        /// </summary>
        /// <returns>the underlying UWP child object</returns>
        public object GetUwpInternalObject()
        {
            return ChildInternal;
        }

        /// <summary>
        /// Gets or sets a value indicating whether this wrapper control instance been disposed
        /// </summary>
        protected bool IsDisposed { get; set; }

        /// <summary>
        /// Creates <see cref="windows.UI.Xaml.Application" /> object, wrapped <see cref="windows.UI.Xaml.Hosting.DesktopWindowXamlSource" /> instance; creates and
        /// sets root UWP XAML element on <see cref="windows.UI.Xaml.Hosting.DesktopWindowXamlSource" />.
        /// </summary>
        /// <param name="hwndParent">Parent window handle</param>
        /// <returns>Handle to XAML window</returns>
        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            // 'EnableMouseInPointer' is called by the WindowsXamlManager during initialization. No need
            // to call it directly here.

            // Create DesktopWindowXamlSource instance
            var desktopWindowXamlSourceNative = _xamlSource.GetInterop();

            // Associate the window where UWP XAML will display content
            desktopWindowXamlSourceNative.AttachToWindow(hwndParent.Handle);

            var windowHandle = desktopWindowXamlSourceNative.WindowHandle;

            // Overridden function must return window handle of new target window (DesktopWindowXamlSource's Window)
            return new HandleRef(this, windowHandle);
        }

        /// <summary>
        /// The default implementation of SetContent applies ChildInternal to desktopWindowXamSource.Content.
        /// Override this method if that shouldn't be the case.
        /// For example, override if your control should be a child of another WindowsXamlHostBase-based control.
        /// </summary>
        protected virtual void SetContent()
        {
            if (_xamlSource != null)
            {
                _xamlSource.Content = _childInternal;
            }
        }

        /// <summary>
        /// WPF framework request to destroy control window.  Cleans up the HwndIslandSite created by DesktopWindowXamlSource
        /// </summary>
        /// <param name="hwnd">Handle of window to be destroyed</param>
        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            Dispose(true);
        }

        /// <summary>
        /// WindowsXamlHost Dispose
        /// </summary>
        /// <param name="disposing">Is disposing?</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                ChildInternal = null;

                if (_window != null)
                {
                    _window.LocationChanged -= OnWindowLocationChanged;
                    _window = null;
                }

                // Required by CA2213: _xamlSource?.Dispose() is insufficient.
                if (_xamlSource != null)
                {
                    _xamlSource.TakeFocusRequested -= OnTakeFocusRequested;
                    _xamlSource.Dispose();
                }
            }

            // BUGBUG: CoreInputSink cleanup is failing when explicitly disposing
            // WindowsXamlManager.  Add dispose call back when that bug is fixed in 19h1.
            base.Dispose(disposing);
        }
    }
}
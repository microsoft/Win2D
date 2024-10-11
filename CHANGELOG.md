# Win2D Changelog

## Win2D-WinUI 1.3.0
_October 9th, 2024_
- Update Windows SDK projections and CsWinRT to latest
- Lower TFM to 10.0.19041.0
- Mark CanvasAnimatedControl as experimental (it has some known issues)

## Win2D-WinUI 1.2.1-experimental2

_August 1, 2024_

- Bring back CanvasAnimatedControl
- Add ICanvasSwapChainFactoryNative interop interface
- Add CanvasSwapChain.CreateForWindowId APIs for HWND swapchains
- Update Windows SDK projections and CsWinRT to latest previews

## Win2D-WinUI 1.2.1-experimental1

_June 17, 2024_

- Update WindowsAppSDK version to 1.6.240829007
- Update CsWinRT version to 2.1.0-prerelease.240602.1
- Update TargetPlatform and TargetFramework verisons to 22000
  - WindowsAppSDK experimental channel uses 22000 instead of 19041. When WindowsAppSDK does a stable release on 19041, Win2D will also drop back down to 19041.

---

## Win2D-WinUI 1.2.0

_March 1, 2024_

- Roll Target Framework and Target Platform version to 19041 because of conflict with WinAppSDK and MAUI project templates
- Add support for file:// URI to CanvasFontFormat
- Update to WinAppSDK 1.5.240227000

---

## Win2D-WinUI 1.1.1

_November 17, 2023_
- Update WindowsAppSDK to 1.4.231115000
- Update Windows SDK to 10.0.22621.31
- Update CsWinRT to 2.0.4
- Fix nuget package .NET projection folder to match the project's TargetFrameworkVersion
- Use non-generic RID folder names in nuget package for .NET 8 compatibility
- Fixes to published interop header which was causing missing D2D type errors for C++ consumers
- Remove .NET Core 3 support

---

## Win2D-WinUI 1.1.0

_September 26, 2023_

- Update to WindowsAppSDK 1.4.230913002
- Change linker options for symbol building
- Offical release of features from 1.1.0-preview1

---

## Win2D-WinUI 1.0.5.1

_May 25, 2023_

- Add file:// URI support for CanvasFontSet as a workaround for msappx:// not working in unpackaged applications.

---

## Win2D-WinUI 1.1.0-preview1

_April 20, 2023_

- Update Win2D to WinAppSDK 1.3.230331000
- Update .NET projections to .NET 6
- Enabled trimming in the .NET projections assembly (see <https://github.com/microsoft/CsWinRT/issues/373>)
- Update target SDK to 10.0.22621.0
- Update minimum SDK to 10.0.17763.0
- New `ICanvasImageInterop` COM interface and infrastructure to support external effects (see <https://github.com/microsoft/Win2D/pull/888>)
- New `CanvasDevice.IsDeviceLost()` overload to check whether a `CanvasDevice` is lost
- New `CanvasDevice.GetDeviceLostReason()` API to get the device lost error code for a `CanvasDevice`
- New `ID2D1DeviceContextPool` and `ID2D1DeviceContextLease` COM interfaces to efficiently rent `ID2D1DeviceContext`-s
- New C exports to support `ICanvasEffect` from external effects (see <https://github.com/microsoft/Win2D/issues/894>)
- Added some missing parameter checks/clears in some public APIs
- Improved consistency for parameter validation and disposal checks in some public APIs
- New `ICanvasFactoryNative` methods to register custom wrappers (see <https://github.com/microsoft/Win2D/issues/910>)
  - `RegisterWrapper` and `UnregisterWrapper`, as mentioned in the linked issue (with updated names)
  - `RegisterEffectFactory` and `UnregisterEffectFactory`, documented in the published header
- Add validation for `GetOrCreate` creating Win2D effects in an invalid state (see <https://github.com/microsoft/Win2D/issues/913>)
- Fix a crash when calling `CanvasSwapChain.Resize(...)` on an instance created through interop over an HWND DXGI swap chain

---

## Win2D-WinUI 1.0.4

_October 4, 2022_

- Update Win2D to WinAppSDK 1.0.4
- Update Win2D to CsWinRT 1.4.1
- Update Win2D to target 17763 instead of 18362 to add downlevel support
- Infrastructure updates
  - Add telemetry
  - Automate symbol publishing in Azure pipeline
  - Pull nuget packages from Azure nuget feeds when building in Azure pipelines

---

## Win2D-WinUI 1.0.3

_April 22, 2022_

- Update Win2D to WinAppSDK 1.0.3
- Update Win2D to use HybridCRT
- Remove BuildDocs from build process, which isn't intended to be used with WinAppSDK
- Add WinAppSDK as a nuget package dependency, enforcing a minimum version

---

## Win2D-WinUI 1.0.1

_March 30, 2022_

- Updates Win2D to depend on Windows App SDK 1.0.1
- Updates Win2D to use CsWinRT 1.3.5

---

## Win2D-WinUI 1.0.0.30

_November 16, 2021_

- Updates Win2D to depend on Windows App SDK 1.0.0

---

## Win2D-WinUI 1.0.0.29-preview3

_October 27, 2021_

- Updates Win2D to depend on Windows App SDK 1.0.0-preview3

This version includes another workaround for AnyCPU that will be removed when AnyCPU is more fully supported by the Windows App SDK.

---

## Win2D-WinUI 1.0.0.28-preview2

_October 14, 2021_

- Updates Win2D to depend on Windows App SDK 1.0.0-preview2

This version includes a workaround for a reference to a non-existent file Microsoft.Windows.ApplicationModel.WindowsAppSDK.winmd in the Windows App SDK's nuget package. This workaround will be removed when the issue is fixed in future versions of the Windows App SDK.

---

## Win2D-WinUI 1.0.0.27-preview1

_September 28, 2021_

- Updates Win2D to depend on Windows App SDK 1.0.0-preview1

This includes a breaking change - `CanvasComposition.CreateCompositionSurfaceForSwapChain` has changed to be an experimental API and is stripped from this release. This is because it relies on experimental Windows App SDK apis which do not exist in version preview1.

---

## Win2D-WinUI 1.0.0.26-experimental1

_September 10, 2021_

- Updates Win2D to depend on Project Reunion 1.0.0-experimental1

This includes a temporary workaround for building for AnyCPU, which is expected to be fully supported in the official 1.0 release.

---

## Win2D-WinUI 0.8.2.23

_August 17, 2021_

- Updates Win2D to depend on Project Reunion 0.8.2
- CanvasBitmap no longer is a DependencyObject
- Updates CsWinRT to 1.3

---

## Win2D-WinUI 0.8.0.21

_June 26, 2021_

- Updates Win2D to depend on Project Reunion 0.8.0

---

## Win2D-WinUI 0.5.7.20 - Support for Project Reunion 0.5.7

_May 26, 2021_

- Updates Win2D to depend on Project Reunion 0.5.7

---

## Win2D-WinUI 0.5.0.14 - CanvasBitmap implements from IDependencyObject correctly

_May 7, 2021_

- CanvasBitmap now properly inherits from DependencyObject such that the QI to IDependencyObject succeeds.

---

## Win2D-WinUI 0.5.0.13 - IGeometrySource2DInterop

_April 12, 2021_

- CanvasGeometry now implements the WinUI3 interface Microsoft.Graphics.IGeometrySource2DInterop instead of the system type Windows.Graphics.IGeometrySource2DInterop.

---

## Win2D-WinUI 0.5.0.12 - Project Reunion 0.5.0 support

_April 1, 2021_

This change moves Win2D onto Project Reunion 0.5.0

- Moving Win2D onto WinUI3 is work in progress, and some features that were available on Win2D-WinUI2 are not supported on Win2D-WinUI3
- Win2D now builds on Project Reunion and WinUI3 instead of WinUI2
- CanvasAnimatedControl is removed until it can be supported on WinUI3
- CanvasBitmap is not removed, but is only supported on UWP (not on Desktop packaged apps)
- WinRT type activation is not yet supported, and relies on manifest-free activation
- Building Win2D now requires Visual Studio 2019
- Target platform min version is now 17763
- The home of the Win2D samples is now the Win2D-Sample Gallery on github: <https://github.com/Microsoft/Win2D-Samples>

---

## Win2D 1.25.0 - XAML Islands support

_April 2, 2020_

This change hooks up Win2D with XamlRoot to get it rendering on Xaml islands.

---

## Win2D 1.24.0 - ARM64 support

_October 28, 2019_

This release adds support for ARM64 platforms, upgrades the Win2D build to Visual Studio 2019, and fixes some documentation errors.

---

## Win2D 1.23.0 - Windows SDK version validation

_May 18, 2018_

This release is functionally identical to 1.22.0, but includes a better error message if you try to build using too old a version of the Windows SDK.

---

## Win2D 1.22.0 - IGeometrySource2D, bugfixes, and removal of 8.1 support

_May 2, 2018_

- CanvasGeometry now implements IGeometrySource2D, which allows using Win2D to construct and manipulate geometry paths for the new Windows.UI.Composition geometry APIs
- Allow use of CanvasGeometry and CanvasPathBuilder without a CanvasDevice
- The Microsoft.Graphics.Canvas.Svg APIs and ColorManagementProfile enhancements, which were introduced with Windows 10 Creators Update, are no longer marked as experimental
- Fixed race condition if TrackAsyncAction is passed an action that has already completed (issue #585)
- Win2D.uwp.targets no longer checks for TargetPlatformIdentifier=UWP (fixes issue #583)
- Enabled ControlFlowGuard compiler setting, to make Win2D compliant with the binskim static analysis tool
- Fixed SVG handling of negative stream seek offsets on x64
- Improved documentation about CanvasStrokeStyle defaults and zero-length dashes

This release removes support for the Windows 8.1 and Windows Phone 8.1 platforms. Legacy support for these remains available in Win2D 1.21.0, as a NuGet package and from the win81 branch on github.

Building an app using Win2D 1.22.0 requires the April 2018 Update Windows SDK (version 17134). Attempting to build with an older SDK will fail:

    error CS7069: Reference to type 'IGeometrySource2D' claims it is defined in 'Windows.Foundation.UniversalApiContract', but it could not be found

Note that apps using this version of Win2D are still compatible with older versions of Windows 10. The April 2018 SDK is required in Visual Studio to build them, but April 2018 update of the Windows OS is not required to run them.

---

## Win2D 1.21.0 - SVG, improved HDR support, and bugfixes

_June 26, 2017_

- Load, draw, and manipulate SVG (scalable vector graphics) documents  (requires Windows 10 Creators Update)
- Load and save JpegXR images using extended precision pixel formats  (16 and 32 bit floating point, or 16 bit normalized integer)
- ColorManagementProfile now supports Simple and Extended types as well as ICC color profiles  (requires Windows 10 Creators Update)
- Added a CanvasComposition.CreateDrawingSession overload that allows specifying DPI
- Building Win2D for UWP now requires Visual Studio 2017  (but if you are just using it in an app rather than compiling it yourself, both VS 2015 and 2017 continue to be supported)
- Bugfixes:
  - #479 - CanvasVirtualControl OnRegionsInvalidated not triggered when launching app
  - #486 - Crash when resource loading completes after Canvas is already unloaded
  - #487 - ComputeHistogram does not take into account the SourceRectangle
  - #491 - CanvasControl & CanvasVirtualControl do not disable the accessibility view
  - #496 - Crash in Microsoft_Graphics_Canvas RegisteredEvent::Release
  - #498 - CanvasGradientMesh TensorPatch Colors documented incorrectly
  - #515 - Crash when passing a CanvasCommandList to CanvasImage.SaveAsync

---

## Win2D 1.20.0 - bugfix release

_November 30, 2016_

- The image effects that were added in Windows 10 Anniversary Edition (AlphaMaskEffect, CrossFadeEffect, OpacityEffect, and TintEffect) are no longer marked as experimental.
- Made sure CanvasControl::Changed always executes on the UI thread. This fixes a crash during lost device recovery in multithreaded apps.
- Close() clears its internal std::vector, to reduce memory usage.
- Fixed XAML layout bug in the Example Gallery GlowTextCustomControl.
- Fixed Example Gallery ink demo crash when you "clear" then "save".
- Win2D doc build works with the latest version of Sandcastle.
- test knows how to find the Windows SDK even if that is installed in subfolders (which is done to support side-by-side in more recent versions of Visual Studio).
- Fixed HSTRING double-free in Win2D unit test.

---

## Win2D 1.19.0 - Windows 10 Anniversary Edition support, new image effects, doc improvements, and bugfixes

_August 9, 2016_

This release adds four new image effects: AlphaMaskEffect, CrossFadeEffect, OpacityEffect, and TintEffect.  These are new in the Windows 10 Anniversary Edition version of Direct2D, so are not supported on older versions of Windows.  They are just helpers that make it easier to implement common operations - identical results could previously be achieved by setting the right options on more configurable effect types:

- AlphaMaskEffect can be achieved using CompositeEffect
- CrossFadeEffect can be achieved using ArithmeticCompositeEffect
- OapcityEffect and TintEffect can be achieved using ColorMatrixEffect

Documentation improvements:

- Nukepayload2 added VB snippets to the readme and doc intro page (thanks for the contribution!)
- More information about how to use Win2D with C++/CX
- Better docs for DpiCompensationEffect

Bugfixes:

- #377 (Long-running CanvasAnimatedControl crashes due to CoreDispatcher::RunAsync failure)
- #354 (InvalidCastException when call CanvasDevice.GetSharedDevice)
- bricelam removed dead code in InkExample (thanks for the contribution!)
- CanvasRegionsInvalidatedEventArgs should not be activatable

We added an example showing how to draw superscript and subscript text.

Finally, the Ink example now includes an option to use the new Windows 10 Anniversary Update pencil rendering style.

---

## Win2D 1.18.0 - bugfixes and SpriteBatch docs

_May 16, 2016_

This release fixes a reentrancy bug in CanvasAnimatedControl.

It also improves the SpriteBatch.Draw docs, explaining how to convert Windows.UI.Color values into a Vector4 tint parameter. Many thanks to Andy De George for contributing this!

---

## Win2D 1.17.0 - bugfixes and stable APIs

_April 4, 2016_

This release fixes a couple of bugs that were reported by customers on github.

It also removes the 'experimental' warning tag from a final set of APIs that we now consider to be stable:

- HDR color support
  - *Hdr properties matching all existing Color properties
  - CanvasSolidColorBrush constructor overload
  - CanvasGradientStopHdr
  - Clear overload
- ColorManagementEffect
- ColorManagementProfile
- ColorManagementEffectQuality
- ColorManagementRenderingIntent
- TableTransfer3DEffect
- EffectTransferTable3D
- CanvasTextFormat
  - CustomTrimmingSign
  - TrimmingSign
- CanvasTextLayout
  - CustomTrimmingSign
  - DrawToTextRenderer
  - GetCustomBrush
  - GetTypography
  - SetCustomBrush
  - SetInlineObject
  - SetTypography
  - TrimmingSign
- CanvasAnalyzedBidi
- CanvasAnalyzedBreakpoint
- CanvasAnalyzedGlyphOrientation
- CanvasAnalyzedScript
- CanvasCharacterRange
- CanvasFontFace
- CanvasFontFileFormatType
- CanvasFontInformation
- CanvasFontProperty
- CanvasFontPropertyIdentifier
- CanvasFontSet
- CanvasFontSimulations
- CanvasGlyph
- CanvasGlyphJustification
- CanvasGlyphMetrics
- CanvasGlyphOrientation
- CanvasGlyphShaping
- CanvasJustificationOpportunity
- CanvasLineBreakCondition
- CanvasNumberSubstitution
- CanvasNumberSubstitutionMethod
- CanvasScaledFont
- CanvasScriptProperties
- CanvasScriptShape
- CanvasTextAnalyzer
- CanvasTextGridFit
- CanvasTextMeasuringMode
- CanvasTextRenderingMode
- CanvasTextRenderingParameters
- CanvasTrimmingSign
- CanvasTypography
- CanvasTypographyFeature
- CanvasTypographyFeatureName
- CanvasUnicodeRange
- ICanvasTextAnalyzerOptions
- ICanvasTextInlineObject
- ICanvasTextRenderer
- CreateGlyphRun
- TextRenderingParameters

---

## Win2D 1.16.0 - changing experimental APIs to stable

_February 22, 2016_

This release contains no new features, but removes the "experimental" warning tag from an additional set of APIs that we now consider to be stable:

- CanvasImage
- CanvasVirtualBitmap
- CanvasVirtualBitmapOptions
- CanvasVirtualControl
- CanvasVirtualImageSource
- CanvasRegionsInvalidatedEventArgs
- PixelShaderEffect
- SamplerCoordinateMapping
- CanvasPrintDocument
- CanvasPrintTaskOptionsChangedEventArgs
- CanvasPreviewEventArgs
- CanvasPrintEventArgs
- CanvasPrintDeferral
- CanvasDevice:
  - IsPixelFormatSupported
  - IsBufferPrecisionSupported
  - MaximumCacheSize
  - LowPriority
- CanvasDrawingSession:
  - EffectTileSize
  - EffectBufferPrecision
- ICanvasEffect:
  - BufferPrecision
  - CacheOutput
  - GetInvalidRectangles
  - GetRequiredSourceRectangle
  - GetRequiredSourceRectangles
  - InvalidateSourceRectangle

---

## Win2D 1.15.0 - custom Arabic text layouts, CanvasFontSet(uri), and deexperimentalization

_February 1, 2016_

Breaking changes in this release

- Renamed some methods of CanvasTextAnalyzer, to improve consistency:
- ChooseFonts -> GetFonts
- AnalyzeBidi -> GetBidi
- AnalyzeBreakpoints -> GetBreakpoints
- AnalyzeNumberSubstitutions -> GetNumberSubstitutions
- AnalyzeScript -> GetScript
- AnalyzeGlyphOrientations -> GetGlyphOrientations
- CanvasFontSet.TryFindFontFace is no longer supported on Windows 8.1.  It had some strange corner case behaviors there, and the other APIs this is most commonly used alongside were new in Win10, so we decided to make TryFindFontFace exclusive to Win10 as well.

New features

- Custom text layout example now handles right-to-left and bidirectional as well as left-to-right languages, and includes some Arabic text to demonstrate this in action.  Many thanks to my friend Nazeeh for helping us get that right!
- Added CanvasFontSet(uri) constructor.
- Added CanvasTextLayout.GetGlyphOrientationTransform, and a new Glyph Rotation example that uses it.
- ICanvasImage.GetBounds now takes an ICanvasResourceCreator rather than CanvasDrawingSession.  This is a breaking change at the ABI level, but source compatible since CanvasDrawingSession implements ICanvasResourceCreator.
- CanvasVirtualControl and CanvasAnimatedControl now show the proper clear color in the Visual Studio XAML designer.

Bugfixes

- CanvasAnimatedControl.Size is now synchronized with the update/draw loop
- CanvasControl now handles E_SURFACE_CONTENTS_LOST on BeginDraw

Removed the "experimental" warning tag from some of our older, more stable APIs

- CanvasBitmap.CreateFromSoftwareBitmap
- CanvasComposition
- CanvasDevice.GetSharedDevice
- CanvasDrawingSession.DrawInk
- CanvasGeometry:
  - CreateInk
  - CreateText
- CanvasGradientMesh
- CanvasGradientMeshPatchEdge
- CanvasGradientMeshPatch
- CanvasDrawingSession.DrawGradientMesh
- CanvasSpriteBatch
- CanvasSpriteFlip
- CanvasSpriteOptions
- CanvasSpriteSortMode
- CanvasDrawingSession.CreateSpriteBatch
- CanvasTextFormat:
  - LastLineWrapping
  - LineSpacingMode
  - GetSystemFontFamilies
  - OpticalAlignment
  - VerticalGlyphOrientation
- CanvasTextLayout:
  - ClusterMetrics
  - LineMetrics
  - LineSpacingMode
  - LayoutBoundsIncludingTrailingWhitespace
  - MaximumBidiReorderingDepth
- CanvasLineSpacingMode
- CanvasLineMetrics
- CanvasClusterMetrics
- CanvasClusterProperties

---

## Win2D 1.14.0 - text analysis, typography, TableTransfer3DEffect, color management, and ComputeHistogram

_January 19, 2016_

- New CanvasTextAnalyzer methods for low level, customized text processing (check out the Custom Text Layouts example  to see them in action)
  - AnalyzeBidi
  - AnalyzeBreakpoints
  - AnalyzeGlyphOrientations
  - AnalyzeNumberSubstitutions
  - GetJustificationOpportunities
  - ApplyJustificationOpportunities
  - AddGlyphsAfterJustification
- Query a CanvasFontFace for its typographic features and glyph support (see the new Typography example)
- Added TableTransfer3DEffect
- Added ColorManagementEffect
- Added CanvasImage.ComputeHistogram

Improvements to existing features:

- CanvasBitmap::SaveAsync now supports bitmaps with pixel formats other than B8G8R8A8UIntNormalized
- CanvasPrintEventArgs no longer keeps the IPrintDocumentPackageTarget alive (this prevented print targets such as PDF from closing their output file)
- Changed when CanvasAnimatedControl calls WaitForVerticalBlank, to reduce glitching
- CanvasVirtualImageSource and CanvasVirtualControl support rendering from non-UI threads
- Added a Device property to CanvasVirtualBitmap
- CanvasGradientMesh.GetBounds now ignores the drawing session transform (to match ICanvasImage.GetBounds)
- Passing a null brush to CanvasDrawingSession.DrawGlyphRun throws a better exception

---

## Win2D 1.13.0 - fixes Store certification failure

_January 6, 2016_

This minor release fixes a bug that caused apps using 1.12.0 to fail Store certification.

---

## Win2D 1.12.0 - bugfixes and error messages

_January 4, 2016_

There are no new features in this release, but a bunch of small improvements:

- Fixed CanvasDrawingSession.Close to release all resources it was holding
- Fixed lost device handling to call RoClearError, so later unrelated exceptions don't incorrectly appear to be lost devices
- Fixed CanvasBitmap.[Get|Set]Pixel[Bytes|Colors] to handle the data hazard if a bitmap is drawn in between being changed
- Fixed CanvasBitmap.[Get|Set]PixelBytes to work with A8 format on feature level 9.x devices
- CanvasDevice.GetSharedDevice now reacts more sensibly if the shared device is closed, or if CanvasDevice.DebugLevel is changed out from under it
- CanvasAnimatedControl uses the RaiseException trick to give the game loop thread a human readable name

We also improved some exception error messages:

- When trying to create a bitmap, rendertarget, swapchain, or image source larger than the max GPU texture size
- Attempting to create multiple simultaneous drawing sessions on the same target
- Calling CanvasImageSource or CanvasVirtualImageSource CreateDrawingSession on non-UI thread

And made Example Gallery better:

- Ink Example defers calling EndDry, to avoid flicker where wet ink could be turned off before the dry ink is visible
- Simplified the ink and D3D interop example code

Plus, various small documentation improvements.

Enjoy, and a happy New Year to you all!

---

## Win2D 1.11.0 - text analysis, block compressed bitmaps, saving large images, and IBuffer pixel access

_December 21, 2015_

- CanvasTextAnalyzer identifies what glyphs are needed to render a piece of text, including the font fallbacks to properly handle different languages.  Example Gallery uses this to create a new Custom Text Layouts example, which wordwraps text to fill an arbitrary geometric shape.
- Merged a pull request from Adrien Guillerot which adds the methods CanvasFontFace.GetGlyphMetrics and GetGdiCompatibleGlyphMetrics.
- Win2D now supports block compressed bitmap formats, which can be loaded from DDS files via CanvasBitmap.LoadAsync.
- Added CanvasImage.SaveAsync, which is useful for two reasons:
  - Unlike CanvasBitmap.SaveAsync, this can save any ICanvasImage, which is handy if you want to save out a command list or image effect without bothering to draw it into a rendertarget first.
  - It can save images larger than the maximum bitmap size supported by the current GPU. The implementation will automatically split up the image into multiple smaller pieces, and tile its operations as needed to make this work.
- Added overloads of CanvasBitmap.GetPixelBytes, SetPixelBytes, and CreateFromBytes that operate on IBuffer rather than byte[] parameters.
- CanvasBitmap.CopyPixelsFromBitmap now supports copying between bitmaps of different devices.
- Added CanvasDevice.LowPriority and MaximumCacheSize properties.
- CanvasDevice.Trim now calls ID2D1Device::ClearResources as well as IDXGIDevice3::Trim.
- Strong name signed Microsoft.Graphics.Canvas.Numerics.dll (github issue #188).
- Bugfix: CanvasDrawingSession.DrawInk was sometimes unintentionally changing blend or antialias mode on the drawing session.
- Bugfix and doc update describes restrictions when using the R8, R8G8, or A8 pixel formats.

---

## Win2D 1.10.0 - sprite batches, Windows.UI.Composition, CanvasVirtualBitmap, more text features, and effect rectangle mapping

_December 7, 2015_

This release adds new features that depend on the Windows 10 November Update, and removes workarounds for older Visual Studio versions.

#### Sprite batches

CanvasSpriteBatch is optimized for efficiently drawing large numbers of bitmaps.  This provides several advantages over the existing DrawImage API:

- Better performance, especially if many sprites are drawn in a row using the same source bitmap (for example when using sprite sheets).
- Optionally sort sprites by bitmap, which can improve performance. To enable sorting, pass CanvasSpriteSortMode.Bitmap to drawingSession.CreateSpriteBatch.
- Optional special filtering behavior avoids bleeding artifacts when using sprite sheets that do not contain gutter regions. This comes at some GPU cost, though, so it's still better to include gutters in your sprite sheets wherever possible. To enable this mode, pass CanvasSpriteOptions.ClampToSourceRect to drawingSession.CreateSpriteBatch.
- Individual sprites within a batch can be easily and efficiently tinted with different colors.

CanvasSpriteBatch is built on top of a new Direct2D feature which requires the Windows 10 November Update, so it will not work on older versions of Windows 10.  Use CanvasSpriteBatch.IsSupported to check if it is available on the current operating system.

The new Sprite Sheets demo (part of Example Gallery) uses CanvasSpriteBatch.

#### Windows.UI.Composition

The Win2D CanvasComposition class supports the composition APIs that were added to Windows 10 in the November Update.  You can now use Win2D to draw onto composition drawing surfaces and swapchains.  The new CompositionExample shows this in action.

#### CanvasVirtualBitmap

CanvasVirtualBitmap can be used to load and draw images larger than the maximum bitmap size supported by the current GPU.  The implementation will automatically split up the bitmap into multiple smaller pieces, and tile its drawing operations as needed to make this work.

Virtual bitmaps are supported on any version of Windows 10 (not just the November Update) but not on Windows 8.1.

#### Text features

- Typography options
- PANOSE
- CanvasTextLayout.LayoutBoundsIncludingTrailingWhitespace
- CanvasTextLayout.MaximumBidiReorderingDepth

#### Effect rectangle mapping

Added an ICanvasEffect interface which provides per-effect control over BufferPrecision, CacheOutput, and a set of APIs for mapping rectangles between the input and output coordinate systems of an effect processing graph.

---

## Win2D 1.9.0 - new text features, high dynamic range colors, ink geometry, and Mandelbrot zooming demo

_November 23, 2015_

New text features:

- Custom inline objects in text layouts
- Custom trimming signs
- Custom drawing effects in text layouts
- Query the bounds of a glyph run
- Convert glyph runs to geometry
- Text layout line metrics and cluster metrics
- Win2D now correctly handles UTF-32 trimming delimiters

Other new features:

- All Color properties now also have a Vector4 equivalent (for instance CanvasSolidColorBrush.ColorHdr) which allows the use of high dynamic range floating point color values
- Convert ink strokes to geometry
- Query what buffer precision and pixel formats are supported by the current GPU
- Control the buffer precision and tile size used when drawing image effects

We also added a Mandelbrot demo to Example Gallery.  This combines a custom PixelShaderEffect (which generates the fractal) with CanvasVirtualControl and ScrollViewer.  It shows how to implement zooming as well as scrolling when using virtual controls.

---

## Win2D 1.8.0 - custom text rendering and PixelShaderEffect border/interpolation modes

_November 9, 2015_

- Custom text rendering via CanvasTextLayout.DrawToTextRenderer and CanvasDrawingSession.DrawGlyphRun
- Added PixelShaderEffect properties controlling border mode (hard vs. soft) and interpolation (linear vs. nearest neighbor)
- Added CanvasDrawingSession.Flush (for advanced interop scenarios)
- Added CanvasDevice.Lock (for advanced interop + multithreading scenarios)
- Switched the documentation to focus primarily on UWP rather than Windows 8.1 (the API differences when targeting 8.1 are now documented as a delta from UWP, where UWP was previously documented as a delta from 8.1)

---

## Win2D 1.7.0 - custom image effects improvements, system font properties

_October 26, 2015_

- Added CanvasFontSet and CanvasFontFace, a means of enumerating and querying properties of system fonts.
- Added PixelShaderEffect.IsSupported query, and better exception messages for feature level mismatches.
- Added sketch effect to ExampleGallery.CustomEffects demo
- Some bugfixes

The new text types can be used to query various properties of fonts. The 'glue' to link them to text layouts will be upcoming, so stay tuned.

PixelShaderEffects now support non 1:1 texture co-ordinate mappings, demonstrated by the new sketch toggler in the custom effects demo.

---

## Win2D 1.6.0 - custom image effects and new text format options

_October 12, 2015_

- Added PixelShaderEffect, which lets you draw using a custom pixel shader
- Text format properties: LastLineWrapping, LineSpacingMode, OpticalAlignment, TrimmingSign, and VerticalGlyphOrientation
- Added CanvasDrawingSession.TextRenderingParameters
- Several bugfixes

Note that PixelShaderEffect is currently limited to 1:1 texture coordinate mappings.  It will not work correctly if your shader samples its input(s) at a different location from the pixel currently being shaded.  This means it cannot be used for effects like warp or blur, which require offsetting the texture coordinates.  We will be adding support for more complex mappings in the next version.

We've also been working on integration between Win2D and Windows.UI.Composition, but that isn't yet ready to be released.

---

## Win2D 1.5.0 - printing and text outline geometry

_September 28, 2015_

- Printing
- Convert the outline of a piece of text to geometry, which can then be processed in all sorts of interesting ways
- The Text Layouts demo in Example Gallery now includes a font selector
- Fixed the last remaining issues with effect interop, so everything stays properly in sync when you change an effect graph using Win2D and Direct2D simultaneously

---

## Win2D 1.4.0 - mostly boring changes that you probably don't care about

_September 14, 2015_

We spent most of the last sprint working on unglamorous tasks in the 'we spend time worrying about this so you don't have to' category.  In particular:

- A bunch of things that didn't use to be thread-safe now are.
- Refactored how Win2D <-> Direct2D interop works.  The new implementation correctly handles some previously unsupported corner cases involving interop of polymorphic types, and is simpler/easier for us to maintain.

We've also been hard at work adding printing support, but that isn't ready for release yet.

This release does include a couple of new things, one of which came from outside Microsoft:

- Added CanvasTextFormat.GetSystemFontFamilies.
- Accepted a pull request from LanceMcCarthy (thanks Lance!) which adds a dynamic blur video effect to Example Gallery.

---

## Win2D 1.3.0 - CanvasVirtualControl and DpiScale

_August 31, 2015_

- CanvasVirtualControl for when you want to draw just the visible part of a potentially very large image.
- DpiScale property (on CanvasControl, CanvasAnimatedControl, and CanvasVirtualControl) for when you want to draw at a lower resolution than the physical display.  This boosts performance at the cost of visual crispness, which can be a good option for some games on high DPI but lower powered devices like phones.
- CanvasBitmap.CreateFromDirect3D11Surface now chooses between Premultiplied or Ignore alpha mode depending on whether the surface format contains alpha.  Before, it always went for premultiplied, which would fail if there wasn't actually any alpha data in the surface.
- CanvasCreateResourcesEventArgs now has a public constructor, so you can create your own instances of it if you need to call a CreateResources event handler directly  (for instance, writing tests for a Win2D app).

---

## Win2D 1.2.0 - gradient meshes and virtual image sources

_August 17, 2015_

- Gradient meshes for drawing fancy color gradients controlled by Bezier curves.
- CanvasVirtualImageSource for drawing just the visible part of a large virtualized surface.  This is a low level API that requires lots of manual plumbing, equivalent to the existing CanvasImageSource.  A higher level, easier to use CanvasVirtualControl layer is coming soon.
- CanvasDevice.GetSharedDevice overload lets you omit the forceSoftwareRenderer parameter.
- Better docs about handling device lost.

---

## Win2D 1.1.0 - now supports inking

_August 3, 2015_

With barely a pause for breath after shipping version 1.0.0, the Win2D team charges ahead toward vNext...

What's new

- Added CanvasDrawingSession.DrawInk, which makes it easy to combine Win2D with Windows.UI.Input.Inking for low latency, high quality pen and stylus rendering.
- Enabled the Direct3D Interop demo in Example Gallery (unblocked now that a UWP NuGet package for DirectXTK is available)

#### A note on version numbers

Win2D 1.0.0 is our stable v1 release.  Versions in the 1.1.x series build on top of this adding new experimental features (see the backlog for a taste of what's to come).  We will strive to make this purely additive, so the 1.0.0 feature set remains stable while we iterate on the design of our 1.1.x additions.  These newer APIs (such as DrawInk) which may change from one version to the next are marked in the documentation as [Experimental].

---

## Announcing Win2D version 1.0.0

_July 29, 2015_

466 days and 821 changes after our first commit, and nearly 11 months after we made the project public, today I am pleased to announce the release of Win2D version 1.0.0.

This adds support for Visual Studio 2015 RTM, and provides a stable API that we will avoid breaking as we move on to adding vNext features.  It is no longer compatible with earlier Visual Studio 2015 RC builds.

Note that the "Win2D" NuGet package is deprecated.  Instead, please use either "Win2D.uwp" or "Win2D.win81" depending on whether you are building for the Universal Windows Platform (UWP) or Windows/Phone 8.1.

I'd like to take this opportunity to thank the early adopters who put up with the churn and lack of features in our early builds to give invaluable feedback on how we could make Win2D better.  To reuse a hoary cliche, what bugs remain are our fault, but all the bugs there aren't is thanks to these brave pioneers!

---

## Win2D 0.0.23 - now using the MIT license

_July 22, 2015_

This release switches Win2D from Apache 2.0 to the MIT license.

It also fills some documentation holes, and tidies up some example code.  Minor changes only as we stabilize ready for a v1 release (we spent half of last sprint writing our own apps to validate Win2D end-to-end).

Note that this release is not compatible with Visual Studio 2015 RTM, because that does not include support for Windows 10 app development.  To develop UWP apps with Win2D 0.0.23, you must continue to use the earlier VS 2015 RC build.  For more information, see Release Dates and Compatibility: Visual Studio 2015 and the Windows 10 SDK.

We plan to release Win2D version 1.0.0, which will include full support for VS 2015 RTM and UWP, shortly after the Windows 10 SDK and Visual Studio UWP development tools on July 29  (same day if all goes well).

---

## Win2D 0.0.22 - new image effects, samples, and docs

_July 6, 2015_

Breaking changes

- ConvertDipsToPixels takes a new parameter specifying rounding behavior  (it turned out there is no such thing as One True Rounding Implementation To Rule Them All, as the correct way to round depends on what the resulting pixel value will be used for)

What's new

- Added 16 new image effects, mostly designed for photo/video editing scenarios.  These are new in Windows 10  (not supported on Windows 8.1 or Phone 8.1):
  - ChromaKey
  - Contrast
  - EdgeDetection
  - Emboss
  - Exposure
  - Grayscale
  - HighlightsAndShadows
  - HueToRgb
  - Invert
  - Posterize
  - RgbToHue
  - Sepia
  - Sharpen
  - Straighten
  - TemperatureAndTint
  - Vignette
- New samples
  - CustomControlExample shows how to implement a custom XAML control that displays glowing text
  - BackgroundTaskExample shows how to use Win2D from a background task that updates a live tile
- Improved documentation
  - Loading resources outside of CreateResources
  - Effect precision and clamping
  - Better CanvasAnimatedControl docs  (now with diagram!)
  - Better TrackAsyncAction docs
- Cleaned up all the remaining CanvasAnimatedControl behavioral corner cases
- Fixed some threading bugs (deadlocks and hazards)

#### Where next?

Win2D is on final approach for a V1 landing.  We are now mostly just waiting for Visual Studio 2015 RTM, as there are some changes we need to coordinate with that.

Note that our next release will switch from Apache 2.0 to MIT license.

---

## Win2D 0.0.21 - controls share devices, better device creation options, CanvasAnimatedControl improvements, and improved docs

_June 22, 2015_

Breaking changes

- The CanvasAnimatedControl.Input property is replaced by a CreateCoreIndependentInputSource method.  This means that apps can now use whatever input policy they want.  The standard XAML pointer events now work.  Or an input source can be created on the game loop thread.  Or one could be created on some other thread.  See the Custom Fonts example for code.
- Debug level is now configured through a static CanvasDevice.DebugLevel property, rather than a CanvasDevice constructor parameter.  This makes it easier to globally turn on debugging for an entire app, including devices created indirectly by CanvasControl or CanvasAnimatedControl.
- The CanvasHardwareAcceleration enum is replaced by a boolean ForceSoftwareRenderer option, which can be passed to the CanvasDevice constructor or set as a property on CanvasControl or CanvasAnimatedControl.

What's new

- CanvasControl and CanvasAnimatedControl now have the ability to share a single CanvasDevice across multiple controls.  This saves memory for apps that use more than one Win2D control at a time, and makes it possible to share graphics resources such as bitmaps across these controls.  By default, CanvasControl devices are shared, while CanvasAnimatedControl devices are not.  You can change this by setting the UseSharedDevice property.
- Added a CustomDevice property to CanvasControl and CanvasAnimatedControl.  This tells the Win2D controls use an arbitrary device of your choosing, in case you need unusual device creation options or to customize the device sharing behavior.
- CanvasAnimatedControl now stops the dispatcher from running while the UI thread owns the device.  This ensures that input events and RunOnGameLoopThreadAsync actions only execute after CreateResources has completed.
- Added CanvasAnimatedControl game loop lifecycle events GameLoopStarting and GameLoopStopped.
- Improved documentation
- How DPI and DIPs work in Win2D
- How Win2D deals with straight vs. premultiplied alpha
- What pixel formats are supported for bitmaps, rendertargets, and swapchains

#### Where next?

We are getting close to a V1 release of Win2D.  As part of this process, we recently spent some time updating our backlog, removing stale items and reprioritizing everything.  The backlog now shows exactly what work we intend to finish before declaring V1, and at least a good approximation of what order we'll do things in after that.

---

## Win2D 0.0.20 - CanvasAnimatedControl input improvements, device lost events

_June 8, 2015_

Breaking Changes

- There is one behavioral breaking change: CanvasTextAntialiasing's default behavior used to be implemented as ClearType. It now defaults to Grayscale.

What's New:

- A new ExampleGallery demo showcasing how to do interactive keyboard input
- Bug fixes related to the XAML controls
- Moved vector art demos from the now-retired CsConsumer, into ExampleGallery
- Added device lost events off of CanvasDevice
- CanvasAnimatedControl can now handle input events and RunOnGameLoopThreadAsync work while paused
- Sharable CanvasDevices
- UWP versions of Win2D samples

---

## Win2D 0.0.19 - CanvasAnimatedControl improvements, new Win10 sample

_May 26, 2015_

- Optimizations in CanvasAnimatedControl for CPU utilization
- Fixed a synchronization issue in CanvasBitmap load/save functions
- Add a cool new webcam integration sample to ExampleGallery, CameraEffectExample, available to run on Windows 10
- Documentation improvements
- Infrastructure improvements for Win2D's samples
- A new helper in ExampleGallery for getting correct threading behavior of CanvasAnimatedControl
- Pull request #81 from contributor nllsotools. (Thanks!)

---

## Win2D 0.0.18 - vacations, doc improvements and Exif rotation support

_May 11, 2015_

What do you get when part of the team is presenting at the Build conference and the other part on vacation?

Not a lot!

<rimshot>

Nevertheless, Win2D version 0.0.18 is now available on NuGet and GitHub:

- Documentation improvements in response to customer feedback
- CanvasBitmap.LoadAsync now respects Exif rotation metadata, so phone photos appear the right way up

---

## Win2D 0.0.17 - breaking changes and compatibility with latest VS toolchain

_April 29, 2015_

It's been such an exciting week as the //build/ conference in San Francisco is in progress! Those attending may have had the opportunity to see our Win2D talk today. The new public version of Visual Studio 2015 RC is also available to try out, and use with Win2D.

Breaking changes

- Building using Visual Studio 2015 now requires version Visual Studio 2015 RC. Earlier previews of VS 2015, such as CTP 6, and so on, are no longer supported.
- Some renaming related to CanvasTextLayout for clarity and consistency.
  - MaximumLayoutSize -> RequestedSize
  - GetMinimumLayoutWidth -> GetMinimumLineLength
  - GetMinimumCharacterAdvanceWidth -> GetMinimumCharacterAdvance
- Removed CanvasLineSpacingMethod. This is now determined from CanvasTextLayout's LineSpacing value.
- Reordered the DPI parameters passed to methods of CanvasBitmap, CanvasRenderTarget, and CanvasSwapChain.

What's New

- Various fixes to ensure Win2D is compatible with the latest Visual Studio toolchain for building UWP apps.
- Improvements in documentation.
- A necessary change to ensure Win2D can be consumed by managed Windows runtime components.
- A cool new Direct3D Interop sample in ExampleGallery

---

## Win2D 0.0.16 - breaking changes and Windows 10 support

_April 13, 2015_

As previously mentioned, this sprint focused on making breaking changes to improve the Win2D API design, in preparation for releasing a more stable version alongside the //build/ conference in a couple of weeks. Specifically:

- Split the Microsoft.Graphics.Canvas namespace into several new namespaces, to better group things and make the documentation easier to browse:
  - Microsoft.Graphics.Canvas
  - Microsoft.Graphics.Canvas.Brushes
  - Microsoft.Graphics.Canvas.Geometry
  - Microsoft.Graphics.Canvas.Text
  - Microsoft.Graphics.Canvas.UI
  - Microsoft.Graphics.Canvas.UI.Xaml
- Reworked the DrawImage method to make its many overloads more consistent and easier to use. In particular, it is now possible to specify a destination rectangle or to modify opacity when drawing any ICanvasImage, rather than only a CanvasBitmap.
- Renamed and merged some CanvasTextFormat properties that experience had shown to be overly confusing.

#### Windows 10 support

Win2D can now be built to support the Windows universal app platform. As this a preview, we are not shipping universal app platform binaries in the NuGet package at this time. In order to use Win2D with the universal app platform you'll need to build it from source. This is early work in progress, and some workarounds are required.

Finally, we added a particle system demo to the Example Gallery test app.

---

## Win2D 0.0.15 - cleanups and breaking changes

_March 30, 2015_

New this sprint:

- Added CanvasTextLayout SetBrush and SetColor methods, which allow regions of different colors within a single layout
- Added CanvasAnimatedControl.RunOnGameLoopThreadAsync
- CanvasSwapChain.ResizeBuffers can now change DPI as well as size
- Fixed bug where ICanvasTimingInformation.IsRunningSlowly was not always returning the correct information
- Added RemoveFromVisualTree method to CanvasControl and CanvasAnimatedControl  (used by Example Gallery to break reference count cycles)
- Reviewed the entire D2D API for things we'd forgotten to expose in Win2D, which produced some new (low priority) backlog items
- We also made a significant breaking change to the effects API:
- Renamed IEffect -> IGraphicsEffect
- Renamed IEffectInput -> IGraphicsEffectSource
- Removed EffectId, Inputs, and Properties from the IEffect interface
- Added CompositeEffect.Sources as a replacement for IEffect.Inputs
- Added IGraphicsEffect Name property

Breaking changes are going to be a theme over the next few weeks as we approach the Build conference.  We plan to release a somewhat stable version of Win2D, including Win10 UAP support, in time for Build.  We're not promising zero breaking changes after that point (this will not be our final 1.0 release) but we're aiming for a greatly reduced rate of churn compared to what you've seen so far.  Which, of course, means the next few weeks are our last chance to tear things up!  So we'll be taking a short break from adding new features in order to revisit some designs that we're not entirely happy with:

- DrawImage overloads are inconsistent and frustrating
- Should we have more nesting underneath the Microsoft.Graphics.Canvas namespace, to remove clutter?
- Reconcile the various text direction / flow options between XAML and DWrite
- Weird interaction between CanvasTextFormat.LineSpacing and LineSpacingMethod
- Is the CanvasSwapChain.ResizeBuffers DPI parameter in the right order?  What about other methods that take a DPI?

Please bear with us and pardon the mess while we fix these things up.  Taking the time to get them right will make the API better in the long run.  Also, please let us know if there are other things you find wrong or confusing about our current API designs.  Not missing things (we can add new APIs easily at any time) but any designs you think are less awesome than they could/should be.

Thanks!

---

## Win2D 0.0.14 - text layouts, opacity masks, geometry readback, and CoreWindow support

_March 16, 2015_

- Text layouts
- Cache the layout of a block of text, allowing it to be drawn many times without having to repeat formatting calculations each time
- Combine different text formats within a single layout
- Measure the extents of the text
- Hit-test against the text
- Opacity masks
- FillRectangle and FillGeometry overloads that take two brushes: the first specifies color, the second opacity
- Read back the contents of a CanvasGeometry
- Tessellate reads the tessellated triangles
- SendPathTo reads the segments (lines, arcs, beziers) that make up a path
- CoreWindow support means you can now use Win2D without XAML
- New helper overloads
- [Draw|Fill|DrawCached]Geometry overloads that take a position offset parameter
- CanvasPathBuilder.BeginFigure overload that takes separate floats instead of a Vector2
- For bonus points, we added more interesting icons to the Example Gallery test app.

---

## Win2D 0.0.13 - geometry, layers, and custom fonts

_March 3, 2015_

- More geometry features
- Combine (union, intersect, etc.)
- Compute stroked, simplified, or outline version of a geometry
- Hit test against points or other geometries
- Compute geometry area and points/normals along arbitrarily complex paths
- Transformed geometry
- Geometry groups
- Cached geometry tessellation (realizations) for faster rendering
- Layers
- Clip drawing to arbitrary regions
- Change the opacity of a group of drawing operations
- Simple custom font loading
- Helper overloads
- Added CanvasControl.Size property
- Create gradient brushes with custom stop collections without also having to specify alpha mode
- More detailed documentation about the game loop timing behavior
- Resizing a CanvasAnimatedControl now triggers redraw even if the control is paused

---

## Win2D 0.0.12 - geometry paths

_February 16, 2015_

- Geometry
- Primitives
- Paths
- Filled and stroked rendering
- CanvasAnimatedControl  improvements
- Paused state works better than before
- Restart the render thread less often
- TDR recovery now works
- Trim call (on app suspend) is now properly threadsafe
- Memory management fixes
- Removed a refcount cycle that was preventing our XAML controls ever being freed
- Fixed a bunch of instance lifecycle and destructor-not-on-UI-thread issues that fell out as a result of now actually destroying our controls

---

## Win2D 0.0.11 - CanvasAnimatedControl

_February 2, 2015_

- CanvasAnimatedControl provides an easy to use game loop programming model:
- Update and Draw events run on a dedicated thread, so smooth animation can continue independently of the main UI thread.
- Supports both fixed timestep and variable update rates.
- Can deliver input events directly to its worker thread.  This minimizes latency, and avoids the synchronization complexities that would occur if input was handled on the UI thread at the same time as Update was running somewhere else.
- DrawImage fast path optimization improves some common operations by an order of magnitude.

---

## Win2D 0.0.10 - Happy New Year 2015 edition

_January 19, 2015_

This is only a minor update as our next big features are paddling furiously away below the water but not quite ready for prime time yet.  New stuff:

- The C++ numerics implementation is now SSE optimized on x86 and x64 platforms
- SizeInPixels properties now return int as opposed to float values
- CanvasAnimatedControl is coming along nicely and will be showing up here Real Soon Now.

---

## Win2D 0.0.9 - command lists and swapchain improvements

_December 15, 2014_

- Added CanvasCommandList
- CanvasSwapChain improvements
- Now handles high DPI following the same pattern as CanvasImageSource
- Present overload that takes a sync interval
- Added Rotation, TransformMatrix, and SourceSize properties
- Simpler ResizeBuffers overload
- API cleanup: added some missing properties and fixed inconsistent naming
- Added CanvasBitmap.Device, Format, and AlphaMode properties
- Added ICanvasBrush.Device property
- Added CanvasImageSource.Background property
- Renamed CanvasAlphaBehavior -> CanvasAlphaMode
- Reordered CanvasDevice.CreateFromDirect3D11Device parameters to be more logical
- CanvasRenderTarget and CanvasSwapChain no longer implement ICanvasResourceCreator

We've also been hard at work on CanvasAnimatedControl, which will provide an easy to use game loop programming model on top of the swapchain functionality, but that isn't quite ready to release yet.

Most of the team is now heading out on vacation, so there will be a small gap in Win2D releases.  Our next sprint starts on Jan 5th, and its results will be published on Jan 19th.

---

## Win2D 0.0.8 - async CreateResources, better DPI support, and CanvasSwapChain

_December 1, 2014_

- CreateResources events can now register AsyncActions to be tracked. This simplifies writing async load functions, and makes it easier to robustly handle error cases such as device lost. During the week we will be posting a series of articles explaining the background and design process that led to this API.
- Assorted tweaks to make high DPI devices easier to work with and more likely to do the Right Thing' by default:
- CanvasRenderTarget and CanvasImageSource sizes are now specified in dips rather than pixels.
- Image effects automatically insert DPI compensation when a source bitmap has different DPI from the target device context.
- Re-raise the CreateResources event in response to monitor DPI changes.
- Added ICanvasResourceCreatorWithDpi interface.
- Added DPI properties to CanvasBitmap, CanvasDrawingSession, CanvasImageSource, and CanvasControl.
- Plumb DPI information through from one object to another in more places than we did before.
- Fixed CanvasControl layout bug - at high DPI the child image control could get the wrong size.
- Added DPI test to Example Gallery.
- With these changes, everything in Example Gallery is now rendering correctly on high DPI devices!
- Added CanvasSwapChain and CanvasSwapChainPanel.  These provide an alternative to CanvasImageSource, hosting Win2D graphics content inside XAML in a way that can be updated independently of the XAML UI renderer, which can provide smoother and lower latency animation.  These types are not currently very easy to use, though, so in the same way that CanvasControl sits on top of CanvasImageSource and handles a lot of the boilerplate operations for you, in the future we plan to add a CanvasAnimatedControl that will simplify working with CanvasSwapChain.
- Renamed SaveToFileAsync and SaveToStreamAsync to SaveAsync (for consistency with LoadAsync)

---

_This marks the start of Win2D NuGet package releases (earlier versions were only pushed to github in source form) so the changelog ends here._

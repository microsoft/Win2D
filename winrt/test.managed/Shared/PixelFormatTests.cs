// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.UI;

#if WINDOWS_UWP
using Windows.Graphics.DirectX;
#else
using Microsoft.Graphics.Canvas.DirectX;
#endif

namespace test.managed
{
    // This test makes sure that:
    //  1) All the formats we think should be supported actually are.
    //  2) The formats we think should not be supported indeed are not.
    //  3) Querying IsPixelFormatSupported matches these expectations.
    //  4) All supported formats can actually be used in the expected ways.
    //  5) The create overloads that do not take an explicit alpha mode infer a sensible choice.

    [TestClass]
    public class PixelFormatTests
    {
        [Flags]
        enum FormatFlags
        {
            PremultipliedAlpha = 1,
            StraightAlpha = 2,
            IgnoreAlpha = 4,
            RenderTarget = 8,
            Optional = 16,
            CannotDraw = 32,
        };


        // Table describing what pixel formats D2D supports.
        // If this changes in some future version of D2D, the docs in PixelFormats.aml should be updated to match.
        static Dictionary<DirectXPixelFormat, FormatFlags> formatFlags = new Dictionary<DirectXPixelFormat, FormatFlags>()
        {
            { DirectXPixelFormat.R8G8B8A8UIntNormalized,     FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                             | FormatFlags.RenderTarget                                                 },
            { DirectXPixelFormat.B8G8R8A8UIntNormalized,     FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                             | FormatFlags.RenderTarget                                                 },
            { DirectXPixelFormat.B8G8R8X8UIntNormalized,                                      FormatFlags.IgnoreAlpha                                                                                                        },
            { DirectXPixelFormat.R8G8B8A8UIntNormalizedSrgb, FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                             | FormatFlags.RenderTarget | FormatFlags.Optional                          },
            { DirectXPixelFormat.B8G8R8A8UIntNormalizedSrgb, FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                             | FormatFlags.RenderTarget | FormatFlags.Optional                          },
            { DirectXPixelFormat.R10G10B10A2UIntNormalized,  FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                                                        | FormatFlags.Optional                          },
            { DirectXPixelFormat.R16G16B16A16UIntNormalized, FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                             | FormatFlags.RenderTarget | FormatFlags.Optional                          },
            { DirectXPixelFormat.R16G16B16A16Float,          FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                             | FormatFlags.RenderTarget | FormatFlags.Optional                          },
            { DirectXPixelFormat.R32G32B32A32Float,          FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                             | FormatFlags.RenderTarget | FormatFlags.Optional                          },
            { DirectXPixelFormat.A8UIntNormalized,           FormatFlags.PremultipliedAlpha                           | FormatFlags.StraightAlpha | FormatFlags.RenderTarget                        | FormatFlags.CannotDraw },
            { DirectXPixelFormat.R8UIntNormalized,                                            FormatFlags.IgnoreAlpha                                                        | FormatFlags.Optional | FormatFlags.CannotDraw },
            { DirectXPixelFormat.R8G8UIntNormalized,                                          FormatFlags.IgnoreAlpha                                                        | FormatFlags.Optional | FormatFlags.CannotDraw },
            { DirectXPixelFormat.BC1UIntNormalized,          FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                                                                                                        },
            { DirectXPixelFormat.BC2UIntNormalized,          FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                                                                                                        },
            { DirectXPixelFormat.BC3UIntNormalized,          FormatFlags.PremultipliedAlpha | FormatFlags.IgnoreAlpha                                                                                                        },
        };


        [TestMethod]
        public void TestBitmapPixelFormats()
        {
            using (new DisableDebugLayer())
            {
                var device = new CanvasDevice();

                foreach (var format in AllPixelFormats())
                {
                    // Unknown formats should not be supported.
                    if (!formatFlags.ContainsKey(format))
                    {
                        Assert.IsFalse(device.IsPixelFormatSupported(format));
                        ValidateCannotCreateBitmap(device, format, FirstSupportedAlphaMode(format));
                        continue;
                    }

                    // Optional formats may be legitimately not supported, depending on the device.
                    if (!device.IsPixelFormatSupported(format))
                    {
                        Assert.IsTrue((formatFlags[format] & FormatFlags.Optional) != 0);
                        ValidateCannotCreateBitmap(device, format, FirstSupportedAlphaMode(format));
                        continue;
                    }

                    // We should be able to create this format using all its supported alpha modes.
                    var lotsOfZeroes = new byte[1024];

                    foreach (var alphaMode in SupportedAlphaModes(format))
                    {
                        CanvasBitmap.CreateFromBytes(device, lotsOfZeroes, 4, 4, format, 96, alphaMode);
                    }

                    // Other alpha modes should not be supported.
                    foreach (var alphaMode in UnsupportedAlphaModes(format))
                    {
                        ValidateCannotCreateBitmap(device, format, alphaMode);
                    }

                    // We should also be able to create this format without explicitly specifying an alpha mode.
                    var bitmap = CanvasBitmap.CreateFromBytes(device, lotsOfZeroes, 4, 4, format);

                    Assert.AreEqual(FirstSupportedAlphaMode(format), bitmap.AlphaMode);

                    // Some formats can be drawn directly, while others cannot.
                    if ((formatFlags[format] & FormatFlags.CannotDraw) == 0)
                    {
                        ValidateCanDrawImage(device, bitmap);
                    }
                    else
                    {
                        ValidateCannotDrawImage(device, bitmap);
                    }

                    // But all formats should be drawable when used as effect inputs.
                    ValidateCanDrawImage(device, new ColorMatrixEffect { Source = bitmap });

                    // Make sure we can get and set pixels of this format.
                    // This would fail if D2D internally created a DXGI surface of some unexpected format.
                    var bytes = bitmap.GetPixelBytes();

                    Assert.IsTrue(bytes.All(b => b == 0));

                    bitmap.SetPixelBytes(bytes);
                }
            }
        }


        [TestMethod]
        public void TestRenderTargetPixelFormats()
        {
            using (new DisableDebugLayer())
            {
                var device = new CanvasDevice();

                foreach (var format in AllPixelFormats())
                {
                    // Unknown formats should not be supported.
                    if (!formatFlags.ContainsKey(format))
                    {
                        ValidateCannotCreateRenderTarget(device, format, FirstSupportedAlphaMode(format));
                        continue;
                    }

                    // Non-renderable formats should also not be supported.
                    if ((formatFlags[format] & FormatFlags.RenderTarget) == 0)
                    {
                        ValidateCannotCreateRenderTarget(device, format, FirstSupportedAlphaMode(format));
                        continue;
                    }

                    // Validate implementation assumption: all rendertarget formats also support premultiplied
                    // alpha. If D2D ever adds support for a rendertarget format where this is not the case,
                    // CanvasRenderTargetFactory::CreateFromDirect3D11Surface should be updated to know about
                    // that, along the same lines as how CanvasBitmap::CreateFromDirect3D11Surface already has
                    // smarts to choose a suitable alpha mode depending on the pixel format.
                    Assert.IsTrue((formatFlags[format] & FormatFlags.PremultipliedAlpha) != 0);

                    // Optional formats may be legitimately not supported, depending on the device.
                    if (!device.IsPixelFormatSupported(format))
                    {
                        Assert.IsTrue((formatFlags[format] & FormatFlags.Optional) != 0);
                        ValidateCannotCreateRenderTarget(device, format, FirstSupportedAlphaMode(format));
                        continue;
                    }

                    // We should be able to create and render to this format using all its supported alpha modes.
                    foreach (var alphaMode in SupportedAlphaModes(format))
                    {
                        var renderTarget = new CanvasRenderTarget(device, 16, 16, 96, format, alphaMode);

                        ValidateCanDrawToRenderTarget(renderTarget);
                    }

                    // Other alpha modes should not be supported.
                    foreach (var alphaMode in UnsupportedAlphaModes(format))
                    {
                        ValidateCannotCreateRenderTarget(device, format, alphaMode);
                    }
                }
            }
        }


        static void ValidateCannotCreateBitmap(CanvasDevice device, DirectXPixelFormat format, CanvasAlphaMode alphaMode)
        {
            ValidateUnsupportedPixelFormatException(() =>
            {
                CanvasBitmap.CreateFromBytes(device, new byte[1024], 4, 4, format, 96, alphaMode);
            });
        }


        static void ValidateCannotCreateRenderTarget(CanvasDevice device, DirectXPixelFormat format, CanvasAlphaMode alphaMode)
        {
            ValidateUnsupportedPixelFormatException(() =>
            {
                new CanvasRenderTarget(device, 16, 16, 96, format, alphaMode);
            });
        }


        static void ValidateCanDrawImage(CanvasDevice device, ICanvasImage image)
        {
            using (var commandList = new CanvasCommandList(device))
            using (var drawingSession = commandList.CreateDrawingSession())
            {
                drawingSession.DrawImage(image);
            }
        }


        static void ValidateCannotDrawImage(CanvasDevice device, ICanvasImage image)
        {
            ValidateUnsupportedPixelFormatException(() =>
            {
                ValidateCanDrawImage(device, image);
            });
        }


        static void ValidateCanDrawToRenderTarget(CanvasRenderTarget renderTarget)
        {
            using (var drawingSession = renderTarget.CreateDrawingSession())
            {
                drawingSession.Clear(Colors.CornflowerBlue);
            }
        }


        static void ValidateUnsupportedPixelFormatException(Action action)
        {
            try
            {
                action();
                Assert.Fail("should throw");
            }
            catch (Exception e)
            {
                Assert.IsTrue(e.Message.Contains("The bitmap pixel format is unsupported."));
                Assert.IsTrue(e.Message.Contains("0x88982F80"));
            }
        }


        static IEnumerable<DirectXPixelFormat> AllPixelFormats()
        {
            return Enum.GetValues(typeof(DirectXPixelFormat)).Cast<DirectXPixelFormat>();
        }


        static IEnumerable<CanvasAlphaMode> SupportedAlphaModes(DirectXPixelFormat format)
        {
            return GetAlphaModes(formatFlags[format]);
        }


        static IEnumerable<CanvasAlphaMode> UnsupportedAlphaModes(DirectXPixelFormat format)
        {
            return GetAlphaModes(~formatFlags[format]);
        }


        static IEnumerable<CanvasAlphaMode> GetAlphaModes(FormatFlags flags)
        {
            if ((flags & FormatFlags.PremultipliedAlpha) != 0)
                yield return CanvasAlphaMode.Premultiplied;

            if ((flags & FormatFlags.IgnoreAlpha) != 0)
                yield return CanvasAlphaMode.Ignore;

            if ((flags & FormatFlags.StraightAlpha) != 0)
                yield return CanvasAlphaMode.Straight;
        }


        static CanvasAlphaMode FirstSupportedAlphaMode(DirectXPixelFormat format)
        {
            return formatFlags.ContainsKey(format) ? SupportedAlphaModes(format).First() 
                                                   : CanvasAlphaMode.Premultiplied;
        }
    }
}

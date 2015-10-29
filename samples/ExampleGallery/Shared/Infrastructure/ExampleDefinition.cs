// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

using System;

namespace ExampleGallery
{
    public class ExampleDefinition
    {
        public ExampleDefinition(string name, Type control)
        {
            Name = name;
            Control = control;
        }

        public string Name { get; private set; }
        public Type Control { get; private set; }

        public override string ToString()
        {
            return this.Name;
        }

        public string ThumbnailWide
        {
            get { return "ms-appx:///Thumbnails/" + ThumbnailFilename("Wide"); }
        }

        public string ThumbnailNarrow
        {
            get { return "ms-appx:///Thumbnails/" + ThumbnailFilename("Narrow"); }
        }

        public string ThumbnailFilename(string suffix)
        {
            return Name.Replace(" ", "") + suffix + ".png";
        }
    }


    public class ExampleDefinitions
    {
        static ExampleDefinition[] definitions =
        {
            // Visual demos.
            new ExampleDefinition("Burning Text", typeof(BurningTextExample)),
            new ExampleDefinition("Game of Life", typeof(GameOfLife)),
#if WINDOWS_UWP
            new ExampleDefinition("Camera Effect", typeof(CameraEffectExample)),
            new ExampleDefinition("Video Effect", typeof(BasicVideoEffectExample)),
#endif
            new ExampleDefinition("Particle System", typeof(ParticleExample)),
            new ExampleDefinition("Vector Art", typeof(VectorArt)),

            // Show capabilities of the API.
            new ExampleDefinition("Effects", typeof(EffectsExample)),
            new ExampleDefinition("Custom Effects", typeof(CustomEffects)),
            new ExampleDefinition("Geometry Operations", typeof(GeometryOperations)),
            new ExampleDefinition("Layers", typeof(LayersExample)),
#if WINDOWS_UWP
            new ExampleDefinition("Ink", typeof(InkExample)),
#endif
            new ExampleDefinition("Shapes", typeof(ShapesExample)),
            new ExampleDefinition("Stroke Styles", typeof(StrokeStyles)),
            new ExampleDefinition("Arc Options", typeof(ArcOptions)),          
#if WINDOWS_UWP
            new ExampleDefinition("Gradient Mesh", typeof(GradientMeshExample)),
#endif
            new ExampleDefinition("Text Layouts", typeof(TextLayouts)),
            new ExampleDefinition("Text Outlines", typeof(TextOutlines)),
            new ExampleDefinition("Custom Fonts", typeof(CustomFonts)),
            new ExampleDefinition("Text Directions", typeof(TextDirectionsExample)),
            new ExampleDefinition("Font Metrics", typeof(FontMetrics)),

            // Show how to use the API.
            new ExampleDefinition("Custom Control", typeof(CustomControlExample)),
            new ExampleDefinition("Virtual Control", typeof(VirtualControlExample)),
            new ExampleDefinition("Animated Control", typeof(AnimatedControlExample)),
            new ExampleDefinition("Direct3D Interop", typeof(Direct3DInteropExample)),
            new ExampleDefinition("Keyboard Input", typeof(KeyboardInputExample)),
            new ExampleDefinition("Background Task", typeof(BackgroundTaskExample)),
#if !WINDOWS_PHONE_APP
            new ExampleDefinition("Printing", typeof(PrintingExample)),
#endif

            // Tests for internal use by the Win2D team.
            new ExampleDefinition("Image Source Update Region", typeof(ImageSourceUpdateRegion)),
            new ExampleDefinition("DPI", typeof(DpiExample)),
            new ExampleDefinition("Control Transforms", typeof(ControlTransforms)),
            new ExampleDefinition("DrawImage Emulations", typeof(DrawImageEmulations)),
            new ExampleDefinition("Bitmap Rotation", typeof(BitmapRotation)),
#if WINDOWS_UWP
            new ExampleDefinition("SpriteBatch Performance", typeof(SpriteBatchPerf)),
#endif
            new ExampleDefinition("Developer Tools", typeof(DeveloperTools)),
            new ExampleDefinition("About", typeof(About)),
        };

        public static ExampleDefinition[] Definitions { get { return definitions; } }
    }
}

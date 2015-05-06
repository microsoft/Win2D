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
            new ExampleDefinition("Effects", typeof(EffectsExample)),
            new ExampleDefinition("Burning Text", typeof(BurningTextExample)),
            new ExampleDefinition("Game of Life", typeof(GameOfLife)),
#if WINDOWS_UAP
            new ExampleDefinition("Video Effect", typeof(BasicVideoEffectExample)),
#endif
            new ExampleDefinition("Particle System", typeof(ParticleExample)),
            new ExampleDefinition("Geometry Operations", typeof(GeometryOperations)),
            new ExampleDefinition("Layers", typeof(LayersExample)),
            new ExampleDefinition("Shapes", typeof(ShapesExample)),
            new ExampleDefinition("Stroke Styles", typeof(StrokeStyles)),
            new ExampleDefinition("Arc Options", typeof(ArcOptions)),
            new ExampleDefinition("Custom Fonts", typeof(CustomFonts)),
            new ExampleDefinition("Text Layouts", typeof(TextLayouts)),
            new ExampleDefinition("Text Directions", typeof(TextDirectionsExample)),
            new ExampleDefinition("Animated Control", typeof(AnimatedControlExample)),
            new ExampleDefinition("Canvas Control", typeof(CanvasControlExample)),
            new ExampleDefinition("Direct3D Interop", typeof(Direct3DInteropExample)),
            new ExampleDefinition("Image Source Update Region", typeof(ImageSourceUpdateRegion)),
            new ExampleDefinition("DPI", typeof(DpiExample)),
            new ExampleDefinition("Control Transforms", typeof(ControlTransforms)),
            new ExampleDefinition("DrawImage Emulations", typeof(DrawImageEmulations)),
            new ExampleDefinition("Bitmap Rotation", typeof(BitmapRotation)),
            new ExampleDefinition("About", typeof(About)),
            new ExampleDefinition("Developer Tools", typeof(DeveloperTools)),
        };

        public static ExampleDefinition[] Definitions { get { return definitions; } }
    }
}

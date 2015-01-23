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
using System.Collections.Generic;

namespace ExampleGallery
{
    public class ExampleDefinition
    {
        public string Name { get; set; }
        public Type Control { get; set; }

        public override string ToString()
        {
            return this.Name;
        }
    }


    public class ExampleDefinitions
    {
        private List<ExampleDefinition> definitions;

        public List<ExampleDefinition> Definitions { get { return this.definitions; } }

        public ExampleDefinitions()
        {
            this.definitions = new List<ExampleDefinition>();

            this.definitions.Add(new ExampleDefinition() { Name = "Shapes", Control = typeof(ShapesExample) });
            this.definitions.Add(new ExampleDefinition() { Name = "Stroke Styles", Control = typeof(StrokeStyles) });
            this.definitions.Add(new ExampleDefinition() { Name = "Effects", Control = typeof(EffectsExample) });
            this.definitions.Add(new ExampleDefinition() { Name = "Burning Text", Control = typeof(BurningTextExample) });
            this.definitions.Add(new ExampleDefinition() { Name = "Game of Life", Control = typeof(GameOfLife) });
            this.definitions.Add(new ExampleDefinition() { Name = "Canvas Control", Control = typeof(CanvasControlExample) });
            this.definitions.Add(new ExampleDefinition() { Name = "ImageSource Update Region", Control = typeof(ImageSourceUpdateRegion) });
            this.definitions.Add(new ExampleDefinition() { Name = "DPI", Control = typeof(DpiExample) });
            this.definitions.Add(new ExampleDefinition() { Name = "Animated Control", Control = typeof(AnimatedControlExample) });
        }
    }
}

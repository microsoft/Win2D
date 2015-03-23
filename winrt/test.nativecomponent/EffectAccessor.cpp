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

#include "pch.h"

#include <Microsoft.Graphics.Canvas.Effects.interop.h>
#include <Canvas.abi.h>

namespace NativeComponent
{
    using namespace Microsoft::WRL;
    using namespace Microsoft::Graphics::Canvas::Effects;

    using AbiEffectNative = ABI::Microsoft::Graphics::Canvas::Effects::IGraphicsEffectD2D1Interop;
    using AbiEffectSource = ABI::Microsoft::Graphics::Canvas::Effects::IGraphicsEffectSource;
    using AbiPropertyValue = ABI::Windows::Foundation::IPropertyValue;

    // Exposes features of IGraphicsEffectD2D1Interop for use by test.managed.EffectTests.
    public ref class EffectAccessor sealed
    {
    public:
        static int GetSourceCount(IGraphicsEffect^ effect)
        {
            auto effectNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(effect));

            unsigned count;
            ThrowIfFailed(effectNative->GetSourceCount(&count));

            return count;
        }

        static IGraphicsEffectSource^ GetSource(IGraphicsEffect^ effect, int index)
        {
            auto effectNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(effect));

            ComPtr<AbiEffectSource> source;
            ThrowIfFailed(effectNative->GetSource(index, &source));

            return reinterpret_cast<IGraphicsEffectSource^>(source.Get());
        }

        static int GetPropertyCount(IGraphicsEffect^ effect)
        {
            auto effectNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(effect));

            unsigned count;
            ThrowIfFailed(effectNative->GetPropertyCount(&count));

            return count;
        }

        static Object^ GetProperty(IGraphicsEffect^ effect, int index)
        {
            auto effectNative = As<AbiEffectNative>(reinterpret_cast<IInspectable*>(effect));

            ComPtr<AbiPropertyValue> propertyValue;
            ThrowIfFailed(effectNative->GetProperty(index, &propertyValue));

            return reinterpret_cast<Object^>(propertyValue.Get());
        }
    };
}

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

// This file is included multiple times, from both Strings.h and Strings.cpp, 
// using different macro definitions to generate both constant declarations and 
// definitions from the same source data.

// If we someday want to localize error messages into languages other than 
// English, this data should be moved out to a proper resources file. But for 
// now, simple C++ constants are "good enough"(tm).

STRING(EffectNullInput, L"Effect input #%d is null.")
STRING(EffectNullProperty, L"Effect property #%d is null.")
STRING(EffectWrongInputType, L"Effect input #%d is an unsupported type. To draw an effect using Win2D, all its inputs must be Win2D ICanvasImage objects.")
STRING(EffectWrongPropertyType, L"Effect property #%d is the wrong type for this effect.")
STRING(ImageBrushRequiresSourceRectangle, L"When using image types other than CanvasBitmap, CanvasImageBrush.SourceRectangle must not be null.")
STRING(UnrecognizedImageFileExtension, L"When saving a CanvasBitmap without specifying a CanvasBitmapFileFormat, the file name must include a recognized file extension such as '.jpeg' or '.png'.");
STRING(WrongArrayLength, L"The array was expected to be of size %d; actual array was of size %d.")
STRING(AutoFileFormatNotAllowed, L"The option CanvasFileFormat.Auto is not allowed when saving to a stream.")
STRING(CanvasDeviceGetDeviceWhenNotCreated, L"The control does not currently have a CanvasDevice associated with it. "
    L"Ensure that resources are created from a CreateResources or Draw event handler.");
STRING(PixelColorsFormatRestriction, L"This method only supports resources with pixel format DirectXPixelFormat::B8G8R8A8UIntNormalized.")
STRING(MultipleAsyncCreateResourcesNotSupported, L"Only one asynchronous CreateResources action can be tracked at a time.")
STRING(ResourceTrackerWrongDevice, L"Existing resource wrapper is associated with a different device.")
STRING(ResourceTrackerWrongDpi, L"Existing resource wrapper has a different DPI.")
STRING(CommandListCannotBeDrawnToAfterItHasBeenUsed, L"CanvasCommandList.CreateDrawingSession cannot be called after the CanvasCommandList has been used as an image.")
STRING(ExpectedPositiveNonzero, L"A positive, non-zero number was expected for this method.")
STRING(DrawImageMinBlendNotSupported, L"This DrawImage overload is not valid when CanvasDrawingSession.Blend is set to CanvasBlend.Min.")
STRING(EndFigureWithoutBeginFigure, L"A call to CanvasPathBuilder.EndFigure occurred without a previous call to CanvasPathBuilder.BeginFigure.")
STRING(PathBuilderClosedMidFigure, L"There was an attempt to use a CanvasPathBuilder, which was missing a call to CanvasPathBuilder.EndFigure.")
STRING(TwoBeginFigures, L"A call to CanvasPathBuilder.BeginFigure occurred, when the figure was already begun.")
STRING(CanOnlyAddPathDataWhileInFigure, L"This operation is only allowed after a successful call to CanvasPathBuilder.BeginFigure.")
STRING(SetFilledRegionDeterminationAfterBeginFigure, L"This operation is not allowed after the first call to CanvasPathBuilder.BeginFigure.")
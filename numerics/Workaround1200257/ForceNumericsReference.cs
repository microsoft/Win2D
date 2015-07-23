// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

namespace Workaround1200257
{
    // Force a reference to the System.Numerics.Vectors assembly, to workaround a .NET Native
    // compiler bug where release builds using Win2D fail if the app does not itself directly
    // use any of the numerics types.
    //
    // TODO: remove this workaround when DevDiv:1200257 is fixed.
    class ForceNumericsReference
    {
        public ForceNumericsReference()
        {
            var a = System.Numerics.Vector2.Zero;
            var b = a.X;
        }
    }
}

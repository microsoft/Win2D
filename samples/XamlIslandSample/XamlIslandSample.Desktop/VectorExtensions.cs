using System;
using System.Collections.Generic;
using System.Text;

namespace System.Numerics
{
    public static class VectorExtensions
    {
        public static Vector2 ToVector2(this global::Windows.Foundation.Size size)
        {
            return new Vector2((float)size.Width, (float)size.Height);
        }
    }
}

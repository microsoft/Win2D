using System;

namespace MathTests
{
    static class MathHelper
    {
        public const float Pi = (float)Math.PI;
        public const float PiOver2 = (float)Math.PI / 2f;
        public const float PiOver4 = (float)Math.PI / 4f;


        public static float ToRadians(float degrees)
        {
            return degrees * (float)Math.PI / 180f;
        }
    }


    class DescriptionAttribute : Attribute
    {
        public DescriptionAttribute(string description)
        {
        }
    }
}

//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#pragma once

#include "pch.h"


#define M_PI 3.14159265358979323846f


namespace MathTests
{
	inline float ToRadians(float degrees)
	{
		return degrees * M_PI / 180.0f;
	}
}


namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<Windows::Math::Plane>(Windows::Math::Plane const& value)
			{
				wchar_t tmp[256];
				swprintf_s(tmp, L"{Normal:{X:%f Y:%f Z:%f} D:%f}", value.Normal.X, value.Normal.Y, value.Normal.Z, value.D);
				return tmp;
			}
		}
	}
}

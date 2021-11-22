#pragma once
//////////////////////////////////////////////////////////////////////
// Wall
// Dark Forces Derived Renderer - Wall functions
//////////////////////////////////////////////////////////////////////
#include <TFE_System/types.h>
#include <TFE_Jedi/Math/fixedPoint.h>
#include <TFE_Jedi/Math/core_math.h>
#include "../redgePair.h"

namespace TFE_Jedi
{
	namespace RClassic_Gpu
	{
		void edgePair_setup(s32 length, s32 x0, f32 dyFloor_dx, f32 yFloor1, f32 yFloor, f32 dyCeil_dx, f32 yCeil, f32 yCeil1, EdgePairFloat* flat);
	}
}

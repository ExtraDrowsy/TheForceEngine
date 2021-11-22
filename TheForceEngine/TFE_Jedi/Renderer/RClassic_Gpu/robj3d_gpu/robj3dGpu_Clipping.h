#pragma once
//////////////////////////////////////////////////////////////////////
// Wall
// Dark Forces Derived Renderer - Wall functions
//////////////////////////////////////////////////////////////////////
#include <TFE_System/types.h>
#include <TFE_Asset/modelAsset_Jedi.h>
#include <TFE_Jedi/Math/fixedPoint.h>
#include <TFE_Jedi/Math/core_math.h>
#include "../../rlimits.h"

namespace TFE_Jedi
{
	namespace RClassic_Gpu
	{
		s32 clipPolygon(Polygon* polygon);
	}
}

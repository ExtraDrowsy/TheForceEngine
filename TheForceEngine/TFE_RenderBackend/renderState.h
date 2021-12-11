#pragma once
//////////////////////////////////////////////////////////////////////
// The Force Engine GPU Library
// This handles low level access to the GPU device and is used by
// both the software and hardware rendering systems in order to
// provide basic access.
// 
// Renderers will create a virtual display or render target.
// swap() handles blitting this result and then rendering UI
// on top.
//////////////////////////////////////////////////////////////////////

#include <TFE_System/types.h>

enum StateEnable
{
	STATE_CULLING = (1 << 0),
	STATE_BLEND = (1 << 1),
	STATE_DEPTH_TEST = (1 << 2),
	STATE_DEPTH_WRITE = (1 << 3),
	STATE_STENCIL_TEST = (1 << 4),
	STATE_SCISSOR = (1 << 5),
};

enum StateBlendFactor
{
	BLEND_ONE = 0,
	BLEND_ZERO,
	BLEND_SRC_ALPHA,
	BLEND_ONE_MINUS_SRC_ALPHA,
};

enum StateBlendFunc
{
	BLEND_FUNC_ADD = 0,
};

enum ColorMaskChannels
{
	CMASK_RED = (1 << 0),
	CMASK_GREEN = (1 << 1),
	CMASK_BLUE = (1 << 2),
	CMASK_ALPHA = (1 << 3),
	CMASK_RGB = CMASK_RED | CMASK_GREEN | CMASK_BLUE,
	CMASK_ALL = CMASK_RED | CMASK_GREEN | CMASK_BLUE | CMASK_ALPHA
};

enum ComparisonFunction
{
	CMP_NEVER = 0,
	CMP_LESS,
	CMP_EQUAL,
	CMP_LEQUAL,
	CMP_GREATER,
	CMP_NOTEQUAL,
	CMP_GEQUAL,
	CMP_ALWAYS,
	CMP_COUNT
};

enum StencilOperation
{
	OP_KEEP = 0,	// Keep the current value (do not change).
	OP_SET_ZERO,	// Set the stencil value to 0.
	OP_REPLACE,		// Replace the stencil value with the reference value.
	OP_INC_CLAMP,	// Increment the stencil value, clamps to 255.
	OP_INC_WRAP,	// Increment the stencil value, wraps to 0 if already 255.
	OP_DEC_CLAMP,	// Decrement the stencil value, clamps to 0.
	OP_DEC_WRAP,	// Decrement the stencil value, wraps to 255 if already 0.
	OP_INVERT,		// Bitwise invert.
};

namespace TFE_RenderState
{
	void clear();

	void setStateEnable(bool enable, u32 stateFlags);
	void setBlendMode(StateBlendFactor srcFactor, StateBlendFactor dstFactor, StateBlendFunc func = BLEND_FUNC_ADD);
	void setDepthFunction(ComparisonFunction func);
	void setColorMask(u32 colorMask);
	void setStencilMask(u32 stencilMask);
	void setStencilFunc(ComparisonFunction func, s32 refValue, u32 mask = 0xff);
	void setStencilOperation(StencilOperation stencilFail = OP_KEEP, StencilOperation depthFail = OP_KEEP, StencilOperation depthPass = OP_KEEP);
	void setDepthBias(f32 factor = 0.0f, f32 bias = 0.0f);
};

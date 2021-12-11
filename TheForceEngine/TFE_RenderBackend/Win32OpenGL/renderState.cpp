#include <TFE_RenderBackend/renderState.h>
#include <TFE_System/system.h>
#include <TFE_RenderBackend/renderBackend.h>
#include <GL/glew.h>
#include <vector>

namespace TFE_RenderState
{
	static const GLenum c_blendFuncGL[] =
	{
		GL_FUNC_ADD, // BLEND_FUNC_ADD
	};

	static const GLenum c_blendFactor[] =
	{
		GL_ONE,  // BLEND_ONE = 0,
		GL_ZERO, // BLEND_ZERO,
		GL_SRC_ALPHA, // BLEND_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA// BLEND_ONE_MINUS_SRC_ALPHA,
	};

	static const GLenum c_comparisionFunc[] =
	{
		GL_NEVER,    //CMP_NEVER
		GL_LESS,     //CMP_LESS
		GL_EQUAL,    //CMP_EQUAL
		GL_LEQUAL,   //CMP_LEQUAL
		GL_GREATER,  //CMP_GREATER
		GL_NOTEQUAL, //CMP_NOTEQUAL
		GL_GEQUAL,   //CMP_GEQUAL
		GL_ALWAYS,   //CMP_ALWAYS
	};

	static const GLenum c_stencilOp[] =
	{
		GL_KEEP,        //OP_KEEP
		GL_ZERO,        //OP_SET_ZERO
		GL_REPLACE,     //OP_REPLACE
		GL_INCR,        //OP_INC_CLAMP
		GL_INCR_WRAP,   //OP_INC_WRAP
		GL_DECR,        //OP_DEC_CLAMP
		GL_DECR_WRAP,   //OP_DEC_WRAP
		GL_INVERT,      //OP_INVERT	
	};

	static u32 s_currentState;
	static u32 s_depthFunc;
	static u32 s_colorMask;
	static u32 s_stencilMask;

	void clear()
	{
		s_currentState = 0u;
		s_colorMask = CMASK_ALL;
		s_depthFunc = CMP_LEQUAL;
		s_stencilMask = 0xff;
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(false);
		glStencilMask(0xff);
		glColorMask(true, true, true, true);
		setDepthBias(0.0f, 0.0f);
		setStencilFunc(CMP_NEVER, 0);
		setStencilOperation();
	}

	void setStateEnable(bool enable, u32 stateFlags)
	{
		if (enable)
		{
			const u32 stateToChange = stateFlags & (~s_currentState);
			if (stateToChange & STATE_CULLING)
			{
				glEnable(GL_CULL_FACE);
			}
			if (stateToChange & STATE_BLEND)
			{
				glEnable(GL_BLEND);
			}
			if (stateToChange & STATE_DEPTH_TEST)
			{
				glEnable(GL_DEPTH_TEST);
			}
			if (stateToChange & STATE_DEPTH_WRITE)
			{
				glDepthMask(true);
			}
			if (stateToChange & STATE_STENCIL_TEST)
			{
				glEnable(GL_STENCIL_TEST);
			}
			if (stateToChange & STATE_SCISSOR)
			{
				glEnable(GL_SCISSOR_TEST);
			}
			s_currentState |= stateFlags;
		}
		else
		{
			const u32 stateToChange = stateFlags & s_currentState;
			if (stateToChange & STATE_CULLING)
			{
				glDisable(GL_CULL_FACE);
			}
			if (stateToChange & STATE_BLEND)
			{
				glDisable(GL_BLEND);
			}
			if (stateToChange & STATE_DEPTH_TEST)
			{
				glDisable(GL_DEPTH_TEST);
			}
			if (stateToChange & STATE_STENCIL_TEST)
			{
				glDisable(GL_STENCIL_TEST);
			}
			if (stateToChange & STATE_DEPTH_WRITE)
			{
				glDepthMask(false);
			}
			if (stateToChange & STATE_SCISSOR)
			{
				glDisable(GL_SCISSOR_TEST);
			}
			s_currentState &= ~stateFlags;
		}
	}
		
	void setBlendMode(StateBlendFactor srcFactor, StateBlendFactor dstFactor, StateBlendFunc func)
	{
		glBlendEquation(c_blendFuncGL[func]);
		glBlendFunc(c_blendFactor[srcFactor], c_blendFactor[dstFactor]);
	}

	void setDepthFunction(ComparisonFunction func)
	{
		if (func != s_depthFunc)
		{
			glDepthFunc(c_comparisionFunc[func]);
			s_depthFunc = func;
		}
	}
			
	void setStencilMask(u32 stencilMask)
	{
		if (stencilMask != s_stencilMask)
		{
			glStencilMask(stencilMask);
			s_stencilMask = stencilMask;
		}
	}
		
	void setStencilFunc(ComparisonFunction func, s32 refValue, u32 mask)
	{
		glStencilFunc(c_comparisionFunc[func], refValue, mask);
	}
		
	void setStencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass)
	{
		glStencilOp(c_stencilOp[stencilFail], c_stencilOp[depthFail], c_stencilOp[depthPass]);
	}
		
	void setColorMask(u32 colorMask)
	{
		if (colorMask != s_colorMask)
		{
			glColorMask((s_colorMask&CMASK_RED)!=0, (s_colorMask&CMASK_GREEN) != 0, (s_colorMask&CMASK_BLUE) != 0, (s_colorMask&CMASK_ALPHA) != 0);
			s_colorMask = colorMask;
		}
	}

	void setDepthBias(f32 factor, f32 bias)
	{
		if (factor != 0.0f || bias != 0.0f)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(factor, bias);
		}
		else
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(0.0f, 0.0f);
		}
	}
}

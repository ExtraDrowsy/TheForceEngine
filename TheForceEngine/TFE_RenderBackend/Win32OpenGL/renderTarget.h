#pragma once
//////////////////////////////////////////////////////////////////////
// Wrapper for OpenGL textures that expose the limited set of 
// required functionality.
//////////////////////////////////////////////////////////////////////

#include <TFE_System/types.h>
#include <TFE_RenderBackend/textureGpu.h>

class RenderTarget
{
public:
	RenderTarget() : m_texture(nullptr), m_gpuHandle(0), m_depthBufferHandle(0), m_hasStencilBuffer(false) {}
	~RenderTarget();

	bool create(TextureGpu* texture, bool depthBuffer, bool stencilBuffer);
	void bind();
	void clear(const f32* color, f32 depth, s32 value = 0);
	void clearDepth(f32 depth);
	void clearStencil(s32 value);
	static void unbind();

	inline const TextureGpu* getTexture() const { return m_texture; }

private:
	const TextureGpu* m_texture;
	u32 m_gpuHandle;
	u32 m_depthBufferHandle;
	bool m_hasStencilBuffer;
};

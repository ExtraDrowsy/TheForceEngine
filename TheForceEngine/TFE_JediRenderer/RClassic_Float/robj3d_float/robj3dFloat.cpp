#include <TFE_System/profiler.h>
#include <TFE_Settings/settings.h>
#include <TFE_RenderBackend/renderBackend.h>

#include "robj3dFloat.h"
#include "robj3dFloat_TransformAndLighting.h"
#include "robj3dFloat_PolygonSetup.h"
#include "robj3dFloat_Culling.h"
#include "robj3dFloat_Clipping.h"
#include "robj3dFloat_PolygonDraw.h"
#include "../../fixedPoint.h"
#include "../../rmath.h"
#include "../../rcommon.h"
#include "../../robject.h"

namespace TFE_JediRenderer
{

namespace RClassic_Float
{
	void robj3d_projectVertices(vec3_float* pos, s32 count, vec3_float* out, bool perspCorrect);
	void robj3d_prepareUv(vec2_float* uv, s32 count, vec3_float* proj);
	void robj3d_drawVertices(s32 vertexCount, const vec3_float* vertices, u8 color, s32 size);
	s32 polygonSort(const void* r0, const void* r1);

	s32 log2i(s32 x)
	{
		s32 l2 = 0;
		while (x > 1)
		{
			x >>= 1;
			l2++;
		}
		return l2;
	}

	void robj3d_draw(SecObject* obj, JediModel* model)
	{
		// Handle transforms and vertex lighting.
		robj3d_transformAndLight(obj, model);

		// Draw vertices and return if the flag is set.
		if (model->flags & MFLAG_DRAW_VERTICES)
		{
			// Scale the points based on the resolution ratio.
			const u32 height = TFE_RenderBackend::getVirtualDisplayHeight();
			const s32 scale = (s32)max(1, height / 200);
			
			// If the MFLAG_DRAW_VERTICES flag is set, draw all vertices as points. 
			robj3d_drawVertices(model->vertexCount, s_verticesVS, model->polygons[0].color, scale);
			return;
		}

		// Cull backfacing polygons. The results are stored as "visPolygons"
		s32 visPolygonCount = robj3d_backfaceCull(model);
		// Nothing to render.
		if (visPolygonCount < 1) { return; }

		// Setup perspective correct information.
		const u32 height = TFE_RenderBackend::getVirtualDisplayHeight();
		s_perspectiveCorrect = TFE_Settings::getGraphicsSettings()->perspectiveCorrectTexturing;
		s_affineCorrectionLen = max(8, previousPowerOf2(height >> 5));
		s_affineCorrectionShift = log2i(s_affineCorrectionLen);

		// Sort polygons from back to front.
		qsort(s_visPolygons, visPolygonCount, sizeof(Polygon*), polygonSort);

		// Draw polygons
		Polygon** visPolygon = s_visPolygons;
		for (s32 i = 0; i < visPolygonCount; i++, visPolygon++)
		{
			Polygon* polygon = *visPolygon;
			if (polygon->vertexCount <= 0) { continue; }

			robj3d_setupPolygon(polygon);

			s32 polyVertexCount = clipPolygon(polygon);
			// Cull the polygon if not enough vertices survive clipping.
			if (polyVertexCount < 3) { continue; }

			// Project the resulting vertices.
			robj3d_projectVertices(s_polygonVerticesVS, polyVertexCount, s_polygonVerticesProj, s_perspectiveCorrect);
			if (s_perspectiveCorrect && (polygon->shading&PSHADE_TEXTURE))
			{
				robj3d_prepareUv(s_polygonUv, polyVertexCount, s_polygonVerticesProj);
			}

			// Draw polygon based on its shading mode.
			robj3d_drawPolygon(polygon, polyVertexCount, obj, model);
		}
	}
		
	void robj3d_drawVertices(s32 vertexCount, const vec3_float* vertices, u8 color, s32 size)
	{
		// cannot draw if the color is transparent.
		if (color == 0) { return; }
		const s32 halfSize = (size > 1) ? (size >> 1) : 0;
		const s32 area = size * size;

		// Loop through the vertices and draw them as pixels.
		const vec3_float* vertex = vertices;
		for (s32 v = 0; v < vertexCount; v++, vertex++)
		{
			const f32 z = vertex->z;
			if (z <= 1.0f) { continue; }

			const s32 pixel_x = roundFloat((vertex->x*s_focalLength) / z + s_halfWidth);
			const s32 pixel_y = roundFloat((vertex->y*s_focalLenAspect) / z + s_halfHeight);

			// If the X position is out of view, skip the vertex.
			if (pixel_x < s_minScreenX || pixel_x > s_maxScreenX)
			{
				continue;
			}
			// Check the 1d depth buffer and Y positon and skip if occluded.
			if (z >= s_depth1d[pixel_x] || pixel_y > s_windowMaxY || pixel_y < s_windowMinY || pixel_y < s_windowTop[pixel_x] || pixel_y > s_windowBot[pixel_x])
			{
				continue;
			}

			for (s32 i = 0; i < area; i++)
			{
				const s32 x = clamp(pixel_x - halfSize + (i % size), s_minScreenX, s_maxScreenX);
				const s32 y = clamp(pixel_y - halfSize + (i / size), s_windowMinY, s_windowMaxY);
				s_display[y*s_width + x] = color;
			}
		}
	}

	void robj3d_projectVertices(vec3_float* pos, s32 count, vec3_float* out, bool perspCorrect)
	{
		for (s32 i = 0; i < count; i++, pos++, out++)
		{
			const f32 rcpZ = 1.0f / pos->z;

			out->x = floorf((pos->x*s_focalLength)*rcpZ + s_halfWidth);
			out->y = floorf((pos->y*s_focalLenAspect)*rcpZ + s_halfHeight);
			out->z = perspCorrect ? rcpZ : pos->z;
		}
	}

	void robj3d_prepareUv(vec2_float* uv, s32 count, vec3_float* proj)
	{
		for (s32 i = 0; i < count; i++, uv++, proj++)
		{
			uv->x *= proj->z;
			uv->z *= proj->z;
		}
	}

	s32 polygonSort(const void* r0, const void* r1)
	{
		Polygon* p0 = *((Polygon**)r0);
		Polygon* p1 = *((Polygon**)r1);
		return signZero(p1->zAvef - p0->zAvef);
	}

}}  // TFE_JediRenderer
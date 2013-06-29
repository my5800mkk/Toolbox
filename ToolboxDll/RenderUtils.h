#ifndef __RENDER_UTILS_H__
#define __RENDER_UTILS_H__

#include <IRenderAuxGeom.h>

struct SRenderUtils
{
	static void DrawBox(Vec2 start, Vec2 size, ColorB color)
	{
		IRenderAuxGeom *pRenderAuxGeom = gEnv->pRenderer->GetIRenderAuxGeom();
		if(pRenderAuxGeom == nullptr)
			return;

		Vec3 qVertices[4];
		qVertices[0] = Vec3(start.x, start.y, 1);
		qVertices[1] = Vec3(start.x + size.x, start.y, 1);
		qVertices[2] = Vec3(start.x, start.y + size.y, 1);
		qVertices[3] = Vec3(start.x + size.x, start.y + size.y, 1);

		const uint16 indices[6] = { 2, 1, 0, 2, 3, 1 };

		pRenderAuxGeom->DrawTriangles(qVertices, 4, indices, 6, color);
	}
};

#endif // __RENDER_UTILS_H__
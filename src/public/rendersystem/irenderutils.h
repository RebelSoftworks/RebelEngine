#ifndef IRENDERUTILS_H
#define IRENDERUTILS_H
#pragma once

#include "irendercontext.h"
#include "irenderdevice.h"

typedef unsigned int OcclusionQueryObjectHandle_t;

class IRenderUtils : public IAppSystem
{
public:
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject();
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t handle);
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t handle);
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t handle);
	virtual bool BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t handle, IRenderContext* context);
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t handle, IRenderContext* context);
};

#endif
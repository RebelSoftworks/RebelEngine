//========= Copyright (c) 2026 Mohamed Ashraf, All rights reserved. ============//
//
// This software is proprietary and confidential.
//
// You are granted a limited, non-exclusive, non-transferable license to use and
// modify this software for personal or internal purposes only. You may distribute
// compiled binary builds of the software provided that such distribution does not
// expose, include, or facilitate access to the source code.
//
// You may not distribute, publish, disclose, or otherwise make available the
// source code, in whole or in part, whether original or modified. All rights
// not expressly granted herein are reserved by the copyright holder.
//
// This software is provided "as is", without warranty of any kind, express or
// implied.
//
//=============================================================================//
#pragma once

#include <vector>
#include <stdint.h>

#include "mathlib/vector.h"
#include "rendersystem/irenderdevice.h"

struct ModelVertex_t
{
    Vector3 position;
    Vector3 normal;
    Vector2 texcoord;
};

struct ModelMesh_t
{
    std::vector<ModelVertex_t> vertices;
    std::vector<uint32_t> indices;

    RenderBufferHandle_t vertexBuffer;
    RenderBufferHandle_t indexBuffer;

    uint32_t vertexCount;
    uint32 indexCount;

    ModelMesh_t()
    {
        vertexBuffer = RenderBufferHandle_t{};
        indexBuffer = RenderBufferHandle_t{};
    }
};

struct Model_t
{
    std::vector<ModelMesh_t> meshes;
};
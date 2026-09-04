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

#include <rendersystem/irenderdevice.h>
#include "tier0/platform.h"
#include "mathlib/vector.h"

#include <cstring>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

extern IRenderDevice* g_Device;

class CSceneSystem : public g_pSceneSystem
{
public:
    void* m_pSelf = nullptr;

    intptr_t CreateWorld(const char* debugName);
    void DestroyWorld(ISceneWorld* world);
};

intptr_t CSceneSystem::CreateWorld(const char* debugName)
{
    ISceneWorld* pWorld = new ISceneWorld;

    SceneWorldData* pData = new SceneWorldData;

    if (debugName)
        pData->debugName = debugName;

    pWorld->m_pSelf =
        pData;

    Msg("CSceneSystem::CreateWorld: %s\n", debugName ? debugName : "<unnamed>" );

    return reinterpret_cast<intptr_t>(pWorld);
}

void CSceneSystem::DestroyWorld(ISceneWorld* pWorld)
{
    if (!pWorld)
        return;

    SceneWorldData* pData =
        static_cast<SceneWorldData*>(
            pWorld->m_pSelf
            );

    delete pData;

    delete pWorld;
}

CSceneSystem* g_pSceneSystem =
new CSceneSystem();

DLL_EXPORT intptr_t SceneSystem_CreateWorld(const char* debugName)
{
    return g_pSceneSystem->CreateWorld(debugName);
}
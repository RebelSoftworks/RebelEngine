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
#include "cbase.h"
#include "source2_gameentities.h"
#include "interfaces.h"

static CSource2GameEntities g_Source2GameEntitiesImpl;
ISource2GameEntities* g_sSource2GameEntities = &g_Source2GameEntitiesImpl;
ISource2GameEntities* g_pSource2ServerGameEntities = g_sSource2GameEntities;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSource2GameEntities, ISource2GameEntities, SOURCE2SERVER_GAME_INTERFACE_VERSION, g_Source2GameEntitiesImpl);

void CSource2GameEntities::CheckTransmit(CCheckTransmitInfo** pInfoInfoList, int nInfoCount, std::bitset<16384>& unionTransmitEdicts, std::bitset<16384>&, const Entity2Networkable_t** pNetworkables, const uint16* pEntityIndicies, int nEntityIndices, bool bEnablePVSBits)
{
}

void CSource2GameEntities::PrepareForFullUpdate(CEntityIndex nPlayerEntityIndex)
{
}

bool CSource2GameEntities::ShouldClientReceiveStringTableUserData(const void* pTable, int stringNumber, const CCheckTransmitInfo* pInfo)
{
	return false;
}

void CSource2GameEntities::ResetChangeAccessorsSerialNumbersToZero()
{
}

bool CSource2GameEntities::GetWorldspaceCenter(CEntityIndex nEntityIndex, Vector4* pCenter) const
{
	return true;
}

void CSource2GameEntities::PrePackEntities(const std::vector<Entity2Networkable_t*>& vecEntities)
{
}

void CSource2GameEntities::AddEntityToSteadyState(const Entity2Networkable_t* pNetworkable)
{
}

void CSource2GameEntities::RemoveEntityFromSteadyState(const Entity2Networkable_t* pNetworkable)
{
}

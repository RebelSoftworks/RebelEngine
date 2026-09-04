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

#include "../../public/game/server/isource2_servergameentities.h"
class CSource2GameEntities : public CBaseAppSystem<ISource2GameEntities>
{
	virtual void			CheckTransmit(CCheckTransmitInfo** pInfoInfoList, int nInfoCount, std::bitset<16384>& unionTransmitEdicts, std::bitset<16384>&, const Entity2Networkable_t** pNetworkables, const uint16* pEntityIndicies, int nEntityIndices, bool bEnablePVSBits) ;
	virtual void			PrepareForFullUpdate(CEntityIndex nPlayerEntityIndex) ;
	virtual bool			ShouldClientReceiveStringTableUserData(const void* pTable, int stringNumber, const CCheckTransmitInfo* pInfo) ;
	virtual void			ResetChangeAccessorsSerialNumbersToZero() ;
	virtual bool			GetWorldspaceCenter(CEntityIndex nEntityIndex, Vector4* pCenter) const ;
	virtual void			PrePackEntities(const std::vector< Entity2Networkable_t* >& vecEntities) ;
	virtual void			AddEntityToSteadyState(const Entity2Networkable_t* pNetworkable) ;
	virtual void			RemoveEntityFromSteadyState(const Entity2Networkable_t* pNetworkable) ;
};
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

#include "mathlib/vector.h"
#include <vector>
#include <bitset>
#include "../../appframework/iappframework.h"
#include "const.h"

struct Entity2Networkable_t;

#define INVALID_ENTITY_INDEX -1

class CEntityIndex
{
public:
	CEntityIndex(int index = INVALID_ENTITY_INDEX) : m_Data(index) {}

	void Invalidate() { m_Data = INVALID_ENTITY_INDEX; }

	int Get() const { return m_Data; }
	operator int() const { return m_Data; }

	bool operator< (int index) const { return m_Data < index; }
	bool operator< (const CEntityIndex& other) const { return m_Data < other.m_Data; }
	bool operator==(const CEntityIndex& other) const { return m_Data == other.m_Data; }
	bool operator!=(const CEntityIndex& other) const { return m_Data != other.m_Data; }

private:
	int m_Data;
};

struct vis_info_t
{
	uint32 m_uVisBitsBufSize;
	uint32 m_SpawnGroupHandle;
	std::bitset<4096> m_VisBits;
};

class CCheckTransmitInfo
{
public:
	std::bitset<MAX_EDICTS>* m_pTransmitEntity; // entities visible/sent to client
	std::bitset<MAX_EDICTS>* m_pTransmitNonPlayers; // non-player entities needing deletion deltas
	std::bitset<MAX_EDICTS>* m_pTransmitOutOfPVS; // entities that left PVS but still need delta update
	std::bitset<MAX_EDICTS>* m_pTransmitHLTV; // HLTV-only extra transmit bitvec (NULL for regular clients)
	vis_info_t m_VisInfo;
	bool m_bFullUpdate = false;
};

class ISource2GameEntities : public IAppSystem
{
public:
	virtual					~ISource2GameEntities() {}
	virtual void			CheckTransmit(CCheckTransmitInfo** pInfoInfoList, int nInfoCount, std::bitset<16384>& unionTransmitEdicts, std::bitset<16384>&, const Entity2Networkable_t** pNetworkables, const uint16* pEntityIndicies, int nEntityIndices, bool bEnablePVSBits) = 0;
	virtual void			PrepareForFullUpdate(CEntityIndex nPlayerEntityIndex) = 0;
	virtual bool			ShouldClientReceiveStringTableUserData(const void* pTable, int stringNumber, const CCheckTransmitInfo* pInfo) = 0;
	virtual void			ResetChangeAccessorsSerialNumbersToZero() = 0;
	virtual bool			GetWorldspaceCenter(CEntityIndex nEntityIndex, Vector4* pCenter) const = 0;
	virtual void			PrePackEntities(const std::vector< Entity2Networkable_t* >& vecEntities) = 0;
	virtual void			AddEntityToSteadyState(const Entity2Networkable_t* pNetworkable) = 0;
	virtual void			RemoveEntityFromSteadyState(const Entity2Networkable_t* pNetworkable) = 0;
};
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
#include "iappframework.h"
#include "platform.h"
#include "globalvars_base.h"
#include "const.h"
#include <mathlib/vector.h>
#include <bitset>

struct CEntityIndex
{
	CEntityIndex(int index)
	{
		_index = index;
	}

	int Get() const
	{
		return _index;
	}

	int _index;

	bool operator==(const CEntityIndex& other) const { return other._index == _index; }
	bool operator!=(const CEntityIndex& other) const { return other._index != _index; }
};

class ISource2Server : public IAppSystem
{
public:
	virtual void			SetGlobals(CGlobalVarsBase* pGlobals) = 0;
	virtual void			GameCreateNetworkStringTables(void) = 0;
	virtual void			WriteSignonMessages(const const char* buf) = 0;
	virtual void			PreWorldUpdate(bool simulating) = 0;
	virtual void*			GetEntity2Networkables(void) const = 0;
	virtual void*			GetEntityInfo() = 0;
	virtual void			ApplyGameSettings(const char* pKV) = 0;
	virtual void			GameFrame(bool simulating, bool bFirstTick, bool bLastTick) = 0;
	virtual bool			ShouldHideFromMasterServer(bool bServerHasPassword) = 0;
	virtual void			GetMatchmakingTags(char* buf, size_t bufSize) = 0;
	virtual void			ServerHibernationUpdate(bool bHibernating) = 0;
	virtual void			GetMatchmakingGameData(const char* buf) = 0;
	virtual bool			ShouldTimeoutClient(int nUserID, float flTimeSinceLastReceived) = 0;
	virtual void			PrintStatus(CEntityIndex nPlayerEntityIndex, const char* output) = 0;
	virtual int				GetServerGameDLLFlags(void) const = 0;
	virtual void			GetTaggedConVarList(const char* pCvarTagList) = 0;
	virtual void*			GetAllServerClasses(void) = 0;
	virtual const char*		GetActiveWorldName(void) const = 0;
	virtual bool			IsPaused(void) const = 0;
	virtual bool			GetNavMeshData(void* pNavMeshData) = 0;
	virtual void			SetNavMeshData(const void* navMeshData) = 0;
	virtual void			RegisterNavListener(void* pNavListener) = 0;
	virtual void			UnregisterNavListener(void* pNavListener) = 0;
	virtual void*			GetSpawnDebugInterface(void) = 0;
	virtual void*			GetToolGameSimulationAPI(void) = 0;
	virtual void			GetAnimationActivityList(void* activityList) = 0;
	virtual void			GetAnimationEventList(void* eventList) = 0;
	virtual void			FilterPlayerCounts(int* pInOutHumans, int* pInOutHumansSlots, int* pInOutBots) = 0;
	virtual void			GameServerSteamAPIActivated(void) = 0;
	virtual void			GameServerSteamAPIDeactivated(void) = 0;
	virtual void			OnHostNameChanged(const char* pHostname) = 0;
	virtual void			PreFatalShutdown(void) const = 0;
	virtual void			UpdateWhenNotInGame(float flFrameTime) = 0;
	virtual void			GetEconItemNamesForModel(const char* pModelName, bool bExcludeItemSets, bool bExcludeIndividualItems, void* econItemNames) = 0;
	virtual void			GetEconItemNamesForCharacter(const char* pCharacterName, bool bExcludeItemSets, bool bExcludeIndividualItems, void* econItemNames) = 0;
	virtual void			GetEconItemsInfoForModel(const char* pModelName, const char* pEconItemName, bool bExcludeItemSets, bool bExcludeIndividualItems, bool bExcludeStockItemSet, void* econInfo) = 0;
	virtual void			GetEconItemsInfoForCharacter(const char* pCharacterName, const char* pEconItemName, bool bExcludeItemSets, bool bExcludeIndividualItems, bool bExcludeStockItemSet, void* econInfo) = 0;
	virtual void			GetDefaultScaleForModel(const char* pModelName, bool bCheckLoadoutScale) = 0;
	virtual void			GetDefaultScaleForCharacter(const char* pCharacterName, bool bCheckLoadoutScale) = 0;
	virtual void			GetDefaultControlPointAutoUpdates(const char* pParticleSystemName, void* autoUpdates) = 0;
	virtual void			GetCharacterNameForModel(const char* pModelName, bool bCheckItemModifiers, const char& characterName) = 0;
	virtual void			GetModelNameForCharacter(const char* pCharacterNamel, int nIndex, const char& modelName) = 0;
	virtual void			GetCharacterList(void* characterNames) = 0;
	virtual void			GetDefaultChoreoDirForModel(const char* pModelName, const char& defaultVCDDir) = 0;
	virtual void*			GetEconItemSystem(void) = 0;
	virtual void			ServerConVarChanged(const char* pVarName, const char* pValue) = 0;
};

#define INVALID_PLAYER_SLOT_INDEX -1
#define INVALID_PLAYER_SLOT CPlayerSlot( INVALID_PLAYER_SLOT_INDEX )

class CPlayerSlot
{
public:
	CPlayerSlot(int slot = INVALID_PLAYER_SLOT_INDEX) : m_Data(slot) {}

	void Invalidate() { m_Data = INVALID_PLAYER_SLOT_INDEX; }
	bool IsValid() const { return m_Data >= 0 && m_Data < ABSOLUTE_PLAYER_LIMIT; }
	int Get() const { return m_Data; }

	CEntityIndex GetEntityIndex() const { return CEntityIndex(m_Data + 1); }
	int GetClientIndex() const { return GetEntityIndex().Get(); }

	static int InvalidSlot() { return INVALID_PLAYER_SLOT; }

	operator int() const { return m_Data; }

	bool operator< (int slot) const { return m_Data < slot; }
	bool operator< (uint32 slot) const { return m_Data < static_cast<int>(slot); }
	bool operator< (const CPlayerSlot& other) const { return m_Data < other.m_Data; }
	bool operator==(const CPlayerSlot& other) const { return m_Data == other.m_Data; }
	bool operator!=(const CPlayerSlot& other) const { return m_Data != other.m_Data; }

	CPlayerSlot& operator++() { ++m_Data; return *this; }
	CPlayerSlot operator++(int) { CPlayerSlot temp = *this; ++m_Data; return temp; }

private:
	int m_Data;
};

struct vis_info_t
{
	uint32 m_uVisBitsBufSize;
	uint32 m_SpawnGroupHandle;
	std::bitset<4096> m_VisBits;
};

class ISource2GameClients : public IAppSystem
{
public:
	virtual void			OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer) = 0;
	virtual bool			ClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, const char* pRejectReason) = 0;
	virtual void			ClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid) = 0;
	virtual void			ClientActive(CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) = 0;
	virtual void			ClientFullyConnect(CPlayerSlot slot) = 0;
	virtual void			ClientDisconnect(CPlayerSlot slot, int reason, const char* pszName, uint64 xuid, const char* pszNetworkID) = 0;
	virtual void			ClientCommand(CPlayerSlot slot, const char* args) = 0;
	virtual void			ClientStringTableData(CPlayerSlot slot, void* pData) = 0;
	virtual void			ClientSettingsChanged(CPlayerSlot slot) = 0;
	virtual void			ClientSetupVisibility(CPlayerSlot slot, vis_info_t* visinfo) = 0;
	virtual void			ProcessUsercmds(CPlayerSlot slot, const void* msg, bool paused) = 0;
	virtual bool			IsPlayerSlotOccupied(CPlayerSlot slot) = 0;
	virtual bool			IsPlayerAlive(CPlayerSlot slot) = 0;
	virtual int				GetPlayerScore(CPlayerSlot slot) = 0;
	virtual void			ClientEarPosition(CPlayerSlot slot, Vector4* pEarOrigin) = 0;
	virtual void			GetBugReportInfo(const char* buf) = 0;
	virtual void			ClientVoice(CPlayerSlot slot) = 0;
	virtual void			NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) = 0;
	virtual void			ClientCommandKeyValues(CPlayerSlot slot, const char* pKeyValues) = 0;
	virtual bool			IsGamePausable() = 0;
	virtual bool			ClientCanPause(CPlayerSlot slot) = 0;
	virtual uint32			GetClientTickCount(CPlayerSlot slot) = 0;
	virtual void			GetClientVisibilityInfo(CPlayerSlot slot, vis_info_t* pOutVisInfo) = 0;
	virtual void			ClientSvcUserMessage(CPlayerSlot slot, int um_type, uint32 size, const void* buf) = 0;
	virtual int				GetClientViewEntity(CPlayerSlot slot, CEntityIndex* outViewEntity) = 0;
	virtual bool			ProcessClientVoiceData(CPlayerSlot slot, void* pVoiceInfo) = 0;
	virtual bool			ValidateClientString(const char* pszCurrent, const char* pszExpected) = 0;
	virtual bool			CanProcessNetMessage(void* pNetMessage, void* pClient) = 0;
	virtual bool			ValidateScriptCommands(const char* pszCommandText, const char* pFilteredOutput) = 0;
};

class CSource2GameClients : public CBaseAppSystem<ISource2GameClients>
{
public:
	virtual void			OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress, bool bFakePlayer) {}
	virtual bool			ClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, const char* pRejectReason)
	{
		return true;
	}
	virtual void			ClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid) {}
	virtual void			ClientActive(CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) {}
	virtual void			ClientFullyConnect(CPlayerSlot slot) {}
	virtual void			ClientDisconnect(CPlayerSlot slot, int reason, const char* pszName, uint64 xuid, const char* pszNetworkID) {}
	virtual void			ClientCommand(CPlayerSlot slot, const char* args) {}
	virtual void			ClientStringTableData(CPlayerSlot slot, void* pData) {}
	virtual void			ClientSettingsChanged(CPlayerSlot slot) {}
	virtual void			ClientSetupVisibility(CPlayerSlot slot, vis_info_t* visinfo) {}
	virtual void			ProcessUsercmds(CPlayerSlot slot, const void* msg, bool paused) {}
	virtual bool			IsPlayerSlotOccupied(CPlayerSlot slot)
	{
		return false;
	}
	virtual bool			IsPlayerAlive(CPlayerSlot slot)
	{
		// TBD
		return true;
	}
	virtual int				GetPlayerScore(CPlayerSlot slot)
	{
		return 1;
	}
	virtual void			ClientEarPosition(CPlayerSlot slot, Vector4* pEarOrigin) {}
	virtual void			GetBugReportInfo(const char* buf) {}
	virtual void			ClientVoice(CPlayerSlot slot) {}
	virtual void			NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) {}
	virtual void			ClientCommandKeyValues(CPlayerSlot slot, const char* pKeyValues) {}
	virtual bool			IsGamePausable()
	{
		return false;
	}
	virtual bool			ClientCanPause(CPlayerSlot slot)
	{
		return false;
	}
	virtual uint32			GetClientTickCount(CPlayerSlot slot)
	{
		return 11;
	}
	virtual void			GetClientVisibilityInfo(CPlayerSlot slot, vis_info_t* pOutVisInfo) {}
	virtual void			ClientSvcUserMessage(CPlayerSlot slot, int um_type, uint32 size, const void* buf) {}
	virtual int				GetClientViewEntity(CPlayerSlot slot, CEntityIndex* outViewEntity)
	{
		return 1;
	}
	virtual bool			ProcessClientVoiceData(CPlayerSlot slot, void* pVoiceInfo)
	{
		return true;
	}
	virtual bool			ValidateClientString(const char* pszCurrent, const char* pszExpected)
	{
		return true;
	}
	virtual bool			CanProcessNetMessage(void* pNetMessage, void* pClient)
	{
		return true;
	}
	virtual bool			ValidateScriptCommands(const char* pszCommandText, const char* pFilteredOutput)
	{
		return true;
	}
};
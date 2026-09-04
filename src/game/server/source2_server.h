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
#include "appframework/iappframework.h"
#include "../../public/game/server/isource2_server.h"

// the original server dll we are built on
ISource2Server* g_pServerDll = nullptr;

class CSource2Server : public CBaseAppSystem<ISource2Server> {
public:

	virtual void Init(CreateInterfaceFn factory);
	virtual bool Connect(CreateInterfaceFn factory);
	virtual void Disconnect();
	virtual void Shutdown(long long par1);
// methods of ISource2Server:
public:
	virtual void			SetGlobals(CGlobalVarsBase* pGlobals) {}
	virtual void			GameCreateNetworkStringTables(void) {}
	virtual void			WriteSignonMessages(const const char* buf) {}
	virtual void			PreWorldUpdate(bool simulating) {}
	virtual void*			GetEntity2Networkables(void) const
	{
		return (int*)10;
	}
	virtual void*			GetEntityInfo()
	{
		return (int*)1;
	}
	virtual void			ApplyGameSettings(const char* pKV) {}
	virtual void			GameFrame(bool simulating, bool bFirstTick, bool bLastTick) {}
	virtual bool			ShouldHideFromMasterServer(bool bServerHasPassword)
	{
		return false;
	}
	virtual void			GetMatchmakingTags(char* buf, size_t bufSize) {}
	virtual void			ServerHibernationUpdate(bool bHibernating) {}
	virtual void			GetMatchmakingGameData(const char* buf) {}
	virtual bool			ShouldTimeoutClient(int nUserID, float flTimeSinceLastReceived)
	{
		return false;
	}
	virtual void			PrintStatus(CEntityIndex nPlayerEntityIndex, const char* output) {}
	virtual int				GetServerGameDLLFlags(void) const
	{
		// TBD
		return 0;
	}
	virtual void			GetTaggedConVarList(const char* pCvarTagList) {}
	virtual void*			GetAllServerClasses(void)
	{
		return (int*)107;
	}
	virtual const char*		GetActiveWorldName(void) const
	{
		return "";
	}
	virtual bool			IsPaused(void) const
	{
		return false;
	}
	virtual bool			GetNavMeshData(void* pNavMeshData)
	{
		return true;
	}
	virtual void			SetNavMeshData(const void* navMeshData) {}
	virtual void			RegisterNavListener(void* pNavListener) {}
	virtual void			UnregisterNavListener(void* pNavListener) {}
	virtual void*			GetSpawnDebugInterface(void)
	{
		return NULL;
	}
	virtual void*			GetToolGameSimulationAPI(void)
	{
		return NULL;
	}
	virtual void			GetAnimationActivityList(void* activityList) {}
	virtual void			GetAnimationEventList(void* eventList) {}
	virtual void			FilterPlayerCounts(int* pInOutHumans, int* pInOutHumansSlots, int* pInOutBots) {}
	virtual void			GameServerSteamAPIActivated(void) {}
	virtual void			GameServerSteamAPIDeactivated(void) {}
	virtual void			OnHostNameChanged(const char* pHostname) {}
	virtual void			PreFatalShutdown(void) const {}
	virtual void			UpdateWhenNotInGame(float flFrameTime) {}
	virtual void			GetEconItemNamesForModel(const char* pModelName, bool bExcludeItemSets, bool bExcludeIndividualItems, void* econItemNames) {}
	virtual void			GetEconItemNamesForCharacter(const char* pCharacterName, bool bExcludeItemSets, bool bExcludeIndividualItems, void* econItemNames) {}
	virtual void			GetEconItemsInfoForModel(const char* pModelName, const char* pEconItemName, bool bExcludeItemSets, bool bExcludeIndividualItems, bool bExcludeStockItemSet, void* econInfo) {}
	virtual void			GetEconItemsInfoForCharacter(const char* pCharacterName, const char* pEconItemName, bool bExcludeItemSets, bool bExcludeIndividualItems, bool bExcludeStockItemSet, void* econInfo) {}
	virtual void			GetDefaultScaleForModel(const char* pModelName, bool bCheckLoadoutScale) {}
	virtual void			GetDefaultScaleForCharacter(const char* pCharacterName, bool bCheckLoadoutScale) {}
	virtual void			GetDefaultControlPointAutoUpdates(const char* pParticleSystemName, void* autoUpdates) {}
	virtual void			GetCharacterNameForModel(const char* pModelName, bool bCheckItemModifiers, const char& characterName) {}
	virtual void			GetModelNameForCharacter(const char* pCharacterNamel, int nIndex, const char& modelName) {}
	virtual void			GetCharacterList(void* characterNames) {}
	virtual void			GetDefaultChoreoDirForModel(const char* pModelName, const char& defaultVCDDir) {}
	virtual void*			GetEconItemSystem(void)
	{
		// Econ slop...
		return NULL;
	}
	virtual void			ServerConVarChanged(const char* pVarName, const char* pValue) {}
};
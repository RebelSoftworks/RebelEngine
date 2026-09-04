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
#include "appframework/iappframework.h"
#include "platform.h"
#include "const.h"
#include "interfaces.h"
#include <steam/steamuniverse.h>

enum HostStateLoopModeType_t
{
    HOST_STATE_LOOP_MODE_IDLE = 0,
    HOST_STATE_LOOP_MODE_GAME,
    HOST_STATE_LOOP_MODE_SOURCETV_RELAY,

    HOST_STATE_LOOP_MODE_COUNT
};

class ISource2ServerConfig : public IAppSystem
{
public:
    virtual const char* GetGameDescription(void) = 0;
    virtual int			GetNetworkVersion(void) = 0;
    virtual float		GetTickInterval(void) const = 0;
    virtual void		GetPlayerLimits(int& minplayers, int& maxplayers, int& defaultMaxPlayers, bool& bIsMultiplayer) const = 0;
    virtual int		    GetMaxSplitscreenPlayers(void) = 0;
    virtual int			GetMaxHumanPlayers() = 0;
    virtual bool		ShouldNotifyLocalClientConnectionStateChanges() = 0;
    virtual bool		AllowPlayerToTakeOverBots() = 0;
    virtual void		OnClientFullyConnect(CEntityIndex nEntityIndex) = 0;
    virtual void		GetHostStateLoopModeInfo(HostStateLoopModeType_t type, const char& loopModeName, const char** ppLoopModeOptions) = 0;
    virtual bool		AllowDedicatedServers(EUniverse universe) const = 0;
    virtual void		GetConVarPrefixesToResetToDefaults(const char& sSemicolonDelimitedPrefixList) const = 0;
    virtual bool		AllowSaveRestore() = 0;
};

class CSource2ServerConfig : public ISource2ServerConfig {
public:
    virtual const char* GetGameDescription(void)
    {
        // AFAIK this isn't a needed function, so we can return anything
        return "Dispersion";
    }

    virtual int			GetNetworkVersion(void)
    {
        return 1; // ???
    }
    virtual float		GetTickInterval(void) const
    {
        return 11;
    }

    virtual void		GetPlayerLimits(int& minplayers, int& maxplayers, int& defaultMaxPlayers, bool& bIsMultiplayer) const
    {
        minplayers = defaultMaxPlayers = 1;
        maxplayers = MAX_PLAYERS;
    }
    virtual int		    GetMaxSplitscreenPlayers(void)
    {
        return MAX_PLAYERS;
    }
    virtual int			GetMaxHumanPlayers()
    {
        return MAX_PLAYERS;
    }
    virtual bool		ShouldNotifyLocalClientConnectionStateChanges()
    {
        return false;
    }
    virtual bool		AllowPlayerToTakeOverBots()
    {
        return false;
    }
    virtual void		OnClientFullyConnect(CEntityIndex nEntityIndex) {}
    virtual void		GetHostStateLoopModeInfo(HostStateLoopModeType_t type, const char& loopModeName, const char** ppLoopModeOptions) {}
    virtual bool		AllowDedicatedServers(EUniverse universe) const
    {
        // Not a multiplayer game, so this really doesn't matter
        return false;
    }
    virtual void		GetConVarPrefixesToResetToDefaults(const char& sSemicolonDelimitedPrefixList) const {}
    virtual bool		AllowSaveRestore() { return false; }
};

class CDefaultGameConfig : public CBaseAppSystem<CSource2ServerConfig> {
public:


};
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
#include "source2_server.h"
#include "../../public/game/server/isource2_serverconfig.h"
#include "interfaces.h"

CDefaultGameConfig g_sDefualtClientConfig;
ISource2ServerConfig* g_pSource2ServerConfig = &g_sDefualtClientConfig;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CDefaultGameConfig, ISource2ServerConfig, SOURCE2SERVERCONFIG_INTERFACE_VERSION, g_sDefualtClientConfig);

CSource2GameClients g_sGameClients;
ISource2GameClients* g_pGameClients = &g_sGameClients;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSource2GameClients, ISource2GameClients, SOURCE2SERVERGAMECLIENTS_INTERFACE_VERSION, g_sGameClients)

CSource2Server g_sSource2Server;
ISource2Server* g_pSource2Server = &g_sSource2Server;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION, g_sSource2Server)

bool CSource2Server::Connect(CreateInterfaceFn factory) {

	Msg("CSource2Server::Connect -- Connected\n");

	return true;
}

void CSource2Server::Disconnect() {

}

void CSource2Server::Init(CreateInterfaceFn factory) {

	Msg("CSource2Server::Init -- Begin\n");

	Msg("CSource2Server::Init -- End\n");

	return;
}

void CSource2Server::Shutdown(long long par1) {

	Msg("CSource2Server::Shutdown");

}

// stub here:
ISource2Client* g_pSource2Client = nullptr;
ISource2ClientConfig* g_pSource2ClientConfig = nullptr;
IPrediction2* g_pSource2ClientPrediction = nullptr;
IRenderDevice* g_Device = nullptr;
IRenderDeviceMgr* g_pRenderDeviceManager = nullptr;
IRenderUtils* g_pRenderUtils = nullptr;
IInputSystem* g_pInputSystem = nullptr;
IInputStackSystem* g_pInputStackSystem = nullptr;

DLL_EXPORT bool BSecureAllowed(unsigned char* a1, int a2, int a3)
{
	return true;
}

DLL_EXPORT int BinaryProperties_GetValue(int a1, void* a2)
{
	return 0;
}

DLL_EXPORT int CountFilesCompletedTrustCheck()
{
	return 0;
}

DLL_EXPORT int CountFilesNeedTrustCheck()
{
	return 0;
}

DLL_EXPORT int CountItemsToReport()
{
	return 0;
}

DLL_EXPORT int GetTotalFilesLoaded()
{
	return 0;
}
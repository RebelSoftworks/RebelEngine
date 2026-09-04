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

#include "engineservicemgr.h"

CEngineServiceMgr g_sEngineServiceMgr;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEngineServiceMgr, IEngineServiceMgr, "EngineServiceMgr001", g_sEngineServiceMgr);

CEngineServiceMgr::CEngineServiceMgr()
{
} 

CEngineServiceMgr::~CEngineServiceMgr()
{
}

bool CEngineServiceMgr::Connect(CreateInterfaceFn factory)
{
	return true;
}

InitReturnVal_t CEngineServiceMgr::Init()
{
	return INIT_OK;
} 

void CEngineServiceMgr::CheapHeapValidity(const char* pTag) const
{
} 

void CEngineServiceMgr::Shutdown()
{	
}

void SetEngineState(PlatWindow_t hWnd, SwapChainHandle_t hSwapChain)
{
}

void ResetEngineState(void)
{
}

PlatWindow_t CEngineServiceMgr::GetEngineWindow()
{
	return (PlatWindow_t)1;
}

SwapChainHandle_t CEngineServiceMgr::GetEngineSwapChain()
{
	return (SwapChainHandle_t)1;
}

void CEngineServiceMgr::GetSwapChainSize(int* pWidth, int* pHeight)
{

}

void CEngineServiceMgr::GetEngineSwapChainSize(int* pWidth, int* pHeight)
{
	GetSwapChainSize(pWidth, pHeight);
} 

void CEngineServiceMgr::RegisterEngineService(const char* pName, IEngineServiceMgr* pService)
{
}

void CEngineServiceMgr::UnregisterEngineService(const char* pName, IEngineServiceMgr* pService)
{
}

CEngineServiceMgr::LoopRequest_t::~LoopRequest_t()
{
}

void CEngineServiceMgr::DiscardLoopRequest(LoopRequest_t** ppRequest, bool bSuccess)
{
} 

void CEngineServiceMgr::ExitMainLoop()
{
}

void CEngineServiceMgr::ClearPrerequisites()
{
} 

void CEngineServiceMgr::CleanupQueuedLoop()
{
}

void CEngineServiceMgr::SerializeAsyncFileReads()
{
}

void CEngineServiceMgr::SetGameWatchdogShutdownTime(int nSecondsFromNow, const char* pszReason, void (*pCallback)(void))
{
}

MainLoopResult_t CEngineServiceMgr::MainLoop(float flCurrTime, float flPrevTime)
{
	return (MainLoopResult_t)1;
} 

void CEngineServiceMgr::PrintStatus()
{
}
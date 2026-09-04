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

#include "../public/rendersystem/irendercontext.h"
#include "../public/interfaces/interfaces.h"
#include <iappframework.h>
#include "../public/tier0/types.h"

DECLARE_HANDLE(PlatWindow_t);

enum  MainLoopResult_t
{

};

class IEngineServiceMgr {
	virtual bool Connect(CreateInterfaceFn) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
};

class CEngineServiceMgr : public IEngineServiceMgr
{
public:
    struct LoopRequest_t
    {
        LoopRequest_t();
        ~LoopRequest_t();

        const char* m_LoopName = nullptr;
        int         m_nLoopIndex = -1;
        bool        m_bIsIdleLoop = false;
        uint32      m_unRequestId = 0;
    };

    enum ExitState_t
    {
        NOT_EXITING = 0,
        EXIT_PENDING,
        EXITING
    };

public:
    CEngineServiceMgr();
    CEngineServiceMgr(const CEngineServiceMgr&);
    ~CEngineServiceMgr();

    bool Connect(CreateInterfaceFn factory) ;
    InitReturnVal_t Init() ;
    void Shutdown() ;

    void RegisterEngineService(const char* name, IEngineServiceMgr* service) ;
    void UnregisterEngineService(const char* name, IEngineServiceMgr* service) ;

    const char* GetActiveLoopName() ;
    IEngineServiceMgr* FindService(const char* name) ;

    void ExitMainLoop() ;

    PlatWindow_t GetEngineWindow() ;
    SwapChainHandle_t GetEngineSwapChain() ;
    void GetEngineSwapChainSize(int* width, int* height) ;

    void PrintStatus() ;

    void InstallSwitchLoopModeStatusNotify() ;
    void UninstallSwitchLoopModeStatusNotify() ;

    const char* LookupLocalizationToken(const char* token) ;

    void SetGameWatchdogShutdownTime(
        int timeout,
        const char* reason,
        void (*callback)()) ;

public:
    void SetEngineState(
        PlatWindow_t window,
        SwapChainHandle_t swapChain);

    void ResetEngineState();

    void GetSwapChainSize(
        int* width,
        int* height);

    MainLoopResult_t MainLoop(
        float frameTime,
        float realTime);

    MainLoopResult_t _MainLoop(
        float frameTime,
        float realTime);

private:
    void ActivateServicesForLoop(const char* loopName);

    void CleanupQueuedLoop();
    void ClearPrerequisites();

    void CheapHeapValidity(const char* context) const;

    void SerializeAsyncFileReads();

    void DiscardLoopRequest(
        LoopRequest_t** request,
        bool deleteKeyValues);

private:
    int             m_nCurrentLoopIndex = -1;

    LoopRequest_t* m_pRequestedLoop = nullptr;
    LoopRequest_t* m_pQueuedLoop = nullptr;

    int             m_nUnsatisfiedPrerequisiteCount = 0;
    bool            m_bStatusChanged = false;

    ExitState_t     m_queuedExitState = NOT_EXITING;

    bool            m_bCheckHeap = false;
};
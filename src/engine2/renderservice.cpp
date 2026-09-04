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
#include "renderservice.h"

CRenderService g_RenderService;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CRenderService, IRenderService, "RenderService_001", g_RenderService);

CRenderService::CRenderService()
    : m_nMultisampleType(sm_nMultisampleType)
{
}

CRenderService::~CRenderService()
{
}

void CRenderService::SetInitialMultisampleType(RenderMultisampleType_t type)
{
    sm_nMultisampleType = type;
    m_nMultisampleType = type;
}

EngineServiceActivateType_t CRenderService::ShouldActivate(const char* pLoop)
{
    return EngineServiceActivateType_t{};
}

InitReturnVal_t CRenderService::Init()
{
    m_nMultisampleType = sm_nMultisampleType;
    return INIT_OK;
}

void CRenderService::Shutdown()
{
}

void CRenderService::SetVideoMode(const RenderDeviceInfo_t& info)
{
    m_nMultisampleType = sm_nMultisampleType;

    OnVideoModeChanged(
        SwapChainHandle_t{},
        info,
        m_nMultisampleType);
}

RenderMultisampleType_t CRenderService::GetMultisampleType() const
{
    return m_nMultisampleType;
}

void CRenderService::OnVideoModeChanged(
    SwapChainHandle_t hSwapChain,
    const RenderDeviceInfo_t& deviceInfo,
    RenderMultisampleType_t multisampleType)
{
    m_nMultisampleType = multisampleType;
}

void CRenderService::OnFrameBoundary(const EventFrameBoundary_t& event)
{
    (void)event;
}

void CRenderService::OnClientOutput(const EventClientOutput_t& event)
{
    (void)event;
}
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

#include "rendersystem/irenderdevice.h"

struct EventFrameBoundary_t
{
public:
    float m_flFrameTime;    
};

enum class RenderMultisampleType_t
{
    RENDER_MULTISAMPLE_INVALID = 0xff,
    RENDER_MULTISAMPLE_NONE = 0x0,
    RENDER_MULTISAMPLE_2X = 0x1,
    RENDER_MULTISAMPLE_4X = 0x2,
    RENDER_MULTISAMPLE_6X = 0x3,
    RENDER_MULTISAMPLE_8X = 0x4,
    RENDER_MULTISAMPLE_16X = 0x5,
    RENDER_MULTISAMPLE_TYPE_COUNT = 0x6,
};

enum EngineServiceActivateType_t
{
};

class IRenderService {
};

struct EventClientOutput_t
{
public:
    float m_flRenderTime;       
    float m_flRealTime;         
    float m_flRenderFrameTimeUnbounded;         
    bool m_bRenderOnly;
    uint8_t _pad_[0x1];
};

class CRenderService : public IRenderService
{
public:
    CRenderService();
    CRenderService(const CRenderService&) = delete;
    virtual ~CRenderService();

    void SetInitialMultisampleType(RenderMultisampleType_t type);

    virtual EngineServiceActivateType_t ShouldActivate(const char* pLoop) ;
    virtual InitReturnVal_t Init() ;
    virtual void Shutdown() ;

    virtual void SetVideoMode(const RenderDeviceInfo_t& info) ;
    virtual RenderMultisampleType_t GetMultisampleType() const ;

    void OnVideoModeChanged(
        SwapChainHandle_t hSwapChain,
        const RenderDeviceInfo_t& deviceInfo,
        RenderMultisampleType_t multisampleType);

    void OnFrameBoundary(const EventFrameBoundary_t& event);
    void OnClientOutput(const EventClientOutput_t& event);

    RenderMultisampleType_t m_nMultisampleType;

    RenderMultisampleType_t sm_nMultisampleType;
};
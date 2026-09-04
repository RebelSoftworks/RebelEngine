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

#include "../rendersystem/irenderdevice.h"
#define GAMEUI_INTERFACE_VERSION "GameUI001"

class IGameUI
{
public:
    virtual ~IGameUI() {}

    virtual bool Init(HWND HWND, IRenderDevice* pDevice) = 0;
    virtual void Shutdown() = 0;
    virtual void Draw() = 0;
    
    virtual bool LoadLayout(const char* pFilename) = 0;
    virtual void BeginFrame(float width, float height) = 0;
    virtual void Render(IRenderContext* pContext) = 0;
    virtual void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual void EndFrame() = 0;
};
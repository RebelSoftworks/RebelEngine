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

#include "inputservice.h"

class CInputService g_sInputService;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CInputService, IInputService, "EngineServiceMgr001", g_sInputService);

CInputService::KeyContext_t::KeyContext_t()
{
}

CInputService::KeyContext_t::~KeyContext_t()
{
}

CInputService::CInputService()
{
}

InitReturnVal_t CInputService::Init()
{
    return INIT_OK;
}

void CInputService::Shutdown()
{
}

bool CInputService::IsAppActive()
{
    return true;
}

void CInputService::InsertCommand(const char* pCommand)
{
}

bool CInputService::ReadKeyBindings()
{
    return false;
}

void CInputService::SetBinding(ButtonCode_t button, const char* pBinding, bool bUser)
{
}

const char* CInputService::GetBinding(ButtonCode_t button)
{
    return nullptr;
}

const char* CInputService::Key_NameForBinding(const char* pBinding, int start, int direction)
{
    return nullptr;
}

bool CInputService::HasMouseFocus()
{
    return true;
}

void CInputService::EnableKeyBindingSystem(bool bEnable)
{
}

void CInputService::SaveKeyBindings()
{
}

void CInputService::UnbindAllKeys()
{
}

void CInputService::SaveConvarState()
{
}

void CInputService::GetCursorPosition(int* x, int* y)
{
    if (x)
        *x = 0;

    if (y)
        *y = 0;
}

void CInputService::SetCursorPosition(int x, int y)
{
}

void CInputService::Pump()
{
}

bool CInputService::FindBindingSubstringMatch(char* pBinding, const char* pSubstring)
{
    return false;
}

void CInputService::SaveKeyBindingsToFile()
{
}

int CInputService::FindInputValue(const char* pName)
{
    return -1;
}

CInputService::BindingInfo_t& CInputService::GetBindingInfo(ButtonCode_t button)
{
    static BindingInfo_t dummy{};
    return dummy;
}

const CInputService::BindingInfo_t& CInputService::GetBindingInfo(ButtonCode_t button) const
{
    static BindingInfo_t dummy{};
    return dummy;
}

void CInputService::OnProfileStorageAvailable()
{
}

void CInputService::OnAppShutdown()
{
}
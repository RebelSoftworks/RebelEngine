
//
// engine2/inputservice.h
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

class IInputService
{
};

enum ButtonCode_t
{
    KEY_SPACE = 65,
    KEY_ESCAPE = 70,
    KEY_UP = 88,
    KEY_LEFT,
    KEY_DOWN,
    KEY_RIGHT,
    MOUSE_LEFT = 107
};

typedef ButtonCode_t MouseCode;
typedef ButtonCode_t KeyCode;

class CInputService : public IInputService
{
public:
    struct BindingInfo_t
    {
        char* m_pBinding = nullptr;
        bool m_bBindingSetByUser : 1;
    };

    struct KeyContext_t
    {
        KeyContext_t();
        ~KeyContext_t();

        BindingInfo_t m_pKeyInfo[369];
    };

public:
    CInputService();
    CInputService(const CInputService&) = delete;

    // IInputService
    InitReturnVal_t Init();
    void Shutdown();

    bool IsAppActive();
    void InsertCommand(const char* pCommand);

    bool ReadKeyBindings();

    void SetBinding(ButtonCode_t button, const char* pBinding, bool bUser);
    const char* GetBinding(ButtonCode_t button);
    const char* Key_NameForBinding(const char* pBinding, int start, int direction);

    bool HasMouseFocus();

    void EnableKeyBindingSystem(bool bEnable);

    void SaveKeyBindings();
    void UnbindAllKeys();
    void SaveConvarState();

    void GetCursorPosition(int* x, int* y);
    void SetCursorPosition(int x, int y);

    void Pump();

private:
    bool FindBindingSubstringMatch(char* pBinding, const char* pSubstring);

    void SaveKeyBindingsToFile();

    int FindInputValue(const char* pName);

    BindingInfo_t& GetBindingInfo(ButtonCode_t button);
    const BindingInfo_t& GetBindingInfo(ButtonCode_t button) const;

    void OnProfileStorageAvailable();
    void OnAppShutdown();
};
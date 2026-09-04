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
#include "isource2_clientconfig.h"
#include "interfaces.h"

class CSource2ClientConfig : public ISource2ClientConfig {
public:

    virtual byte GetMaxSplitscreenPlayers() { return 1; }
    virtual void FUN_1011e5ab0() { return; }
    virtual byte FUN_1011ec470() { return 0; }

    virtual undefined8 FUN_1011e7f50() { return 1; }

    virtual void FUN_1011ed080() {}
    virtual void FUN_1011eb5b0() {}

    virtual undefined8 FUN_1011e68c0() { return 0; }
    virtual undefined8 FUN_1011e6c40() { return 6; }

    virtual void FUN_1011ec620(undefined8 par1, undefined8 par2, undefined8 par3) { return; }

    virtual byte FUN_1011e8830() { return 1; }
    virtual undefined8 FUN_1011e6890() { return 1; }
    virtual undefined8 FUN_1011e6a40() { return 4; }

    virtual void FUN_1011e39e0(undefined8 par1, undefined8 par2, undefined8 par3, unsigned int par4) { return; }
    virtual undefined4* FUN_1011e68e0(undefined8 par1, undefined4* par2) { *par2 = 0x1afdd6d2; return par2; }

    virtual byte FUN_1011e8820() { return 1; }
    virtual byte FUN_1011e8810() { return 0; }

};

class CDefaultGameConfig : public CBaseAppSystem<CSource2ClientConfig> {
public:


};
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
#include "prediction2.h"
#include "interfaces.h"

CPrediction2 g_sPrediction2;
IPrediction2* g_pSource2ClientPrediction = &g_sPrediction2;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CPrediction2, IPrediction2, SOURCE2CLIENTPREDICTION_INTERFACE_VERSION, g_sPrediction2);

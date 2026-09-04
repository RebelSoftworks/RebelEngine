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

#include "vmodel.h"

class IRenderDevice;

class IModelLoader
{
public:
    virtual ~IModelLoader()
    {
    }

    virtual bool Load(
        const char* filename,
        Model_t& model) = 0;

    virtual bool Upload(
        Model_t& model,
        IRenderDevice* device) = 0;

    virtual void Destroy(
        Model_t& model,
        IRenderDevice* device) = 0;
};
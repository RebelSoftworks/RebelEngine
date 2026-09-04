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
typedef enum
{
	BITBUFERROR_VALUE_OUT_OF_RANGE = 0,
	BITBUFERROR_BUFFER_OVERRUN,

	BITBUFERROR_NUM_ERRORS
} BitBufErrorType;

#include <array>
#include <string>
#include <unordered_set>
#include <assert.h>

void Source2BitBufErrorHandler(BitBufErrorType errorType, const char* pDebugName)
{
    if (!pDebugName)
    {
        pDebugName = "(unknown)";
    }

    static std::array<std::unordered_set<std::string>, BITBUFERROR_NUM_ERRORS> errorNames;

    auto& table = errorNames[static_cast<size_t>(errorType)];

    if (table.emplace(pDebugName).second)
    {
        switch (errorType)
        {
        case BITBUFERROR_VALUE_OUT_OF_RANGE:
            std::printf("Error in bitbuf [%s]: out of range value. Debug in bitbuf_errorhandler.cpp\n", pDebugName);
            break;

        case BITBUFERROR_BUFFER_OVERRUN:
            std::printf("Error in bitbuf [%s]: buffer overrun. Debug in bitbuf_errorhandler.cpp\n", pDebugName);
            break;

        default:
            break;
        }
    }

    assert(false, "%s: %s errorType: %d", __FUNCTION__, pDebugName, errorType);
}

void InstallBitBufErrorHandler(void)
{
	Source2BitBufErrorHandler(BITBUFERROR_VALUE_OUT_OF_RANGE, "InstallBitBufErrorHandler");
}


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
#include "interfaces/interfaces.h"

struct AppSystemInfo_t
{
	const char* m_pModuleName;
	const char* m_pInterfaceName;
};

enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,

	INIT_LAST_VAL
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,
	APP_SYSTEM_TIER4,
	APP_SYSTEM_TIER5,

	APP_SYSTEM_TIER_OTHER
};

enum AppSystemBuildType_t
{
	APP_SYSTEM_BUILD_UNKNOWN = -1,
	APP_SYSTEM_BUILD_DEBUG = 0,
	APP_SYSTEM_BUILD_RELEASE,
	APP_SYSTEM_BUILD_RETAIL,
	APP_SYSTEM_BUILD_PROFILE,
	APP_SYSTEM_BUILD_MIXED,
	APP_SYSTEM_BUILD_MIXED_DEBUG,

	APP_SYSTEM_BUILD_COUNT
};

class IAppSystem {
public:
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;
	virtual void* QueryInterface(const char* pInterfaceName) = 0;

	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
	virtual void PreShutdown() = 0;

	virtual const AppSystemInfo_t* GetDependencies() { return nullptr; }
	virtual AppSystemTier_t GetTier() { return APP_SYSTEM_TIER_OTHER; }

	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName) {}
	virtual bool IsSingleton() { return true; }
	virtual AppSystemBuildType_t GetBuildType() { return APP_SYSTEM_BUILD_RETAIL; } // new for source 2
};

template <class IInterface>
class CBaseAppSystem : public IAppSystem, public IInterface {
public:

	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory) { return true; }
	virtual void Disconnect() {}

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface(const char* pInterfaceName) { return nullptr; }

	// Init, shutdown
	virtual InitReturnVal_t Init() { return INIT_OK; }
	virtual void Shutdown() {}
	virtual void PreShutdown() {}

	virtual const AppSystemInfo_t* GetDependencies() { return nullptr; }
	virtual AppSystemTier_t GetTier() { return APP_SYSTEM_TIER_OTHER; }

	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName)
	{
		ReconnectInterface(factory, pInterfaceName);
	}
};

class CMaterialSystem2AppSystemDict 
{
public:
	void* m_unk1168;
	void* m_hSwapChain;
	int m_nThreadCount;
	CreateInterfaceFn m_RenderFactory;
	const char* m_WindowTitle;
	const char* m_WindowIcon;
	const char* m_DefaultRenderSystemOption;
	bool m_unk1224;
	const char* m_WindowInitialImage;
	bool m_bUnk1240;
	bool m_bInitWithoutMaterialSystem;
	bool m_bEnableExtendedMaterialInfo;
	bool m_bDefaultToRendersystemEmpty;
	bool m_bDiscardMouseFocusClick;
	bool m_bRenderSystemOptionOnCommandLine;
};
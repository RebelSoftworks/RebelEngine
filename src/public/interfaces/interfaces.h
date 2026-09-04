//===== Copyright � 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef INTERFACES_H
#define INTERFACES_H

#if defined( COMPILER_MSVC )
#pragma once
#endif

#include "tier0/interface.h"

// All interfaces derive from this.
class IBaseInterface
{
public:
	virtual	~IBaseInterface() {}
};

typedef void* (*InstantiateInterfaceFn)();

// Used internally to register classes.
class InterfaceReg
{
public:
	InterfaceReg(InstantiateInterfaceFn fn, const char *pName);

public:
	InstantiateInterfaceFn	m_CreateFn;
	const char				*m_pName;

	InterfaceReg			*m_pNext; // For the global list.
};

// interface return status
enum
{
	IFACE_OK = 0,
	IFACE_FAILED
};

//-----------------------------------------------------------------------------
// Fills out global DLL exported interface pointers
//-----------------------------------------------------------------------------
void ConnectInterfaces(CreateInterfaceFn* pFactoryList, int nFactoryCount);
void DisconnectInterfaces();

//-----------------------------------------------------------------------------
// Reconnects an interface
//-----------------------------------------------------------------------------
void ReconnectInterface(CreateInterfaceFn factory, const char* pInterfaceName);

// Use this to expose an interface that can have multiple instances.
// e.g.:
// EXPOSE_INTERFACE( CInterfaceImp, IInterface, "MyInterface001" )
// This will expose a class called CInterfaceImp that implements IInterface (a pure class)
// clients can receive a pointer to this class by calling CreateInterface( "MyInterface001" )
//
// In practice, the shared header file defines the interface (IInterface) and version name ("MyInterface001")
// so that each component can use these names/vtables to communicate
//
// A single class can support multiple interfaces through multiple inheritance
//
// Use this if you want to write the factory function.
#if !defined(_STATIC_LINKED) || !defined(_SUBSYSTEM)
#define EXPOSE_INTERFACE_FN(functionName, interfaceName, versionName) \
	static InterfaceReg __g_Create##interfaceName##_reg(functionName, versionName);
#else
#define EXPOSE_INTERFACE_FN(functionName, interfaceName, versionName) \
	namespace _SUBSYSTEM \
	{	\
		static InterfaceReg __g_Create##interfaceName##_reg(functionName, versionName); \
	}
#endif

#if !defined(_STATIC_LINKED) || !defined(_SUBSYSTEM)
#define EXPOSE_INTERFACE(className, interfaceName, versionName) \
	static void* __Create##className##_interface() {return static_cast<interfaceName *>( new className );} \
	static InterfaceReg __g_Create##className##_reg(__Create##className##_interface, versionName );
#else
#define EXPOSE_INTERFACE(className, interfaceName, versionName) \
	namespace _SUBSYSTEM \
	{	\
		static void* __Create##className##_interface() {return static_cast<interfaceName *>( new className );} \
		static InterfaceReg __g_Create##className##_reg(__Create##className##_interface, versionName ); \
	}
#endif

// Use this to expose a singleton interface with a global variable you've created.
#if !defined(_STATIC_LINKED) || !defined(_SUBSYSTEM)
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, globalVarName) \
	static void* __Create##className##interfaceName##_interface() {return static_cast<interfaceName *>( &globalVarName );} \
	static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName);
#else
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, globalVarName) \
	namespace _SUBSYSTEM \
	{ \
		static void* __Create##className##interfaceName##_interface() {return static_cast<interfaceName *>( &globalVarName );} \
		static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName); \
	}
#endif

// Use this to expose a singleton interface. This creates the global variable for you automatically.
#if !defined(_STATIC_LINKED) || !defined(_SUBSYSTEM)
#define EXPOSE_SINGLE_INTERFACE(className, interfaceName, versionName) \
	static className __g_##className##_singleton; \
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, __g_##className##_singleton)
#else
#define EXPOSE_SINGLE_INTERFACE(className, interfaceName, versionName) \
	namespace _SUBSYSTEM \
	{	\
		static className __g_##className##_singleton; \
	}	\
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, __g_##className##_singleton)
#endif

//-----------------------------------------------------------------------------
// This function is automatically exported and allows you to access any interfaces exposed with the above macros.
// if pReturnCode is set, it will return one of the following values (IFACE_OK, IFACE_FAILED)
// extend this for other error conditions/code
//-----------------------------------------------------------------------------
DLL_EXPORT void* CreateInterface(const char *pName, int *pReturnCode);

//-----------------------------------------------------------------------------
// Macros to declare interfaces appropriate for various tiers
//-----------------------------------------------------------------------------
#if 1 || defined( TIER1_LIBRARY ) || defined( TIER2_LIBRARY ) || defined( TIER3_LIBRARY ) || defined( TIER4_LIBRARY ) || defined( APPLICATION )
#define DECLARE_TIER1_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define DECLARE_TIER1_INTERFACE( _Interface, _Global )
#endif

#if 1 || defined( TIER2_LIBRARY ) || defined( TIER3_LIBRARY ) || defined( TIER4_LIBRARY ) || defined( APPLICATION )
#define DECLARE_TIER2_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define DECLARE_TIER2_INTERFACE( _Interface, _Global )
#endif

#if 1 || defined( TIER3_LIBRARY ) || defined( TIER4_LIBRARY ) || defined( APPLICATION )
#define DECLARE_TIER3_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define DECLARE_TIER3_INTERFACE( _Interface, _Global )
#endif

//#include "game\client\isource2_client.h"
//#include "game\client\isource2_clientconfig.h"
//#include "game\client\isource2_clientprediction.h"

class ISource2Client;
class ISource2ClientConfig;
class IPrediction2;
class ISource2ServerConfig;
class ISource2GameEntities;
class IRenderDevice;
class IRenderDeviceMgr;
class IInputSystem;
class IRenderUtils;
class IInputStackSystem;
class ISource2Server;
class ISource2GameClients;
class IMapLoader;
class IPhysics2;
class IPhysics2ActorManager;
class IPhysics2ResourceManager;

#define SOURCE2CLIENT_INTERFACE_VERSION	"Source2Client001"
DECLARE_TIER3_INTERFACE( ISource2Client, g_pSource2Client );

#define SOURCE2CLIENTCONFIG_INTERFACE_VERSION "Source2ClientConfig001"
DECLARE_TIER3_INTERFACE( ISource2ClientConfig, g_pSource2ClientConfig );

#define SOURCE2CLIENTPREDICTION_INTERFACE_VERSION "Source2ClientPrediction001"
DECLARE_TIER3_INTERFACE( IPrediction2, g_pSource2ClientPrediction );

#define SOURCE2SERVERCONFIG_INTERFACE_VERSION "Source2ServerConfig001"
DECLARE_TIER3_INTERFACE(ISource2ServerConfig, g_pSource2ServerConfig)

#define SOURCE2SERVER_GAME_INTERFACE_VERSION "Source2GameEntities001"
DECLARE_TIER3_INTERFACE(ISource2GameEntities, g_sSource2GameEntities)

#define SOURCE2SERVERGAMECLIENTS_INTERFACE_VERSION "Source2GameClients001"
DECLARE_TIER3_INTERFACE(ISource2GameClients, g_pGameClients)

#define RENDER_DEVICE_MGR_INTERFACE_VERSION		"RenderDeviceMgr001"
DECLARE_TIER2_INTERFACE(IRenderDeviceMgr, g_pRenderDeviceManager);

#define RENDER_DEVICE_INTERFACE_VERSION			"RenderDevice003"
DECLARE_TIER3_INTERFACE(IRenderDevice, g_pRenderDevice);

#define RENDER_UTILS_INTERFACE_VERSION		"RenderUtils_001"
DECLARE_TIER2_INTERFACE(IRenderUtils, g_pRenderUtils);

#define INPUTSYSTEM_INTERFACE_VERSION			"InputSystemVersion001"
DECLARE_TIER2_INTERFACE(IInputSystem, g_pInputSystem);

#define INPUTSTACK_INTERFACE_VERSION			"InputStackSystemVersion001"
DECLARE_TIER2_INTERFACE(IInputStackSystem, g_pInputStackSystem);

#define SOURCE2SERVER_INTERFACE_VERSION "Source2Server001"
DECLARE_TIER2_INTERFACE(ISource2Server, g_pSource2Server);

// Dispersion only start
#define MAP_LOADER_INTERFACE_VERSION "MapLoader001"
DECLARE_TIER2_INTERFACE(IMapLoader, g_pMapLoader)
// Dispersion only end

#define VPHSYICS2_INTERFACE_VERSION "VPhysics2_Interface_001"
DECLARE_TIER2_INTERFACE( IPhysics2, g_pPhysics2 );
DECLARE_TIER2_INTERFACE( IPhysics2ActorManager, g_pPhysics2ActorManager );
DECLARE_TIER2_INTERFACE( IPhysics2ResourceManager, g_pPhysics2ResourceManager );

#endif // INTERFACES_H


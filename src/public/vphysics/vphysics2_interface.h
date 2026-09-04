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
#ifndef VPHYSICS2_INTERFACE
#define VPHYSICS2_INTERFACE

#pragma once

#include "mathlib/vector.h"
#include "appframework/iappframework.h"
#include <interfaces.h>

struct dphyslevelV0_t
{
	enum { DATA_VERSION_WITH_DISPLACEMENT = 0xC0000001 };
	enum { DATA_VERSION = 0xC0000002 };
	int32 toolVersion;
	int32 dataVersion;
	int32 sizeofDiskPhysics2LevelMesh;
	int32 buildTime;
	int32 nReserved2[8];
};

enum Physics2FlagEnum
{
	PHYSICS2_IS_ACTOR_STATIC = 1,
	PHYSICS2_IS_ACTOR_FAST = 1 << 1,
	PHYSICS2_ASSERT_RELEASE_IS_FINAL = 1
};

enum Physics2CookedMeshTypes
{
	PHYSICS2_MESH_TYPE_POLYTOPE,
	PHYSICS2_MESH_TYPE_POLYSOUP,
	PHYSICS2_MESH_TYPE_MOPP,
	PHYSICS2_MESH_TYPE_CUSTOM
};

enum Physics2StockShapes
{
	PHYSICS2_STOCK_SPHERE_1M,
	PHYSICS2_STOCK_BOX_1M,
	PHYSICS2_STOCK_SHAPE_COUNT
};

enum Physics2CreateWorldFlags
{
	PHYSICS2_CREATE_CLIENT_WORLD = 1<<0,
	PHYSICS2_CREATE_SERVER_WORLD = 1<<1
};

typedef union
{
	float  m128_f32[4];
	uint32 m128_u32[4];
} fltx4;

class CPhysics2Shape{};
class CPhysics2Polytope: public CPhysics2Shape{};
class CPhysics2Polysoup: public CPhysics2Shape{};
class CPhysics2Box: public CPhysics2Shape{};
class CPhysics2Mopp: public CPhysics2Shape{};
class CPhysics2Actor;
class CPhysics2Inertia;

class IPhysics2CookedMeshBase
{
public:
	virtual unsigned int GetType()const = 0;
	virtual unsigned int NumVertices() const = 0;
	virtual unsigned int NumTriangles() const = 0;
	virtual void GetVertices(float *pVertsOut, unsigned int nByteStride, const fltx4 & factor) = 0;
	virtual void GetTriangles(int *pTrisOut, unsigned int nByteStride) = 0;
	virtual unsigned int GetSizeOf()const = 0;
};

class IPhysics2CookedPolytope : public IPhysics2CookedMeshBase
{
public:
	virtual CPhysics2Polytope* GetPolytope() = 0;
	virtual ~IPhysics2CookedPolytope(){}
};

class IPhysics2CookedPolysoup : public IPhysics2CookedMeshBase
{
public:
	virtual CPhysics2Polysoup* GetPolysoup() = 0;
	virtual void ExportObj(const char *szPath) = 0;
	virtual ~IPhysics2CookedPolysoup(){}
};

class IPhysics2CookedMopp: public IPhysics2CookedMeshBase
{
public:
	virtual CPhysics2Mopp* GetMopp() = 0;
	virtual ~IPhysics2CookedMopp(){}
};


class IPhysics2CookedInertia
{
public:
	virtual CPhysics2Inertia* GetInertia() = 0;
	virtual ~IPhysics2CookedInertia() {}
};


class IPhysics2Level
{
public:
	virtual bool HasMopp() = 0;
};

class IPhysics2Cook;
class IPhysics2World;
class IPhysics2ResourceManager;
class IPhysics2Collision;
class IPhysics2ActorManager;

class IPhysics2: public IAppSystem
{
public:
	virtual IPhysics2World* CreateWorld(unsigned int flags = 0) = 0;
	virtual int NumWorlds() = 0;
	virtual IPhysics2World * GetWorld(int nWorld) = 0;
	virtual void Destroy(IPhysics2World *pWorld) = 0;
	virtual IPhysics2Cook *GetCook() = 0;
	virtual IPhysics2ResourceManager *GetResourceManager() = 0;
	virtual IPhysics2Collision *GetCollision() = 0;
	virtual IPhysics2ActorManager *GetActorManager() = 0;
	virtual int GetSerializeVersion() = 0;
	virtual int NumThreads() = 0;
};


class IPhysics2ActorManager
{
public:
	virtual unsigned int*	GetUserData(const CPhysics2Actor *pActor) = 0;
	virtual void SetUserData(CPhysics2Actor *pActor, unsigned int* userData) = 0;
	virtual const fltx4 GetPosition(const CPhysics2Actor *pActor) = 0;
	virtual const Quaternion GetOrientation(const CPhysics2Actor *pActor) = 0;
	virtual void SetPosition(const CPhysics2Actor *pActor, const fltx4 &vel) = 0;
	virtual void SetOrientation(const CPhysics2Actor *pActor, const Quaternion &orientation) = 0;
	virtual void SetVelocity(const CPhysics2Actor *pActor, const fltx4 &vel) = 0;
	virtual void Destroy(CPhysics2Actor *pActor) = 0;
};

extern IPhysics2ActorManager* g_pPhysics2ActorManager;

class IPhysics2Collision
{
public:
	virtual void Login(IPhysics2Level*) = 0;
	virtual void Logout(IPhysics2Level*) = 0;
};

class IPhysics2Cook
{
public:
	virtual IPhysics2CookedPolytope *CookPolytopeFromPlanes( const Vector4*pPlanes, int planeCount, float mergeDistance = 0.001f) = 0;
	virtual IPhysics2CookedPolytope *CookPolytopeFromVertices( const Vector4*pVertices, int vertexCount, float mergeDistance = 0.001f) = 0;
	virtual IPhysics2CookedPolytope *CookPolytope(IPhysics2CookedMeshBase *pMesh) = 0;
	virtual void* Serialize(CPhysics2Polytope *pShape) = 0;
	virtual void Destroy(IPhysics2CookedPolytope *pShape, unsigned flags = 0) = 0;
	virtual IPhysics2CookedPolysoup *CookPolysoupFromMeshes( IPhysics2CookedMeshBase *const* ppMeshes, int numMeshes ) = 0;
	virtual void* Serialize(CPhysics2Polysoup *pPolysoup) = 0;
	virtual void Destroy(IPhysics2CookedPolysoup *) = 0;

	virtual IPhysics2CookedInertia *CookInertia(CPhysics2Shape *pShape) = 0;
	virtual void* Serialize(CPhysics2Inertia *pInertia) = 0;
	virtual void Destroy(IPhysics2CookedInertia *pInertia, unsigned flags = 0) = 0;

	virtual IPhysics2CookedMopp *CookMopp(IPhysics2CookedMeshBase *const*ppMeshes, int numMeshes) = 0;
	virtual void* Serialize(CPhysics2Mopp *pMopp) = 0;
	virtual void Destroy(IPhysics2CookedMopp *) = 0;
	virtual void ExportObj(const char *szFileName, IPhysics2CookedMeshBase *const*ppMeshes, int numMeshes) = 0;
};

class IPhysics2ResourceManager
{
public:
	virtual CPhysics2Polytope *UnserializePolytope(const void *pBuffer) = 0;
	virtual void Release(CPhysics2Polytope *pShape, unsigned flag = 0) = 0;
	virtual CPhysics2Inertia *UnserializeInertia(const void *pBuffer) = 0;
	virtual void Release(CPhysics2Inertia *pInertia, unsigned flag = 0) = 0;
	virtual CPhysics2Polysoup *UnserializePolysoup(const void* pBuffer) = 0;
	virtual void Release(CPhysics2Polysoup* pPolysoup, unsigned flag = 0) = 0;
	virtual CPhysics2Mopp *UnserializeMopp(const void* pBuffer) = 0;
	virtual void Release(CPhysics2Mopp* pPolysoup, unsigned flag = 0) = 0;
	virtual IPhysics2Level* UnserializeLevel(const struct dphyslevelV0_t *pRoot) = 0;
	virtual void Release(IPhysics2Level*) = 0;
	virtual CPhysics2Box *CreateBoxShape(const fltx4& halfSize) = 0;
	virtual void Release(CPhysics2Box *pBox) = 0;
	virtual CPhysics2Shape *GetStockShape(unsigned int nShape) = 0;
	virtual void Release(CPhysics2Shape* pShape, unsigned flag = 0) = 0;
};

class IPhysics2World
{
public:
	virtual CPhysics2Actor* AddActor(CPhysics2Shape *pShape, CPhysics2Inertia *pInertia, unsigned int flags = 0) = 0;
	virtual void Destroy(CPhysics2Actor*) = 0;
	virtual int NumActiveActors() = 0;
	virtual void GetActiveActors(CPhysics2Actor **ppActors) = 0;
	virtual void Reset() = 0;
	virtual void Simulate ( float deltaTime ) = 0;
};

class CPhysics2Actor
{
public:
	inline unsigned int* GetUserData()const{return g_pPhysics2ActorManager->GetUserData(this);}
	inline void SetUserData(unsigned int* userData){g_pPhysics2ActorManager->SetUserData(this, userData);}
	inline const fltx4 GetPosition()const {	return g_pPhysics2ActorManager->GetPosition(this);}
	inline const Quaternion GetOrientation()const {return g_pPhysics2ActorManager->GetOrientation(this);}
	inline void SetPosition(const fltx4 &pos){return g_pPhysics2ActorManager->SetPosition(this, pos);}
	inline void SetOrientation(const Quaternion &q){g_pPhysics2ActorManager->SetOrientation(this, q);}
	inline void SetVelocity(const fltx4 &vel){return g_pPhysics2ActorManager->SetVelocity(this, vel);}
};


#endif
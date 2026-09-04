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
#ifndef BASEENTITY_H
#define BASEENTITY_H

#include "mathlib/vector.h"
#include "stdio.h"
#include "datamap.h"

#define DECLARE_CLASS_NOBASE( className ) \
	typedef className ThisClass;
#define DECLARE_CLASS( className, baseName ) \
	typedef baseName BaseClass; \
	typedef className ThisClass;

#define LINK_ENTITY_TO_CLASS( mapClassName, DLLClassName) \
	static CEntityFactory<DLLClassName> g_EntityFactory_##mapClassName( #mapClassName );

class CBaseEntity;
typedef Vector4 Vector;

class IEntityFactory
{
public:
	virtual CBaseEntity* Create() = 0;
};


template<class T>
class CEntityFactory : public IEntityFactory
{
public:
	CEntityFactory(const char* szClassName)
	{
		EntitySystem()->RegisterEntityClass(this, szClassName);
	};
	virtual CBaseEntity* Create() {
		return new T;
	}
};



typedef void(*fnThink)();
class CBaseEntity
{
public:
	DECLARE_CLASS_NOBASE(CBaseEntity);
	DECLARE_DATADESC_NOBASE()

	typedescription_t* FindDataByName(const char* szName);
	typedescription_t* FindDataByMapName(const char* szName);

	virtual ~CBaseEntity();
	virtual void Spawn();

	virtual void SetAbsAngles(float fPitch, float fYaw, float fRoll);
	virtual void SetAbsOrigin(Vector origin);
	virtual void SetScale(float fScale);

	virtual QAngle GetAbsAngles(void);
	virtual Vector GetAbsOrigin(void);
	virtual float GetScale(void);

	virtual void SetThink(fnThink pfnThink);
	virtual void SetNextThink(float fThink);

	fnThink m_pfnThink = NULL;
	CTransformUnaligned m_transform;

};

#endif
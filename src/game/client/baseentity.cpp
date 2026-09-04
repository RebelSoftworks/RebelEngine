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
#include "baseentity.h"
#include "datamap.h"
#include <string.h>

CBaseEntity::~CBaseEntity()
{

}

void CBaseEntity::Spawn()
{

}

void CBaseEntity::SetAbsAngles(float fPitch, float fYaw, float fRoll)
{
}

void CBaseEntity::SetAbsOrigin(Vector origin)
{
	m_transform.m_position = origin;
}

void CBaseEntity::SetScale(float fScale)
{
	m_transform.m_scale.x = fScale;
	m_transform.m_scale.y = fScale;
	m_transform.m_scale.z = fScale;
}


QAngle CBaseEntity::GetAbsAngles(void)
{
	QAngle q{ 0.0f, 0.0f, 0.0f };
	return q;
}

Vector CBaseEntity::GetAbsOrigin(void)
{
	Vector v;
	return v;
}

float CBaseEntity::GetScale(void)
{
	return 0.0f;
}

void CBaseEntity::SetThink(fnThink pfnThink)
{
	m_pfnThink = pfnThink;
}

void CBaseEntity::SetNextThink(float fThink)
{

}
typedescription_t* CBaseEntity::FindDataByName(const char* szName)
{
	datamap_t* pDataMap;
	int i;

	pDataMap = GetDataMap();

lookforname:
	for (i = 0; i < pDataMap->m_iNumFields; i++)
	{
		if (!strcmp(pDataMap->m_pData[i].m_szFieldName, szName))
			return &pDataMap->m_pData[i];
	}
	pDataMap = pDataMap->m_pBase;

	if (pDataMap)
		goto lookforname;

	return NULL;
};

typedescription_t* CBaseEntity::FindDataByMapName(const char* szName)
{
	datamap_t* pDataMap;
	int i;

	pDataMap = GetDataMap();

lookforname:
	for (i = 0; i < pDataMap->m_iNumFields; i++)
	{
		if (!strcmp(pDataMap->m_pData[i].m_szEditorName, szName))
			return &pDataMap->m_pData[i];
	}
	pDataMap = pDataMap->m_pBase;

	if (pDataMap)
		goto lookforname;

	return NULL;
};


BEGIN_DATADESC_NOBASE(CBaseEntity)
	DEFINE_KEYFIELD(m_transform.m_position, FIELD_VECTOR, "origin")
	DEFINE_KEYFIELD(m_transform.m_rotation, FIELD_QUATERNION_QANGLE, "angles")
	DEFINE_KEYFIELD(m_transform.m_scale, FIELD_VECTOR, "scales")
END_DATADESC()
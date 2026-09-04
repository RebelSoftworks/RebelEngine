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

// typedefs
typedef unsigned char uint8;
typedef signed char int8;

typedef short					int16;
typedef unsigned short			uint16;
typedef int						int32;
typedef unsigned int			uint32;
typedef long long				int64;
typedef unsigned long long		uint64;

// for ghidra vtable exports
typedef uint8 uint8_t;
typedef uint16 uint16_t;
typedef uint32 uint32_t;
typedef uint64 uint64_t;

typedef void* undefined;
typedef int undefined4;
typedef uint64_t undefined8;
//typedef char byte;
typedef float vec_t;

#ifdef _WIN32
typedef wchar_t ucs2; // under windows wchar_t is ucs2
#else
typedef unsigned short ucs2;
#endif

#if defined(__linux__)
#define _vsnprintf vsnprintf
typedef int64 LARGE_INTEGER;
#endif // __linux__

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define FORWARD_DECLARE_HANDLE(name) typedef struct name##__ *name

#define DECLARE_DERIVED_POINTER_HANDLE( _name, _basehandle ) struct _name##__ : public _basehandle##__ {}; typedef struct _name##__ *_name
#define DECLARE_ALIASED_POINTER_HANDLE( _name, _alias ) typedef struct _alias##__ *name
#define CREATE_NATIVE(type, var) type __##var; type *var = &__##var
#define SET_NATIVE(var, v) var->m_pSelf = (void*)(v)
#define GN(var) (decltype(var))(var->m_pSelf)

template <typename A, typename Parent>
class RWVariable
{
private:
	Parent* m_parent;
	A(*m_ReadOp)(Parent*);
	void(*m_WriteOp)(Parent*, A);
public:
	RWVariable(void* ReadOp, void* WriteOp, Parent* parent)
	{
		m_ReadOp = (A(*)(Parent*))ReadOp;
		m_WriteOp = (void(*)(Parent*, A))WriteOp;
		m_parent = parent;
	}
	inline RWVariable& operator = (const A& other)
	{
		m_WriteOp((Parent*)m_parent->m_pSelf, other);
		return *this;
	}
	inline operator A()
	{
		return m_ReadOp((Parent*)m_parent->m_pSelf);
	}
};
#define RW_VAR(type, name, parent, readop, writeop) RWVariable<type, parent> name{readop, writeop,this};
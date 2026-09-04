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
#ifndef IRENDERCONTEXT_H
#define IRENDERCONTEXT_H
#pragma once

#include "../mathlib/vector.h"
#include "../tier0/types.h"
#include <tier0/wchartypes.h>
#include "Color.h"
#include <unordered_map>
#include <cstring>

enum RenderPrimitiveType_t
{
    RENDER_PRIM_TRIANGLES,
    RENDER_PRIM_LINES
};

enum RenderTextureDimension_t
{
    RENDER_TEXTURE_DIMENSION_2D
};

enum RenderShaderType_t
{
    RENDER_VERTEX_SHADER,
    RENDER_PIXEL_SHADER
};

enum RenderColorSpace_t
{
    RENDER_SRGB
};

enum RsFilter_t
{
    RS_FILTER_MIN_MAG_MIP_POINT
};

enum RsTextureAddressMode_t
{
    RS_TEXTURE_ADDRESS_WRAP
};

enum RsComparison_t
{
    RS_COMPARISON_ALWAYS
};

enum RenderBarrierAccessFlags_t
{
    RENDER_BARRIER_ACCESS_NONE = 0
};

enum RenderBarrierPipelineStageFlags_t
{
    RENDER_STAGE_ALL = 0
};

enum RenderImageLayout_t
{
    RENDER_IMAGE_LAYOUT_DEFAULT
};

enum RenderClearFlags_t
{
    RENDER_CLEAR_FLAGS_CLEAR_COLOR = 1 << 0,
    RENDER_CLEAR_FLAGS_CLEAR_DEPTH = 1 << 1,
    RENDER_CLEAR_FLAGS_CLEAR_STENCIL = 1 << 2
};

struct RenderShaderHandle_t
{
    uint64 handle;
};

struct RenderBufferHandle_t
{
    uint64 handle;

    RenderBufferHandle_t() = default;

    RenderBufferHandle_t(uint64 value)
        : handle(value)
    {
    }

    constexpr operator uint64() const
    {
        return handle;
    }

    constexpr bool IsValid() const
    {
        return handle != 0;
    }
};

struct IndexBufferHandle_t {
    uint64 handle;

    IndexBufferHandle_t() = default;

    IndexBufferHandle_t(uint64 value)
        : handle(value)
    {
    }

    constexpr operator uint64() const
    {
        return handle;
    }

    constexpr bool IsValid() const
    {
        return handle != 0;
    }
};
struct VertexBufferHandle_t {
    uint64 handle;

    VertexBufferHandle_t() = default;

    VertexBufferHandle_t(uint64 value)
        : handle(value)
    {
    }

    constexpr operator uint64() const
    {
        return handle;
    }

    constexpr bool IsValid() const
    {
        return handle != 0;
    }
};

struct ConstantBuffer_t {};

DECLARE_POINTER_HANDLE( SwapChainHandle_t );
DECLARE_POINTER_HANDLE( RenderResourceHandle_t );
DECLARE_DERIVED_POINTER_HANDLE(ConstantBufferHandle_t, RenderResourceHandle_t);

struct ITexture
{
};

constexpr ITexture RENDER_TEXTURE_HANDLE_INVALID;

struct Rect_t
{
    int x{}, y{}, w{}, h{};

    bool IsEmpty() const { return w == 0 || h == 0; }
};

using NativeRect = Rect_t;

struct RenderViewport_t
{
    int x{}, y{}, w{}, h{};

    void Init(int _x, int _y, int _w, int _h)
    {
        x = _x; y = _y; w = _w; h = _h;
    }

    void Init(const Rect_t& r)
    {
        Init(r.x, r.y, r.w, r.h);
    }
};

struct RenderUAVBarrier_t
{
    ITexture texture;
    RenderBarrierPipelineStageFlags_t srcStage;
    RenderBarrierPipelineStageFlags_t dstStage;
    RenderBarrierAccessFlags_t srcAccess;
    RenderBarrierAccessFlags_t dstAccess;
    int mipStart;
    int mipCount;
    RenderImageLayout_t layout;
};

struct RenderBufferBarrier_t
{
    RenderBufferHandle_t buffer;
    RenderBarrierPipelineStageFlags_t srcStage;
    RenderBarrierPipelineStageFlags_t dstStage;
    RenderBarrierAccessFlags_t srcAccess;
    RenderBarrierAccessFlags_t dstAccess;
};

struct DynamicLockDesc_t
{
    void* m_pMemory{ nullptr };
    int m_nSize;
};


struct RenderTargetDesc_t
{
    ITexture color{};
    ITexture depth{};
    RenderColorSpace_t colorSpace{ RENDER_SRGB };

    RenderTargetDesc_t() = default;

    RenderTargetDesc_t(ITexture c, ITexture d, RenderColorSpace_t cs)
        : color(c), depth(d), colorSpace(cs) {
    }

    RenderTargetDesc_t(SwapChainHandle_t, bool, bool) {}
};

// This is empty anyway in Source 2 codebase apparently
class IReadTexturePixelsCallback {};
class IReadBufferCallback {};

// TODO: Move IScene* stuff to a seperate header file (maybe scenesystem/isceneutils.h)
class ISceneLayer
{
public:
    struct Desc
    {
        ITexture m_pColorTargets[1]{};
    };

    virtual Desc GetRenderTargetDesc();
};

// TODO: Make a mesh.h once we have an impl?
class MeshTraceInput {};
class MeshTraceOutput {};
// TODO: Make an engine2.h?
static void* g_engine_nativeFunctions[2747];
static void* g_engine_callbackFunctions[79];

class IPVS
{
public:
    void* m_pSelf;
    inline operator IPVS* ()
    {
        return (IPVS*)m_pSelf;
    }
    inline bool IsEmptyPVS()
    {
        return false;
    }
    inline int IsInPVS(int numSources, void* sources, Vector4 position)
    {
        return
            ((int(*)(void* self, int numSources, void* sources, Vector4 position))g_engine_nativeFunctions[2088])(m_pSelf, numSources, sources, position);
    }
    inline int IsAbsBoxInPVS(int numSources, void* sources, Vector4 mins, Vector4 maxs)
    {
        return
            ((int(*)(void* self, int numSources, void* sources, Vector4 mins, Vector4 maxs))g_engine_nativeFunctions[2089])(m_pSelf, numSources, sources, mins, maxs);
    }
    inline int IsSkyVisibleFromPosition(Vector4 position)
    {
        return
            ((int(*)(void* self, Vector4 position))g_engine_nativeFunctions[2090])(m_pSelf, position);
    }
    inline int IsSunVisibleFromPosition(Vector4 position)
    {
        return
            ((int(*)(void* self, Vector4 position))g_engine_nativeFunctions[2091])(m_pSelf, position);
    }
};

class IWorldReference
{
public:
    void* m_pSelf;
   
    IWorldReference() : m_pSelf(nullptr) {}

    inline void Release()
    {
    }
    inline int IsWorldLoaded()
    {
        return 1; // yes yes
    }
    inline int IsErrorWorld()
    {
        return 0; // no
    }
    inline int IsMarkedForDeletion()
    {
        return 0; // no
    }
    inline int GetWorldBounds(Vector4* vMin, Vector4* vMax)
    {
        if (!m_pSelf || !vMin || !vMax)
            return 0;

        return 1;
    }
    inline int GetSceneWorld()
    {
        return 1;
    }
    inline void PrecacheAllWorldNodes(unsigned int flags)
    {
    }
    inline const char* GetFolder()
    {
        return "maps";
    }
    inline int GetEntityCount(const char* pEntityLumpName)
    {
        // we don't support any entities other than CBaseEntity, so NONE!
        return 0;
    }

};

struct WorldVertex_t
{
    Vector3 m_Position;
    Vector3 m_Normal;
    Vector2 m_TexCoord;
};

struct WorldSurface_t
{
    uint32_t m_nFirstIndex = 0;
    uint32_t m_nIndexCount = 0;
};

#include <string>
#include <vector>
struct MapPlane_t
{
    Vector3 normal;
    float distance = 0.0f;
};

struct MapBrushSide_t
{
    MapPlane_t plane;

    std::string material;

    Vector3 textureU{};
    Vector3 textureV{};

    float textureShiftU = 0.0f;
    float textureShiftV = 0.0f;

    float textureScaleU = 1.0f;
    float textureScaleV = 1.0f;
};

struct MapBrush_t
{
    std::vector<MapBrushSide_t> sides;
};

struct SceneWorldData
{
    std::string debugName;

    std::vector<MapBrush_t> brushes;

    std::vector<WorldVertex_t> vertices;
    std::vector<uint32_t> indices;
    std::vector<WorldSurface_t> surfaces;

    IPVS* pPVS = nullptr;
    RenderBufferHandle_t vertexBuffer{};
    RenderBufferHandle_t indexBuffer{};

    bool buffersUploaded = false;
};

class ISceneWorld
{
public:
    void* m_pSelf;
    ISceneWorld() : m_pSelf(nullptr) {}

    inline void DeleteAllObjects()
    {
    }
    inline void Release()
    {
    }
    inline int GetSceneObjectCount()
    {
        return 0;
    }
    inline int IsEmpty()
    {
        return 0;
    }
    inline const char* GetWorldDebugName()
    {
        return "defend_castle";
    }
    inline void SetDeleteAtEndOfFrame(int bDelete)
    {
    }
    inline int GetDeleteAtEndOfFrame()
    {
        return 0x64;
    }
    inline void DeleteEndOfFrameObjects()
    {
    }
    inline int MeshTrace(MeshTraceInput input, MeshTraceOutput* output)
    {
        return 0;
    }
    inline IPVS* GetPVS()
    {
        return static_cast<SceneWorldData*>(m_pSelf)->pPVS;
    }
    inline void SetPVS(IPVS* pPVS)
    {
        static_cast<SceneWorldData*>(m_pSelf)->pPVS = pPVS;
    }
    inline void Add3DSkyboxWorld(ISceneWorld* world)
    {
    }
    inline void Remove3DSkyboxWorld(ISceneWorld* world)
    {
    }
    inline void Set3DSkyboxParameters(Vector4 origin, QAngle angle, float scale)
    {
    }
};

class HMaterialStrong {};
class CRenderAttributes
{
public:
    void* m_pSelf;
    void SetFloatValue(uint32_t token, float value)
    {
        Attribute& attr = m_Storage[token];
        attr.type = TYPE_FLOAT;
        attr.flValue = value;
    }
    float GetFloatValue(uint32_t token, float defaultValue) const
    {
        auto it = m_Storage.find(token);

        if (it == m_Storage.end())
            return defaultValue;

        if (it->second.type != TYPE_FLOAT)
            return defaultValue;

        return it->second.flValue;
    }

    void SetVectorValue(uint32_t token, const Vector3& value)
    {
        Attribute& attr = m_Storage[token];
        attr.type = TYPE_VECTOR;
        attr.vecValue = value;
    }

    Vector3 GetVectorValue(uint32_t token, const Vector3& defaultValue) const
    {
        auto it = m_Storage.find(token);

        if (it == m_Storage.end())
            return defaultValue;

        if (it->second.type != TYPE_VECTOR)
            return defaultValue;

        return it->second.vecValue;
    }

    void SetTextureValue(uint32_t token, ITexture* texture, int mip)
    {
        Attribute& attr = m_Storage[token];
        attr.type = TYPE_TEXTURE;
        attr.pTexture = texture;
        attr.mip = mip;
    }

    void Clear(bool freeMemory, bool resetParent)
    {
        m_Storage.clear();

        (void)freeMemory;
        (void)resetParent;
    }
    bool IsEmpty() const
    {
        return m_Storage.empty();
    }

    void SetBoolValue(uint32_t token, bool value)
    {
        Attribute& attr = m_Storage[token];
        attr.type = TYPE_BOOL;
        attr.bValue = value;
    }

    bool GetBoolValue(uint32_t token, bool defaultValue) const
    {
        auto it = m_Storage.find(token);

        if (it == m_Storage.end())
            return defaultValue;

        if (it->second.type != TYPE_BOOL)
            return defaultValue;

        return it->second.bValue;
    }

private:
    enum AttributeType
    {
        TYPE_NONE,
        TYPE_FLOAT,
        TYPE_BOOL,
        TYPE_VECTOR,
        TYPE_TEXTURE
    };

    struct Attribute
    {
        AttributeType type;

        float flValue;
        Vector3 vecValue;
        bool bValue;
        ITexture* pTexture;
        int mip;

        Attribute()
            : type(TYPE_NONE)
            , bValue(false)
            , flValue(0.0f)
            , vecValue{ 0,0,0 }
            , pTexture(nullptr)
            , mip(0)
        {
        }
    };

    std::unordered_map<uint32_t, Attribute> m_Storage;
};
class ISceneView {};
class HModelStrong {};
struct AABB_t
{
    float fMin[3];
    float fMax[3];

    bool operator==(const AABB_t& other) const
    {
        return fMin == other.fMin &&
            fMax == other.fMax;
    }
};

class CSceneObject
{
public:
    void* m_pSelf;
    inline operator CSceneObject* ()
    {
        return (CSceneObject*)m_pSelf;
    }
    inline void ChangeFlags(int64 nNewFlags, int64 nNewFlagsMask)
    {
        ((void(*)(void* self, int64 nNewFlags, int64 nNewFlagsMask))g_engine_nativeFunctions[1052])(m_pSelf, nNewFlags, nNewFlagsMask);
    }
    inline void SetFlags(int64 nFlagsToOR)
    {
        ((void(*)(void* self, int64 nFlagsToOR))g_engine_nativeFunctions[1053])(m_pSelf, nFlagsToOR);
    }
    inline int HasFlags(int64 nFlags)
    {
        return
            ((int(*)(void* self, int64 nFlags))g_engine_nativeFunctions[1054])(m_pSelf, nFlags);
    }
    inline int64 GetFlags()
    {
        return
            ((int64(*)(void* self))g_engine_nativeFunctions[1055])(m_pSelf);
    }
    inline int64 GetOriginalFlags()
    {
        return
            ((int64(*)(void* self))g_engine_nativeFunctions[1056])(m_pSelf);
    }
    inline void ClearFlags(int64 nFlagsToClear)
    {
        ((void(*)(void* self, int64 nFlagsToClear))g_engine_nativeFunctions[1057])(m_pSelf, nFlagsToClear);
    }
    inline void SetCullDistance(float dist)
    {
        ((void(*)(void* self, float dist))g_engine_nativeFunctions[1058])(m_pSelf, dist);
    }
    inline void EnableLightingCache()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1059])(m_pSelf);
    }
    inline void SetLightingOrigin(Vector4 vPos, int worldspace)
    {
        ((void(*)(void* self, Vector4 vPos, int worldspace))g_engine_nativeFunctions[1060])(m_pSelf, vPos, worldspace);
    }
    inline Vector4 GetLightingOrigin()
    {
        return
            ((Vector4(*)(void* self))g_engine_nativeFunctions[1061])(m_pSelf);
    }
    inline int HasLightingOrigin()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[1062])(m_pSelf);
    }
    inline void SetTintRGBA(Color color)
    {
        ((void(*)(void* self, Color color))g_engine_nativeFunctions[1063])(m_pSelf, color);
    }
    inline Color GetTintRGBA()
    {
        return
            ((Color(*)(void* self))g_engine_nativeFunctions[1064])(m_pSelf);
    }
    inline void SetAlphaFade(float nAlpha)
    {
        ((void(*)(void* self, float nAlpha))g_engine_nativeFunctions[1065])(m_pSelf, nAlpha);
    }
    inline float GetAlphaFade()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[1066])(m_pSelf);
    }
    inline void SetMaterialOverrideForMeshInstances(HMaterialStrong* mat)
    {
        ((void(*)(void* self, HMaterialStrong * mat))g_engine_nativeFunctions[1067])(m_pSelf, mat);
    }
    inline void ClearMaterialOverrideList()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1068])(m_pSelf);
    }
    inline void SetMaterialOverride(HMaterialStrong* hMaterial, uint32 nAttr, int nAttrValueMatch)
    {
        ((void(*)(void* self, HMaterialStrong * hMaterial, uint32 nAttr, int nAttrValueMatch))g_engine_nativeFunctions[1069])(m_pSelf, hMaterial, nAttr, nAttrValueMatch);
    }
    inline int IsLoaded()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[1070])(m_pSelf);
    }
    inline int IsRenderingEnabled()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[1071])(m_pSelf);
    }
    inline void SetLoaded()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1072])(m_pSelf);
    }
    inline void ClearLoaded()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1073])(m_pSelf);
    }
    inline void DisableRendering()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1074])(m_pSelf);
    }
    inline void EnableRendering()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1075])(m_pSelf);
    }
    inline void SetRenderingEnabled(int bEnabled)
    {
        ((void(*)(void* self, int bEnabled))g_engine_nativeFunctions[1076])(m_pSelf, bEnabled);
    }
    inline float GetBoundingSphereRadius()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[1077])(m_pSelf);
    }
    inline void SetTransform(CTransformUnaligned tx)
    {
        ((void(*)(void* self, CTransformUnaligned tx))g_engine_nativeFunctions[1078])(m_pSelf, tx);
    }
    inline CTransformUnaligned GetCTransform()
    {
        return
            ((CTransformUnaligned(*)(void* self))g_engine_nativeFunctions[1079])(m_pSelf);
    }
    inline void SetBounds(AABB_t box)
    {
        ((void(*)(void* self, AABB_t box))g_engine_nativeFunctions[1080])(m_pSelf, box);
    }
    inline AABB_t GetBounds()
    {
        return
            ((AABB_t(*)(void* self))g_engine_nativeFunctions[1081])(m_pSelf);
    }
    inline void SetBoundsInfinite()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1082])(m_pSelf);
    }
    inline int GetParent()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[1083])(m_pSelf);
    }
    inline void AddChildObject(uint32 nId, CSceneObject* pChild, unsigned int nChildUpdateFlags)
    {
        ((void(*)(void* self, uint32 nId, CSceneObject * pChild, unsigned int nChildUpdateFlags))g_engine_nativeFunctions[1084])(m_pSelf, nId, pChild, nChildUpdateFlags);
    }
    inline void RemoveChild(CSceneObject* obj)
    {
        ((void(*)(void* self, CSceneObject * obj))g_engine_nativeFunctions[1085])(m_pSelf, obj);
    }
    inline const CRenderAttributes* GetAttributesPtrForModify()
    {
        return
            ((const CRenderAttributes * (*)(void* self))g_engine_nativeFunctions[1086])(m_pSelf);
    }
    inline void EnableMeshGroups(uint64 nMask)
    {
        ((void(*)(void* self, uint64 nMask))g_engine_nativeFunctions[1087])(m_pSelf, nMask);
    }
    inline void DisableMeshGroups(uint64 nMask)
    {
        ((void(*)(void* self, uint64 nMask))g_engine_nativeFunctions[1088])(m_pSelf, nMask);
    }
    inline void ResetMeshGroups(uint64 nMask)
    {
        ((void(*)(void* self, uint64 nMask))g_engine_nativeFunctions[1089])(m_pSelf, nMask);
    }
    inline uint64 GetCurrentMeshGroupMask()
    {
        return
            ((uint64(*)(void* self))g_engine_nativeFunctions[1090])(m_pSelf);
    }
    inline int GetWorld()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[1091])(m_pSelf);
    }
    inline void SetLOD(int nLOD)
    {
        ((void(*)(void* self, int nLOD))g_engine_nativeFunctions[1092])(m_pSelf, nLOD);
    }
    inline void DisableLOD()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1093])(m_pSelf);
    }
    inline uint64 GetCurrentLODGroupMask()
    {
        return
            ((uint64(*)(void* self))g_engine_nativeFunctions[1094])(m_pSelf);
    }
    inline int GetCurrentLODLevel()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[1095])(m_pSelf);
    }
    inline HModelStrong* GetModelHandle()
    {
        return
            ((HModelStrong * (*)(void* self))g_engine_nativeFunctions[1096])(m_pSelf);
    }
    inline void SetMaterialGroup(uint32 token)
    {
        ((void(*)(void* self, uint32 token))g_engine_nativeFunctions[1097])(m_pSelf, token);
    }
    inline void SetBodyGroup(uint32 token, int value)
    {
        ((void(*)(void* self, uint32 token, int value))g_engine_nativeFunctions[1098])(m_pSelf, token, value);
    }
    inline void SetBatchable(int bIsBatchable)
    {
        ((void(*)(void* self, int bIsBatchable))g_engine_nativeFunctions[1099])(m_pSelf, bIsBatchable);
    }
    inline int IsNotBatchable()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[1100])(m_pSelf);
    }
    inline void SetUniqueBatchGroup(int bUnique)
    {
        ((void(*)(void* self, int bUnique))g_engine_nativeFunctions[1101])(m_pSelf, bUnique);
    }
    inline void RemoveTag(unsigned int tag)
    {
        ((void(*)(void* self, unsigned int tag))g_engine_nativeFunctions[1102])(m_pSelf, tag);
    }
    inline void RemoveAllTags()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[1103])(m_pSelf);
    }
    inline int GetTagCount()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[1104])(m_pSelf);
    }
    inline unsigned int GetTagAt(int i)
    {
        return
            ((unsigned int(*)(void* self, int i))g_engine_nativeFunctions[1105])(m_pSelf, i);
    }
    inline void AddTag(unsigned int tag)
    {
        ((void(*)(void* self, unsigned int tag))g_engine_nativeFunctions[1106])(m_pSelf, tag);
    }
    inline int HasTag(unsigned int tag)
    {
        return
            ((int(*)(void* self, unsigned int tag))g_engine_nativeFunctions[1107])(m_pSelf, tag);
    }
    inline void SetForceLayerID(uint32 nTok)
    {
        ((void(*)(void* self, uint32 nTok))g_engine_nativeFunctions[1108])(m_pSelf, nTok);
    }
    inline void SetLayerMatchID(uint32 nTok)
    {
        ((void(*)(void* self, uint32 nTok))g_engine_nativeFunctions[1109])(m_pSelf, nTok);
    }
    inline void UpdateFlagsBasedOnMaterial()
    {
        ((void(*)(void* __self))g_engine_nativeFunctions[1110])(m_pSelf);
    }
    inline void SetMaterialOverrideByIndex(int index, HMaterialStrong* material)
    {
        ((void(*)(void* self, int index, HMaterialStrong * material))g_engine_nativeFunctions[1111])(m_pSelf, index, material);
    }
};

class HRenderTextureStrong {};
struct SceneSystemPerFrameStats_t {};
// TBA: I'm tired boss
typedef Vector4 Vector;
class CEnvMapSceneObject
{
public:
    void* m_pSelf;
    inline operator CEnvMapSceneObject* ()
    {
        return (CEnvMapSceneObject*)m_pSelf;
    }

    inline void CalculateRadianceSH()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[302])(m_pSelf);
    }
    inline void CalculateRadianceSH(HRenderTextureStrong* hTexture)
    {
        ((void(*)(void* self, HRenderTextureStrong * hTexture))g_engine_nativeFunctions[303])(m_pSelf, hTexture);
    }
    inline void CalculateNormalizationSH()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[304])(m_pSelf);
    }
    inline void CalculateBounds()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[305])(m_pSelf);
    }
    inline void SetWorldPosition(Vector pos)
    {
        ((void(*)(void* self, Vector pos))g_engine_nativeFunctions[306])(m_pSelf, pos);
    }
    inline Vector GetWorldPosition()
    {
        return
            ((Vector(*)(void* self))g_engine_nativeFunctions[307])(m_pSelf);
    }
    inline void SetWorldDirection(Quaternion dir)
    {
        ((void(*)(void* self, Quaternion dir))g_engine_nativeFunctions[308])(m_pSelf, dir);
    }
    inline Vector GetWorldDirection()
    {
        return
            ((Vector(*)(void* self))g_engine_nativeFunctions[309])(m_pSelf);
    }
    inline void SetColor(Vector color)
    {
        ((void(*)(void* self, Vector color))g_engine_nativeFunctions[310])(m_pSelf, color);
    }
    inline void SetBounceColor(Vector color)
    {
        ((void(*)(void* self, Vector color))g_engine_nativeFunctions[311])(m_pSelf, color);
    }
    inline Vector GetColor()
    {
        return
            ((Vector(*)(void* self))g_engine_nativeFunctions[312])(m_pSelf);
    }
    inline void SetRadius(float radius)
    {
        ((void(*)(void* self, float radius))g_engine_nativeFunctions[313])(m_pSelf, radius);
    }
    inline float GetRadius()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[314])(m_pSelf);
    }
    inline void SetTheta(float f)
    {
        ((void(*)(void* self, float f))g_engine_nativeFunctions[315])(m_pSelf, f);
    }
    inline float GetTheta()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[316])(m_pSelf);
    }
    inline void SetPhi(float f)
    {
        ((void(*)(void* self, float f))g_engine_nativeFunctions[317])(m_pSelf, f);
    }
    inline float GetPhi()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[318])(m_pSelf);
    }
    inline void SetFallOff(float f)
    {
        ((void(*)(void* self, float f))g_engine_nativeFunctions[319])(m_pSelf, f);
    }
    inline float GetFallOff()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[320])(m_pSelf);
    }
    inline int GetShadowTextureResolution()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[321])(m_pSelf);
    }
    inline void SetShadowTextureResolution(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[322])(m_pSelf, v);
    }
    inline int GetShadows()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[323])(m_pSelf);
    }
    inline void SetShadows(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[324])(m_pSelf, v);
    }
    inline void SetConstantAttn(float f)
    {
        ((void(*)(void* self, float f))g_engine_nativeFunctions[325])(m_pSelf, f);
    }
    inline float GetConstantAttn()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[326])(m_pSelf);
    }
    inline void SetLinearAttn(float f)
    {
        ((void(*)(void* self, float f))g_engine_nativeFunctions[327])(m_pSelf, f);
    }
    inline float GetLinearAttn()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[328])(m_pSelf);
    }
    inline void SetQuadraticAttn(float f)
    {
        ((void(*)(void* self, float f))g_engine_nativeFunctions[329])(m_pSelf, f);
    }
    inline float GetQuadraticAttn()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[330])(m_pSelf);
    }
    inline void SetLightCookie(HRenderTextureStrong* f)
    {
        ((void(*)(void* self, HRenderTextureStrong * f))g_engine_nativeFunctions[331])(m_pSelf, f);
    }
    inline HRenderTextureStrong* GetLightCookie()
    {
        return
            ((HRenderTextureStrong * (*)(void* self))g_engine_nativeFunctions[332])(m_pSelf);
    }
    inline int GetShadowCascades()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[333])(m_pSelf);
    }
    inline void SetShadowCascades(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[334])(m_pSelf, v);
    }
    inline float GetCascadeDistanceScale()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[335])(m_pSelf);
    }
    inline void SetCascadeDistanceScale(float dist)
    {
        ((void(*)(void* self, float dist))g_engine_nativeFunctions[336])(m_pSelf, dist);
    }
    inline float GetFogContributionStength()
    {
        return
            ((float(*)(void* self))g_engine_nativeFunctions[337])(m_pSelf);
    }
    inline void SetFogContributionStength(float v)
    {
        ((void(*)(void* self, float v))g_engine_nativeFunctions[338])(m_pSelf, v);
    }
    inline int GetFogLightingMode()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[339])(m_pSelf);
    }
    inline void SetFogLightingMode(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[340])(m_pSelf, v);
    }
    inline void SetBakeLightIndex(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[341])(m_pSelf, v);
    }
    inline void SetBakeLightIndexScale(float v)
    {
        ((void(*)(void* self, float v))g_engine_nativeFunctions[342])(m_pSelf, v);
    }
    inline void SetUsesIndexedBakedLighting(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[343])(m_pSelf, v);
    }
    inline void SetRenderDiffuse(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[344])(m_pSelf, v);
    }
    inline void SetRenderSpecular(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[345])(m_pSelf, v);
    }
    inline void SetRenderTransmissive(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[346])(m_pSelf, v);
    }
    inline void SetLightSourceSize0(float v)
    {
        ((void(*)(void* self, float v))g_engine_nativeFunctions[347])(m_pSelf, v);
    }
    inline void SetLightSourceSize1(float v)
    {
        ((void(*)(void* self, float v))g_engine_nativeFunctions[348])(m_pSelf, v);
    }
    inline void SetShadowTextureWidth(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[349])(m_pSelf, v);
    }
    inline void SetShadowTextureHeight(int v)
    {
        ((void(*)(void* self, int v))g_engine_nativeFunctions[350])(m_pSelf, v);
    }
    inline int GetShadowTextureWidth()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[351])(m_pSelf);
    }
    inline int GetShadowTextureHeight()
    {
        return
            ((int(*)(void* self))g_engine_nativeFunctions[352])(m_pSelf);
    }
    inline unsigned int GetLightFlags()
    {
        return
            ((unsigned int(*)(void* self))g_engine_nativeFunctions[353])(m_pSelf);
    }
    inline void SetLightFlags(unsigned int flags)
    {
        ((void(*)(void* self, unsigned int flags))g_engine_nativeFunctions[354])(m_pSelf, flags);
    }
    inline int64 GetLightShape()
    {
        return
            ((int64(*)(void* self))g_engine_nativeFunctions[355])(m_pSelf);
    }
    inline void SetLightShape(int64 shape)
    {
        ((void(*)(void* self, int64 shape))g_engine_nativeFunctions[356])(m_pSelf, shape);
    }
    inline void SetLightSourceDim0(float v)
    {
        ((void(*)(void* self, float v))g_engine_nativeFunctions[357])(m_pSelf, v);
    }
    inline void SetLightSourceDim1(float v)
    {
        ((void(*)(void* self, float v))g_engine_nativeFunctions[358])(m_pSelf, v);
    }
    RW_VAR(int64, m_nProjectionMode, CEnvMapSceneObject, g_engine_nativeFunctions[359], g_engine_nativeFunctions[360]);
    RW_VAR(Vector, m_vBoxProjectMins, CEnvMapSceneObject, g_engine_nativeFunctions[361], g_engine_nativeFunctions[362]);
    RW_VAR(Vector, m_vBoxProjectMaxs, CEnvMapSceneObject, g_engine_nativeFunctions[363], g_engine_nativeFunctions[364]);
    RW_VAR(Vector, m_vColor, CEnvMapSceneObject, g_engine_nativeFunctions[365], g_engine_nativeFunctions[366]);
    RW_VAR(int, m_nRenderPriority, CEnvMapSceneObject, g_engine_nativeFunctions[367], g_engine_nativeFunctions[368]);
    RW_VAR(HRenderTextureStrong*, m_hEnvMapTexture, CEnvMapSceneObject, g_engine_nativeFunctions[369], g_engine_nativeFunctions[370]);
    RW_VAR(Vector4, m_vNormalizationSH, CEnvMapSceneObject, g_engine_nativeFunctions[371], g_engine_nativeFunctions[372]);
    RW_VAR(float, m_flFeathering, CEnvMapSceneObject, g_engine_nativeFunctions[373], g_engine_nativeFunctions[374]);
};

struct SceneVolumetricFogVolume_t {};
class CSceneLightProbeVolumeObject {};

class IRenderContext;

class IRayTraceSceneWorld
{
public:
    void* m_pSelf;
    inline operator IRayTraceSceneWorld* ()
    {
        return (IRayTraceSceneWorld*)m_pSelf;
    }
    inline void BeginBuild()
    {
        ((void(*)(void* self))g_engine_nativeFunctions[2092])(m_pSelf);
    }
    inline void AddSceneWorldToBuild(ISceneWorld* pWorld, IRenderContext* pRenderContext)
    {
        ((void(*)(void* self, ISceneWorld * pWorld, IRenderContext * pRenderContext))g_engine_nativeFunctions[2093])(m_pSelf, pWorld, pRenderContext);
    }
    inline int EndBuild(IRenderContext* pRenderContext, CRenderAttributes* attrs)
    {
        return
            ((int(*)(void* self, IRenderContext * pRenderContext, CRenderAttributes * attrs))g_engine_nativeFunctions[2094])(m_pSelf, pRenderContext, attrs);
    }
    inline int BuildTLASForWorld(ISceneWorld* pWorld, CRenderAttributes* attrs)
    {
        return
            ((int(*)(void* __self, ISceneWorld * __pWorld, CRenderAttributes * __attrs))g_engine_nativeFunctions[2095])(m_pSelf, pWorld, attrs);
    }
};

class g_pWorldRendererMgr
{
public:
    void* m_pSelf;
    inline operator g_pWorldRendererMgr* ()
    {
        return (g_pWorldRendererMgr*)m_pSelf;
    }
    inline void ServiceWorldRequests()
    {
        ((void(*)())g_engine_nativeFunctions[1564])();
    }
    inline unsigned int UpdateObjectsForRendering(uint32 worldGroupId, Vector4 eyePos, float flLODScale, float flMaxVisibleDistance)
    {
        return 1;
    }
    inline const IWorldReference* CreateWorld(const char* pMapName, ISceneWorld* pSceneWorld, int bAsyncLoad, int bIgnoreExistingWorlds, int bLoadVis, int bPrecacheOnly, uint32 worldGroupId, CTransformUnaligned transform)
    {
        return
            ((const IWorldReference * (*)(const char* __pMapName, ISceneWorld * __pSceneWorld, int __bAsyncLoad, int __bIgnoreExistingWorlds, int __bLoadVis, int __bPrecacheOnly, uint32 __worldGroupId, CTransformUnaligned __transform))g_engine_nativeFunctions[1566])(pMapName, pSceneWorld, bAsyncLoad, bIgnoreExistingWorlds, bLoadVis, bPrecacheOnly, worldGroupId, transform);
    }
    inline int MountWorldVPK(const char* pWorldName, const char* pVpkPath)
    {
        return
            ((int(*)(const char* pWorldName, const char* pVpkPath))g_engine_nativeFunctions[1567])(pWorldName, pVpkPath);
    }
    inline int UnmountWorldVPK(const char* pWorldName)
    {
        return
            ((int(*)(const char* pWorldName))g_engine_nativeFunctions[1568])(pWorldName);
    }
};

class g_pEnginePVSManager
{
public:
    void* m_pSelf;
    inline operator g_pEnginePVSManager* ()
    {
        return (g_pEnginePVSManager*)m_pSelf;
    }
    inline const IPVS* BuildPvs(IWorldReference* world)
    {
        return
            ((const IPVS * (*)(IWorldReference * world))g_engine_nativeFunctions[1418])(world);
    }
    inline void DestroyPvs(IPVS* pvs)
    {
        ((void(*)(IPVS * pvs))g_engine_nativeFunctions[1419])(pvs);
    }
};

class IRenderContext
{
public:
    virtual ~IRenderContext() = default;

    virtual void Draw(RenderPrimitiveType_t, int, int) = 0;
    virtual void DrawInstanced(RenderPrimitiveType_t, int, int, int) = 0;
    virtual void DrawIndexed(RenderPrimitiveType_t, int, int, int, int) = 0;
    virtual void DrawIndexedInstanced(RenderPrimitiveType_t, int, int, int, int, int) = 0;

    virtual void DrawInstancedIndirect(RenderPrimitiveType_t, RenderBufferHandle_t, uint32_t) = 0;
    virtual void DrawIndexedInstancedIndirect(RenderPrimitiveType_t, RenderBufferHandle_t, uint32_t) = 0;

    virtual void TextureBarrier(int, ITexture*, RenderImageLayout_t*) = 0;
    virtual void UAVBarrier(int, RenderUAVBarrier_t*) = 0;
    virtual void BufferBarrier(const RenderBufferBarrier_t&) = 0;

    virtual void Clear(Vector4, RenderClearFlags_t) = 0;
    virtual void SetCameraPosition(const Vector3& position) = 0;
    virtual void SetCameraRotation(const QAngle& rotation) = 0;
    virtual void SetProjection(float fov, float aspect, float zNear, float zFar) = 0;


    virtual void BindRenderTargets(const RenderTargetDesc_t&) = 0;

    virtual void SetViewports(int, const RenderViewport_t*) = 0;
    virtual void GetViewport(RenderViewport_t*, int) = 0;

    virtual void Submit() = 0;

    virtual bool BindIndexBuffer(IndexBufferHandle_t, int) = 0;
    virtual bool BindIndexBuffer(RenderBufferHandle_t, int, int) = 0;

    virtual bool BindVertexBuffer(int, VertexBufferHandle_t, int) = 0;
    virtual bool BindVertexBuffer(int, VertexBufferHandle_t, int, int) = 0;

    virtual void BindVertexShader(RenderShaderHandle_t, void*) = 0;
    virtual void BindShader(RenderShaderType_t, RenderShaderHandle_t) = 0;

    virtual void* GetInputLayoutForVertexBuffer(VertexBufferHandle_t) = 0;

    virtual ConstantBufferHandle_t LockDynamicConstantBuffer(int, DynamicLockDesc_t* lock) = 0;

    virtual void UnlockDynamicConstantBuffer(ConstantBufferHandle_t, int) = 0;

    virtual void BindConstantBuffer(RenderShaderType_t, ConstantBufferHandle_t, int) = 0;

    virtual void SetSamplerStatePS(int, RsFilter_t, RsTextureAddressMode_t, RsTextureAddressMode_t, RsTextureAddressMode_t) = 0;

    virtual void BindTexture(int, ITexture, RenderTextureDimension_t, RenderShaderType_t, RenderColorSpace_t) = 0;

    virtual void ReadTexturePixels(ITexture, IReadTexturePixelsCallback*, Rect_t*, int, int, bool) = 0;

    virtual void ReadBuffer(RenderBufferHandle_t, IReadBufferCallback*, int, int, bool) = 0;

    virtual void BeginPIXEvent(unsigned long color, const char* szName) {}
    virtual void EndPIXEvent() {}
    virtual void SetPIXMarker(unsigned long color, const char* szName) {}

    void TextureBarrierTransition(
        ITexture hSrc,
        int /*mips*/,
        RenderBarrierPipelineStageFlags_t srcStage,
        RenderBarrierPipelineStageFlags_t dstStage,
        RenderImageLayout_t layout,
        RenderBarrierAccessFlags_t srcFlags,
        RenderBarrierAccessFlags_t dstFlags)
    {
        RenderUAVBarrier_t barrier{
            hSrc, srcStage, dstStage, srcFlags, dstFlags, -1, -1, layout };

        TextureBarrier(1, &hSrc, &layout);
        UAVBarrier(1, &barrier);
    }

    void BufferBarrierTransition(
        RenderBufferHandle_t hSrc,
        RenderBarrierPipelineStageFlags_t srcStage,
        RenderBarrierPipelineStageFlags_t dstStage,
        RenderBarrierAccessFlags_t srcFlags,
        RenderBarrierAccessFlags_t dstFlags)
    {
        BufferBarrier({ hSrc, srcStage, dstStage, srcFlags, dstFlags });
    }

    void Clear(Vector4 col, bool clearColor, bool clearDepth, bool clearStencil)
    {
        int flags = 0;
        if (clearColor) flags |= RENDER_CLEAR_FLAGS_CLEAR_COLOR;
        if (clearDepth) flags |= RENDER_CLEAR_FLAGS_CLEAR_DEPTH;
        if (clearStencil) flags |= RENDER_CLEAR_FLAGS_CLEAR_STENCIL;

        Clear(col, (RenderClearFlags_t)flags);
    }

    void SetViewport(int x, int y, int w, int h)
    {
        RenderViewport_t vp;
        vp.Init(x, y, w, h);
        SetViewports(1, &vp);
    }

    // Dispersion only
    virtual void SetModelTransform(const Vector3& position, const QAngle& rotation, const Vector3& scale) = 0;
    // Dispersion end

    RenderViewport_t GetViewport()
    {
        RenderViewport_t vp;
        GetViewport(&vp, 0);
        return vp;
    }

    void BindPixelShader(RenderShaderHandle_t shader)
    {
        BindShader(RENDER_PIXEL_SHADER, shader);
    }

    void SetDynamicConstantBufferData(RenderShaderType_t type, void* data, int size, int slot)
    {
        DynamicLockDesc_t lock;
        auto h = LockDynamicConstantBuffer(size, &lock);

        if (lock.m_pMemory)
        {
            memcpy(lock.m_pMemory, data, size);
            UnlockDynamicConstantBuffer(h, size);
            BindConstantBuffer(type, h, slot);
        }
    }

    void BindTexture(int slot, ITexture tex)
    {
        SetSamplerStatePS(0,
            RS_FILTER_MIN_MAG_MIP_POINT,
            RS_TEXTURE_ADDRESS_WRAP,
            RS_TEXTURE_ADDRESS_WRAP,
            RS_TEXTURE_ADDRESS_WRAP);

        BindTexture(slot, tex,
            RENDER_TEXTURE_DIMENSION_2D,
            RENDER_PIXEL_SHADER,
            RENDER_SRGB);
    }
};


class g_pSceneSystem
{
public:
    void* m_pSelf;
    intptr_t CreateWorld(const char* debugName);
    void DestroyWorld(ISceneWorld* world);
};

class CFrustum {};

class CViewSetup
{
public:
    Vector position;
    Vector angle;
    float fov;
    float aspectRatio;
    float zNear;
    float zFar;
    float cameraX;
    float cameraY;
    float cameraZ;
    Vector camPos;
};

struct ViewHistory {
    int ViewHash;
    CFrustum Frustum;
    ViewHistory Get(int);
    void Store(ViewHistory);
    ViewHistory();
    ~ViewHistory();
} ;

struct SkyboxRenderSetup {
    Vector m_Origin;
    QAngle m_Angles;
    Vector m_Offset;
    float m_Scale;
    ISceneWorld* m_World;
    CFrustum m_Frustum;
};

struct Camera_t
{
public:
    void InitViewParameters(
        const Vector& origin,
        const QAngle& angles)
    {
    }

    void InitPerspectiveProjection(
        float flNear,
        float flFar,
        float flFOV,
        float flAspect)
    {
    }

    void InitOrthoProjection(
        float flNear,
        float flFar,
        float flWidth,
        float flHeight)
    {
    }

    void Init(
        const Vector& origin,
        const QAngle& angles,
        float flNear,
        float flFar,
        float flFOV,
        float flAspect)
    {
    }

    void InitOrtho(
        const Vector& origin,
        const QAngle& angles,
        float flNear,
        float flFar,
        float flWidth,
        float flHeight)
    {
    }
};

class CCameraRenderer
{
public:
    void* m_pSelf;
    inline operator CCameraRenderer* ()
    {
        return (CCameraRenderer*)m_pSelf;
    }
    inline void DeleteThis();
    CCameraRenderer(const char* name, int cameraId);
    ~CCameraRenderer();
    inline void ClearSceneWorlds();
    void CreateView(const RenderViewport_t& viewport, SwapChainHandle_t swapChain, ISceneView* parentView);
    inline void AddSceneWorld(ISceneWorld* world);
    inline void SetRenderAttributes(CRenderAttributes* attributes);
    inline void Render(SwapChainHandle_t targetSwapChain);
    inline void RenderToTexture(HRenderTextureStrong* hTexture, ISceneView* parentView);
    inline void RenderToCubeTexture(HRenderTextureStrong* hTexture, int nSlice);
    inline void RenderToBitmap(void* pixels, int width, int height, int bytesPerPixel);
    inline void RenderStereo(int eye, int eyeWidth, int eyeHeight, int bSubmitThisEye);
    inline void SubmitStereo(int eyeWidth, int eyeHeight);
    inline void BlitStereo(int eyeWidth, int eyeHeight);
    inline void ClearRenderTags();
    inline void ClearExcludeTags();
    inline void AddRenderTag(unsigned int hash);
    inline void AddExcludeTag(unsigned int hash);
    int ViewUniqueId, EnablePostprocessing, EnableEngineOverlays, Ortho;
    float OrthoSize;
    int IsRenderingStereo, HasOverrideProjection, FlipX, FlipY;
    Vector CameraPosition, Viewport, ClipSpaceBounds, MiddleEyePosition, MiddleEyeRotation;
    QAngle CameraRotation;
    float FieldOfView, ZNear, ZFar;
    int64 SceneViewFlags;
    Rect_t Rect;
    Quaternion OverrideProjection;
};

class g_pEngineServiceMgr
{
public:
    void* m_pSelf;
    inline operator g_pEngineServiceMgr* ()
    {
        return (g_pEngineServiceMgr*)m_pSelf;
    }
    inline SwapChainHandle_t GetEngineSwapChain()
    {
        return (SwapChainHandle_t)1;
    }
    inline void GetEngineSwapChainSize(int* w, int* h)
    {
        ((void(*)(int* w, int* h))g_engine_nativeFunctions[1421])(w, h);
    }
    inline void SetEngineState(void* hWnd, SwapChainHandle_t hSwapChain)
    {
        ((void(*)(void* __hWnd, SwapChainHandle_t __hSwapChain))g_engine_nativeFunctions[1422])(hWnd, hSwapChain);
    }
    inline void ExitMainLoop()
    {
        ((void(*)())g_engine_nativeFunctions[1423])();
    }
};
#endif // IRENDERCONTEXT_H
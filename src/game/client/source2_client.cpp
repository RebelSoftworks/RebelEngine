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
#include "source2_client.h"
#include "source2_clientconfig.h"
#include "maploader.h"
#include "murmur2.h"
#include "interfaces.h"
#include "mathlib/vector.h"
#include "rendersystem/irenderdevice.h"
#include "rendersystem/iviewrender.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

CDefaultGameConfig g_sDefualtClientConfig;
ISource2ClientConfig* g_pSource2ClientConfig = &g_sDefualtClientConfig;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CDefaultGameConfig, ISource2ClientConfig, SOURCE2CLIENTCONFIG_INTERFACE_VERSION, g_sDefualtClientConfig);

CSource2Client g_sSource2Client;
ISource2Client* g_pSource2Client = &g_sSource2Client;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSource2Client, ISource2Client, SOURCE2CLIENT_INTERFACE_VERSION, g_sSource2Client);

static CGlobalVarsBase dummyvars( true );
CGlobalVarsBase *gpGlobals = &dummyvars;

class g_pSceneSystem* g_pSceneSystem = new class g_pSceneSystem;
class g_pEnginePVSManager* g_pEnginePVSManager = new class g_pEnginePVSManager;
class g_pWorldRendererMgr* g_pWorldRendererMgr = new class g_pWorldRendererMgr;
class g_pEngineServiceMgr* g_pEngineServiceMgr = new class g_pEngineServiceMgr;

extern IRenderDeviceMgr* g_pRenderDeviceManager;
extern IRenderDevice* g_Device;

bool CSource2Client::Connect(CreateInterfaceFn factory) {
	
	Msg("CSource2Client::Connect -- Connected\n");

	return true;
}

void CSource2Client::Disconnect() {

}

void CSource2Client::Init(CreateInterfaceFn factory)
{
    Msg("CSource2Client::Init -- Begin\n");

    pViewRendering->Init();

    Msg("CSource2Client::Init -- End\n");
}

void CSource2Client::Shutdown(long long par1) {

	Msg("CSource2Client::Shutdown");

}

// TODO: Move this maploader to engine2?
class CMapLoader : public IMapLoader
{
public:
    bool LoadMap(const char* szMap) override;
    const MapWorld_t* GetWorld() const override;
    bool UploadToGPU(IRenderDevice* pDevice) override;
    RenderBufferHandle_t GetVertexBuffer() const override;
    int GetVertexCount() const override;

private:
    bool ParseMap(std::istream& stream);
    bool ParseWorld(std::istream& stream);
    bool ParseVertices(std::istream& stream);
    bool ParseIndices(std::istream& stream);
    bool SkipBlock(std::istream& stream);

private:
    MapWorld_t m_World;
    std::vector<MapVertex_t> m_RenderVertices;
    RenderBufferHandle_t m_VertexBuffer = 0;
};

static CMapLoader s_MapLoader;

IMapLoader* MapLoader()
{
    return &s_MapLoader;
}

// tiny helper
static std::string GetGameRoot()
{
    HMODULE hClient = GetModuleHandleA("client.dll");

    if (!hClient)
        return {};

    char szPath[MAX_PATH]{};

    DWORD length = GetModuleFileNameA(hClient, szPath, MAX_PATH);

    if (!length || length >= MAX_PATH)
        return {};

    PathRemoveFileSpecA(szPath);
    PathRemoveFileSpecA(szPath);
    PathRemoveFileSpecA(szPath);

    return szPath;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CMapLoader, IMapLoader, MAP_LOADER_INTERFACE_VERSION, s_MapLoader);

static bool ReadToken(std::istream& stream, std::string& token)
{
    token.clear();

    char c;

    while (stream.get(c))
    {
        if (c == '#')
        {
            while (stream.get(c))
            {
                if (c == '\n')
                    break;
            }

            continue;
        }

        if (!std::isspace(static_cast<unsigned char>(c)))
            break;
    }

    if (!stream)
        return false;

    if (c == '{' || c == '}')
    {
        token.push_back(c);
        return true;
    }

    if (c == '"')
    {
        while (stream.get(c))
        {
            if (c == '"')
                return true;

            token.push_back(c);
        }

        return false;
    }

    token.push_back(c);

    while (stream.get(c))
    {
        if (std::isspace(static_cast<unsigned char>(c)) ||
            c == '{' ||
            c == '}')
        {
            if (c == '{' || c == '}')
                stream.unget();

            break;
        }

        token.push_back(c);
    }

    return true;
}

bool CMapLoader::LoadMap(const char* szMap)
{
    if (!szMap)
        return false;

    std::string mapPath = GetGameRoot() + "\\" + szMap;

    std::ifstream file(mapPath);

    if (!file.is_open())
    {
        Msg("CMapLoader::LoadMap: Failed to open %s\n", szMap);
        return false;
    }

    m_World = MapWorld_t{};
    m_RenderVertices.clear();

    if (!ParseMap(file))
    {
        Msg("CMapLoader::LoadMap: Failed to parse %s\n", szMap);
        return false;
    }

    for (const MapTriangle_t& triangle : m_World.triangles)
    {
        if (triangle.i0 >= m_World.vertices.size() ||
            triangle.i1 >= m_World.vertices.size() ||
            triangle.i2 >= m_World.vertices.size())
        {
            Msg("CMapLoader::LoadMap: Invalid triangle index\n");
            return false;
        }

        m_RenderVertices.push_back(m_World.vertices[triangle.i0]);
        m_RenderVertices.push_back(m_World.vertices[triangle.i1]);
        m_RenderVertices.push_back(m_World.vertices[triangle.i2]);
    }

    Msg(
        "CMapLoader::LoadMap: Loaded %s (%zu vertices, %zu triangles)\n",
        szMap,
        m_World.vertices.size(),
        m_World.triangles.size()
    );

    return true;
}

bool CMapLoader::ParseMap(std::istream& stream)
{
    std::string token;

    if (!ReadToken(stream, token))
        return false;

    if (token != "DispersionMap")
        return false;

    if (!ReadToken(stream, token))
        return false;

    if (token != "{")
        return false;

    while (ReadToken(stream, token))
    {
        if (token == "}")
            break;

        if (token == "version")
        {
            if (!ReadToken(stream, token))
                return false;

            continue;
        }

        if (token == "world")
        {
            if (!ParseWorld(stream))
                return false;

            continue;
        }

        if (token == "entity")
        {
            if (!SkipBlock(stream))
                return false;

            continue;
        }

        return false;
    }

    return !m_World.vertices.empty() && !m_World.triangles.empty();
}

bool CMapLoader::ParseWorld(std::istream& stream)
{
    std::string token;

    if (!ReadToken(stream, token))
        return false;

    if (token != "{")
        return false;

    while (ReadToken(stream, token))
    {
        if (token == "}")
            return true;

        if (token == "name")
        {
            if (!ReadToken(stream, m_World.name))
                return false;

            continue;
        }

        if (token == "material")
        {
            if (!ReadToken(stream, m_World.material))
                return false;

            continue;
        }

        if (token == "vertices")
        {
            if (!ParseVertices(stream))
                return false;

            continue;
        }

        if (token == "indices")
        {
            if (!ParseIndices(stream))
                return false;

            continue;
        }

        if (token == "entity")
        {
            if (!SkipBlock(stream))
                return false;

            continue;
        }

        return false;
    }

    return false;
}

bool CMapLoader::ParseVertices(std::istream& stream)
{
    std::string token;

    if (!ReadToken(stream, token))
        return false;

    if (token != "{")
        return false;

    while (ReadToken(stream, token))
    {
        if (token == "}")
            return true;

        if (token != "v")
            return false;

        std::string sx;
        std::string sy;
        std::string sz;

        if (!ReadToken(stream, sx))
            return false;

        if (!ReadToken(stream, sy))
            return false;

        if (!ReadToken(stream, sz))
            return false;

        MapVertex_t vertex{};

        vertex.x = std::strtof(sx.c_str(), nullptr);
        vertex.y = std::strtof(sy.c_str(), nullptr);
        vertex.z = std::strtof(sz.c_str(), nullptr);

        m_World.vertices.push_back(vertex);
    }

    return false;
}

bool CMapLoader::ParseIndices(std::istream& stream)
{
    std::string token;

    if (!ReadToken(stream, token))
        return false;

    if (token != "{")
        return false;

    while (ReadToken(stream, token))
    {
        if (token == "}")
            return true;

        if (token != "i")
            return false;

        std::string s0;
        std::string s1;
        std::string s2;

        if (!ReadToken(stream, s0))
            return false;

        if (!ReadToken(stream, s1))
            return false;

        if (!ReadToken(stream, s2))
            return false;

        MapTriangle_t triangle{};

        triangle.i0 = static_cast<uint32_t>(std::strtoul(s0.c_str(), nullptr, 10));
        triangle.i1 = static_cast<uint32_t>(std::strtoul(s1.c_str(), nullptr, 10));
        triangle.i2 = static_cast<uint32_t>(std::strtoul(s2.c_str(), nullptr, 10));

        m_World.triangles.push_back(triangle);
    }

    return false;
}

bool CMapLoader::SkipBlock(std::istream& stream)
{
    std::string token;

    if (!ReadToken(stream, token))
        return false;

    if (token != "{")
        return false;

    int depth = 1;

    while (depth > 0 && ReadToken(stream, token))
    {
        if (token == "{")
            ++depth;
        else if (token == "}")
            --depth;
    }

    return depth == 0;
}

bool CMapLoader::UploadToGPU(IRenderDevice* pDevice)
{
    if (!pDevice)
        return false;

    if (m_RenderVertices.empty())
        return false;

    if (m_VertexBuffer)
    {
        pDevice->DestroyGPUBuffer(m_VertexBuffer);
        m_VertexBuffer = 0;
    }

    BufferDesc_t desc{};

    m_VertexBuffer = pDevice->CreateGPUBuffer(
        RENDER_BUFFER_TYPE_VERTEX,
        desc,
        static_cast<RenderBufferFlags_t>(0),
        "DispersionMapVertexBuffer"
    );

    if (!m_VertexBuffer)
        return false;

    if (!pDevice->UploadGPUBuffer(
        m_VertexBuffer,
        m_RenderVertices.data(),
        static_cast<uint32>(m_RenderVertices.size() * sizeof(MapVertex_t))
    ))
    {
        pDevice->DestroyGPUBuffer(m_VertexBuffer);
        m_VertexBuffer = 0;
        return false;
    }

    Msg(
        "CMapLoader::UploadToGPU: Uploaded %zu vertices\n",
        m_RenderVertices.size()
    );

    return true;
}

const MapWorld_t* CMapLoader::GetWorld() const
{
    return &m_World;
}

RenderBufferHandle_t CMapLoader::GetVertexBuffer() const
{
    return m_VertexBuffer;
}

int CMapLoader::GetVertexCount() const
{
    return static_cast<int>(m_RenderVertices.size());
}

// Renderer handling
class CViewRendering : public IViewRendering
{
public:
	CCameraRenderer* pCameraRenderer;
	CRenderAttributes* pRenderAttributes;

	virtual void Init() override;
	virtual void Frame(SwapChainHandle_t hSwapChain) override;
};

void CCameraRenderer::AddSceneWorld(ISceneWorld* pWorld)
{
}

void CCameraRenderer::ClearExcludeTags()
{
}

void CCameraRenderer::ClearRenderTags()
{
}

void CCameraRenderer::Render(SwapChainHandle_t swapChain)
{
}

void CCameraRenderer::SetRenderAttributes(CRenderAttributes* attributes)
{
}


void CViewRendering::Init()
{
    pCameraRenderer = new CCameraRenderer("RenderToSwapChain", 1);
    pRenderAttributes = new CRenderAttributes;

    if (!g_Device)
    {
        Msg("CViewRendering::Init: render device is null\n");
        return;
    }

    Msg("CViewRendering::Init: renderer initialized\n");
}

void CViewRendering::Frame(SwapChainHandle_t hSwapChain)
{
}

static CViewRendering s_viewRendering;
IViewRendering* pViewRendering = &s_viewRendering;

// stub here:
ISource2ServerConfig* g_pSource2ServerConfig = nullptr;
ISource2GameEntities* g_sSource2GameEntities = nullptr;
IInputSystem* g_pInputSystem = nullptr;
ISource2GameClients* g_pGameClients = nullptr;
IInputStackSystem* g_pInputStackSystem = nullptr;
ISource2Server* g_pSource2Server = nullptr;

DLL_EXPORT bool BSecureAllowed(unsigned char* a1, int a2, int a3)
{
	return true;
}

DLL_EXPORT int BinaryProperties_GetValue(int a1, void* a2)
{
	return 0;
}

DLL_EXPORT int CountFilesCompletedTrustCheck()
{
	return 0;
}

DLL_EXPORT int CountFilesNeedTrustCheck()
{
	return 0;
}

DLL_EXPORT int CountItemsToReport()
{
	return 0;
}

DLL_EXPORT int GetTotalFilesLoaded()
{
	return 0;
}
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

#include <types.h>
#include <string>
#include <vector>
#include <map>
#include <shlwapi.h>
#include <Windows.h>
#include <mutex>
#include "../public/rendersystem/irenderdevice.h"
#include "mathlib/vector.h"
#include <fstream>
#include "../public/game/client/isource2_client.h"
#include "../public/game/client/isource2_clientprediction.h"
#include <sstream>
#include <cstring>
#include "../public/appframework/iappframework.h"
#include "interfaces/interfaces.h"
#include <unordered_map>
#include <maploader.h>
#include <gameui/igameui.h>
#include <imodelloader.h>
#include "gametimer.h"
#include <windowsx.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "opengl32.lib")

static std::mutex g_LogMutex;
static std::mutex g_EngineMutex;
static std::vector<std::string> g_Spew;
static std::map<std::string, std::string> g_Engine2Config;
static std::unordered_map<std::string, bool> g_engine2Bools;
static bool g_bLogging = false;
static HWND g_hWnd = nullptr;
static HDC g_hDC = nullptr;
static HGLRC g_hGLRC = nullptr;
static bool g_bRunning = true;
static bool g_KeyW = false;
static bool g_KeyA = false;
static bool g_KeyS = false;
static bool g_KeyD = false;
static bool g_KeyUp = false;
static bool g_KeyDown = false;
static bool g_KeyLeft = false;
static bool g_KeyRight = false;
static Vector3 g_CameraPosition{
    128.0f,
    128.0f,
    400.0f
};
static QAngle g_CameraRotation{
    0.0f,
    0.0f,
    0.0f
};
static float g_PlayerMaxSpeed = 320.0f;
static float g_PlayerAcceleration = 3000.0f;
static float g_PlayerFriction = 12.0f;
static float g_MouseDeltaX = 0.0f;
static float g_MouseDeltaY = 0.0f;

static bool g_MouseCaptured = false;
static bool g_IgnoreMouseMove = false;
static Vector3 g_PlayerVelocity{
    0.0f,
    0.0f,
    0.0f
};

static float g_MouseSensitivity = 0.07f;
IGameUI* g_pGameUI = nullptr;
CGameTimer g_GameTimer;


static LRESULT CALLBACK EngineWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (g_pGameUI)
    {
        g_pGameUI->ProcessMessage(
            uMsg,
            wParam,
            lParam
        );
    }

    switch (uMsg)
    {
    case WM_CLOSE:
    {
        g_bRunning = false;
        DestroyWindow(hWnd);
        return 0;
    }

    case WM_DESTROY:
    {
        g_bRunning = false;
        PostQuitMessage(0);
        return 0;
    }

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_ESCAPE:
            g_bRunning = false;
            DestroyWindow(hWnd);
            break;
        case VK_UP:
            g_KeyUp = true;
            break;

        case VK_DOWN:
            g_KeyDown = true;
            break;

        case VK_LEFT:
            g_KeyLeft = true;
            break;

        case VK_RIGHT:
            g_KeyRight = true;
            break;
        case 'W':
            g_KeyW = true;
            break;

        case 'A':
            g_KeyA = true;
            break;

        case 'S':
            g_KeyS = true;
            break;

        case 'D':
            g_KeyD = true;
            break;
        }

        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (!g_MouseCaptured)
            break;

        if (g_IgnoreMouseMove)
        {
            g_IgnoreMouseMove = false;
            return 0;
        }

        const int mouseX = GET_X_LPARAM(lParam);
        const int mouseY = GET_Y_LPARAM(lParam);

        RECT clientRect{};
        GetClientRect(g_hWnd, &clientRect);

        const int centerX =
            (clientRect.right - clientRect.left) / 2;

        const int centerY =
            (clientRect.bottom - clientRect.top) / 2;

        g_MouseDeltaX +=
            static_cast<float>(mouseX - centerX);

        g_MouseDeltaY +=
            static_cast<float>(mouseY - centerY);
        POINT centerPoint{
            centerX,
            centerY
        };

        ClientToScreen(
            g_hWnd,
            &centerPoint
        );

        g_IgnoreMouseMove = true;

        SetCursorPos(
            centerPoint.x,
            centerPoint.y
        );

        return 0;
    }
    case WM_KEYUP:
    {
        switch (wParam)
        {
        case VK_UP:
            g_KeyUp = false;
            break;

        case VK_DOWN:
            g_KeyDown = false;
            break;

        case VK_LEFT:
            g_KeyLeft = false;
            break;

        case VK_RIGHT:
            g_KeyRight = false;
            break;
        case 'W':
            g_KeyW = false;
            break;

        case 'A':
            g_KeyA = false;
            break;

        case 'S':
            g_KeyS = false;
            break;

        case 'D':
            g_KeyD = false;
            break;
        }

        return 0;
    }
    }

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

static bool CreateOpenGLWindow(HINSTANCE hInstance)
{
    WNDCLASSA wc{};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = EngineWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Return to Ravenholm";

    if (!RegisterClassA(&wc))
        return false;

    g_hWnd = CreateWindowExA(0, wc.lpszClassName, "Return to Ravenholm", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        1280, 720, nullptr, nullptr, hInstance, nullptr);

    if (!g_hWnd)
        return false;

    g_hDC = GetDC(g_hWnd);

    if (!g_hDC)
        return false;

    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    const int pixelFormat = ChoosePixelFormat(g_hDC, &pfd);

    if (!pixelFormat)
        return false;

    if (!SetPixelFormat(g_hDC, pixelFormat, &pfd))
        return false;

    g_hGLRC = wglCreateContext(g_hDC);

    if (!g_hGLRC)
        return false;

    if (!wglMakeCurrent(g_hDC, g_hGLRC))
        return false;

    ShowWindow(g_hWnd, SW_SHOW);
    UpdateWindow(g_hWnd);

    return true;
}

static void DestroyOpenGLWindow()
{
    if (g_hGLRC)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(g_hGLRC);
        g_hGLRC = nullptr;
    }

    if (g_hDC && g_hWnd)
    {
        ReleaseDC(g_hWnd, g_hDC);
        g_hDC = nullptr;
    }

    if (g_hWnd)
    {
        DestroyWindow(g_hWnd);
        g_hWnd = nullptr;
    }
}

bool Source2PreInit(CMaterialSystem2AppSystemDict& appDict, const char* pModName) {
    (void)appDict;
    if (pModName) g_Engine2Config["mod"] = pModName;
    return true;
}

ISource2Client* g_pSource2Client;
IPrediction2* g_pSource2ClientPrediction;
ISource2ClientConfig* g_pSource2ClientConfig; // TBD: Load this interface
IRenderDevice* g_Device;
IRenderContext* g_RenderContext;
RenderBufferHandle_t g_TriangleBuffer = 0;
IMapLoader* g_pMapLoader = nullptr;

bool ClientDLL_Load()
{
    char szRoot[MAX_PATH];
    char szClientPath[MAX_PATH];
    char szBinPath[MAX_PATH];

    GetModuleFileNameA(nullptr, szRoot, MAX_PATH);
    PathRemoveFileSpecA(szRoot);

    snprintf(
        szBinPath,
        sizeof(szBinPath),
        "%s\\bin\\win64",
        szRoot
    );

    SetDllDirectoryA(szBinPath);

    snprintf(
        szClientPath,
        sizeof(szClientPath),
        "%s\\vr\\bin\\win64\\client.dll",
        szRoot
    );

    HMODULE hClient = LoadLibraryExA(
        szClientPath,
        nullptr,
        LOAD_WITH_ALTERED_SEARCH_PATH
    );

    if (!hClient)
    {
        char szError[512];

        snprintf(
            szError,
            sizeof(szError),
            "LoadLibraryExA failed.\n\n"
            "Path:\n%s\n\n"
            "Error: %lu",
            szClientPath,
            GetLastError()
        );

        MessageBoxA(nullptr, szError, "Failed to load client.dll", MB_OK | MB_ICONERROR);
        return false;
    }

    auto CreateInterface =
        reinterpret_cast<CreateInterfaceFn>(
            GetProcAddress(hClient, "CreateInterface"));

    if (!CreateInterface)
    {
        MessageBoxA(nullptr,
            "Failed to get CreateInterface export.",
            "Error",
            MB_OK | MB_ICONERROR);

        FreeLibrary(hClient);
        return false;
    }

    void* pRawInterface =
        CreateInterface(SOURCE2CLIENT_INTERFACE_VERSION, nullptr);

    if (!pRawInterface)
    {
        MessageBoxA(nullptr,
            "Failed to get ISource2Client interface.",
            "Error",
            MB_OK | MB_ICONERROR);

        FreeLibrary(hClient);
        return false;
    }

    void* pRawInterface2 = CreateInterface(RENDER_DEVICE_INTERFACE_VERSION, nullptr);

    if (!pRawInterface2)
    {
        MessageBoxA(nullptr,
            "Failed to get IRenderDevice interface.",
            "Error",
            MB_OK | MB_ICONERROR);

        FreeLibrary(hClient);
        return false;
    }

	void* pRawInterface3 = CreateInterface(MAP_LOADER_INTERFACE_VERSION, nullptr);

	if (!pRawInterface3)
	{
		MessageBoxA(nullptr,
			"Failed to get IMapLoader interface.",
			"Error",
			MB_OK | MB_ICONERROR);
		FreeLibrary(hClient);
		return false;
	}

    g_pSource2Client = reinterpret_cast<ISource2Client*>(pRawInterface);

    g_Device = reinterpret_cast<IRenderDevice*>(pRawInterface2);

	g_pMapLoader = reinterpret_cast<IMapLoader*>(pRawInterface3);

    printf("Client Interface successfully mapped to address: %p\n",
        g_pSource2Client);

    printf("Renderer Interface successfully mapped to address: %p\n",
        g_Device);

    printf("MapLoader Interface successfuly mapped to address: %p\n", g_pMapLoader);

    g_pSource2Client->Init(CreateInterface);

    return true;
}

IMapLoader* MapLoader()
{
	return g_pMapLoader;
}

ISource2Server* g_pSource2Server;
ISource2ServerConfig* g_pSource2ServerConfig;
ISource2GameEntities* g_sSource2GameEntities;

bool ServerDLL_Load()
{
    char szRoot[MAX_PATH];
    char szClientPath[MAX_PATH];
    char szBinPath[MAX_PATH];

    GetModuleFileNameA(nullptr, szRoot, MAX_PATH);
    PathRemoveFileSpecA(szRoot);

    snprintf(
        szBinPath,
        sizeof(szBinPath),
        "%s\\bin\\win64",
        szRoot
    );

    SetDllDirectoryA(szBinPath);

    snprintf(
        szClientPath,
        sizeof(szClientPath),
        "%s\\vr\\bin\\win64\\server.dll",
        szRoot
    );

    HMODULE hClient = LoadLibraryExA(
        szClientPath,
        nullptr,
        LOAD_WITH_ALTERED_SEARCH_PATH
    );

    if (!hClient)
    {
        char szError[512];

        snprintf(
            szError,
            sizeof(szError),
            "LoadLibraryExA failed.\n\n"
            "Path:\n%s\n\n"
            "Error: %lu",
            szClientPath,
            GetLastError()
        );

        MessageBoxA(nullptr, szError, "Failed to load server.dll", MB_OK | MB_ICONERROR);
        return false;
    }

    auto CreateInterface =
        reinterpret_cast<CreateInterfaceFn>(
            GetProcAddress(hClient, "CreateInterface"));

    if (!CreateInterface)
    {
        MessageBoxA(nullptr,
            "Failed to get CreateInterface export.",
            "Error",
            MB_OK | MB_ICONERROR);

        FreeLibrary(hClient);
        return false;
    }

    void* pRawInterface =
        CreateInterface(SOURCE2SERVER_INTERFACE_VERSION, nullptr);

    if (!pRawInterface)
    {
        MessageBoxA(nullptr,
            "Failed to get ISource2Server interface.",
            "Error",
            MB_OK | MB_ICONERROR);

        FreeLibrary(hClient);
        return false;
    }

    g_pSource2Server = reinterpret_cast<ISource2Server*>(pRawInterface);

    printf("Server Interface successfully mapped to address: %p\n",
        g_pSource2Server);

    return true;
}

bool GameUIDLL_Load()
{
    char szRoot[MAX_PATH];
    char szClientPath[MAX_PATH];
    char szBinPath[MAX_PATH];

    GetModuleFileNameA(nullptr, szRoot, MAX_PATH);
    PathRemoveFileSpecA(szRoot);

    snprintf(
        szBinPath,
        sizeof(szBinPath),
        "%s\\bin\\win64",
        szRoot
    );

    SetDllDirectoryA(szBinPath);

    snprintf(
        szClientPath,
        sizeof(szClientPath),
        "%s\\vr\\bin\\win64\\gameui.dll",
        szRoot
    );

    HMODULE hClient = LoadLibraryExA(
        szClientPath,
        nullptr,
        LOAD_WITH_ALTERED_SEARCH_PATH
    );

    if (!hClient)
    {
        char szError[512];

        snprintf(
            szError,
            sizeof(szError),
            "LoadLibraryExA failed.\n\n"
            "Path:\n%s\n\n"
            "Error: %lu",
            szClientPath,
            GetLastError()
        );

        MessageBoxA(nullptr, szError, "Failed to load gameui.dll", MB_OK | MB_ICONERROR);
        return false;
    }

    auto CreateInterface =
        reinterpret_cast<CreateInterfaceFn>(
            GetProcAddress(hClient, "CreateInterface"));

    if (!CreateInterface)
    {
        MessageBoxA(nullptr,
            "Failed to get CreateInterface export.",
            "Error",
            MB_OK | MB_ICONERROR);

        FreeLibrary(hClient);
        return false;
    }

    void* pRawInterface =
        CreateInterface(GAMEUI_INTERFACE_VERSION, nullptr);

    if (!pRawInterface)
    {
        MessageBoxA(nullptr,
            "Failed to get IGameUI interface.",
            "Error",
            MB_OK | MB_ICONERROR);

        FreeLibrary(hClient);
        return false;
    }

    g_pGameUI = reinterpret_cast<IGameUI*>(pRawInterface);

    printf("GameUI Interface successfully mapped to address: %p\n",
        g_pGameUI);

    return true;
}

extern IModelLoader* ModelLoader();
Model_t g_ViewModel;

bool RenderTest()
{
    if (!g_Device)
        return false;

    g_RenderContext = g_Device->CreateRenderContext(0);

    if (!g_RenderContext)
    {
        Msg("RenderTest: failed to create render context\n");
        return false;
    }

    if (!MapLoader()->LoadMap("maps/dispersion_test.dmap"))
    {
        Msg("RenderTest: failed to load map\n");
        return false;
    }

    if (!MapLoader()->UploadToGPU(g_Device))
    {
        Msg("RenderTest: failed to upload map\n");
        return false;
    }

    char cwd[MAX_PATH];

    if (GetCurrentDirectoryA(MAX_PATH, cwd))
    {
        Msg("Current directory: %s\n", cwd);
    }

    if (!ModelLoader()->Load("vr/models/viewmodel.vmdl", g_ViewModel))
    {
        Msg("RenderTest: failed to load model\n");
    }

    if (!ModelLoader()->Upload(g_ViewModel, g_Device))
    {
        Msg("RenderTest: failed to upload model to GPU\n");
    }

    return true;
}

static void Accelerate(
    const Vector3& wishDirection,
    float wishSpeed,
    float acceleration,
    float deltaTime)
{
    const float currentSpeed =
        VectorDot(
            g_PlayerVelocity,
            wishDirection
        );

    const float addSpeed =
        wishSpeed - currentSpeed;

    if (addSpeed <= 0.0f)
        return;

    float accelerationSpeed =
        acceleration *
        deltaTime;

    if (accelerationSpeed > addSpeed)
        accelerationSpeed = addSpeed;

    g_PlayerVelocity =
        VectorAdd(
            g_PlayerVelocity,
            VectorMultiply(
                wishDirection,
                accelerationSpeed
            )
        );
}

static Vector3 GetForwardVector(const QAngle& angles)
{
    constexpr float DEG_TO_RAD =
        0.017453292519943295769f;

    const float yaw =
        angles.y * DEG_TO_RAD;

    const float cy = std::cos(yaw);
    const float sy = std::sin(yaw);

    return Vector3{
        -sy,
        0.0f,
        -cy
    };
}

static Vector3 GetRightVector(const QAngle& angles)
{
    constexpr float DEG_TO_RAD =
        0.017453292519943295769f;

    const float yaw =
        angles.y * DEG_TO_RAD;

    const float cy = std::cos(yaw);
    const float sy = std::sin(yaw);

    return Vector3{
        cy,
        0.0f,
        -sy
    };
}

static void ApplyFriction(float deltaTime)
{
    Vector3 horizontalVelocity{
        g_PlayerVelocity.x,
        0.0f,
        g_PlayerVelocity.z
    };

    const float speed =
        VectorLength(horizontalVelocity);

    if (speed <= 0.0f)
    {
        g_PlayerVelocity.x = 0.0f;
        g_PlayerVelocity.z = 0.0f;
        return;
    }

    const float drop =
        speed *
        g_PlayerFriction *
        deltaTime;

    float newSpeed =
        speed - drop;

    if (newSpeed < 0.0f)
        newSpeed = 0.0f;

    if (newSpeed < 1.0f)
        newSpeed = 0.0f;

    const float scale =
        newSpeed / speed;

    g_PlayerVelocity.x *= scale;
    g_PlayerVelocity.z *= scale;
}

static void UpdateCamera()
{
    if (!g_RenderContext)
        return;

    const float deltaTime = g_GameTimer.GetDeltaTime();

    g_CameraRotation.y +=
        g_MouseDeltaX *
        g_MouseSensitivity;

    g_CameraRotation.x +=
        g_MouseDeltaY *
        g_MouseSensitivity;

    if (g_CameraRotation.x > 89.0f)
        g_CameraRotation.x = 89.0f;

    if (g_CameraRotation.x < -89.0f)
        g_CameraRotation.x = -89.0f;

    while (g_CameraRotation.y > 180.0f)
        g_CameraRotation.y -= 360.0f;

    while (g_CameraRotation.y < -180.0f)
        g_CameraRotation.y += 360.0f;

    g_RenderContext->SetCameraRotation(g_CameraRotation);


    Vector3 forward =
        GetForwardVector(
            g_CameraRotation
        );

    Vector3 right =
        GetRightVector(
            g_CameraRotation
        );

    forward.y = 0.0f;

    forward = VectorNormalize(forward);

    Vector3 wishDirection{
        0.0f,
        0.0f,
        0.0f
    };

    if (g_KeyW)
    {
        wishDirection =
            VectorAdd(
                wishDirection,
                forward
            );
    }

    if (g_KeyS)
    {
        wishDirection.x -= forward.x;
        wishDirection.z -= forward.z;
    }

    if (g_KeyD)
    {
        wishDirection =
            VectorAdd(
                wishDirection,
                right
            );
    }

    if (g_KeyA)
    {
        wishDirection.x -= right.x;
        wishDirection.z -= right.z;
    }

    const float wishLength =
        VectorLength(wishDirection);

    if (wishLength > 0.00001f)
    {
        wishDirection =VectorMultiply(wishDirection, 1.0f / wishLength);

        Accelerate( wishDirection, g_PlayerMaxSpeed,
            g_PlayerAcceleration, deltaTime);
    }
    else
    {
        ApplyFriction(deltaTime);
        // look at my abs
        if (std::abs(g_PlayerVelocity.x) < 2.0f)
            g_PlayerVelocity.x = 0.0f;

        if (std::abs(g_PlayerVelocity.z) < 2.0f)
            g_PlayerVelocity.z = 0.0f;
    }

    g_CameraPosition.x += g_PlayerVelocity.x * deltaTime;

    g_CameraPosition.z += g_PlayerVelocity.z * deltaTime;

    g_RenderContext->SetCameraPosition(g_CameraPosition);

    g_MouseDeltaX = 0.0f;
    g_MouseDeltaY = 0.0f;
}

[[nodiscard]] void RenderViewModel()
{
    if (!g_RenderContext)
        return;

    if (!g_Device)
        return;

    if (g_ViewModel.meshes.empty())
        return;

    Vector2 size =
        g_Device->GetBackbufferDimensions(
            SwapChainHandle_t{}
        );

    if (size.y <= 0.0f)
        return;

    const float aspect =
        size.x / size.y;

    g_RenderContext->SetProjection(
        54.0f,
        aspect,
        1.0f,
        1000.0f
    );


    //g_RenderContext->SetModelTransform(
    //    Vector3{
    //        g_CameraPosition.x,
    //        g_CameraPosition.y,
    //        g_CameraPosition.z - 10.0f
    //    },
    //    QAngle{
    //        0.0f,
    //        0.0f,
    //        0.0f
    //    },
    //    Vector3{
    //        1.0f,
    //        1.0f,
    //        1.0f
    //    }
    //);

    g_RenderContext->SetModelTransform(
        Vector3{
            g_CameraPosition.x,
            g_CameraPosition.y,
            g_CameraPosition.z - 10.0f
        },
        QAngle{
            0.0f,
            180.0f,
            0.0f
        },
        Vector3{ 0.25f, 0.25f, 0.25f }
    );

    for (ModelMesh_t& mesh : g_ViewModel.meshes)
    {
        if (!mesh.vertexBuffer)
            continue;

        if (mesh.vertices.empty())
            continue;

        if (!g_RenderContext->BindVertexBuffer(
            0,
            static_cast<VertexBufferHandle_t>(
                mesh.vertexBuffer
                ),
            sizeof(ModelVertex_t)
        ))
        {
            continue;
        }

        if (
            mesh.indexBuffer &&
            !mesh.indices.empty())
        {
            if (!g_RenderContext->BindIndexBuffer(
                static_cast<IndexBufferHandle_t>(
                    mesh.indexBuffer
                    ),
                0
            ))
            {
                continue;
            }

            g_RenderContext->DrawIndexed(
                RENDER_PRIM_TRIANGLES,
                static_cast<int>(
                    mesh.indices.size()
                    ),
                0,
                0,
                0
            );
        }
        else
        {
            g_RenderContext->Draw(
                RENDER_PRIM_TRIANGLES,
                static_cast<int>(
                    mesh.vertices.size()
                    ),
                0
            );
        }
    }

    g_RenderContext->SetProjection(
        68.0f,
        aspect,
        1.0f,
        10000.0f
    );

    g_RenderContext->SetModelTransform(
        Vector3{
            0.0f,
            0.0f,
            0.0f
        },
        QAngle{
            0.0f,
            0.0f,
            0.0f
        },
        Vector3{
            1.0f,
            1.0f,
            1.0f
        }
    );
}

void RenderMap()
{
    if (!g_Device || !g_RenderContext)
        return;

    if (!MapLoader()->GetVertexBuffer())
        return;

    Vector2 size = g_Device->GetBackbufferDimensions(SwapChainHandle_t{});

    g_RenderContext->SetCameraPosition(g_CameraPosition);

    float aspect = size.x / size.y;

    g_RenderContext->SetProjection(68.0f, aspect, 1.0f, 10000.0f);

    RenderViewport_t viewport{};

    viewport.Init(
        0,
        0,
        static_cast<int>(size.x),
        static_cast<int>(size.y)
    );

    g_RenderContext->SetViewports(1, &viewport);

    g_RenderContext->Clear(Vector4{ 0.05f, 0.05f, 0.05f, 1.0f },
        static_cast<RenderClearFlags_t>(RENDER_CLEAR_FLAGS_CLEAR_COLOR |
    RENDER_CLEAR_FLAGS_CLEAR_DEPTH | RENDER_CLEAR_FLAGS_CLEAR_STENCIL));

    g_RenderContext->BindVertexBuffer(0, static_cast<VertexBufferHandle_t>(MapLoader()->GetVertexBuffer()), sizeof(MapVertex_t));

    g_RenderContext->Draw(RENDER_PRIM_TRIANGLES, MapLoader()->GetVertexCount(), 0);

    RenderViewModel();

    g_RenderContext->Submit();

    // main menu will be handled in launcher
    //if (g_pGameUI)
    //{
    //    g_pGameUI->BeginFrame(
    //        size.x,
    //        size.y
    //    );

    //    g_pGameUI->Draw();

    //    g_pGameUI->Render(
    //        g_RenderContext
    //    );

    //    g_pGameUI->EndFrame();
    //}

    g_Device->Present(SwapChainHandle_t{});
}

bool Source2Init(CMaterialSystem2AppSystemDict& appDict)
{
    ClientDLL_Load();
    ServerDLL_Load();
    GameUIDLL_Load();
	return true;
}

void Source2Shutdown(void) {
}

static void CaptureMouse()
{
    if (g_MouseCaptured)
        return;

    g_MouseCaptured = true;

    ShowCursor(FALSE);

    RECT clientRect{};
    GetClientRect(g_hWnd, &clientRect);

    const int centerX =
        (clientRect.right - clientRect.left) / 2;

    const int centerY =
        (clientRect.bottom - clientRect.top) / 2;

    POINT centerPoint{
        centerX,
        centerY
    };

    ClientToScreen(
        g_hWnd,
        &centerPoint
    );

    g_IgnoreMouseMove = true;

    SetCursorPos(
        centerPoint.x,
        centerPoint.y
    );

    SetCapture(g_hWnd);
}

static void ReleaseMouse()
{
    if (!g_MouseCaptured)
        return;

    g_MouseCaptured = false;

    ReleaseCapture();

    ShowCursor(TRUE);
}

DLL_EXPORT int Source2Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd, const char* szBaseDir, const char* szGame)
{
	CMaterialSystem2AppSystemDict appDict = {};
	appDict.m_nThreadCount = 4;
	appDict.m_RenderFactory = nullptr;
    appDict.m_WindowTitle = "Return to Ravenholm";
	appDict.m_WindowIcon = nullptr;
	appDict.m_DefaultRenderSystemOption = "gl";
	appDict.m_unk1224 = false;
	appDict.m_WindowInitialImage = nullptr;
	appDict.m_bUnk1240 = false;
	appDict.m_bInitWithoutMaterialSystem = false;
	appDict.m_bEnableExtendedMaterialInfo = false;
	appDict.m_bDefaultToRendersystemEmpty = false;
	appDict.m_bDiscardMouseFocusClick = false;
	appDict.m_bRenderSystemOptionOnCommandLine = false;
	
    if (!Source2Init(appDict))
        return -1;

    if (!CreateOpenGLWindow(hInstance))
    {
        Source2Shutdown();
        return -1;
    }

    CaptureMouse();

    g_GameTimer.Init();

    if (!RenderTest())
    {
        DestroyOpenGLWindow();
        Source2Shutdown();
        return -1;
    }

    if (!g_pGameUI->Init(g_hWnd, g_Device))
    {
        DestroyOpenGLWindow();
        Source2Shutdown();
        return -1;
    }

    if (!g_pGameUI->LoadLayout("vr/panorama/mainmenu.vxml"))
    {
        Msg(
            "Failed to load main menu\n"
        );
    }

    MSG msg{};

    while (g_bRunning)
    {
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                g_bRunning = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        if (!g_bRunning)
            break;

        g_GameTimer.BeginFrame();
        UpdateCamera();
        RenderMap();
    }

    DestroyOpenGLWindow();
    Source2Shutdown();

    return 0;
}

// stub here:
IRenderDeviceMgr* g_pRenderDeviceManager = nullptr;
IRenderUtils* g_pRenderUtils = nullptr;
IInputSystem* g_pInputSystem = nullptr;
ISource2GameClients* g_pGameClients = nullptr;
IInputStackSystem* g_pInputStackSystem = nullptr;

DLL_EXPORT bool BSecureAllowed(unsigned char* a1, int a2, int a3)
{
    return true;
}

DLL_EXPORT int BinmaryProperties_GetValue(int a1, void* a2)
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
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

#include "igameui.h"
#include "../thirdparty/imgui/imgui.h"
#include "../public/rendersystem/irenderdevice.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <GL/gl.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#define IM_OFFSETOF(_TYPE,_MEMBER) ((size_t)&(((_TYPE*)0)->_MEMBER))

#pragma comment(lib, "opengl32.lib")

static GLuint g_ImGuiFontTexture = 0;

class ConVar;

struct ConsoleCommand
{
    const char* name;
    const char* help;
    void (*callback)(const std::vector<std::string>& args);
};

static std::vector<ConVar*> g_ConVars;
static std::vector<ConsoleCommand> g_Commands;

class ConVar
{
public:
    ConVar(
        const char* pName,
        const char* pDefaultValue,
        const char* pHelp = "")
        : m_Name(pName ? pName : "")
        , m_Value(pDefaultValue ? pDefaultValue : "")
        , m_Help(pHelp ? pHelp : "")
    {
        g_ConVars.push_back(this);
    }

    const char* GetName() const
    {
        return m_Name.c_str();
    }

    const char* GetString() const
    {
        return m_Value.c_str();
    }

    int GetInt() const
    {
        return std::atoi(m_Value.c_str());
    }

    float GetFloat() const
    {
        return static_cast<float>(
            std::atof(m_Value.c_str())
            );
    }

    bool GetBool() const
    {
        if (
            m_Value == "1" ||
            m_Value == "true" ||
            m_Value == "yes" ||
            m_Value == "on")
        {
            return true;
        }

        return false;
    }

    const char* GetHelp() const
    {
        return m_Help.c_str();
    }

    void SetValue(const char* pValue)
    {
        m_Value = pValue ? pValue : "";
    }

    void SetValue(int value)
    {
        m_Value = std::to_string(value);
    }

    void SetValue(float value)
    {
        m_Value = std::to_string(value);
    }

    void SetValue(bool value)
    {
        m_Value = value ? "1" : "0";
    }

    static ConVar* Find(const char* pName)
    {
        if (!pName)
            return nullptr;

        for (ConVar* pConVar : g_ConVars)
        {
            if (_stricmp(
                pConVar->GetName(),
                pName) == 0)
            {
                return pConVar;
            }
        }

        return nullptr;
    }

private:
    std::string m_Name;
    std::string m_Value;
    std::string m_Help;
};

static ConVar cv_host_name(
    "host_name",
    "Dispersion",
    "Server host name"
);

static ConVar cv_cl_showfps(
    "cl_showfps",
    "0",
    "Show FPS"
);

static ConVar cv_fov(
    "fov",
    "90",
    "Player field of view"
);

static ConVar cv_r_drawentities(
    "r_drawentities",
    "1",
    "Render entities"
);

static ConVar cv_r_wireframe(
    "r_wireframe",
    "0",
    "Render wireframe"
);

static bool CreateImGuiFontTexture()
{
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels = nullptr;
    int width = 0;
    int height = 0;

    io.Fonts->GetTexDataAsRGBA32(
        &pixels,
        &width,
        &height
    );

    if (!pixels || width <= 0 || height <= 0)
        return false;

    glGenTextures(
        1,
        &g_ImGuiFontTexture
    );

    if (!g_ImGuiFontTexture)
        return false;

    glBindTexture(
        GL_TEXTURE_2D,
        g_ImGuiFontTexture
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glPixelStorei(
        GL_UNPACK_ALIGNMENT,
        1
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels
    );

    io.Fonts->SetTexID(
        g_ImGuiFontTexture
    );

    return glGetError() == GL_NO_ERROR;
}

static void DestroyImGuiFontTexture()
{
    if (g_ImGuiFontTexture)
    {
        glDeleteTextures(
            1,
            &g_ImGuiFontTexture
        );

        g_ImGuiFontTexture = 0;
    }

    if (ImGui::GetCurrentContext())
    {
        ImGui::GetIO().Fonts->SetTexID(
            nullptr
        );
    }
}

static void RenderImGuiOpenGL(
    ImDrawData* drawData)
{
    if (!drawData)
        return;

    int framebufferWidth =
        static_cast<int>(
            drawData->DisplaySize.x *
            drawData->FramebufferScale.x
            );

    int framebufferHeight =
        static_cast<int>(
            drawData->DisplaySize.y *
            drawData->FramebufferScale.y
            );

    if (
        framebufferWidth <= 0 ||
        framebufferHeight <= 0)
    {
        return;
    }

    glPushAttrib(
        GL_ENABLE_BIT |
        GL_COLOR_BUFFER_BIT |
        GL_TRANSFORM_BIT |
        GL_SCISSOR_BIT |
        GL_TEXTURE_BIT |
        GL_VIEWPORT_BIT
    );

    glViewport(
        0,
        0,
        framebufferWidth,
        framebufferHeight
    );

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(
        0.0,
        drawData->DisplaySize.x,
        drawData->DisplaySize.y,
        0.0,
        -1.0,
        1.0
    );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnableClientState(
        GL_VERTEX_ARRAY
    );

    glEnableClientState(
        GL_TEXTURE_COORD_ARRAY
    );

    glEnableClientState(
        GL_COLOR_ARRAY
    );

    for (
        int listIndex = 0;
        listIndex < drawData->CmdListsCount;
        ++listIndex)
    {
        const ImDrawList* cmdList =
            drawData->CmdLists[listIndex];

        const ImDrawVert* vertices =
            cmdList->VtxBuffer.Data;

        const ImDrawIdx* indices =
            cmdList->IdxBuffer.Data;

        glVertexPointer(
            2,
            GL_FLOAT,
            sizeof(ImDrawVert),
            reinterpret_cast<const char*>(vertices) +
            IM_OFFSETOF(ImDrawVert, pos)
        );

        glTexCoordPointer(
            2,
            GL_FLOAT,
            sizeof(ImDrawVert),
            reinterpret_cast<const char*>(vertices) +
            IM_OFFSETOF(ImDrawVert, uv)
        );

        glColorPointer(
            4,
            GL_UNSIGNED_BYTE,
            sizeof(ImDrawVert),
            reinterpret_cast<const char*>(vertices) +
            IM_OFFSETOF(ImDrawVert, col)
        );

        unsigned int indexOffset = 0;

        for (
            int commandIndex = 0;
            commandIndex < cmdList->CmdBuffer.Size;
            ++commandIndex)
        {
            const ImDrawCmd* command =
                &cmdList->CmdBuffer[commandIndex];

            if (command->UserCallback)
            {
                command->UserCallback(
                    cmdList,
                    command
                );
            }
            else
            {
                ImVec4 clipRect =
                    command->ClipRect;

                int clipX =
                    static_cast<int>(clipRect.x);

                int clipY =
                    static_cast<int>(
                        drawData->DisplaySize.y -
                        clipRect.w
                        );

                int clipWidth =
                    static_cast<int>(
                        clipRect.z -
                        clipRect.x
                        );

                int clipHeight =
                    static_cast<int>(
                        clipRect.w -
                        clipRect.y
                        );

                if (
                    clipWidth > 0 &&
                    clipHeight > 0)
                {
                    glScissor(
                        clipX,
                        clipY,
                        clipWidth,
                        clipHeight
                    );

                    GLuint texture =
                        command->GetTexID();

                    glBindTexture(
                        GL_TEXTURE_2D,
                        texture
                    );

                    glDrawElements(
                        GL_TRIANGLES,
                        static_cast<GLsizei>(
                            command->ElemCount
                            ),
                        sizeof(ImDrawIdx) == 2
                        ? GL_UNSIGNED_SHORT
                        : GL_UNSIGNED_INT,
                        indices + indexOffset
                    );
                }
            }

            indexOffset +=
                command->ElemCount;
        }
    }

    glDisableClientState(
        GL_VERTEX_ARRAY
    );

    glDisableClientState(
        GL_TEXTURE_COORD_ARRAY
    );

    glDisableClientState(
        GL_COLOR_ARRAY
    );

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
}

class CGameUI : public IGameUI
{
public:
    CGameUI();
    ~CGameUI();

    bool Init(
        HWND hWnd,
        IRenderDevice* pDevice) override;

    void Shutdown() override;

    void ProcessMessage(
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    void BeginFrame(
        float width,
        float height);

    void Draw() override;

    void Render(
        IRenderContext* pContext) override;

    void EndFrame() override;

    bool LoadLayout(
        const char* pFilename) override;

    bool WantsKeyboard() const;
    bool WantsMouse() const;

private:
    void DrawConsole();

    void ExecuteCommand(
        const char* pCommand);

    void Print(
        const char* pText);

    void Printf(
        const char* pFormat,
        ...);

    void ExecuteSet(
        const std::vector<std::string>& args);

    void ExecuteGet(
        const std::vector<std::string>& args);

    void ExecuteHelp(
        const std::vector<std::string>& args);

    void ExecuteClear(
        const std::vector<std::string>& args);

    void ExecuteQuit(
        const std::vector<std::string>& args);

    void ExecuteEcho(
        const std::vector<std::string>& args);

    static void CommandSet(
        const std::vector<std::string>& args);

    static void CommandGet(
        const std::vector<std::string>& args);

    static void CommandHelp(
        const std::vector<std::string>& args);

    static void CommandClear(
        const std::vector<std::string>& args);

    static void CommandQuit(
        const std::vector<std::string>& args);

    static void CommandEcho(
        const std::vector<std::string>& args);

private:
    HWND m_hWnd;
    IRenderDevice* m_pDevice;

    bool m_bInitialized;
    bool m_bConsoleVisible;

    LARGE_INTEGER m_Frequency;
    LARGE_INTEGER m_LastTime;

    char m_CommandBuffer[512];

    std::vector<std::string> m_Output;
    std::vector<std::string> m_History;

    int m_HistoryPosition;
};

CGameUI g_GameUI;
IGameUI* g_pGameUI = &g_GameUI;

static CGameUI* g_pConsole = nullptr;

static std::vector<std::string> Tokenize(
    const char* pCommand)
{
    std::vector<std::string> tokens;

    if (!pCommand)
        return tokens;

    std::string current;
    bool quoted = false;

    for (
        const char* p = pCommand;
        *p;
        ++p)
    {
        char character = *p;

        if (character == '"')
        {
            quoted = !quoted;
            continue;
        }

        if (
            character == ' ' ||
            character == '\t')
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }

            continue;
        }

        current += character;
    }

    if (!current.empty())
        tokens.push_back(current);

    return tokens;
}

CGameUI::CGameUI()
    : m_hWnd(nullptr)
    , m_pDevice(nullptr)
    , m_bInitialized(false)
    , m_bConsoleVisible(true)
    , m_HistoryPosition(-1)
{
    m_Frequency.QuadPart = 0;
    m_LastTime.QuadPart = 0;

    m_CommandBuffer[0] = '\0';

    g_pConsole = this;
}

CGameUI::~CGameUI()
{
    Shutdown();

    if (g_pConsole == this)
        g_pConsole = nullptr;
}

bool CGameUI::Init(
    HWND hWnd,
    IRenderDevice* pDevice)
{
    if (m_bInitialized)
        return true;

    if (!hWnd)
        return false;

    m_hWnd = hWnd;
    m_pDevice = pDevice;

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io =
        ImGui::GetIO();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;

    io.DisplaySize =
        ImVec2(1280.0f, 720.0f);

    ImGui::StyleColorsDark();

    if (!CreateImGuiFontTexture())
    {
        ImGui::DestroyContext();
        return false;
    }

    QueryPerformanceFrequency(
        &m_Frequency
    );

    QueryPerformanceCounter(
        &m_LastTime
    );

    g_Commands.push_back({
        "set",
        "Set a ConVar",
        &CGameUI::CommandSet
        });

    g_Commands.push_back({
        "get",
        "Get a ConVar",
        &CGameUI::CommandGet
        });

    g_Commands.push_back({
        "help",
        "List console commands and ConVars",
        &CGameUI::CommandHelp
        });

    g_Commands.push_back({
        "clear",
        "Clear the console",
        &CGameUI::CommandClear
        });

    g_Commands.push_back({
        "quit",
        "Quit the game",
        &CGameUI::CommandQuit
        });

    g_Commands.push_back({
        "echo",
        "Print text to the console",
        &CGameUI::CommandEcho
        });

    Print(
        "========================================"
    );

    Print(
        " Dispersion Engine Console"
    );

    Print(
        " Type 'help' for available commands."
    );

    Print(
        "========================================"
    );

    Print(
        ""
    );

    m_bInitialized = true;

    return true;
}

void CGameUI::Shutdown()
{
    if (!m_bInitialized)
        return;

    DestroyImGuiFontTexture();

    ImGui::DestroyContext();

    m_hWnd = nullptr;
    m_pDevice = nullptr;

    m_bInitialized = false;
}

void CGameUI::ProcessMessage(
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    if (!m_bInitialized)
        return;

    ImGuiIO& io =
        ImGui::GetIO();

    switch (message)
    {
    case WM_MOUSEMOVE:
        io.AddMousePosEvent(
            static_cast<float>(
                GET_X_LPARAM(lParam)
                ),
            static_cast<float>(
                GET_Y_LPARAM(lParam)
                )
        );
        break;

    case WM_LBUTTONDOWN:
        io.AddMouseButtonEvent(
            0,
            true
        );
        break;

    case WM_LBUTTONUP:
        io.AddMouseButtonEvent(
            0,
            false
        );
        break;

    case WM_RBUTTONDOWN:
        io.AddMouseButtonEvent(
            1,
            true
        );
        break;

    case WM_RBUTTONUP:
        io.AddMouseButtonEvent(
            1,
            false
        );
        break;

    case WM_MBUTTONDOWN:
        io.AddMouseButtonEvent(
            2,
            true
        );
        break;

    case WM_MBUTTONUP:
        io.AddMouseButtonEvent(
            2,
            false
        );
        break;

    case WM_MOUSEWHEEL:
        io.AddMouseWheelEvent(
            0.0f,
            static_cast<float>(
                GET_WHEEL_DELTA_WPARAM(wParam)
                ) /
            static_cast<float>(WHEEL_DELTA)
        );
        break;

    case WM_MOUSEHWHEEL:
        io.AddMouseWheelEvent(
            static_cast<float>(
                GET_WHEEL_DELTA_WPARAM(wParam)
                ) /
            static_cast<float>(WHEEL_DELTA),
            0.0f
        );
        break;

    case WM_CHAR:
        if (
            wParam > 0 &&
            wParam < 0x10000)
        {
            io.AddInputCharacter(
                static_cast<unsigned int>(
                    wParam
                    )
            );
        }
        break;
    }
}

void CGameUI::BeginFrame(
    float width,
    float height)
{
    if (!m_bInitialized)
        return;

    LARGE_INTEGER currentTime{};

    QueryPerformanceCounter(
        &currentTime
    );

    double deltaSeconds =
        static_cast<double>(
            currentTime.QuadPart -
            m_LastTime.QuadPart
            ) /
        static_cast<double>(
            m_Frequency.QuadPart
            );

    m_LastTime =
        currentTime;

    ImGuiIO& io =
        ImGui::GetIO();

    io.DisplaySize =
        ImVec2(width, height);

    io.DisplayFramebufferScale =
        ImVec2(1.0f, 1.0f);

    io.DeltaTime =
        deltaSeconds > 0.0
        ? static_cast<float>(
            deltaSeconds
            )
        : 1.0f / 60.0f;

    ImGui::NewFrame();
}

void CGameUI::Draw()
{
    if (!m_bInitialized)
        return;

    if (!m_bConsoleVisible)
        return;

    DrawConsole();
}

void CGameUI::DrawConsole()
{
    ImGuiIO& io =
        ImGui::GetIO();

    ImGui::SetNextWindowPos(
        ImVec2(0.0f, 0.0f),
        ImGuiCond_Always
    );

    ImGui::SetNextWindowSize(
        ImVec2(
            io.DisplaySize.x,
            io.DisplaySize.y * 0.55f
        ),
        ImGuiCond_Always
    );

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;

    if (!ImGui::Begin(
        "##DispersionConsole",
        nullptr,
        flags))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginChild(
        "##ConsoleOutput",
        ImVec2(
            0.0f,
            -ImGui::GetFrameHeightWithSpacing()
        ),
        true))
    {
        for (const std::string& line : m_Output)
        {
            ImGui::TextUnformatted(
                line.c_str()
            );
        }

        if (ImGui::GetScrollY() >=
            ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }

    ImGui::EndChild();

    ImGuiInputTextFlags inputFlags =
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_CallbackHistory;

    if (ImGui::InputText(
        "##ConsoleInput",
        m_CommandBuffer,
        sizeof(m_CommandBuffer),
        inputFlags,
        [](ImGuiInputTextCallbackData* data)
        {
            if (
                data->EventFlag !=
                ImGuiInputTextFlags_CallbackHistory)
            {
                return 0;
            }

            CGameUI* console =
                g_pConsole;

            if (!console ||
                console->m_History.empty())
            {
                return 0;
            }

            if (
                data->EventKey ==
                ImGuiKey_UpArrow)
            {
                if (
                    console->m_HistoryPosition ==
                    -1)
                {
                    console->m_HistoryPosition =
                        static_cast<int>(
                            console->m_History.size()
                            ) - 1;
                }
                else if (
                    console->m_HistoryPosition > 0)
                {
                    --console->m_HistoryPosition;
                }
            }
            else if (
                data->EventKey ==
                ImGuiKey_DownArrow)
            {
                if (
                    console->m_HistoryPosition !=
                    -1)
                {
                    ++console->m_HistoryPosition;

                    if (
                        console->m_HistoryPosition >=
                        static_cast<int>(
                            console->m_History.size()
                            ))
                    {
                        console->m_HistoryPosition =
                            -1;

                        data->Buf[0] =
                            '\0';

                        data->BufTextLen = 0;
                        data->BufDirty = true;

                        return 0;
                    }
                }
            }

            if (
                console->m_HistoryPosition >= 0 &&
                console->m_HistoryPosition <
                static_cast<int>(
                    console->m_History.size()
                    ))
            {
                const std::string& command =
                    console->m_History[
                        console->m_HistoryPosition
                    ];

                strncpy_s(
                    data->Buf,
                    data->BufSize,
                    command.c_str(),
                    _TRUNCATE
                );

                data->BufTextLen =
                    static_cast<int>(
                        strlen(data->Buf)
                        );

                data->BufDirty = true;
            }

            return 0;
        }))
    {
        if (m_CommandBuffer[0] != '\0')
        {
            ExecuteCommand(
                m_CommandBuffer
            );

            m_History.push_back(
                m_CommandBuffer
            );

            m_HistoryPosition = -1;

            m_CommandBuffer[0] =
                '\0';
        }
    }

    ImGui::SetItemDefaultFocus();

    ImGui::End();
}

void CGameUI::ExecuteCommand(
    const char* pCommand)
{
    if (!pCommand ||
        !pCommand[0])
    {
        return;
    }

    std::vector<std::string> args =
        Tokenize(pCommand);

    if (args.empty())
        return;

    Printf(
        "] %s",
        pCommand
    );

    for (const ConsoleCommand& command :
        g_Commands)
    {
        if (
            _stricmp(
                command.name,
                args[0].c_str()
            ) == 0)
        {
            command.callback(args);
            return;
        }
    }

    ConVar* pConVar =
        ConVar::Find(
            args[0].c_str()
        );

    if (pConVar)
    {
        if (args.size() == 1)
        {
            Printf(
                "\"%s\" = \"%s\" - %s",
                pConVar->GetName(),
                pConVar->GetString(),
                pConVar->GetHelp()
            );

            return;
        }

        pConVar->SetValue(
            args[1].c_str()
        );

        Printf(
            "\"%s\" changed to \"%s\"",
            pConVar->GetName(),
            pConVar->GetString()
        );

        return;
    }

    Printf(
        "Unknown command or ConVar: %s",
        args[0].c_str()
    );
}

void CGameUI::Print(
    const char* pText)
{
    if (!pText)
        return;

    m_Output.emplace_back(
        pText
    );

    if (m_Output.size() > 1024)
    {
        m_Output.erase(
            m_Output.begin()
        );
    }
}

void CGameUI::Printf(const char* pFormat, ...)
{
    if (!pFormat)
        return;

    char buffer[2048];

    va_list args;

    va_start(
        args,
        pFormat
    );

    vsnprintf_s(
        buffer,
        sizeof(buffer),
        _TRUNCATE,
        pFormat,
        args
    );

    va_end(args);

    Print(buffer);
}

void CGameUI::CommandSet(
    const std::vector<std::string>& args)
{
    if (!g_pConsole)
        return;

    if (args.size() < 3)
    {
        g_pConsole->Print(
            "Usage: set <variable> <value>"
        );

        return;
    }

    ConVar* pConVar =
        ConVar::Find(
            args[1].c_str()
        );

    if (!pConVar)
    {
        g_pConsole->Printf(
            "Unknown ConVar: %s",
            args[1].c_str()
        );

        return;
    }

    pConVar->SetValue(
        args[2].c_str()
    );

    g_pConsole->Printf(
        "\"%s\" changed to \"%s\"",
        pConVar->GetName(),
        pConVar->GetString()
    );
}

void CGameUI::CommandGet(
    const std::vector<std::string>& args)
{
    if (!g_pConsole)
        return;

    if (args.size() < 2)
    {
        g_pConsole->Print(
            "Usage: get <variable>"
        );

        return;
    }

    ConVar* pConVar =
        ConVar::Find(
            args[1].c_str()
        );

    if (!pConVar)
    {
        g_pConsole->Printf(
            "Unknown ConVar: %s",
            args[1].c_str()
        );

        return;
    }

    g_pConsole->Printf(
        "\"%s\" = \"%s\" - %s",
        pConVar->GetName(),
        pConVar->GetString(),
        pConVar->GetHelp()
    );
}

void CGameUI::CommandHelp(const std::vector<std::string>& args)
{

    if (!g_pConsole)
        return;

    g_pConsole->Print("--- Commands ---");

    for (const ConsoleCommand& command : g_Commands)
    {
        g_pConsole->Printf("%s - %s", command.name, command.help);
    }

    g_pConsole->Print("--- ConVars ---");

    for (ConVar* pConVar : g_ConVars)
    {
        g_pConsole->Printf(
            "%s = %s - %s",
            pConVar->GetName(),
            pConVar->GetString(),
            pConVar->GetHelp()
        );
    }
}

void CGameUI::CommandClear(const std::vector<std::string>& args)
{
    if (!g_pConsole)
        return;

    g_pConsole->m_Output.clear();
}

void CGameUI::CommandQuit(const std::vector<std::string>& args)
{
    if (!g_pConsole)
        return;

    g_pConsole->Print("Shutting down...");

    PostQuitMessage(0);
}

void CGameUI::CommandEcho(const std::vector<std::string>& args)
{
    if (!g_pConsole)
        return;

    if (args.size() < 2)
        return;

    std::string text;

    for (size_t i = 1; i < args.size(); ++i)
    {
        if (!text.empty())
            text += ' ';

        text += args[i];
    }

    g_pConsole->Print(
        text.c_str()
    );
}

void CGameUI::Render(IRenderContext* pContext)
{
    (void)pContext;

    if (!m_bInitialized)
        return;

    ImGui::Render();

    RenderImGuiOpenGL(ImGui::GetDrawData());
}

void CGameUI::EndFrame()
{
}

bool CGameUI::LoadLayout(const char* pFilename)
{
    return true;
}

bool CGameUI::WantsKeyboard() const
{
    return m_bInitialized &&
        m_bConsoleVisible &&
        ImGui::GetIO().WantCaptureKeyboard;
}

bool CGameUI::WantsMouse() const
{
    return m_bInitialized &&
        m_bConsoleVisible &&
        ImGui::GetIO().WantCaptureMouse;
}

ISource2Client* g_pSource2Client = nullptr;
ISource2ClientConfig* g_pSource2ClientConfig = nullptr;
IRenderDeviceMgr* g_pRenderDeviceManager = nullptr;
IRenderUtils* g_pRenderUtils = nullptr;
IRenderDevice* g_Device = nullptr;
IPrediction2* g_pSource2ClientPrediction = nullptr;
ISource2ServerConfig* g_pSource2ServerConfig = nullptr;
ISource2GameEntities* g_sSource2GameEntities = nullptr;
IInputSystem* g_pInputSystem = nullptr;
ISource2GameClients* g_pGameClients = nullptr;
IInputStackSystem* g_pInputStackSystem = nullptr;
ISource2Server* g_pSource2Server = nullptr;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION, g_GameUI);
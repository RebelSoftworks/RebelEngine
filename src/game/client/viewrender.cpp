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
#include "../../public/rendersystem/irenderdevice.h"
#include "../../public/rendersystem/irenderutils.h"
#include <interfaces.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma comment(lib, "opengl32.lib") // -lGL in Linux/OSX/whatever the fuck
#endif // _WIN32
#include <GL/gl.h>

#include <vector>
#include <cstdint>
#include <cstring>
#include <cstdio>

static GLenum GLPrimitive(RenderPrimitiveType_t type)
{
    switch (type)
    {
    case RENDER_PRIM_TRIANGLES:
        return GL_TRIANGLES;

    case RENDER_PRIM_LINES:
        return GL_LINES;

    default:
        return GL_TRIANGLES;
    }
}

static void GLCheckError(const char* operation)
{
    const GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        printf("[OpenGL] %s failed: 0x%04X\n", operation, static_cast<unsigned int>(error));
    }
}

struct GLBuffer
{
    RenderBufferType_t m_Type = RENDER_BUFFER_TYPE_VERTEX;
    std::vector<unsigned char> m_Data;
};

static std::vector<GLBuffer> g_GLBuffers;

static GLBuffer* GetGLBuffer(RenderBufferHandle_t handle)
{
    if (!handle)
        return nullptr;

    const size_t index = static_cast<size_t>(handle) - 1;

    if (index >= g_GLBuffers.size())
        return nullptr;

    return &g_GLBuffers[index];
}

static void BuildPerspectiveMatrix(float fovDegrees, float aspect, float zNear, float zFar, float* matrix)
{
    // first time i've had to use pi in my life
    const float fovRadians = fovDegrees * 3.14159265358979323846f / 180.0f;

    const float f = 1.0f / std::tan(fovRadians * 0.5f);

    std::memset(matrix, 0, sizeof(float) * 16);

    matrix[0] = f / aspect;
    matrix[5] = f;

    matrix[10] = (zFar + zNear) / (zNear - zFar);

    matrix[11] = -1.0f;

    matrix[14] = (2.0f * zFar * zNear) / (zNear - zFar);
}

class CRenderContextGL : public IRenderContext
{
public:
    ~CRenderContextGL() override = default;

    void SetCameraPosition(const Vector3& position) override
    {
        m_CameraPosition = position;
    }

    void SetCameraRotation(const QAngle& rotation) override
    {
        m_CameraRotation = rotation;
    }

    void SetProjection(float fov, float aspect, float zNear, float zFar) override
    {
        m_FOV = fov;
        m_Aspect = aspect;
        m_ZNear = zNear;
        m_ZFar = zFar;
    }

    void SetModelTransform(const Vector3& position, const QAngle& rotation, const Vector3& scale)
    {
        m_ModelPosition = position;

        m_ModelRotation = rotation;

        m_ModelScale = scale;
    }

    void ApplyCamera()
    {
        float projection[16]{};

        BuildPerspectiveMatrix(
            m_FOV,
            m_Aspect,
            m_ZNear,
            m_ZFar,
            projection
        );

        glMatrixMode(GL_PROJECTION);

        glLoadMatrixf(projection);

        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();

        glRotatef(
            -m_CameraRotation.x,
            1.0f,
            0.0f,
            0.0f
        );

        glRotatef(
            -m_CameraRotation.y,
            0.0f,
            1.0f,
            0.0f
        );

        glRotatef(
            -m_CameraRotation.z,
            0.0f,
            0.0f,
            1.0f
        );

        glTranslatef(
            -m_CameraPosition.x,
            -m_CameraPosition.y,
            -m_CameraPosition.z
        );

        glTranslatef(
            m_ModelPosition.x,
            m_ModelPosition.y,
            m_ModelPosition.z
        );

        glRotatef(
            m_ModelRotation.y,
            0.0f,
            1.0f,
            0.0f
        );

        glRotatef(
            m_ModelRotation.x,
            1.0f,
            0.0f,
            0.0f
        );

        glRotatef(
            m_ModelRotation.z,
            0.0f,
            0.0f,
            1.0f
        );

        glScalef(
            m_ModelScale.x,
            m_ModelScale.y,
            m_ModelScale.z
        );
    }

    void Draw(RenderPrimitiveType_t primitiveType, int vertexCount, int firstVertex) override
    {
        if (vertexCount <= 0)
            return;

        ApplyCamera();

        glDrawArrays(GLPrimitive(primitiveType), firstVertex, vertexCount);

        GLCheckError("glDrawArrays");
    }

    void DrawInstanced(RenderPrimitiveType_t primitiveType, int vertexCount, int firstVertex, int instanceCount) override
    {
        if (vertexCount <= 0 || instanceCount <= 0)
            return;

        for (int i = 0; i < instanceCount; ++i)
            glDrawArrays(GLPrimitive(primitiveType), firstVertex, vertexCount);

        GLCheckError("glDrawArrays");
    }

    void DrawIndexed(RenderPrimitiveType_t primitiveType, int indexCount, int firstIndex, int vertexOffset, int) override
    {
        if (indexCount <= 0)
            return;

        GLBuffer* indexBuffer = GetGLBuffer(m_IndexBuffer);

        if (!indexBuffer)
            return;

        if (indexBuffer->m_Type != RENDER_BUFFER_TYPE_INDEX)
            return;

        if (indexBuffer->m_Data.empty())
            return;

        const size_t offset = static_cast<size_t>(firstIndex) * sizeof(uint32_t); // was uint16_t

        if (offset >= indexBuffer->m_Data.size())
            return;

        const void* indices = indexBuffer->m_Data.data() + offset;

        ApplyCamera();

        glDrawElements(GLPrimitive(primitiveType), indexCount, GL_UNSIGNED_SHORT, indices);
        GLCheckError("glDrawElements");
    }

    void DrawIndexedInstanced(RenderPrimitiveType_t primitiveType, int indexCount, int firstIndex, int vertexOffset, int instanceCount, int) override
    {
        if (indexCount <= 0 || instanceCount <= 0)
            return;

        GLBuffer* indexBuffer = GetGLBuffer(m_IndexBuffer);

        if (!indexBuffer)
            return;

        if (indexBuffer->m_Type != RENDER_BUFFER_TYPE_INDEX)
            return;

        const size_t offset = static_cast<size_t>(firstIndex) * sizeof(uint32_t); // was uint16_t

        if (offset >= indexBuffer->m_Data.size())
            return;

        const void* indices = indexBuffer->m_Data.data() + offset;

        for (int i = 0; i < instanceCount; ++i)
            glDrawElements(GLPrimitive(primitiveType), indexCount, GL_UNSIGNED_SHORT, indices);

        GLCheckError("glDrawElements");
    }

    void DrawInstancedIndirect(RenderPrimitiveType_t, RenderBufferHandle_t, uint32_t) override {}
    void DrawIndexedInstancedIndirect(RenderPrimitiveType_t, RenderBufferHandle_t, uint32_t) override {}
    void TextureBarrier(int, ITexture*, RenderImageLayout_t*) override {}
    void UAVBarrier(int, RenderUAVBarrier_t*) override {}
    void BufferBarrier(const RenderBufferBarrier_t&) override {}

    void Clear(Vector4 color, RenderClearFlags_t) override
    {
        glClearColor(color.x, color.y, color.z, color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        GLCheckError("glClear");
    }

    void BindRenderTargets(const RenderTargetDesc_t&) override {}

    void SetViewports(int count, const RenderViewport_t* viewports) override
    {
        if (!viewports || count <= 0)
            return;

        const RenderViewport_t& viewport = viewports[0];

        glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
        GLCheckError("glViewport");
    }

    void GetViewport(RenderViewport_t* viewport, int) override
    {
        if (!viewport)
            return;

        GLint values[4]{};

        glGetIntegerv(GL_VIEWPORT, values);

        viewport->Init(values[0], values[1], values[2], values[3]);
    }

    void Submit() override
    {
        glFlush();
    }

    bool BindVertexBuffer(int, VertexBufferHandle_t handle, int stride) override
    {
        GLBuffer* buffer = GetGLBuffer(static_cast<RenderBufferHandle_t>(handle));

        if (!buffer)
            return false;

        if (buffer->m_Type != RENDER_BUFFER_TYPE_VERTEX)
            return false;

        if (buffer->m_Data.empty())
            return false;

        m_VertexBuffer = static_cast<RenderBufferHandle_t>(handle);
        m_VertexStride = stride;

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, stride, buffer->m_Data.data());

        GLCheckError("glVertexPointer");

        return true;
    }

    bool BindVertexBuffer(int slot, VertexBufferHandle_t handle, int stride, int offset) override
    {
        return BindVertexBuffer(slot, handle, stride);
    }

    bool BindIndexBuffer(IndexBufferHandle_t handle, int offset) override
    {
        GLBuffer* buffer = GetGLBuffer(static_cast<RenderBufferHandle_t>(handle));

        if (!buffer)
            return false;

        if (buffer->m_Type != RENDER_BUFFER_TYPE_INDEX)
            return false;

        if (buffer->m_Data.empty())
            return false;

        m_IndexBuffer = static_cast<RenderBufferHandle_t>(handle);

        return true;
    }

    bool BindIndexBuffer(RenderBufferHandle_t handle, int offset, int flags) override
    {
        return BindIndexBuffer(static_cast<IndexBufferHandle_t>(handle), offset);
    }

    void BindVertexShader(RenderShaderHandle_t, void*) override {}
    void BindShader(RenderShaderType_t, RenderShaderHandle_t) override {}

    void* GetInputLayoutForVertexBuffer(VertexBufferHandle_t) override
    {
        return nullptr;
    }

    ConstantBufferHandle_t LockDynamicConstantBuffer(int, DynamicLockDesc_t*) override
    {
        return ConstantBufferHandle_t{};
    }

    void UnlockDynamicConstantBuffer(ConstantBufferHandle_t, int) override {}
    void BindConstantBuffer(RenderShaderType_t, ConstantBufferHandle_t, int) override {}

    void SetSamplerStatePS(int, RsFilter_t, RsTextureAddressMode_t, RsTextureAddressMode_t, RsTextureAddressMode_t) override {}

    void BindTexture(int, ITexture, RenderTextureDimension_t, RenderShaderType_t, RenderColorSpace_t) override {}

    void ReadTexturePixels(ITexture, IReadTexturePixelsCallback*, Rect_t*, int, int, bool) override {}
    void ReadBuffer(RenderBufferHandle_t, IReadBufferCallback*, int, int, bool) override {}

    void BeginPIXEvent(unsigned long, const char*) override {}
    void EndPIXEvent() override {}
    void SetPIXMarker(unsigned long, const char*) override {}

private:
    RenderBufferHandle_t m_VertexBuffer = 0;
    RenderBufferHandle_t m_IndexBuffer = 0;
    Vector3 m_CameraPosition{ 128.0f, 128, 400.0f }; // do we have to initialize it? not every maps the same pos
    QAngle m_CameraRotation{ 0.0f, 0.0f, 0.0f };
    Vector3 m_ModelPosition{
        0.0f,
        0.0f,
        0.0f
    };

    QAngle m_ModelRotation{
        0.0f,
        0.0f,
        0.0f
    };

    Vector3 m_ModelScale{
        1.0f,
        1.0f,
        1.0f
    };
    float m_FOV = 68.0f;
    float m_Aspect = 16.0f / 9.0f;
    float m_ZNear = 1.0f;
    float m_ZFar = 10000.0f;
    int m_VertexStride = 0;
};

class CRenderDeviceGL : public IRenderDevice
{
public:
    SamplerStateHandle_t FindOrCreateSamplerState(CSamplerStateDesc)
    {
        return SamplerStateHandle_t{};
    }

    int GetSamplerIndex(SamplerStateHandle_t)
    {
        return 0;
    }

    bool UploadWorld(SceneWorldData*)
    {
        return true;
    }

    RenderDeviceInfo_t GetSwapChainInfo(SwapChainHandle_t)
    {
        return RenderDeviceInfo_t{};
    }

    ITexture FindOrCreateFileTexture(const char*, RenderSystemAssetFileLoadMode)
    {
        return ITexture{};
    }

    ITexture FindOrCreateTexture2(const char*, bool, TextureCreationConfig_t, void*, int)
    {
        return ITexture{};
    }

    void ClearTexture(ITexture, Color) {}
    void AsyncSetTextureData2(ITexture, void*, int, Rect3D) {}

    ITexture GetSwapChainTexture(SwapChainHandle_t, SwapChainBuffer)
    {
        return ITexture{};
    }

    bool GetGPUFrameTimeMS(SwapChainHandle_t, float, uint32)
    {
        return false;
    }

    CTextureDesc GetTextureDesc(ITexture)
    {
        return CTextureDesc{};
    }

    CTextureDesc GetOnDiskTextureDesc(ITexture)
    {
        return CTextureDesc{};
    }

    RenderMultisampleType GetTextureMultisampleType(ITexture)
    {
        return RENDER_MULTISAMPLE_NONE;
    }

    uint32 ComputeTextureMemorySize(ITexture)
    {
        return 0;
    }

    IRenderContext* CreateRenderContext(uint32)
    {
        static CRenderContextGL context;
        return &context;
    }

    void ReleaseRenderContext(IRenderContext*) {}

    bool ReadTexturePixels(ITexture, Rect_t, int, int, Rect_t, void*, ImageFormat, int)
    {
        return false;
    }

    void DestroySwapChain(SwapChainHandle_t) {}

    bool Present(SwapChainHandle_t)
    {
        HWND hwnd = GetActiveWindow();

        if (!hwnd)
            return false;

        HDC dc = GetDC(hwnd);

        if (!dc)
            return false;

        const BOOL result = SwapBuffers(dc);

        ReleaseDC(hwnd, dc);

        return result == TRUE;
    }

    void Flush()
    {
        glFlush();
    }

    void ForceFlushGPU(SwapChainHandle_t)
    {
        glFinish();
    }

    bool CanRenderToSwapChain(SwapChainHandle_t)
    {
        return true;
    }

    bool IsUsing32BitDepthBuffer()
    {
        return false;
    }

    Vector2 GetBackbufferDimensions(SwapChainHandle_t)
    {
        HWND hwnd = GetActiveWindow();

        if (!hwnd)
            return Vector2{ 1280.0f, 720.0f };

        RECT rect{};

        if (!GetClientRect(hwnd, &rect))
            return Vector2{ 1280.0f, 720.0f };

        return Vector2{
            static_cast<float>(rect.right - rect.left),
            static_cast<float>(rect.bottom - rect.top)
        };
    }

    RenderDeviceAPI_t GetRenderDeviceAPI()
    {
        return RENDER_DEVICE_API_GL;
    }

    void MarkTextureUsed(ITexture, int) {}

    bool IsTextureRenderTarget(ITexture)
    {
        return false;
    }

    bool IsRayTracingSupported()
    {
        return false;
    }

    RenderBufferHandle_t CreateGPUBuffer(RenderBufferType_t type, BufferDesc_t, RenderBufferFlags_t, const char*)
    {
        GLBuffer buffer{};
        buffer.m_Type = type;

        g_GLBuffers.push_back(std::move(buffer));

        return static_cast<RenderBufferHandle_t>(g_GLBuffers.size());
    }

    void DestroyGPUBuffer(RenderBufferHandle_t handle)
    {
        GLBuffer* buffer = GetGLBuffer(handle);

        if (!buffer)
            return;

        buffer->m_Data.clear();
        buffer->m_Data.shrink_to_fit();
    }

    bool UploadGPUBuffer(RenderBufferHandle_t handle, const void* data, uint32 size)
    {
        if (!data || size == 0)
            return false;

        GLBuffer* buffer = GetGLBuffer(handle);

        if (!buffer)
            return false;

        buffer->m_Data.resize(size);
        std::memcpy(buffer->m_Data.data(), data, size);

        return true;
    }

    bool ReadBuffer(RenderBufferHandle_t handle, uint32 offset, void* output, uint32 size)
    {
        if (!output || size == 0)
            return false;

        GLBuffer* buffer = GetGLBuffer(handle);

        if (!buffer)
            return false;

        if (static_cast<size_t>(offset) + size > buffer->m_Data.size())
            return false;

        std::memcpy(output, buffer->m_Data.data() + offset, size);

        return true;
    }

    int* GetDeviceSpecificInfo(DeviceSpecificInfo_t)
    {
        return nullptr;
    }

    int* GetGraphicsAPISpecificTextureHandle(ITexture)
    {
        return nullptr;
    }

    int* GetDeviceSpecificTexture(ITexture)
    {
        return nullptr;
    }

    int GetTextureViewIndex(ITexture, byte, RenderTextureDimension_t)
    {
        return 0;
    }

    void GetTextureResidencyInfo(ITexture*, const char*) {}

    Vector4 GetSheetInfo(ITexture)
    {
        return Vector4{};
    }

    int GetSequenceCount(ITexture)
    {
        return 0;
    }

    SheetSequence_t GetSequence(ITexture, int)
    {
        return SheetSequence_t{};
    }
};

CRenderDeviceGL g_sDevice;
IRenderDevice* g_Device = &g_sDevice;

class CRenderDeviceMgr : public CBaseAppSystem<IRenderDeviceMgr>
{
public:
    int GetAdapterCount()
    {
        return 1;
    }

    void GetAdapterInfo(int, RenderAdapterInfo_t& info)
    {
        info = RenderAdapterInfo_t{};
    }

    int FindAdapterForRawAdapterAndOutput(int, int)
    {
        return 0;
    }

    int FindAdapterForDesktopRect(int, int, int, int)
    {
        return 0;
    }

    int GetModeCount(int)
    {
        return 1;
    }

    void GetModeInfo(RenderDisplayMode_t* info, int, int)
    {
        if (info)
            *info = RenderDisplayMode_t{};
    }

    void GetCurrentModeInfo(RenderDisplayMode_t* info, int)
    {
        if (info)
            *info = RenderDisplayMode_t{};
    }

    IRenderDevice* CreateDevice(int, int, int = 0)
    {
        return g_Device;
    }

    void AddModeChangeCallback(RenderModeChangeCallbackFunc_t) {}
    void RemoveModeChangeCallback(RenderModeChangeCallbackFunc_t) {}
    void DestroyDevice() {}
    void InstallRenderDeviceSetup(void*) {}
    void AddDeviceEventListener(void*) {}
    void RemoveDeviceEventListener(void*) {}

    void* GetVideoConfig()
    {
        return nullptr;
    }

    void InvokeModeChangeCallbacks(const RenderDeviceInfo_t&) {}
    void WriteVideoConfig() {}
    void ResetVideoConfig() {}
    void GetAllDisplayModes(RenderDisplayMode_t&, int, bool) {}

    uint32 GetPlatWindowFlags()
    {
        return 1;
    }

    void GetVideoMemoryInfo(int, uint64*, uint64*, uint64*) {}
};

class CRenderUtils : public CBaseAppSystem<IRenderUtils>
{
public:
    OcclusionQueryObjectHandle_t CreateOcclusionQueryObject()
    {
        return 0;
    }

    void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t) {}

    int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t)
    {
        return 0;
    }

    void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t) {}

    bool BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t, IRenderContext*)
    {
        return false;
    }

    void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t, IRenderContext*) {}
};

CRenderDeviceMgr g_sRenderDeviceManager;
IRenderDeviceMgr* g_pRenderDeviceManager = &g_sRenderDeviceManager;

CRenderUtils g_sRenderUtils;
IRenderUtils* g_pRenderUtils = &g_sRenderUtils;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CRenderDeviceMgr, IRenderDeviceMgr, RENDER_DEVICE_MGR_INTERFACE_VERSION, g_sRenderDeviceManager);
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CRenderDeviceGL, IRenderDevice, RENDER_DEVICE_INTERFACE_VERSION, g_sDevice);
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CRenderUtils, IRenderUtils, RENDER_UTILS_INTERFACE_VERSION, g_sRenderUtils);
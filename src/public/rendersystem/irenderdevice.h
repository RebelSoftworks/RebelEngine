#ifndef IRENDERDEVICE_H
#define IRENDERDEVICE_H
#pragma once

#include "../public/rendersystem/irendercontext.h"
#include "../tier0/types.h"
#include "../mathlib/vector.h"
#include "../appframework/iappframework.h"
#include "../Color.h"

struct CSamplerStateDesc
{
    uint8_t m_nFilterMode;
    uint8_t m_nMipLodBias;
    uint8_t m_nMipLodBiasSign;

    uint8_t m_nAddressU;
    uint8_t m_nAddressV;
    uint8_t m_nAddressW;

    uint8_t m_nAnisoExp;
    uint8_t m_nComparisonFunc;

    uint8_t m_nAllowGlobalMipBiasOverride;

    uint8_t m_nMinLod;
    uint8_t m_nMaxLod;

    uint32_t m_nBorderColor8Bit;

    uint16_t m_nPad;

    CSamplerStateDesc() = default;

	//public CSamplerStateDesc(SamplerState samplerState)
	//{
	//	m_nFilterMode = (byte)samplerState.Filter;
	//	m_nMipLodBias = (byte)samplerState.MipLodBias;
	//	m_nMipLodBiasSign = 0;

	//	m_nAddressU = (byte)samplerState.AddressModeU;
	//	m_nAddressV = (byte)samplerState.AddressModeV;
	//	m_nAddressW = (byte)samplerState.AddressModeW;

	//	m_nAnisoExp = (byte)samplerState.MaxAnisotropy;
	//	m_nComparisonFunc = 0;

	//	m_nAllowGlobalMipBiasOverride = 0;

	//	m_nMinLod = 0;
	//	m_nMaxLod = 15;

	//	m_nBorderColor8Bit = samplerState.BorderColor.ToColor32().RawInt;

	//	m_nPad = 0;
	//}
};

struct SamplerStateHandle_t
{
	// "native pointer"
};

struct Rect3D
{
	int x, y, z;
	int width, height, depth;

	Rect3D(int x, int y, int z, int width, int height, int depth)
	{
		this->x = x;
		this->y = y;
		this->z = z;

		this->width = width;
		this->height = height;
		this->depth = depth;
	}

	void Clear()
	{
		x = 0;
		y = 0;
		z = 0;
		width = 0;
		height = 0;
		depth = 0;
	}

	int Size()
	{
		return width * height * depth;
	}

	bool Intersects(Rect3D other)
	{
		if ((x + width <= other.x) || (other.x + other.width <= x)) return false;
		if ((y + height <= other.y) || (other.y + other.height <= y)) return false;
		if ((z + depth <= other.z) || (other.z + other.depth <= z)) return false;

		return true;
	}
};

enum ImageFormat : int
{
	None = -3,
	Default = -2,
	/// <summary>
	/// Four 8-bit components representing RGBA.
	/// </summary>
	RGBA8888 = 0,
	/// <summary>
	/// Four 8-bit components representing ABGR.
	/// </summary>
	ABGR8888,
	/// <summary>
	/// Three 8-bit components representing RGB.
	/// </summary>
	RGB888,
	/// <summary>
	/// Three 8-bit components representing BGR.
	/// </summary>
	BGR888,
	/// <summary>
	/// Three components representing RGB.
	/// Red and blue are 5 bit components, green is 6 bit totalling 16 bits.
	/// </summary>
	RGB565,
	/// <summary>
	/// Single 8-bit component representing luminance.
	/// </summary>
	I8,
	/// <summary>
	/// Two 8-bit components representing luminance and alpha.
	/// </summary>
	IA88,
	/// <summary>
	/// Single 8-bit component representing alpha.
	/// </summary>
	A8 = 8,
	ARGB8888 = 11,
	BGRA8888,
	/// <summary>
	/// Compressed texture format with no alpha.
	/// </summary>
	DXT1,
	/// <summary>
	/// Compressed texture format with alpha.
	/// </summary>
	DXT3,
	/// <summary>
	/// Compressed texture format with alpha, generally better than DXT3.
	/// </summary>
	DXT5,
	BGRX8888,
	BGR565,
	BGRX5551,
	BGRA4444,
	DXT1_ONEBITALPHA,
	BGRA5551,
	/// <summary>
	/// Four 16-bit float components representing RGBA.
	/// </summary>
	RGBA16161616F = 24,
	/// <summary>
	/// Four 16-bit integer components representing RGBA.
	/// </summary>
	RGBA16161616,
	/// <summary>
	/// Three 32-bit float components representing RGB.
	/// </summary>
	RGB323232F = 28,
	/// <summary>
	/// Single 32-bit float component representing R.
	/// </summary>
	R32F = 27,
	/// <summary>
	/// Four 32-bit float components representing RGBA.
	/// </summary>
	RGBA32323232F = 29,

	// Compressed normal map formats
	ATI2N = 36,         // One-surface ATI2N / DXN format
	ATI1N,         // Two-surface ATI1N format

	// should we be exposing any of this shit
	// supporting these specific formats as non-tiled for procedural cpu access
	LINEAR_BGRX8888 = 41,
	LINEAR_RGBA8888,
	LINEAR_ABGR8888,
	LINEAR_ARGB8888,
	LINEAR_BGRA8888,
	LINEAR_RGB888,
	LINEAR_BGR888,
	LINEAR_BGRX5551,
	LINEAR_I8,
	LINEAR_RGBA16161616,

	LE_BGRX8888,
	LE_BGRA8888,

	RG1616F,
	RG3232F,
	RGBX8888,

	RGBA1010102 = 57,   // 10 bit-per component render targets
	BGRA1010102,
	R16F,          // 16 bit FP format

	// Depth-stencil texture formats
	D16,
	D15S1,
	D32,
	D24S8 = 63,
	LINEAR_D24S8,
	D24X8,
	D24X4S4,
	D24FS8,

	DXT5_NM = 78,

	RG1616,
	R16,           // 16 bit int format

	RGBA8888_LINEAR,
	BGRA8888_LINEAR,
	BGRX8888_LINEAR,

	RGBX555,
	BC6H,
	BC7,
	R32_UINT,

	R8G8B8_ETC2,
	R8G8B8A8_ETC2_EAC,
	R11_EAC,
	RG11_EAC,
	D32FS8,
	RGBA32323232,
	I16F,

	RG3232,
};

struct RenderDisplayMode_t
{
	int m_nVersion;
	int m_nWidth;                   // 0 when running windowed means use desktop resolution
	int m_nHeight;
	ImageFormat m_Format;           // use ImageFormats (ignored for windowed mode)
	int m_nRefreshRateNumerator;    // Refresh rate. Use 0 in numerator + denominator for a default setting.
	int m_nRefreshRateDenominator;  // Refresh rate = numerator / denominator.
	uint32 m_nFlags;
};

struct DisplayMode_t {
	int Width;
	int Height;
	float RefreshRate;
	ImageFormat Format;
};

struct RenderDeviceInfo_t
{
	int m_nVersion;
	RenderDisplayMode_t m_DisplayMode;
	int m_nBackBufferCount;             // valid values are 1 or 2 [2 results in triple buffering]
	void* m_nMultisampleType;
	byte m_nModeUsage;                 // RENDER_DISPLAY_MODE usage flags for fullscreen/windowed.
	byte m_bUseStencil;
	byte m_bWaitForVSync;           // Would we not present until vsync?
	byte m_bUsingMultipleWindows;   // Forces D3DPresent to use _COPY instead
	byte m_bIsMainWindow;

	byte m_padding01;
};

enum RenderSystemAssetFileLoadMode
{
	Immediate,                                     // asset is created and loaded from disk immediately
	Asynchronous,                                  // asset will start loading asynchronously
	Streamed,                                      // asset will be asynchronously loaded when referenced.
};

struct TextureCreationConfig_t
{
};

enum SwapChainBuffer
{
	Invalid = -1,
	BufferColor = 0,
	BufferDepth,
};

enum RenderMultisampleType
{
	RENDER_MULTISAMPLE_INVALID = -1,
	RENDER_MULTISAMPLE_NONE = 0,
	RENDER_MULTISAMPLE_2X = 1,
	RENDER_MULTISAMPLE_4X = 2,
	RENDER_MULTISAMPLE_6X = 3,
	RENDER_MULTISAMPLE_8X = 4,
	RENDER_MULTISAMPLE_16X = 5,
};

struct CTextureDesc
{
};

enum RenderDeviceAPI_t
{
	RENDER_DEVICE_API_DX9 = 0,
	RENDER_DEVICE_API_DX11,
	RENDER_DEVICE_API_GL,
	RENDER_DEVICE_API_MANTLE,
	RENDER_DEVICE_API_VULKAN,

	// Empty
	RENDER_DEVICE_API_EMPTY,

	RENDER_DEVICE_API_MAX
};

struct BufferDesc_t
{
	int m_nElementCount;        // Number of vertices/indices
	int m_nElementSizeInBytes;  // Size of a single vertex/index
	unsigned int m_pDebugName;           // Used to debug buffers
	unsigned int m_pBudgetGroupName;
	int m_nSize;
};

enum RenderBufferType_t
{
	RENDER_BUFFER_TYPE_VERTEX,
	RENDER_BUFFER_TYPE_INDEX,
	RENDER_BUFFER_TYPE_CONSTANT,
	RENDER_BUFFER_TYPE_STRUCTURED,
};

enum RenderBufferFlags_t
{
	RENDER_BUFFER_FLAG_NONE = 0,
	RENDER_BUFFER_FLAG_DYNAMIC = 1 << 0,
	RENDER_BUFFER_FLAG_DRAW_INDIRECT_ARGS = 1 << 1,
	RENDER_BUFFER_FLAG_APPEND_CONSUME = 1 << 2,
};

enum DeviceSpecificInfo_t
{
	DSI_D3D9_DEVICE,                            // get the IDirect3DDevice9 device, can be NULL for n/a devices
	DSI_D3D_PRESENT_PARAMETERS,                 // get the D3DPRESENT_PARAMETERS, can be NULL for n/a devices

	DSI_D3D11_DEVICE,                           // get the ID3D11Device device, can be NULL for n/a devices
	DSI_D3D11_DEVICE_IMMEDIATE_CONTEXT,         // get the immediate ID3D11DeviceContext, can be NULL for n/a devices
	DSI_VULKAN_INSTANCE,                        // get the VkInstance, can be NULL for n/a devices
	DSI_VULKAN_PHYSICAL_DEVICE,                 // get the VkPhysicalDevice, can be NULL for n/a devices
	DSI_VULKAN_DEVICE,                          // get the VkDevice, can be NULL for n/a devices
	DSI_VULKAN_QUEUE,                           // get the VkQueue, can be NULL for n/a devices
	DSI_VULKAN_MEMORY_PROPERTIES,               // get the VkPhysicalDeviceMemoryProperties, can be NULL for n/a devices
	DSI_VULKAN_QUEUE_FAMILY_INDEX,              // get the Vulkan queueFamilyIndex, can be NULL for n/a devices
	DSI_VULKAN_RAY_TRACING_PIPELINE_PROPERTIES  // get the VkPhysicalDeviceRayTracingPipelinePropertiesKHR, can be NULL for n/a devices
};

class SheetSequence_t
{
	uint32 m_nId;
	bool m_bClamp;
	bool m_bAlphaCrop;
	bool m_bNoColor;
	bool m_bNoAlpha;
	float m_flTotalTime;

	inline int FrameCount()
	{
		return 0;
	}
};

// IRenderDevice:

class IRenderDevice
{
public:
	virtual SamplerStateHandle_t FindOrCreateSamplerState(CSamplerStateDesc samplerDesc) = 0;
	virtual int GetSamplerIndex(SamplerStateHandle_t samplerState) = 0;

	virtual RenderDeviceInfo_t GetSwapChainInfo(SwapChainHandle_t swapChain) = 0;
	virtual ITexture FindOrCreateFileTexture(const char* pFileName, RenderSystemAssetFileLoadMode nLoadMode) = 0;
	virtual ITexture FindOrCreateTexture2(const char* pResourceName, bool bIsAnonymous, TextureCreationConfig_t pDescriptor, void* data, int dataSize) = 0;
	virtual void ClearTexture(ITexture hTexture, Color color) = 0;
	virtual void AsyncSetTextureData2(ITexture hTexture, void* pData, int nDataSize, Rect3D rect) = 0;

	virtual ITexture GetSwapChainTexture(SwapChainHandle_t swapChain, SwapChainBuffer bufferType) = 0;
	virtual bool GetGPUFrameTimeMS(SwapChainHandle_t swapChain, float pGPUFrameTimeMSOut, uint32 pFrameNumberOut) = 0;

	virtual CTextureDesc GetTextureDesc(ITexture hTexture) = 0;
	virtual CTextureDesc GetOnDiskTextureDesc(ITexture hTexture) = 0;

	virtual RenderMultisampleType GetTextureMultisampleType(ITexture hTexture) = 0;
	virtual uint32 ComputeTextureMemorySize(ITexture hTexture) = 0;

	virtual IRenderContext* CreateRenderContext(uint32 flags) = 0;
	virtual void ReleaseRenderContext(IRenderContext* context) = 0;

	virtual bool ReadTexturePixels(ITexture hTexture, Rect_t pSrcRect, int nSrcSlice, int nSrcMip, Rect_t pDstRect, void* pData, ImageFormat dstFormat, int nDstStride) = 0;

	virtual void DestroySwapChain(SwapChainHandle_t hSwapChain) = 0;

	virtual bool Present(SwapChainHandle_t chain) = 0;
	virtual void Flush() = 0;
	virtual void ForceFlushGPU(SwapChainHandle_t hSwapChain) = 0;
	virtual bool CanRenderToSwapChain(SwapChainHandle_t chain) = 0;

	virtual bool IsUsing32BitDepthBuffer() = 0;

	virtual Vector2 GetBackbufferDimensions(SwapChainHandle_t chain) = 0;

	virtual RenderDeviceAPI_t GetRenderDeviceAPI() = 0;

	// Dispersion only start
	virtual bool UploadGPUBuffer(RenderBufferHandle_t handle, const void* data, uint32 size) = 0;
	// Dispersion only end

	virtual bool UploadWorld(SceneWorldData* pWorld) = 0;

	virtual void MarkTextureUsed(ITexture texture, int nRequiredMipSize) = 0;

	virtual bool IsTextureRenderTarget(ITexture texture) = 0;

	virtual bool IsRayTracingSupported() = 0;

	virtual RenderBufferHandle_t CreateGPUBuffer(RenderBufferType_t nType, BufferDesc_t desc, RenderBufferFlags_t usage, const char* pDebugName) = 0;

	virtual void DestroyGPUBuffer(RenderBufferHandle_t hGPUBuffer) = 0;
	virtual bool ReadBuffer(RenderBufferHandle_t hBuffer, uint32 nOffsetInBytes, void* pBuf, uint32 nBytesToRead) = 0;

	virtual int* GetDeviceSpecificInfo(DeviceSpecificInfo_t info) = 0;
	virtual int* GetGraphicsAPISpecificTextureHandle(ITexture hTexture) = 0;

	virtual int* GetDeviceSpecificTexture(ITexture hTexture) = 0;

	virtual int GetTextureViewIndex(ITexture hTexture, byte colorSpace, RenderTextureDimension_t dim) = 0;

	virtual void GetTextureResidencyInfo(ITexture* pTextures, const char* pNames) = 0;

	virtual Vector4 GetSheetInfo(ITexture texture) = 0;

	virtual int GetSequenceCount(ITexture texture) = 0;

	virtual SheetSequence_t GetSequence(ITexture texture, int index) = 0;
};

struct RenderAdapterInfo_t {
	char m_pDriverName[512];
	unsigned int m_VendorID;
	unsigned int m_DeviceID;
	unsigned int m_SubSysID;
	unsigned int m_Revision;
	int m_nDXSupportLevel;
	int m_nMinDXSupportLevel;
	int m_nMaxDXSupportLevel;
	unsigned int m_nDriverVersionHigh;
	unsigned int m_nDriverVersionLow;
	char m_pDriverVersion[128];
};

typedef void (*RenderModeChangeCallbackFunc_t)(void);

class IRenderDeviceMgr : public IAppSystem {
public:
	virtual int GetAdapterCount();
	virtual void GetAdapterInfo(int nAdapter, RenderAdapterInfo_t& info);
	virtual int FindAdapterForRawAdapterAndOutput(int nAdapter, int nOutput);
	virtual int FindAdapterForDesktopRect(int nAdapter, int par2, int par3, int par4);
	virtual int	 GetModeCount(int nAdapter);
	virtual void GetModeInfo(RenderDisplayMode_t* pInfo, int nAdapter, int nMode);
	virtual void GetCurrentModeInfo(RenderDisplayMode_t* pInfo, int nAdapter);
	virtual IRenderDevice* CreateDevice(int nAdapter, int nFlags, int nDXLevel = 0);
	virtual void AddModeChangeCallback(RenderModeChangeCallbackFunc_t func);
	virtual void RemoveModeChangeCallback(RenderModeChangeCallbackFunc_t func);
	virtual void DestroyDevice();
	virtual void InstallRenderDeviceSetup(void* setup); // TODO, reimplement IRenderDeviceSetup.
	virtual void AddDeviceEventListener(void* eventlistener); // TODO, reimplement IRenderDeviceEventListener
	virtual void RemoveDeviceEventListener(void* eventlistener);
	virtual void* GetVideoConfig(); // TODO, reimplement KeyValues
	virtual void InvokeModeChangeCallbacks(const RenderDeviceInfo_t& mode);
	virtual void WriteVideoConfig();
	virtual void ResetVideoConfig();
	virtual void GetAllDisplayModes(RenderDisplayMode_t& mode, int count, bool recieve);
	virtual uint32 GetPlatWindowFlags();
	virtual void GetVideoMemoryInfo(int par1, uint64* par2, uint64* par3, uint64* par4);
};

#endif // IRENDERDEVICE_H
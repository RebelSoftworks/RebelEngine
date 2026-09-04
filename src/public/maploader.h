#ifndef MAP_LOADER_H
#define MAP_LOADER_H
#include <cstdint>
#include <string>
#include <vector>

#include <rendersystem/irenderdevice.h>
#include <mathlib/vector.h>

struct MapVertex_t
{
    float x;
    float y;
    float z;
};

struct MapTriangle_t
{
    uint32_t i0;
    uint32_t i1;
    uint32_t i2;
};

struct MapWorld_t
{
    std::string name;
    std::string material;
    std::vector<MapVertex_t> vertices;
    std::vector<MapTriangle_t> triangles;
};

class IMapLoader
{
public:
    virtual ~IMapLoader() = default;

    virtual bool LoadMap(const char* szMap) = 0;
    virtual const MapWorld_t* GetWorld() const = 0;
    virtual bool UploadToGPU(IRenderDevice* pDevice) = 0;
    virtual RenderBufferHandle_t GetVertexBuffer() const = 0;
    virtual int GetVertexCount() const = 0;
};

IMapLoader* MapLoader();

#endif

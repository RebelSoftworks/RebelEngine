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
#include <imodelloader.h>
#include <fstream>
#include <vector>
#include <string>
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <../thirdparty/json/json.hpp>

using json = nlohmann::json;

struct GLBHeader_t
{
    uint32_t magic;
    uint32_t version;
    uint32_t length;
};

struct GLBChunkHeader_t
{
    uint32_t length;
    uint32_t type;
};

static const uint32_t GLB_MAGIC =
0x46546C67;

static const uint32_t GLB_VERSION =
2;

static const uint32_t GLB_CHUNK_JSON =
0x4E4F534A;

static const uint32_t GLB_CHUNK_BIN =
0x004E4942;

static bool ReadFile(
    const char* filename,
    std::vector<unsigned char>& data)
{
    std::ifstream file(
        filename,
        std::ios::binary);

    if (!file.is_open())
        return false;

    file.seekg(
        0,
        std::ios::end);

    std::streamsize size =
        file.tellg();

    if (size <= 0)
        return false;

    file.seekg(
        0,
        std::ios::beg);

    data.resize(
        static_cast<size_t>(size));

    file.read(
        reinterpret_cast<char*>(
            data.data()),
        size);

    return file.good();
}

template <typename T>
static bool ReadValue(
    const std::vector<unsigned char>& data,
    size_t offset,
    T& value)
{
    if (
        offset + sizeof(T) >
        data.size())
    {
        return false;
    }

    std::memcpy(
        &value,
        data.data() + offset,
        sizeof(T));

    return true;
}

static int GetComponentSize(
    int componentType)
{
    switch (componentType)
    {
    case 5120:
    case 5121:
        return 1;

    case 5122:
    case 5123:
        return 2;

    case 5125:
    case 5126:
        return 4;
    }

    return 0;
}

static int GetTypeComponents(
    const std::string& type)
{
    if (type == "SCALAR")
        return 1;

    if (type == "VEC2")
        return 2;

    if (type == "VEC3")
        return 3;

    if (type == "VEC4")
        return 4;

    return 0;
}

class CGLBLoaderImpl
{
public:
    bool Load(
        const char* filename,
        Model_t& model);

private:
    bool LoadPrimitive(
        const json& primitive,
        const json& root,
        const std::vector<unsigned char>& binary,
        ModelMesh_t& mesh);

    bool GetAccessorData(
        const json& root,
        const std::vector<unsigned char>& binary,
        int accessorIndex,
        const unsigned char*& data,
        int& count,
        int& componentType,
        int& componentCount,
        int& stride);
};

bool CGLBLoaderImpl::GetAccessorData(
    const json& root,
    const std::vector<unsigned char>& binary,
    int accessorIndex,
    const unsigned char*& data,
    int& count,
    int& componentType,
    int& componentCount,
    int& stride)
{
    if (
        !root.contains("accessors") ||
        !root.contains("bufferViews"))
    {
        return false;
    }

    const json& accessors =
        root["accessors"];

    if (
        accessorIndex < 0 ||
        accessorIndex >=
        static_cast<int>(
            accessors.size()))
    {
        return false;
    }

    const json& accessor =
        accessors[accessorIndex];

    if (!accessor.contains("bufferView"))
        return false;

    const int bufferViewIndex =
        accessor["bufferView"].get<int>();

    const json& bufferViews =
        root["bufferViews"];

    if (
        bufferViewIndex < 0 ||
        bufferViewIndex >=
        static_cast<int>(
            bufferViews.size()))
    {
        return false;
    }

    const json& bufferView =
        bufferViews[bufferViewIndex];

    count =
        accessor["count"].get<int>();

    componentType =
        accessor["componentType"].get<int>();

    componentCount =
        GetTypeComponents(
            accessor["type"].get<std::string>());

    const int componentSize =
        GetComponentSize(componentType);

    if (
        componentSize == 0 ||
        componentCount == 0)
    {
        return false;
    }

    size_t bufferViewOffset =
        bufferView.value(
            "byteOffset",
            0);

    size_t accessorOffset =
        accessor.value(
            "byteOffset",
            0);

    size_t offset =
        bufferViewOffset +
        accessorOffset;

    stride =
        bufferView.value(
            "byteStride",
            componentSize *
            componentCount);

    if (offset >= binary.size())
        return false;

    data =
        binary.data() +
        offset;

    return true;
}

bool CGLBLoaderImpl::LoadPrimitive(
    const json& primitive,
    const json& root,
    const std::vector<unsigned char>& binary,
    ModelMesh_t& mesh)
{
    if (!primitive.contains("attributes"))
        return false;

    const json& attributes =
        primitive["attributes"];

    if (!attributes.contains("POSITION"))
        return false;

    const int positionAccessor =
        attributes["POSITION"].get<int>();

    const unsigned char* positionData =
        nullptr;

    int vertexCount = 0;
    int positionComponentType = 0;
    int positionComponents = 0;
    int positionStride = 0;

    if (!GetAccessorData(
        root,
        binary,
        positionAccessor,
        positionData,
        vertexCount,
        positionComponentType,
        positionComponents,
        positionStride))
    {
        return false;
    }

    if (
        positionComponentType != 5126 ||
        positionComponents != 3)
    {
        return false;
    }

    const unsigned char* normalData =
        nullptr;

    int normalCount = 0;
    int normalComponentType = 0;
    int normalComponents = 0;
    int normalStride = 0;

    if (attributes.contains("NORMAL"))
    {
        GetAccessorData(
            root,
            binary,
            attributes["NORMAL"].get<int>(),
            normalData,
            normalCount,
            normalComponentType,
            normalComponents,
            normalStride);
    }

    const unsigned char* texcoordData =
        nullptr;

    int texcoordCount = 0;
    int texcoordComponentType = 0;
    int texcoordComponents = 0;
    int texcoordStride = 0;

    if (
        attributes.contains(
            "TEXCOORD_0"))
    {
        GetAccessorData(
            root,
            binary,
            attributes[
                "TEXCOORD_0"].get<int>(),
                texcoordData,
                texcoordCount,
                texcoordComponentType,
                texcoordComponents,
                texcoordStride);
    }

    mesh.vertices.resize(
        vertexCount);

    for (
        int i = 0;
        i < vertexCount;
        ++i)
    {
        ModelVertex_t& vertex =
            mesh.vertices[i];

        const float* position =
            reinterpret_cast<const float*>(
                positionData +
                i * positionStride);

        vertex.position.x =
            position[0];

        vertex.position.y =
            position[1];

        vertex.position.z =
            position[2];

        if (
            normalData &&
            normalComponentType == 5126 &&
            normalComponents == 3)
        {
            const float* normal =
                reinterpret_cast<const float*>(
                    normalData +
                    i * normalStride);

            vertex.normal.x =
                normal[0];

            vertex.normal.y =
                normal[1];

            vertex.normal.z =
                normal[2];
        }

        if (
            texcoordData &&
            texcoordComponentType == 5126 &&
            texcoordComponents == 2)
        {
            const float* texcoord =
                reinterpret_cast<const float*>(
                    texcoordData +
                    i * texcoordStride);

            vertex.texcoord.x =
                texcoord[0];

            vertex.texcoord.y =
                texcoord[1];
        }
    }

    if (!primitive.contains("indices"))
        return true;

    const int indexAccessor =
        primitive["indices"].get<int>();

    const unsigned char* indexData =
        nullptr;

    int indexCount = 0;
    int indexComponentType = 0;
    int indexComponents = 0;
    int indexStride = 0;

    if (!GetAccessorData(
        root,
        binary,
        indexAccessor,
        indexData,
        indexCount,
        indexComponentType,
        indexComponents,
        indexStride))
    {
        return false;
    }

    if (indexComponents != 1)
        return false;

    mesh.indices.resize(
        indexCount);

    for (
        int i = 0;
        i < indexCount;
        ++i)
    {
        const unsigned char* ptr =
            indexData +
            i * indexStride;

        uint32_t index = 0;

        switch (
            indexComponentType)
        {
        case 5121:
            index =
                *reinterpret_cast<
                const uint8_t*>(
                    ptr);
            break;

        case 5123:
            index =
                *reinterpret_cast<
                const uint16_t*>(
                    ptr);
            break;

        case 5125:
            index =
                *reinterpret_cast<
                const uint32_t*>(
                    ptr);
            break;

        default:
            return false;
        }

        mesh.indices[i] =
            index;
    }

    return true;
}

bool CGLBLoaderImpl::Load(
    const char* filename,
    Model_t& model)
{
    std::vector<unsigned char> file;

    if (!ReadFile(filename, file))
    {
        printf(
            "Failed to open GLB: %s\n",
            filename);

        return false;
    }

    if (
        file.size() <
        sizeof(GLBHeader_t))
    {
        return false;
    }

    GLBHeader_t header{};

    if (!ReadValue(
        file,
        0,
        header))
    {
        return false;
    }

    if (
        header.magic != GLB_MAGIC)
    {
        printf(
            "Not a valid GLB file\n");

        return false;
    }

    if (
        header.version != GLB_VERSION)
    {
        printf(
            "Unsupported GLB version: %u\n",
            header.version);

        return false;
    }

    size_t offset =
        sizeof(GLBHeader_t);

    std::string jsonText;

    std::vector<unsigned char>
        binary;

    while (
        offset +
        sizeof(GLBChunkHeader_t) <=
        file.size())
    {
        GLBChunkHeader_t chunk{};

        if (!ReadValue(
            file,
            offset,
            chunk))
        {
            return false;
        }

        offset +=
            sizeof(GLBChunkHeader_t);

        if (
            offset +
            chunk.length >
            file.size())
        {
            return false;
        }

        if (
            chunk.type ==
            GLB_CHUNK_JSON)
        {
            jsonText.assign(
                reinterpret_cast<
                const char*>(
                    file.data() +
                    offset),
                chunk.length);
        }
        else if (
            chunk.type ==
            GLB_CHUNK_BIN)
        {
            binary.resize(
                chunk.length);

            std::memcpy(
                binary.data(),
                file.data() +
                offset,
                chunk.length);
        }

        offset +=
            chunk.length;
    }

    if (jsonText.empty())
    {
        printf(
            "GLB has no JSON chunk\n");

        return false;
    }

    if (binary.empty())
    {
        printf(
            "GLB has no binary chunk\n");

        return false;
    }

    json root;

    try
    {
        root =
            json::parse(
                jsonText);
    }
    catch (
        const std::exception& e)
    {
        printf(
            "Failed to parse GLB JSON: %s\n",
            e.what());

        return false;
    }

    if (!root.contains("meshes"))
    {
        printf(
            "GLB contains no meshes\n");

        return false;
    }

    model.meshes.clear();

    const json& meshes =
        root["meshes"];

    for (
        size_t meshIndex = 0;
        meshIndex < meshes.size();
        ++meshIndex)
    {
        const json& gltfMesh =
            meshes[meshIndex];

        if (
            !gltfMesh.contains(
                "primitives"))
        {
            continue;
        }

        const json& primitives =
            gltfMesh[
                "primitives"];

        for (
            size_t primitiveIndex = 0;
            primitiveIndex <
            primitives.size();
            ++primitiveIndex)
        {
            ModelMesh_t mesh;

            if (!LoadPrimitive(
                primitives[
                    primitiveIndex],
                    root,
                    binary,
                    mesh))
            {
                continue;
            }

            model.meshes.push_back(
                mesh);
        }
    }

    printf(
        "Loaded GLB: %s (%u meshes)\n",
        filename,
        static_cast<unsigned int>(
            model.meshes.size()));

    return !model.meshes.empty();
}

class CModelLoader : public IModelLoader
{
public:
    virtual ~CModelLoader()
    {
    }

    virtual bool Load(
        const char* filename,
        Model_t& model);

    virtual bool Upload(
        Model_t& model,
        IRenderDevice* device);

    virtual void Destroy(
        Model_t& model,
        IRenderDevice* device);
};

bool CModelLoader::Load(const char* fileName, Model_t& model)
{
	CGLBLoaderImpl loader;
	return loader.Load(fileName, model);
}

bool CModelLoader::Upload(Model_t& model, IRenderDevice* device)
{
    for (size_t i = 0;i < model.meshes.size();++i)
    {
        ModelMesh_t& mesh =
            model.meshes[i];

        mesh.vertexBuffer =
            device->CreateGPUBuffer(
                RENDER_BUFFER_TYPE_VERTEX,
                BufferDesc_t{},
                RenderBufferFlags_t{},
                "glb_vertex_buffer"
            );

        device->UploadGPUBuffer(
            mesh.vertexBuffer,
            mesh.vertices.data(),
            static_cast<uint32_t>(
                mesh.vertices.size() *
                sizeof(ModelVertex_t))
        );

        mesh.indexBuffer =
            device->CreateGPUBuffer(
                RENDER_BUFFER_TYPE_INDEX,
                BufferDesc_t{},
                RenderBufferFlags_t{},
                "glb_index_buffer"
            );

        device->UploadGPUBuffer(
            mesh.indexBuffer,
            mesh.indices.data(),
            static_cast<uint32_t>(
                mesh.indices.size() *
                sizeof(uint32_t))
        );
    }

    return true;
}

void CModelLoader::Destroy(Model_t& model, IRenderDevice* device)
{
}

static CModelLoader g_ModelLoader;
IModelLoader* g_pModelLoader = &g_ModelLoader;
IModelLoader* ModelLoader()
{
	return g_pModelLoader;
}
#ifndef EUPHORBE_MATERIAL_H
#define EUPHORBE_MATERIAL_H

#include <Euphorbe/Resource/Resource.h>

#include "Image.h"
#include "Buffer.h"
#include "ShaderCompiler.h"

#define EUPHORBE_MAX_DESCRIPTORS 32

typedef enum E_PrimitiveTopology E_PrimitiveTopology;
enum E_PrimitiveTopology
{
    E_PrimitiveTopologyTriangleList = 3,
    E_PrimitiveTopologyTriangleStrip = 4
};

typedef enum E_PolygonMode E_PolygonMode;
enum E_PolygonMode
{
    E_PolygonModeFill = 0,
    E_PolygonModeLine = 1,
    E_PolygonModePoint = 2
};

typedef enum E_CullMode E_CullMode;
enum E_CullMode
{
    E_CullModeNone = 0,
    E_CullModeFront = 0x00000001,
    E_CullModeBack = 0x00000002,
    E_CullModeFrontAndBack = 0x00000003
};

typedef enum E_FrontFace E_FrontFace;
enum E_FrontFace
{
    E_FrontFaceCCW = 0,
    E_FrontFaceCW = 1
};

typedef enum E_CompareOP E_CompareOP;
enum E_CompareOP
{
    E_CompareOPNever = 0,
    E_CompareOPLess = 1,
    E_CompareOPEqual = 2,
    E_CompareOPLessEqual = 3,
    E_CompareOPGreater = 4,
    E_CompareOPNotEqual = 5,
    E_CompareOPGreaterEqual = 6,
    E_CompareOPAlways = 7
};

typedef struct E_MaterialRenderInfo E_MaterialRenderInfo;
struct E_MaterialRenderInfo
{
    E_ImageFormat color_formats[32];
    E_ImageFormat depth_format;
    i32 color_attachment_count;
};

// Descriptor info
typedef enum E_DescriptorType E_DescriptorType;
enum E_DescriptorType
{
    E_DescriptorTypeSampler = 0,
    E_DescriptorTypeCombinedImageSampler = 1,
    E_DescriptorTypeSampledImage = 2,
    E_DescriptorTypeStorageImage = 3,
    E_DescriptorTypeUniformBuffer = 6
};

typedef struct E_Descriptor E_Descriptor;
struct E_Descriptor
{
    E_DescriptorType type;
    i32 binding;
};

typedef struct E_DescriptorSetLayout E_DescriptorSetLayout;
struct E_DescriptorSetLayout
{
    E_Descriptor descriptors[EUPHORBE_MAX_DESCRIPTORS];
    i32 descriptor_count;
};

typedef struct E_MaterialCreateInfo E_MaterialCreateInfo;
struct E_MaterialCreateInfo
{
    // Pipeline render info
    E_PrimitiveTopology primitive_topology;
    E_PolygonMode polygon_mode;
    E_CullMode cull_mode;
    E_FrontFace front_face;
    E_CompareOP depth_op;
    E_MaterialRenderInfo render_info;

    // Shaders
    E_ResourceFile* vertex_shader;
    E_ResourceFile* fragment_shader;
    E_ResourceFile* compute_shader;

    // Descriptors
    E_DescriptorSetLayout descriptor_set_layouts[EUPHORBE_MAX_DESCRIPTORS];
    i32 descriptor_set_layout_count;

    // Push constants
    b32 has_push_constants;
    i32 push_constants_size;
};

typedef struct E_Material E_Material;
struct E_Material
{
    E_MaterialCreateInfo* material_create_info;
    void* rhi_handle; // E_VulkanMaterial for example
    b32 loaded_from_file;
};

typedef struct E_MaterialInstance E_MaterialInstance;
struct E_MaterialInstance
{
    E_Material* material;
    void* rhi_handle;
};

// WARNING: Vertex shader MUST contain input variables!
E_Material* E_CreateMaterial(E_MaterialCreateInfo* create_info);
E_Material* E_CreateMaterialFromFile(const char* path);
E_Material* E_CreateComputeMaterialFromFile(const char* path);
void E_FreeMaterial(E_Material* material);

// Instances
E_MaterialInstance* E_CreateMaterialInstance(E_Material* material, i32 set_layout_index);
void E_MaterialInstanceWriteBuffer(E_MaterialInstance* instance, i32 binding, E_Buffer* buffer, i32 buffer_size);
void E_MaterialInstanceWriteSampler(E_MaterialInstance* instance, i32 binding, E_Sampler* sampler);
void E_MaterialInstanceWriteSampledImage(E_MaterialInstance* instance, i32 binding, E_Image* image);
void E_MaterialInstanceWriteImage(E_MaterialInstance* instance, i32 binding, E_Image* image, E_Sampler* sampler);
void E_MaterialInstanceWriteStorageImage(E_MaterialInstance* instance, i32 binding, E_Image* image, E_Sampler* sampler);
void E_FreeMaterialInstance(E_MaterialInstance* instance);

#endif
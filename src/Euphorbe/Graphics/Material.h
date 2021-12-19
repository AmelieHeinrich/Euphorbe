#ifndef EUPHORBE_MATERIAL_H
#define EUPHORBE_MATERIAL_H

#include "Image.h"
#include "ShaderCompiler.h"

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
    E_CompareOPLessOrEqual = 3,
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

typedef struct E_MaterialCreateInfo E_MaterialCreateInfo;
struct E_MaterialCreateInfo
{
    E_PrimitiveTopology primitive_topology;
    E_PolygonMode polygon_mode;
    E_CullMode cull_mode;
    E_FrontFace front_face;
    E_CompareOP depth_op;
    E_MaterialRenderInfo render_info;

    E_Shader* vertex_shader;
    E_Shader* fragment_shader;
};

typedef struct E_Material E_Material;
struct E_Material
{
    E_MaterialCreateInfo* material_create_info;
    void* rhi_handle; // E_VulkanMaterial for example
};

// WARNING: Vertex shader MUST contain input variables!
E_Material* E_CreateMaterial(E_MaterialCreateInfo* create_info);
void E_FreeMaterial(E_Material* material);

#endif
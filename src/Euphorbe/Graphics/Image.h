#ifndef EUPHORBE_IMAGE_H
#define EUPHORBE_IMAGE_H

#include <Euphorbe/Core/Common.h>

typedef enum E_ImageFormat E_ImageFormat;
enum E_ImageFormat
{
    E_ImageFormatRGBA8 = 37,
    E_ImageFormatRGBA16 = 97,
    E_ImageFormatD32_Float = 126,
};

typedef enum E_ImageLayout E_ImageLayout;
enum E_ImageLayout
{
    E_ImageLayoutUndefined = 0,
    E_ImageLayoutColor = 2,
    E_ImageLayoutDepth = 3,
    E_ImageLayoutDepthReadOnly = 4,
    E_ImageLayoutShaderRead = 5,
    E_ImageLayoutTransferSource = 6,
    E_ImageLayoutTransferDest = 7,
    E_ImageLayoutSwapchainPresent = 1000001002
};

typedef enum E_ImageAccess E_ImageAccess;
enum E_ImageAccess
{
    E_ImageAccessNone = 0x00000000,
    E_ImageAccessShaderRead = 0x00000020,
    E_ImageAccessShaderWrite = 0x00000040,
    E_ImageAccessColorRead = 0x00000080,
    E_ImageAccessColorWrite = 0x00000100,
    E_ImageAccessDepthRead = 0x00000200,
    E_ImageAccessDepthWrite = 0x00000400,
    E_ImageAccessTransferRead = 0x00000800,
    E_ImageAccessTransferWrite = 0x00001000
};

typedef enum E_ImagePipelineStage E_ImagePipelineStage;
enum E_ImagePipelineStage
{   
    E_ImagePipelineStageTop = 0x00000001,
    E_ImagePipelineStageVertexShader = 0x00000008,
    E_ImagePipelineStageFragmentShader = 0x00000080,
    E_ImagePipelineStageBottom = 0x00002000,
    E_ImagePipelineStageColorOutput = 0x00000400,
    E_ImagePipelineStageTransfer = 0x00001000,
    E_ImagePipelineStageEarlyFragment = 0x00000100,
    E_ImagePipelineStageLateFragment = 0x00000200
};

typedef struct E_Image E_Image;
struct E_Image
{
    i32 width;
    i32 height;
    E_ImageFormat format;
    void* rhi_handle;
};

E_Image* E_MakeImage(i32 width, i32 height, E_ImageFormat format);
E_Image* E_MakeImageFromFile(const char* path);
void E_FreeImage(E_Image* image);

void E_ImageTransitionLayout(E_Image* image, E_ImageAccess srcAccess, E_ImageAccess dstAccess, E_ImageLayout old, E_ImageLayout new, E_ImagePipelineStage srcStage, E_ImagePipelineStage dstStage);
void E_ImageResize(E_Image* image, i32 width, i32 height);

void E_ImageDrawToGUI(E_Image* image, i32 width, i32 height);

#endif
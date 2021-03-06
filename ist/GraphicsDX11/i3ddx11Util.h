﻿#ifndef __ist_i3ddx11_Util__
#define __ist_i3ddx11_Util__

#include "i3ddx11Shader.h"
#include "i3ddx11RenderTarget.h"

class SFMT;

namespace ist {
namespace i3ddx11 {

// 画像ファイル/ストリームからテクスチャ生成
bool CreateTexture2DFromFile(Texture2D& tex, const char *filename);
bool CreateTexture2DFromStream(Texture2D& tex, std::istream& st);

// 乱数テクスチャ生成
bool GenerateRandomTexture(Texture2D &tex, const uvec2 &size, I3D_COLOR_FORMAT format);
bool GenerateRandomTexture(Texture2D &tex, const uvec2 &size, I3D_COLOR_FORMAT format, SFMT& random);

// ファイル/ストリームから各種シェーダ生成
bool CreateVertexShaderFromFile(VertexShader& sh, const char *filename);
bool CreateGeometryShaderFromFile(GeometryShader& sh, const char *filename);
bool CreateFragmentShaderFromFile(PixelShader& sh, const char *filename);
bool CreateVertexShaderFromStream(VertexShader& sh, std::istream& st);
bool CreateGeometryShaderFromStream(GeometryShader& sh, std::istream& st);
bool CreateFragmentShaderFromStream(PixelShader& sh, std::istream& st);
bool CreateVertexShaderFromString(VertexShader& sh, const char* source);
bool CreateGeometryShaderFromString(GeometryShader& sh, const char* source);
bool CreateFragmentShaderFromString(PixelShader& sh, const char* source);


template<class BufferObjectType>
bool MapAndWrite(BufferObjectType& bo, const void *data, size_t data_size)
{
    if(void *p = bo.map(I3D_MAP_WRITE)) {
        ::memcpy(p, data, data_size);
        bo.unmap();
        return true;
    }
    return false;
}
template<class BufferObjectType>
bool MapAndRead(BufferObjectType& bo, void *data, size_t data_size)
{
    if(void *p = bo.map(I3D_MAP_READ)) {
        ::memcpy(data, p, data_size);
        bo.unmap();
        return true;
    }
    return false;
}



RenderTarget* CreateRenderTarget(Device *dev, uint32 num_color_buffers, const uvec2 &size,
    I3D_COLOR_FORMAT color_format);

RenderTarget* CreateRenderTarget(Device *dev, uint32 num_color_buffers, const uvec2 &size,
    I3D_COLOR_FORMAT *color_formats);

RenderTarget* CreateRenderTarget(Device *dev, uint32 num_color_buffers, const uvec2 &size,
    I3D_COLOR_FORMAT color_format, I3D_COLOR_FORMAT depthstencil_format);

RenderTarget* CreateRenderTarget(Device *dev, uint32 num_color_buffers, const uvec2 &size,
    I3D_COLOR_FORMAT *color_formats, I3D_COLOR_FORMAT depthstencil_format);



} // namespace i3ddx11
} // namespace ist
#endif // __ist_i3ddx11_Util__

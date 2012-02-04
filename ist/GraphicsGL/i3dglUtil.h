#ifndef __ist_i3dgl_Util__
#define __ist_i3dgl_Util__

#include "i3dglShader.h"
#include "i3dglRenderTarget.h"

class SFMT;

namespace ist {
namespace i3dgl {

// �摜�t�@�C��/�X�g���[������e�N�X�`������
Texture2D* CreateTexture2DFromFile(Device *dev, const char *filename);
Texture2D* CreateTexture2DFromStream(Device *dev, std::istream& st);

// �����e�N�X�`������
Texture2D* GenerateRandomTexture(Device *dev, const uvec2 &size, I3D_COLOR_FORMAT format);
Texture2D* GenerateRandomTexture(Device *dev, const uvec2 &size, I3D_COLOR_FORMAT format, SFMT& random);

// �t�@�C��/�X�g���[������e��V�F�[�_����
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
    if(data_size > bo.getDesc().size) { istAssert("exceeded buffer size.\n"); }

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
    I3D_COLOR_FORMAT color_format, uint32 mipmaps=0);

RenderTarget* CreateRenderTarget(Device *dev, uint32 num_color_buffers, const uvec2 &size,
    I3D_COLOR_FORMAT *color_formats, uint32 mipmaps=0);

RenderTarget* CreateRenderTarget(Device *dev, uint32 num_color_buffers, const uvec2 &size,
    I3D_COLOR_FORMAT color_format, I3D_COLOR_FORMAT depthstencil_format, uint32 mipmaps=0);

RenderTarget* CreateRenderTarget(Device *dev, uint32 num_color_buffers, const uvec2 &size,
    I3D_COLOR_FORMAT *color_formats, I3D_COLOR_FORMAT depthstencil_format, uint32 mipmaps=0);


Buffer* CreateVertexBuffer(Device *dev, uint32 size, I3D_USAGE usage, void *data=NULL);
Buffer* CreateIndexBuffer(Device *dev, uint32 size, I3D_USAGE usage, void *data=NULL);
Buffer* CreateUniformBuffer(Device *dev, uint32 size, I3D_USAGE usage, void *data=NULL);

} // namespace i3d
} // namespace ist
#endif // __ist_i3dgl_Util__

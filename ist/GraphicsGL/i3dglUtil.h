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
VertexShader*   CreateVertexShaderFromFile(Device *dev, const char *filename);
GeometryShader* CreateGeometryShaderFromFile(Device *dev, const char *filename);
PixelShader*    CreatePixelShaderFromFile(Device *dev, const char *filename);

VertexShader*   CreateVertexShaderFromStream(Device *dev, std::istream& st);
GeometryShader* CreateGeometryShaderFromStream(Device *dev, std::istream& st);
PixelShader*    CreatePixelShaderFromStream(Device *dev, std::istream& st);

VertexShader*   CreateVertexShaderFromString(Device *dev, const std::string &source);
GeometryShader* CreateGeometryShaderFromString(Device *dev, const std::string &source);
PixelShader*    CreatePixelShaderFromString(Device *dev, const std::string &source);


template<class BufferObjectType>
inline bool MapAndWrite(BufferObjectType& bo, const void *data, size_t data_size)
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
inline bool MapAndRead(BufferObjectType& bo, void *data, size_t data_size)
{
    if(data_size > bo.getDesc().size) { istAssert("exceeded buffer size.\n"); }

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

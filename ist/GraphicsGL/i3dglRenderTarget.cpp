#include "stdafx.h"
#include "../Base.h"
#include "i3dglTexture.h"
#include "i3dglRenderTarget.h"

namespace ist {
namespace i3dgl {

bool DetectGLFormat(I3D_COLOR_FORMAT fmt, GLint &internal_format, GLint &format, GLint &type)
{
    switch(fmt)
    {
    case I3D_R8U:       internal_format=GL_R8;      format=GL_RED;  type=GL_UNSIGNED_BYTE;  break;
    case I3D_R16F:      internal_format=GL_R16F;    format=GL_RED;  type=GL_FLOAT;          break;
    case I3D_R32F:      internal_format=GL_R32F;    format=GL_RED;  type=GL_FLOAT;          break;
    case I3D_RG8U:      internal_format=GL_RG8;     format=GL_RG;   type=GL_UNSIGNED_BYTE;  break;
    case I3D_RG16F:     internal_format=GL_RG16F;   format=GL_RG;   type=GL_FLOAT;          break;
    case I3D_RG32F:     internal_format=GL_RG32F;   format=GL_RG;   type=GL_FLOAT;          break;
    case I3D_RGB8U:     internal_format=GL_RGB8;    format=GL_RGB;  type=GL_UNSIGNED_BYTE;  break;
    case I3D_RGB16F:    internal_format=GL_RGB16F;  format=GL_RGB;  type=GL_FLOAT;          break;
    case I3D_RGB32F:    internal_format=GL_RGB32F;  format=GL_RGB;  type=GL_FLOAT;          break;
    case I3D_RGBA8U:    internal_format=GL_RGBA8;   format=GL_RGBA; type=GL_UNSIGNED_BYTE;  break;
    case I3D_RGBA16F:   internal_format=GL_RGBA16F; format=GL_RGBA; type=GL_FLOAT;          break;
    case I3D_RGBA32F:   internal_format=GL_RGBA32F; format=GL_RGBA; type=GL_FLOAT;          break;
    case I3D_DEPTH32F:  internal_format=GL_DEPTH_COMPONENT; format=GL_DEPTH_COMPONENT; type=GL_FLOAT; break;
    case I3D_DEPTH24_STENCIL8:    internal_format=GL_DEPTH24_STENCIL8;  format=GL_DEPTH_STENCIL; type=GL_UNSIGNED_INT_24_8; break;
    case I3D_DEPTH32F_STENCIL8:   internal_format=GL_DEPTH32F_STENCIL8; format=GL_DEPTH_STENCIL; type=GL_FLOAT_32_UNSIGNED_INT_24_8_REV; break;
    default:
        istAssert("unknown format: %d", fmt);
        return false;
    }
    return true;
}



RenderTarget::RenderTarget(Device *dev)
    : super(dev)
    , m_num_color_buffers(0)
    , m_depthstencil(NULL)
{
    glGenFramebuffers(1, &m_handle);

    stl::fill_n(m_color_buffers, _countof(m_color_buffers), (Texture2D*)NULL);
}

RenderTarget::~RenderTarget()
{
    releaseBuffers();
    if(m_handle!=0) {
        glDeleteFramebuffers(1, &m_handle);
        m_handle = 0;
    }
}

void RenderTarget::releaseBuffers()
{
    istSafeRelease(m_depthstencil);
    for(uint32 i=0; i<m_num_color_buffers; ++i) {
        istSafeRelease(m_color_buffers[i]);
    }
}


bool RenderTarget::setRenderBuffers(Texture2D **rb, uint32 num, Texture2D *depthstencil)
{
    if(num>=I3D_MAX_RENDER_TARGETS) {
        istPrint("number of render targets must be less than %d\n", I3D_MAX_RENDER_TARGETS);
        return false;
    }

    // ���o�b�t�@�� rb, depthstencil �����������w���Ă�\�������邽�߁A��ɎQ�ƃJ�E���^����
    for(uint32 i=0; i<num; ++i) { istSafeAddRef(rb[i]); }
    istSafeAddRef(depthstencil);

    // ���o�b�t�@�̎Q�ƃJ�E���^����
    releaseBuffers();

    m_num_color_buffers = num;
    stl::copy(rb, rb+num, m_color_buffers);
    m_depthstencil = depthstencil;

    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    for(uint32 i=0; i<num; ++i) {
        GLuint h = rb[i] ? rb[i]->getHandle() : 0;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, h, 0);
    }
    {
        GLuint h = depthstencil ? depthstencil->getHandle() : 0;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, h, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, h, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool RenderTarget::getRenderBuffers(Texture2D **rb, uint32 &num, Texture2D *&depthstencil)
{
    if(num < m_num_color_buffers) {
        return false;
    }

    num = m_num_color_buffers;
    stl::copy(m_color_buffers, m_color_buffers+m_num_color_buffers, rb);
    depthstencil = m_depthstencil;
    return true;
}

void RenderTarget::setNumColorBuffers(uint32 v)
{
    m_num_color_buffers = v;
}

void RenderTarget::setColorBuffer(uint32 i, Texture2D *rb)
{
    istSafeAddRef(rb);
    istSafeRelease(m_color_buffers[i]);

    {
        GLuint h = rb ? rb->getHandle() : 0;
        glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, h, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    m_color_buffers[i] = rb;

    uint32 used = 0;
    for(uint32 i=0; i<_countof(m_color_buffers); ++i) {
        if(m_color_buffers[i]) { used = i+1; }
    }
    m_num_color_buffers = used;
}

void RenderTarget::setDepthStencilBuffer(Texture2D *rb)
{
    istSafeAddRef(rb);
    istSafeRelease(m_depthstencil);

    {
        GLuint h = rb ? rb->getHandle() : 0;
        glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, h, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, h, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    m_depthstencil = rb;
}

uint32 RenderTarget::getNumColorBuffers() const
{
    return m_num_color_buffers;
}

Texture2D* RenderTarget::getColorBuffer(uint32 i)
{
    if(i >= m_num_color_buffers) { return NULL; }
    return m_color_buffers[i];
}

Texture2D* RenderTarget::getDepthStencilBuffer()
{
    return m_depthstencil;
}

void RenderTarget::bind() const
{
    static const GLuint attaches[16] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7,
        GL_COLOR_ATTACHMENT8,
        GL_COLOR_ATTACHMENT9,
        GL_COLOR_ATTACHMENT10,
        GL_COLOR_ATTACHMENT11,
        GL_COLOR_ATTACHMENT12,
        GL_COLOR_ATTACHMENT13,
        GL_COLOR_ATTACHMENT14,
        GL_COLOR_ATTACHMENT15,
    };
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    glDrawBuffers(m_num_color_buffers, attaches);
}

void RenderTarget::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//
//bool RenderTarget::attachRenderBuffer(RenderBuffer& rb, I3D_RT_ATTACH attach)
//{
//    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attach, GL_RENDERBUFFER, rb.getHandle());
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    return true;
//}

} // namespace i3d
} // namespace ist


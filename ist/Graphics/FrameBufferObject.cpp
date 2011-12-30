#include "stdafx.h"
#include "../Base/Assert.h"
#include "FrameBufferObject.h"

namespace ist {
namespace graphics {

Texture2D::Texture2D()
: m_handle(0)
, m_width(0)
, m_height(0)
{
}

Texture2D::~Texture2D()
{
    finalize();
}

bool Texture2D::initialize()
{
    glGenTextures(1, &m_handle);
    return true;
}

bool Texture2D::initialize(GLsizei width, GLsizei height, IST_COLOR_FORMAT fmt, void *data)
{
    return initialize() && allocate(width, height, fmt, data);
}

void Texture2D::finalize()
{
    if(m_handle!=0) {
        glDeleteTextures(1, &m_handle);
    }
    m_handle = 0;
}

bool Texture2D::allocate(GLsizei width, GLsizei height, IST_COLOR_FORMAT fmt, void *data)
{
    GLint internal_format = 0;
    GLint format = 0;
    GLint type = 0;
    switch(fmt)
    {
    case IST_R8U:       internal_format=GL_R8;      format=GL_RED;  type=GL_UNSIGNED_BYTE;  break;
    case IST_R16F:      internal_format=GL_R16F;    format=GL_RED;  type=GL_FLOAT;          break;
    case IST_R32F:      internal_format=GL_R32F;    format=GL_RED;  type=GL_FLOAT;          break;
    case IST_RG8U:      internal_format=GL_RG8;     format=GL_RG;   type=GL_UNSIGNED_BYTE;  break;
    case IST_RG16F:     internal_format=GL_RG16F;   format=GL_RG;   type=GL_FLOAT;          break;
    case IST_RG32F:     internal_format=GL_RG32F;   format=GL_RG;   type=GL_FLOAT;          break;
    case IST_RGB8U:     internal_format=GL_RGB8;    format=GL_RGB;  type=GL_UNSIGNED_BYTE;  break;
    case IST_RGB16F:    internal_format=GL_RGB16F;  format=GL_RGB;  type=GL_FLOAT;          break;
    case IST_RGB32F:    internal_format=GL_RGB32F;  format=GL_RGB;  type=GL_FLOAT;          break;
    case IST_RGBA8U:    internal_format=GL_RGBA8;   format=GL_RGBA; type=GL_UNSIGNED_BYTE;  break;
    case IST_RGBA16F:   internal_format=GL_RGBA16F; format=GL_RGBA; type=GL_FLOAT;          break;
    case IST_RGBA32F:   internal_format=GL_RGBA32F; format=GL_RGBA; type=GL_FLOAT;          break;
    case IST_DEPTH32F:  internal_format=GL_DEPTH_COMPONENT; format=GL_DEPTH_COMPONENT; type=GL_FLOAT; break;
    case IST_DEPTH24_STENCIL8:    internal_format=GL_DEPTH24_STENCIL8;  format=GL_DEPTH_STENCIL; type=GL_UNSIGNED_INT_24_8; break;
    case IST_DEPTH32F_STENCIL8:   internal_format=GL_DEPTH32F_STENCIL8; format=GL_DEPTH_STENCIL; type=GL_FLOAT_32_UNSIGNED_INT_24_8_REV; break;
    default:
        istAssert("unknown format: %d", fmt);
        return false;
    }

    m_width = width;
    m_height = height;
    glBindTexture( GL_TEXTURE_2D, m_handle );
    glTexImage2D( GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data );
    glBindTexture( GL_TEXTURE_2D, 0 );
    return true;
}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_handle);
}
void Texture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind(int slot) const
{
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D, m_handle);
}
void Texture2D::unbind(int slot) const
{
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture2D::getHandle() const { return m_handle; }
GLsizei Texture2D::getWidth() const { return m_width; }
GLsizei Texture2D::getHeight() const { return m_height; }



RenderBuffer::RenderBuffer()
: m_handle(0)
, m_width(0)
, m_height(0)
{
}

RenderBuffer::~RenderBuffer()
{
    finalize();
}

bool RenderBuffer::initialize()
{
    glGenRenderbuffers(1, &m_handle);
    return true;
}

bool RenderBuffer::initialize(GLsizei width, GLsizei height, IST_COLOR_FORMAT fmt)
{
    return initialize() && allocate(width, height, fmt);
}

void RenderBuffer::finalize()
{
    if(m_handle!=0) {
        glDeleteRenderbuffers(1, &m_handle);
    }
    m_handle = 0;
}

bool RenderBuffer::allocate(GLsizei width, GLsizei height, IST_COLOR_FORMAT fmt)
{
    GLint internal_format = 0;
    switch(fmt)
    {
    case IST_R8U:       internal_format=GL_R8; break;
    case IST_R16F:      internal_format=GL_R16F; break;
    case IST_R32F:      internal_format=GL_R32F; break;
    case IST_RG8U:      internal_format=GL_RG8; break;
    case IST_RG16F:     internal_format=GL_RG16F; break;
    case IST_RG32F:     internal_format=GL_RG32F; break;
    case IST_RGB8U:     // fall through
    case IST_RGB16F:    // 
    case IST_RGB32F:    istAssert("render buffer can't use RGB format."); break;
    case IST_RGBA8U:    internal_format=GL_RGBA8; break;
    case IST_RGBA16F:   internal_format=GL_RGBA16F; break;
    case IST_RGBA32F:   internal_format=GL_RGBA32F; break;
    case IST_DEPTH32F:  internal_format=GL_DEPTH_COMPONENT32; break;
    case IST_DEPTH24_STENCIL8:    internal_format=GL_DEPTH24_STENCIL8; break;
    case IST_DEPTH32F_STENCIL8:   internal_format=GL_DEPTH32F_STENCIL8; break;
    default:
        istAssert("unknown format: %d", fmt);
        return false;
    }

    m_width = width;
    m_height = height;
    glBindRenderbuffer( GL_RENDERBUFFER, m_handle );
    glRenderbufferStorage( GL_RENDERBUFFER, internal_format, width, height );
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
    return true;
}

void RenderBuffer::bind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_handle);
}

void RenderBuffer::unbind() const
{
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
}

GLuint RenderBuffer::getHandle() const { return m_handle; }
GLsizei RenderBuffer::getWidth() const { return m_width; }
GLsizei RenderBuffer::getHeight() const { return m_height; }



FrameBufferObject::FrameBufferObject()
: m_handle(0)
, m_attaches(0)
{
}

FrameBufferObject::~FrameBufferObject()
{
    finalize();
}

bool FrameBufferObject::initialize()
{
    glGenFramebuffers(1, &m_handle);
    return true;
}

void FrameBufferObject::finalize()
{
    if(m_handle!=0) {
        glDeleteFramebuffers(1, &m_handle);
    }
    m_handle = 0;
}

bool FrameBufferObject::attachRenderBuffer(RenderBuffer& rb, IST_RT_ATTACH attach)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attach, GL_RENDERBUFFER, rb.getHandle());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(attach!=IST_ATTACH_DEPTH) {
        m_attaches |= 1<<(attach-IST_ATTACH_COLOR0);
    }
    return true;
}

bool FrameBufferObject::attachTexture(Texture2D& tex, IST_RT_ATTACH attach, GLint level)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, tex.getHandle(), level);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(attach!=IST_ATTACH_DEPTH) {
        m_attaches |= 1<<(attach-IST_ATTACH_COLOR0);
    }
    return true;
}

void FrameBufferObject::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

    GLint num_attaches = 0;
    GLuint attaches[16];
    for(int i=0; i<_countof(attaches); ++i) {
        if((m_attaches & (1<<i)) != 0) {
            attaches[num_attaches++] = IST_ATTACH_COLOR0+i;
        }
    }
    glDrawBuffers(num_attaches, attaches);
}

void FrameBufferObject::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace graphics
} // namespace ist



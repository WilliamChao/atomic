#include "stdafx.h"
#include "../Base.h"
#include "i3dglDevice.h"
#include "i3dglDeviceContext.h"

namespace ist {
namespace i3dgl {

#ifdef _WIN32

Device::Device(HWND hwnd) : m_hwnd(hwnd), m_context(NULL)
{
    m_hdc = ::GetDC(m_hwnd);

    int pixelformat;
    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),    //���̍\���̂̃T�C�Y
        1,                  //OpenGL�o�[�W����
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,       //�_�u���o�b�t�@�g�p�\
        PFD_TYPE_RGBA,      //RGBA�J���[
        32,                 //�F��
        0, 0,               //RGBA�̃r�b�g�ƃV�t�g�ݒ�        
        0, 0,                //G
        0, 0,                //B
        0, 0,                //A
        0,                  //�A�L�������[�V�����o�b�t�@
        0, 0, 0, 0,         //RGBA�A�L�������[�V�����o�b�t�@
        32,                 //Z�o�b�t�@    
        0,                  //�X�e���V���o�b�t�@
        0,                  //�g�p���Ȃ�
        PFD_MAIN_PLANE,     //���C���[�^�C�v
        0,                  //�\��
        0, 0, 0             //���C���[�}�X�N�̐ݒ�E���g�p
    };

    // glew �p�̉��̃R���e�L�X�g����
    if(((pixelformat = ::ChoosePixelFormat(m_hdc, &pfd)) == 0)
        || ((::SetPixelFormat(m_hdc, pixelformat, &pfd) == FALSE))
        || (!(m_hglrc=::wglCreateContext(m_hdc)))) {
            istPrint("OpenGL initialization failed");
    }
    wglMakeCurrent(m_hdc, m_hglrc);
    glewInit();
    {
        const GLubyte *version = glGetString(GL_VERSION);
        const GLubyte *vendor = glGetString(GL_VENDOR);
        istPrint("OpenGL version: %s, vendor: %s\n", version, vendor);
    }

    m_context = istNew(DeviceContext)(this);
}

Device::~Device()
{
    for(uint32 i=0; i<m_resources.size(); ++i) {
        istSafeDelete(m_resources[i]);
    }

    istSafeDelete(m_context);

    if(m_hglrc!=NULL) {
        ::wglMakeCurrent(NULL, NULL);
        ::wglDeleteContext(m_hglrc);
        m_hglrc = NULL;
    }
    if(m_hdc!=NULL) {
        ::ReleaseDC(m_hwnd, m_hdc);
        m_hdc = NULL;
    }
}
#endif // _WIN32


void Device::addResource( DeviceResource *v )
{
    if(!v) { return; }

    if(!m_vacant.empty()) {
        ResourceHandle drh = m_vacant.back();
        m_vacant.pop_back();
        m_resources[drh] = v;
        v->setDeviceResourceHandle(drh);
    }
    else {
        v->setDeviceResourceHandle(m_resources.size());
        m_resources.push_back(v);
    }
}

void Device::deleteResource( ResourceHandle v )
{
    istSafeDelete(m_resources[v]);
    m_vacant.push_back(v);
}

Buffer* Device::createBuffer(const BufferDesc &desc)
{
    Buffer *r = istNew(Buffer)(this, desc);
    addResource(r);
    return r;
}

VertexArray* Device::createVertexArray()
{
    VertexArray *r = istNew(VertexArray)(this);
    addResource(r);
    return r;
}

VertexShader* Device::createVertexShader()
{
    VertexShader *r = istNew(VertexShader)(this);
    addResource(r);
    return r;
}

PixelShader* Device::createPixelShader()
{
    PixelShader *r = istNew(PixelShader)(this);
    addResource(r);
    return r;
}

GeometryShader* Device::createGeometryShader()
{
    GeometryShader *r = istNew(GeometryShader)(this);
    addResource(r);
    return r;
}

ShaderProgram* Device::createShaderProgram()
{
    ShaderProgram *r = istNew(ShaderProgram)(this);
    addResource(r);
    return r;
}

Texture2D* Device::createTexture2D()
{
    Texture2D *r = istNew(Texture2D)(this);
    addResource(r);
    return r;
}

RenderTarget* Device::createRenderTarget()
{
    RenderTarget *r = istNew(RenderTarget)(this);
    addResource(r);
    return r;
}

void Device::swapBuffers()
{
    ::SwapBuffers(m_hdc);
}

} // namespace i3d
} // namespace ist

#include "stdafx.h"
#include "../Base.h"
#include "i3dDevice.h"

namespace ist {
namespace i3d {

#ifdef _WIN32

Device::Device(HWND hwnd) : m_hwnd(hwnd)
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
}

Device::~Device()
{
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

    v->setOwnerDevice(this);
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

VertexBuffer* Device::createVertexBuffer()
{
    VertexBuffer *r = istNew(VertexBuffer)();
    m_resources.push_back(r);
    return r;
}

IndexBuffer* Device::createIndexBuffer()
{
    IndexBuffer *r = istNew(IndexBuffer)();
    m_resources.push_back(r);
    return r;
}

UniformBuffer* Device::createUniformBuffer()
{
    UniformBuffer *r = istNew(UniformBuffer)();
    m_resources.push_back(r);
    return r;
}

PixelBuffer* Device::createPixelBuffer()
{
    PixelBuffer *r = istNew(PixelBuffer)();
    m_resources.push_back(r);
    return r;
}

VertexArray* Device::createVertexArray()
{
    VertexArray *r = istNew(VertexArray)();
    m_resources.push_back(r);
    return r;
}

VertexShader* Device::createVertexShader()
{
    VertexShader *r = istNew(VertexShader)();
    m_resources.push_back(r);
    return r;
}

PixelShader* Device::createPixelShader()
{
    PixelShader *r = istNew(PixelShader)();
    m_resources.push_back(r);
    return r;
}

GeometryShader* Device::createGeometryShader()
{
    GeometryShader *r = istNew(GeometryShader)();
    m_resources.push_back(r);
    return r;
}

void Device::swapBuffers()
{
    ::SwapBuffers(m_hdc);
}

} // namespace i3d
} // namespace ist

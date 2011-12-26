#ifndef __atomic_Graphics_Renderer_h__
#define __atomic_Graphics_Renderer_h__

#include "ResourceManager.h"
#include "Light.h"

namespace atomic {


class IRenderer : public boost::noncopyable
{
public:
    virtual ~IRenderer() {}
    virtual void beforeDraw()=0;    // ���C���X���b�h����A�`�揈���̑O�ɌĂ΂��B���_�f�[�^�̗p�ӂȂǂ��s��
    virtual void draw()=0;          // �`��X���b�h����Ă΂��B���_�f�[�^�� GPU �ւ̓]���A�`��R�}���h���s�Ȃǂ��s��
};


class PassGBuffer_SPH;
class PassGBuffer_ParticleSet;
class PassDeferredShading_DirectionalLights;
class PassDeferredShading_PointLights;
class PassPostprocess_Bloom;


class AtomicRenderer : public boost::noncopyable
{
private:
    // shared resources
    VertexArray             *m_va_screenquad;
    AtomicShader            *m_sh_out;
    RenderTargetGBuffer     *m_rt_gbuffer;
    RenderTargetDeferred    *m_rt_deferred;

    // internal resources
    PassGBuffer_SPH                         *m_renderer_sph;
    PassDeferredShading_DirectionalLights   *m_renderer_dir_lights;
    PassDeferredShading_PointLights         *m_renderer_point_lights;
    PassPostprocess_Bloom                   *m_renderer_bloom;
    stl::vector<IRenderer*>                 m_renderers[PASS_END];

    Viewport        m_default_viewport;
    RenderStates    m_render_states;

private:
    static AtomicRenderer *s_inst;

    void passShadow();
    void passGBuffer();
    void passDeferredShading();
    void passForwardShading();
    void passPostprocess();
    void passHUD();
    void passOutput();

public:
    AtomicRenderer();
    ~AtomicRenderer();
    static AtomicRenderer* getInstance() { return s_inst; }
    static void initializeInstance();
    static void finalizeInstance();

    void beforeDraw();  // ���C���X���b�h����A�`�揈���̑O�ɌĂ΂��
    void draw();        // �ȉ��`��X���b�h����Ă΂��

    PassGBuffer_SPH* getSPHRenderer()                               { return m_renderer_sph; }
    PassDeferredShading_DirectionalLights* getDirectionalLights()   { return m_renderer_dir_lights; }
    PassDeferredShading_PointLights* getPointLights()               { return m_renderer_point_lights; }
    const Viewport* getDefaultViewport() const                      { return &m_default_viewport; }
    RenderStates* getRenderStates()                                 { return &m_render_states; }
};

#define atomicGetRenderer()             AtomicRenderer::getInstance()
#define atomicGetSPHRenderer()          atomicGetRenderer()->getSPHRenderer()
#define atomicGetDirectionalLights()    atomicGetRenderer()->getDirectionalLights()
#define atomicGetPointLights()          atomicGetRenderer()->getPointLights()
#define atomicGetDefaultViewport()      atomicGetRenderer()->getDefaultViewport()




class PassGBuffer_SPH : public IRenderer
{
private:
    VertexArray         *m_va_cube;
    AtomicShader        *m_sh_fluid;
    AtomicShader        *m_sh_rigid;
    VertexBufferObject  *m_vbo_fluid;
    VertexBufferObject  *m_vbo_rigid;

    stl::vector<Task*>          m_tasks;
    stl::vector<PSetInstance>   m_rinstances;
    stl::vector<PSetParticle>   m_rparticles;

    void resizeTasks(uint32 n);

public:
    PassGBuffer_SPH();
    ~PassGBuffer_SPH();
    void beforeDraw();  // ���C���X���b�h����A�`�揈���̑O�ɌĂ΂��
    void draw();    // �`��X���b�h����Ă΂��

    void addRigidInstance(PSET_RID psid, const mat4 &t, const vec4 &diffuse, const vec4 &glow, const vec4 &flash);
};


class PassDeferredShading_DirectionalLights : public IRenderer
{
private:
    typedef DirectionalLight light_t;
    typedef stl::vector<DirectionalLight> InstanceCont;
    InstanceCont        m_instances;
    AtomicShader        *m_shader;
    VertexArray         *m_va_quad;
    VertexBufferObject  *m_vbo_instance;

public:
    PassDeferredShading_DirectionalLights();
    void beforeDraw();
    void draw();

    void addInstance(const DirectionalLight& v);
};

class PassDeferredShading_PointLights : public IRenderer
{
public:

private:
    typedef stl::vector<PointLight> InstanceCont;
    InstanceCont        m_instances;
    AtomicShader        *m_shader;
    IndexBufferObject   *m_ibo_sphere;
    VertexArray         *m_va_sphere;
    VertexBufferObject  *m_vbo_instance;

public:
    PassDeferredShading_PointLights();
    void beforeDraw();
    void draw();

    void addInstance(const PointLight& v) { m_instances.push_back(v); }
};

//
//class PassPostprocess_FXAA : public Renderer
//{
//private:
//    ColorBuffer *m_rt_RGBL;
//    ShaderFXAA *m_sh_FXAA;
//
//public:
//    PassPostprocess_FXAA();
//    void beforeDraw();
//    void draw();
//};

class PassPostprocess_Bloom : public IRenderer
{
private:
    RenderTargetGBuffer     *m_rt_gbuffer;
    RenderTargetDeferred    *m_rt_deferred;
    RenderTargetGauss       *m_rt_gauss0;
    RenderTargetGauss       *m_rt_gauss1;
    VertexArray             *m_va_luminance;
    VertexArray             *m_va_blur;
    VertexArray             *m_va_composite;
    AtomicShader            *m_sh_luminance;
    AtomicShader            *m_sh_hblur;
    AtomicShader            *m_sh_vblur;
    AtomicShader            *m_sh_composite;
    UniformBufferObject     *m_ubo_states;

public:
    PassPostprocess_Bloom();
    void beforeDraw();
    void draw();
};

} // namespace atomic
#endif // __atomic_Graphics_Renderer_h__

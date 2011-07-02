#ifndef __atomic_Graphics_Renderer__
#define __atomic_Graphics_Renderer__

#include "GraphicResourceManager.h"

namespace atomic {


class Renderer : public boost::noncopyable
{
public:
    virtual ~Renderer() {}
    virtual void beforeDraw()=0;  // ���C���X���b�h����A�`�揈���̑O�ɌĂ΂��
    virtual void draw()=0;    // �`��X���b�h����Ă΂��
};


class PassGBuffer_Cube;
class PassDeferred_SphereLight;


class AtomicRenderer : public boost::noncopyable
{
private:
    ShaderGBuffer *m_sh_gbuffer;
    ShaderDeferred *m_sh_deferred;
    ProgramObject *m_sh_out;

    RenderTargetGBuffer *m_rt_gbuffer;
    RenderTargetDeferred *m_rt_deferred;

    PassGBuffer_Cube *m_renderer_cube;
    PassDeferred_SphereLight *m_renderer_sphere_light;
    stl::vector<Renderer*> m_renderers[PASS_END];

private:
    static AtomicRenderer *s_inst;

    void pass_Shadow();
    void pass_GBuffer();
    void pass_Deferred();
    void pass_Forward();
    void pass_Postprocess();
    void pass_UI();
    void pass_Output();

public:
    AtomicRenderer();
    ~AtomicRenderer();
    static AtomicRenderer* getInstance() { return s_inst; }
    static void initializeInstance();
    static void finalizeInstance();

    void beforeDraw();  // ���C���X���b�h����A�`�揈���̑O�ɌĂ΂��
    void draw();        // �ȉ��`��X���b�h����Ă΂��

    PassGBuffer_Cube* getCubeRenderer() { return m_renderer_cube; }
    PassDeferred_SphereLight* getSphereLightRenderer() { return m_renderer_sphere_light; }
};

#define GetCubeRenderer() AtomicRenderer::getInstance()->getCubeRenderer()
#define GetSphereLightRenderer() AtomicRenderer::getInstance()->getSphereLightRenderer()


class PassGBuffer_Cube : public Renderer
{
private:
    stl::vector<XMVECTOR> m_instance_pos;
    ShaderGBuffer *m_sh_gbuffer;
    ModelData *m_model;
    VertexBufferObject *m_vbo_instance_pos;

public:
    PassGBuffer_Cube();
    void beforeDraw();  // ���C���X���b�h����A�`�揈���̑O�ɌĂ΂��
    void draw();    // �`��X���b�h����Ă΂��

    void pushInstance(XMVECTOR v) { m_instance_pos.push_back(v); }
};

class PassDeferred_SphereLight : public Renderer
{
private:
    stl::vector<XMVECTOR> m_instance_pos;
    ShaderDeferred *m_sh_deferred;
    ModelData *m_model;
    VertexBufferObject *m_vbo_instance_pos;

public:
    PassDeferred_SphereLight();
    void beforeDraw();
    void draw();

    void pushInstance(XMVECTOR v) { m_instance_pos.push_back(v); }
};

} // namespace atomic
#endif // __atomic_Graphics_Renderer__

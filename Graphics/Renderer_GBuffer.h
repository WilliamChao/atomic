#ifndef __atomic_Graphics_Renderer_GBuffer__
#define __atomic_Graphics_Renderer_GBuffer__
namespace atomic {


class PassGBuffer_SPH : public IRenderer
{
private:
    VertexArray     *m_va_cube;
    VertexBuffer    *m_vbo_fluid;
    VertexBuffer    *m_vbo_rigid;
    AtomicShader    *m_sh_fluid;
    AtomicShader    *m_sh_rigid;

    stl::vector<Task*>          m_tasks;
    stl::vector<PSetInstance>   m_rinstances;
    stl::vector<PSetParticle>   m_rparticles;

    void resizeTasks(uint32 n);

public:
    PassGBuffer_SPH();
    ~PassGBuffer_SPH();
    void beforeDraw();  // ���C���X���b�h����A�`�揈���̑O�ɌĂ΂��
    void draw();    // �`��X���b�h����Ă΂��

    void addPSetInstance(PSET_RID psid, const mat4 &t, const vec4 &diffuse, const vec4 &glow, const vec4 &flash);
};


} // namespace atomic
#endif // __atomic_Graphics_Renderer_GBuffer__

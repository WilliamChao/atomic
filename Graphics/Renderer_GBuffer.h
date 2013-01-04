﻿#ifndef atomic_Graphics_Renderer_GBuffer_h
#define atomic_Graphics_Renderer_GBuffer_h
namespace atomic {

class UpdateRigidParticle
{
private:
    const PSetUpdateInfo *m_rinst;
    PSetParticle *m_particles;

public:
    UpdateRigidParticle(const PSetUpdateInfo &ri, PSetParticle *p);
    void exec();
};


class PassGBuffer_Particle : public IRenderer
{
private:
    VertexArray     *m_va_cube;
    Buffer          *m_vbo;
    AtomicShader    *m_sh;
    stl::vector<IndivisualParticle> m_particles;

public:
    PassGBuffer_Particle();
    ~PassGBuffer_Particle();
    void beforeDraw();
    void draw();

    void addParticle(const IndivisualParticle *particles, uint32 num);
};

class PassGBuffer_SPH : public IRenderer
{
private:
    VertexArray     *m_va_cube;
    Buffer          *m_vbo_fluid;
    Buffer          *m_vbo_rigid;
    AtomicShader    *m_sh_fluid;
    AtomicShader    *m_sh_rigid;

    stl::vector<PSetUpdateInfo> m_rupdateinfo;
    stl::vector<PSetParticle>   m_rparticles;
    stl::vector<PSetInstance>   m_rinstances;
    stl::vector<UpdateRigidParticle>    m_updater;

public:
    PassGBuffer_SPH();
    ~PassGBuffer_SPH();
    void beforeDraw();
    void draw();

    void addPSetInstance(PSET_RID psid, const PSetInstance inst);
};

} // namespace atomic
#endif // atomic_Graphics_Renderer_GBuffer_h

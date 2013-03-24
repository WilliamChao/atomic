﻿#ifndef atomic_Graphics_Renderer_GBuffer_h
#define atomic_Graphics_Renderer_GBuffer_h
namespace atomic {



class PassGBuffer_Particle : public IRenderer
{
public:
    PassGBuffer_Particle();
    ~PassGBuffer_Particle();
    void beforeDraw();
    void draw();

    void addParticle(const IndivisualParticle *particles, uint32 num);

private:
    ist::vector<IndivisualParticle> m_particles;
};

class PassGBuffer_Fluid : public IRenderer
{
public:
    PassGBuffer_Fluid();
    ~PassGBuffer_Fluid();
    void beforeDraw();
    void draw();

    void addPSetInstance(PSET_RID psid, const PSetInstance &inst);

private:
    ist::vector<PSetUpdateInfo> m_rupdateinfo;
    ist::vector<PSetParticle>   m_rparticles;
    ist::vector<PSetInstance>   m_rinstances;
};


class PassGBuffer_BG : public IRenderer
{
public:
    PassGBuffer_BG();
    ~PassGBuffer_BG();
    void beforeDraw();
    void draw();

private:
    bool m_enabled;
};

} // namespace atomic
#endif // atomic_Graphics_Renderer_GBuffer_h

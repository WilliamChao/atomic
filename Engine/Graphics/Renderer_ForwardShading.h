﻿#ifndef atm_Engine_Graphics_Renderer_ForwardShading_h
#define atm_Engine_Graphics_Renderer_ForwardShading_h
namespace atm {


class atmAPI PassForward_Generic : public IRenderer
{
public:
    PassForward_Generic();
    ~PassForward_Generic();
    void beforeDraw();
    void draw();

    struct InstanceParams {
        mat4 transform;
        vec4 params[4];
    };
    void drawModel(SH_RID shader, MODEL_RID model, const mat4 &transform);
    void drawModel(SH_RID shader, MODEL_RID model, const InstanceParams &params);

private:
    typedef std::vector<InstanceParams> ParamCont;
    typedef std::map<MODEL_RID, ParamCont> ModelParamCont;
    typedef std::map<SH_RID, ModelParamCont> ShModelParamCont;
    ShModelParamCont m_commands;
    ParamCont m_params;
    int32 m_frame;
};

class atmAPI PassForward_Indicators : public IRenderer
{
public:
    PassForward_Indicators();
    ~PassForward_Indicators();
    void beforeDraw();
    void draw();

private:
};

class atmAPI PassForward_Barrier : public IRenderer
{
public:
    PassForward_Barrier();
    ~PassForward_Barrier();
    void beforeDraw();
    void draw();

    void addParticles(PSET_RID psid, const PSetInstance &inst, uint32 n=0);

private:
    PSetDrawData m_solids;
};

class atmAPI PassForward_Background : public IRenderer
{
public:
    PassForward_Background();
    ~PassForward_Background();
    void beforeDraw();
    void draw();

    void setBGShader(SH_RID v) { m_shader=v; }

private:
    int32 m_shader;
};


} // namespace atm
#endif // atm_Engine_Graphics_Renderer_ForwardShading_h

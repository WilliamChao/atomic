#include "stdafx.h"
#include "types.h"
#include "Game/AtomicApplication.h"
#include "Game/AtomicGame.h"
#include "Game/World.h"
#include "AtomicRenderingSystem.h"
#include "Renderer.h"
#include "Util.h"

namespace atomic {


PassPostprocess_FXAA::PassPostprocess_FXAA()
{
    m_rt_deferred   = atomicGetRenderTarget(RT_DEFERRED);
    m_rt_RGBL       = atomicGetRenderTarget(RT_POSTPROCESS);
    m_sh_FXAA_luma  = atomicGetShader(SH_FXAA_LUMA);
    m_sh_FXAA       = atomicGetShader(SH_FXAA);
    m_va_quad       = atomicGetVertexArray(VA_SCREEN_QUAD);

    m_loc_fxaa_param = m_sh_FXAA->getUniformBlockIndex("fxaa_params");
}

void PassPostprocess_FXAA::beforeDraw()
{
}

void PassPostprocess_FXAA::draw()
{
    if(!atomicGetConfig()->posteffect_antialias) { return; }

    UniformBuffer *ubo_fxaa                 = atomicGetUniformBufferObject(UBO_FXAA_PARAMS);
    m_fxaaparams.fxaaQualityRcpFrame        = vec2(1.0f, 1.0f) / vec2(atomicGetWindowSize());
    m_fxaaparams.fxaaQualitySubpix          = 0.75f;
    m_fxaaparams.fxaaQualityEdgeThreshold   = 0.166f;
    m_fxaaparams.fxaaQualityEdgeThresholdMin= 0.0833f;
    MapAndWrite(*ubo_fxaa, &m_fxaaparams, sizeof(m_fxaaparams));

    // �P�x���o
    {
        m_sh_FXAA_luma->bind();
        m_rt_RGBL->bind();
        m_rt_deferred->getColorBuffer(GBUFFER_COLOR)->bind(GLSL_COLOR_BUFFER);
        m_va_quad->bind();
        glDrawArrays(GL_QUADS, 0, 16);
        m_rt_RGBL->unbind();
        m_sh_FXAA_luma->unbind();
    }
    // FXAA
    {
        m_sh_FXAA->bind();
        m_sh_FXAA->setUniformBlock(m_loc_fxaa_param, GLSL_FXAA_BINDING, ubo_fxaa->getHandle());
        m_rt_deferred->bind();
        m_rt_RGBL->getColorBuffer(0)->bind(GLSL_COLOR_BUFFER);
        m_va_quad->bind();
        glDrawArrays(GL_QUADS, 0, 16);
        m_rt_deferred->unbind();
        m_sh_FXAA_luma->unbind();
    }
}



PassPostprocess_Bloom::PassPostprocess_Bloom()
: m_rt_deferred(NULL)
, m_rt_gauss0(NULL)
, m_rt_gauss1(NULL)
, m_va_luminance(NULL)
, m_va_blur(NULL)
, m_va_composite(NULL)
, m_sh_luminance(NULL)
, m_sh_hblur(NULL)
, m_sh_vblur(NULL)
, m_sh_composite(NULL)
, m_ubo_states(NULL)
{
    m_rt_gbuffer    = atomicGetRenderTarget(RT_GBUFFER);
    m_rt_deferred   = atomicGetRenderTarget(RT_DEFERRED);
    m_rt_gauss0     = atomicGetRenderTarget(RT_GAUSS0);
    m_rt_gauss1     = atomicGetRenderTarget(RT_GAUSS1);
    m_va_luminance  = atomicGetVertexArray(VA_BLOOM_LUMINANCE_QUADS);
    m_va_blur       = atomicGetVertexArray(VA_BLOOM_BLUR_QUADS);
    m_va_composite  = atomicGetVertexArray(VA_BLOOM_COMPOSITE_QUAD);
    m_sh_luminance  = atomicGetShader(SH_BLOOM_LUMINANCE);
    m_sh_hblur      = atomicGetShader(SH_BLOOM_HBLUR);
    m_sh_vblur      = atomicGetShader(SH_BLOOM_VBLUR);
    m_sh_composite  = atomicGetShader(SH_BLOOM_COMPOSITE);
    m_ubo_states    = atomicGetUniformBufferObject(UBO_BLOOM_PARAMS);
}

void PassPostprocess_Bloom::beforeDraw()
{
}

void PassPostprocess_Bloom::draw()
{
    if(!atomicGetConfig()->posteffect_bloom) { return; }

    Viewport vp(ivec2(), m_rt_gauss0->getColorBuffer(0)->getSize());
    vp.bind();

    // �P�x���o
    {
        m_sh_luminance->bind();
        m_rt_gauss0->bind();
        m_rt_deferred->getColorBuffer(GBUFFER_COLOR)->bind(GLSL_COLOR_BUFFER);
        m_rt_gbuffer->getColorBuffer(GBUFFER_GLOW)->bind(GLSL_GLOW_BUFFER);
        m_va_luminance->bind();
        glDrawArrays(GL_QUADS, 0, 16);
        m_rt_gauss0->unbind();
        m_sh_luminance->unbind();
    }

    // ���u���[
    {
        m_sh_hblur->bind();
        m_rt_gauss1->bind();
        m_rt_gauss0->getColorBuffer(GBUFFER_COLOR)->bind(GLSL_COLOR_BUFFER);
        m_va_blur->bind();
        glDrawArrays(GL_QUADS, 0, 16);
        m_rt_gauss1->unbind();
        m_sh_hblur->unbind();
    }

    // �c�u���[
    {
        m_sh_vblur->bind();
        m_rt_gauss0->bind();
        m_rt_gauss1->getColorBuffer(GBUFFER_COLOR)->bind(GLSL_COLOR_BUFFER);
        m_va_blur->bind();
        glDrawArrays(GL_QUADS, 0, 16);
        m_rt_gauss0->unbind();
        m_sh_vblur->unbind();
    }

    // ���Z
    atomicGetDefaultViewport()->bind();
    {
        m_sh_composite->bind();
        m_rt_deferred->bind();
        m_rt_gauss0->getColorBuffer(GBUFFER_COLOR)->bind(GLSL_COLOR_BUFFER);
        m_va_composite->bind();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisable(GL_BLEND);
        m_rt_gauss0->getColorBuffer(0)->unbind();
        m_rt_deferred->unbind();
        m_sh_composite->unbind();
    }
}


PassPostprocess_Fade::PassPostprocess_Fade()
{
    m_rt_deferred   = atomicGetRenderTarget(RT_DEFERRED);
    m_sh_fade       = atomicGetShader(SH_FADE);
    m_va_quad       = atomicGetVertexArray(VA_SCREEN_QUAD);
    m_ubo_fade      = atomicGetUniformBufferObject(UBO_FADE_PARAMS);

    m_loc_fade_param = m_sh_fade->getUniformBlockIndex("fade_params");
}

void PassPostprocess_Fade::beforeDraw()
{
    uint32 frame = atomicGetFrame();
    if(frame > m_end_frame) { return; }

    vec4 diff = m_end_color-m_begin_color;
    uint32 f = m_end_frame-m_begin_frame;
    float32 l = float32(frame-m_begin_frame)/f;
    m_params.color = m_begin_color + diff*l;
}

void PassPostprocess_Fade::draw()
{
    if(m_params.color.a==0.0f) { return; }

    MapAndWrite(*m_ubo_fade, &m_params, sizeof(m_params));

    m_sh_fade->bind();
    m_sh_fade->setUniformBlock(m_loc_fade_param, GLSL_FADE_BINDING, m_ubo_fade->getHandle());
    m_rt_deferred->bind();
    m_va_quad->bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisable(GL_BLEND);
    m_rt_deferred->unbind();
    m_sh_fade->unbind();
}

void PassPostprocess_Fade::setFade(const vec4 &v, uint32 frame)
{
    m_begin_color = m_params.color;
    m_end_color = v;
    m_begin_frame = atomicGetFrame();
    m_end_frame = m_begin_frame+frame;
}

} // namespace atomic
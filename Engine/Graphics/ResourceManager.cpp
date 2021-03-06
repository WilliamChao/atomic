﻿#include "atmPCH.h"
#include "ist/ist.h"
#include "types.h"
#include "Engine/Game/AtomicApplication.h"
#include "Engine/Graphics/AtomicRenderingSystem.h"
#include "Engine/Graphics/ResourceManager.h"
#include "Engine/Graphics/Light.h"
#include "psym/psym.h"

namespace atm {

const int MAX_RIGID_PARTICLES = 65536 * 4;
const int MAX_EFFECT_PARTICLES = 65536 * 4;
const int MAX_BLOODSTAIN_PARTICLES = 65536 * 4;

uvec2 CalcFrameBufferSize()
{
    uvec2 r = uvec2(128);
    uvec2 wsize = atmGetWindowSize();
    while(r.x < wsize.x) { r.x *= 2; }
    while(r.y < wsize.y) { r.y *= 2; }
    return r;
}


GraphicResourceManager* GraphicResourceManager::s_inst = nullptr;

void GraphicResourceManager::intializeInstance()
{
    s_inst = istNew(GraphicResourceManager)();
    s_inst->initialize();
}

void GraphicResourceManager::finalizeInstance()
{
    s_inst->finalize();
    istSafeDelete(s_inst);
}

inline AtomicShader* CreateAtomicShader(const char* filename)
{
    AtomicShader *sh = istNew(AtomicShader)();
    sh->createShaders(filename);
    return sh;
}


bool GraphicResourceManager::initialize()
{
    m_font = nullptr;
    m_title_font = nullptr;
    stl::fill_n(m_blend_states, _countof(m_blend_states), (BlendState*)nullptr);
    stl::fill_n(m_depth_states, _countof(m_depth_states), (DepthStencilState*)nullptr);
    stl::fill_n(m_sampler, _countof(m_sampler), (Sampler*)nullptr);
    stl::fill_n(m_tex1d, _countof(m_tex1d), (Texture1D*)nullptr);
    stl::fill_n(m_tex2d, _countof(m_tex2d), (Texture2D*)nullptr);
    stl::fill_n(m_va, _countof(m_va), (VertexArray*)nullptr);
    stl::fill_n(m_vbo, _countof(m_vbo), (Buffer*)nullptr);
    stl::fill_n(m_ibo, _countof(m_ibo), (Buffer*)nullptr);
    stl::fill_n(m_ubo, _countof(m_ubo), (Buffer*)nullptr);
    stl::fill_n(m_rt, _countof(m_rt), (RenderTarget*)nullptr);
    stl::fill_n(m_shader, _countof(m_shader), (AtomicShader*)nullptr);

    //// どうも 2 の n 乗サイズのフレームバッファの方が若干描画早いっぽい。 
    uvec2 rt_size = atmGetWindowSize();
    //uvec2 rt_size = CalcFrameBufferSize();

    // initialize opengl resources
    i3d::Device *dev = atmGetGLDevice();
    i3d::DeviceContext *dc = atmGetGLDeviceContext();
    {
        m_font = CreateSpriteFont("Resources/font.sff", "Resources/font.png", atmGetEasyDrawer());
        m_title_font = CreateSpriteFont("Resources/ascii.sff", "Resources/ascii.png", atmGetEasyDrawer());
    }
    for(uint32 i=0; i<_countof(m_va); ++i) {
        m_va[i] = dev->createVertexArray();
    }

    {
        BlendStateDesc desc;
        m_blend_states[BS_NO_BLEND]     = dev->createBlendState(desc);

        desc.enable_blend = true;
        desc.func_src_rgb = desc.func_src_a = I3D_BLEND_SRC_ALPHA;
        desc.func_dst_rgb = desc.func_dst_a = I3D_BLEND_INV_SRC_ALPHA;
        m_blend_states[BS_BLEND_ALPHA]  = dev->createBlendState(desc);

        desc.func_src_rgb = desc.func_src_a = I3D_BLEND_SRC_ALPHA;
        desc.func_dst_rgb = desc.func_dst_a = I3D_BLEND_ONE;
        m_blend_states[BS_BLEND_ADD]    = dev->createBlendState(desc);

        desc.func_src_rgb = desc.func_src_a = I3D_BLEND_ONE;
        desc.func_dst_rgb = desc.func_dst_a = I3D_BLEND_INV_SRC_COLOR;
        m_blend_states[BS_BLEND_SCREEN]    = dev->createBlendState(desc);
    }
    {
        DepthStencilStateDesc desc;
        m_depth_states[DS_NO_DEPTH_NO_STENCIL]  = dev->createDepthStencilState(desc);

        desc.depth_enable = true;
        desc.depth_write = true;
        desc.depth_func = I3D_DEPTH_LESS;
        m_depth_states[DS_DEPTH_ENABLED]        = dev->createDepthStencilState(desc);

        desc.stencil_enable = true;
        desc.stencil_op_onpass = I3D_STENCIL_REPLACE;
        desc.stencil_ref = STENCIL_FLUID;
        m_depth_states[DS_GBUFFER_FLUID]        = dev->createDepthStencilState(desc);

        desc.stencil_ref = STENCIL_RIGID;
        m_depth_states[DS_GBUFFER_RIGID]        = dev->createDepthStencilState(desc);

        desc.depth_enable = true;
        desc.depth_write = false;
        desc.stencil_ref = 0;
        desc.stencil_func = I3D_STENCIL_EQUAL;
        m_depth_states[DS_GBUFFER_BG]           = dev->createDepthStencilState(desc);

        desc.depth_enable = true;
        desc.depth_func = I3D_DEPTH_ALWAYS;
        m_depth_states[DS_GBUFFER_UPSAMPLING]   = dev->createDepthStencilState(desc);

        desc.depth_enable = false;
        desc.depth_write = false;
        desc.depth_func = I3D_DEPTH_LESS;
        desc.stencil_enable = false;
        desc.stencil_func = I3D_STENCIL_ALWAYS;
        m_depth_states[DS_LIGHTING_FRONT]       = dev->createDepthStencilState(desc);
        m_depth_states[DS_LIGHTING_BACK]        = dev->createDepthStencilState(desc);
    }

    const uvec2 unitsphere_div(32,16);
    {
        CreateFloorQuad(m_va[VA_FLOOR_QUAD], m_vbo[VBO_FLOOR_QUAD], vec4(-PSYM_GRID_SIZE*0.5f, -PSYM_GRID_SIZE*0.5f, -0.15f, 0.0f), vec4(PSYM_GRID_SIZE, PSYM_GRID_SIZE, 0.0f, 0.0f));
        CreateScreenQuad(m_va[VA_SCREEN_QUAD], m_vbo[VBO_SCREEN_QUAD]);
        CreateBloomLuminanceQuads(m_va[VA_BLOOM_LUMINANCE_QUADS], m_vbo[VBO_BLOOM_LUMINANCE_QUADS]);
        CreateBloomBlurQuads(m_va[VA_BLOOM_BLUR_QUADS], m_vbo[VBO_BLOOM_BLUR_QUADS]);
        CreateBloomCompositeQuad(m_va[VA_BLOOM_COMPOSITE_QUAD], m_vbo[VBO_BLOOM_COMPOSITE_QUAD]);

        CreateCube(m_va[VA_UNIT_CUBE], m_vbo[VBO_UNIT_CUBE], 0.5f);
        CreateCube(m_va[VA_FLUID_CUBE], m_vbo[VBO_FLUID_CUBE], 0.015f);
        CreateSphere(m_va[VA_UNIT_SPHERE], m_vbo[VBO_UNIT_SPHERE], m_ibo[IBO_UNITSPHERE], 1.00f, unitsphere_div.x,unitsphere_div.y);
        CreateSphere(m_va[VA_BLOOSTAIN_SPHERE], m_vbo[VBO_BLOODSTAIN_SPHERE], m_ibo[IBO_BLOODSTAIN_SPHERE], 0.075f, 8,8);

        CreateFieldGridLines(m_va[VA_FIELD_GRID], m_vbo[VBO_FIELD_GRID]);
        CreateDistanceFieldQuads(m_va[VA_DISTANCE_FIELD],
            m_vbo[VBO_DISTANCE_FIELD_QUAD], m_vbo[VBO_DISTANCE_FIELD_POS], m_vbo[VBO_DISTANCE_FIELD_DIST]);

        m_vbo[VBO_GB_FLUID]             = CreateVertexBuffer(dev, sizeof(psym::Particle)*PSYM_MAX_PARTICLE_NUM, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_GB_RIGID_SPHERICAL]   = CreateVertexBuffer(dev, sizeof(PSetParticle)*MAX_RIGID_PARTICLES, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_GB_RIGID_SOLID]       = CreateVertexBuffer(dev, sizeof(PSetParticle)*MAX_RIGID_PARTICLES, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_FW_RIGID_BARRIER]     = CreateVertexBuffer(dev, sizeof(PSetParticle)*MAX_RIGID_PARTICLES, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_PARTICLES]            = CreateVertexBuffer(dev, sizeof(SingleParticle)*MAX_EFFECT_PARTICLES, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_DIRLIGHT_INSTANCES]   = CreateVertexBuffer(dev, sizeof(DirectionalLight)*ATOMIC_MAX_DIRECTIONAL_LIGHTS, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_POINTLIGHT_INSTANCES] = CreateVertexBuffer(dev, sizeof(PointLight)*ATOMIC_MAX_POINT_LIGHTS, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_BLOODSTAIN_PARTICLES] = CreateVertexBuffer(dev, sizeof(BloodstainParticle)*MAX_BLOODSTAIN_PARTICLES, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_GENERIC_PARAMS1]      = CreateVertexBuffer(dev, 512*1024, I3D_USAGE_DYNAMIC);
        m_vbo[VBO_GENERIC_PARAMS2]      = CreateVertexBuffer(dev, 512*1024, I3D_USAGE_DYNAMIC);
    }
    {
        m_ubo[UBO_RENDERSTATES_3D]          = CreateUniformBuffer(dev, sizeof(RenderStates), I3D_USAGE_DYNAMIC);
        m_ubo[UBO_RENDERSTATES_BG]          = CreateUniformBuffer(dev, sizeof(RenderStates), I3D_USAGE_DYNAMIC);
        m_ubo[UBO_RENDERSTATES_2D]          = CreateUniformBuffer(dev, sizeof(RenderStates), I3D_USAGE_DYNAMIC);
        m_ubo[UBO_FXAA_PARAMS]              = CreateUniformBuffer(dev, sizeof(FXAAParams), I3D_USAGE_DYNAMIC);
        m_ubo[UBO_FADE_PARAMS]              = CreateUniformBuffer(dev, sizeof(FadeParams), I3D_USAGE_DYNAMIC);
        m_ubo[UBO_FILL_PARAMS]              = CreateUniformBuffer(dev, sizeof(FillParams), I3D_USAGE_DYNAMIC);
        m_ubo[UBO_MULTIRESOLUTION_PARAMS]   = CreateUniformBuffer(dev, sizeof(MultiresolutionParams), I3D_USAGE_DYNAMIC);
        m_ubo[UBO_DEBUG_SHOW_BUFFER_PARAMS] = CreateUniformBuffer(dev, sizeof(DebugShowBufferParams), I3D_USAGE_DYNAMIC);
    }
    {
        // create shaders
        m_shader[SH_GBUFFER_FLOOR]      = CreateAtomicShader("GBuffer_Floor");
        //m_shader[SH_GBUFFER_FLUID]      = CreateAtomicShader("GBuffer_Fluid");
        //m_shader[SH_GBUFFER_RIGID]      = CreateAtomicShader("GBuffer_Rigid");
        m_shader[SH_GBUFFER_FLUID_SPHERICAL]= CreateAtomicShader("GBuffer_FluidBlood");
        m_shader[SH_GBUFFER_FLUID_SOLID]    = CreateAtomicShader("GBuffer_FluidSolid");
        m_shader[SH_GBUFFER_RIGID_SPHERICAL]= CreateAtomicShader("GBuffer_RigidSpherical");
        m_shader[SH_GBUFFER_RIGID_SOLID]    = CreateAtomicShader("GBuffer_RigidSolid");
        m_shader[SH_GBUFFER_PARTICLES]  = CreateAtomicShader("GBuffer_ParticleSpherical");
        m_shader[SH_GBUFFER_UPSAMPLING] = CreateAtomicShader("GBuffer_Upsampling");
        m_shader[SH_BLOODSTAIN]         = CreateAtomicShader("Deferred_Bloodstain");
        m_shader[SH_UPSAMPLING]         = CreateAtomicShader("Deferred_Upsampling");
        m_shader[SH_POINTLIGHT]         = CreateAtomicShader("Deferred_PointLight");
        m_shader[SH_DIRECTIONALLIGHT]   = CreateAtomicShader("Deferred_DirectionalLight");
        m_shader[SH_MICROSCOPIC]        = CreateAtomicShader("Postprocess_Microscopic");
        m_shader[SH_FXAA_LUMA]          = CreateAtomicShader("FXAA_luma");
        m_shader[SH_FXAA]               = CreateAtomicShader("FXAA");
        m_shader[SH_BLOOM_LUMINANCE]    = CreateAtomicShader("Bloom_Luminance");
        m_shader[SH_BLOOM_HBLUR]        = CreateAtomicShader("Bloom_HBlur");
        m_shader[SH_BLOOM_VBLUR]        = CreateAtomicShader("Bloom_VBlur");
        m_shader[SH_BLOOM_COMPOSITE]    = CreateAtomicShader("Bloom_Composite");
        m_shader[SH_FADE]               = CreateAtomicShader("Fade");
        m_shader[SH_FILL]               = CreateAtomicShader("Fill");
        m_shader[SH_FILL3D]             = CreateAtomicShader("Fill3D");
        m_shader[SH_OUTPUT]             = CreateAtomicShader("Out");
        m_shader[SH_DEBUG_SHOW_RGB]     = CreateAtomicShader("Debug_ShowRGB");
        m_shader[SH_DEBUG_SHOW_AAA]     = CreateAtomicShader("Debug_ShowAAA");
        m_shader[SH_BARRIER]            = CreateAtomicShader("Forward_Barrier");
        m_shader[SH_BARRIER_PARTICLES]  = CreateAtomicShader("Forward_BarrierRigidParticles");
        m_shader[SH_FEEDBACK_BLUR]      = CreateAtomicShader("FeedbackBlur");

        m_shader[SH_BG1]    = CreateAtomicShader("BG1");
        m_shader[SH_BG2]    = CreateAtomicShader("BG2");
        m_shader[SH_BG3]    = CreateAtomicShader("BG3");
        m_shader[SH_BG4]    = CreateAtomicShader("BG4");
        m_shader[SH_BG5]    = CreateAtomicShader("BG5");
        m_shader[SH_BG6]    = CreateAtomicShader("BG6");
        m_shader[SH_BG7]    = CreateAtomicShader("BG7");
        m_shader[SH_BG8]    = CreateAtomicShader("BG8");
        m_shader[SH_BG9]    = CreateAtomicShader("BG9");
    }
    {
        // samplers
        m_sampler[SAMPLER_GBUFFER]          = dev->createSampler(SamplerDesc(I3D_REPEAT, I3D_REPEAT, I3D_REPEAT, I3D_NEAREST, I3D_NEAREST));
        m_sampler[SAMPLER_TEXTURE_DEFAULT]  = dev->createSampler(SamplerDesc(I3D_REPEAT, I3D_REPEAT, I3D_REPEAT, I3D_LINEAR, I3D_LINEAR));
    }
    {
        // create textures
        m_tex2d[TEX2D_RANDOM] = GenerateRandomTexture(dev, uvec2(64, 64), I3D_RGB8);
        m_tex2d[TEX2D_PSET_PARAMS_GB_SP] = dev->createTexture2D(Texture2DDesc(I3D_RGBA32F, uvec2(12, 4096)));
        m_tex2d[TEX2D_PSET_PARAMS_GB_SO] = dev->createTexture2D(Texture2DDesc(I3D_RGBA32F, uvec2(12, 4096)));
        m_tex2d[TEX2D_PSET_PARAMS_FW_SO] = dev->createTexture2D(Texture2DDesc(I3D_RGBA32F, uvec2(12, 4096)));
        m_tex2d[TEX2D_PSET_PARAMS_FW_BARRIER] = dev->createTexture2D(Texture2DDesc(I3D_RGBA32F, uvec2(12, 4096)));
    }
    {
        // create render targets
        m_rt[RT_GBUFFER]    = i3d::CreateRenderTarget(dev, 4, rt_size, I3D_RGBA16F, I3D_DEPTH24_STENCIL8, 3, 3);
        m_rt[RT_GAUSS0]     = i3d::CreateRenderTarget(dev, 1, uvec2(512, 256), I3D_RGBA16F);
        m_rt[RT_GAUSS1]     = i3d::CreateRenderTarget(dev, 1, uvec2(512, 256), I3D_RGBA16F);
        m_rt[RT_OUTPUT0]    = i3d::CreateRenderTarget(dev, 1, rt_size, I3D_RGBA16F);
        m_rt[RT_OUTPUT1]    = i3d::CreateRenderTarget(dev, 1, rt_size, I3D_RGBA16F);
        m_rt[RT_OUTPUT2]    = i3d::CreateRenderTarget(dev, 1, rt_size, I3D_RGBA16F);
        m_rt[RT_PREV_FRAME] = i3d::CreateRenderTarget(dev, 1, rt_size, I3D_RGBA16F);
        m_rt[RT_OUTPUT_HALF]    = i3d::CreateRenderTarget(dev, 1, rt_size/uvec2(2,2), I3D_RGBA16F);
        m_rt[RT_OUTPUT_QUARTER] = i3d::CreateRenderTarget(dev, 1, rt_size/uvec2(4,4), I3D_RGBA16F);

        m_rt[RT_GENERIC]    = i3d::CreateRenderTarget(dev, 0, rt_size, I3D_RGBA16F);
    }

    {
        CreateCubeParticleSet(m_pset[PSET_UNIT_CUBE],  m_rinfo[PSET_UNIT_CUBE],  0.5f, 10000.0f);
        CreateSphereParticleSet(m_pset[PSET_UNIT_SPHERE],  m_rinfo[PSET_UNIT_SPHERE],  0.5f, 10000.0f);
        CreateHollowCubeParticleSet(m_pset[PSET_HOLLOW_CUBE],  m_rinfo[PSET_HOLLOW_CUBE],  0.5f, 10000.0f );
        CreateCubeParticleSet(m_pset[PSET_CUBE_SMALL],  m_rinfo[PSET_CUBE_SMALL],  0.1f);
        CreateCubeParticleSet(m_pset[PSET_CUBE_MEDIUM], m_rinfo[PSET_CUBE_MEDIUM], 0.2f);
        CreateCubeParticleSet(m_pset[PSET_CUBE_LARGE],  m_rinfo[PSET_CUBE_LARGE],  0.4f);
        CreateSphereParticleSet(m_pset[PSET_SPHERE_SMALL],  m_rinfo[PSET_SPHERE_SMALL],  0.125f);
        CreateSphereParticleSet(m_pset[PSET_SPHERE_MEDIUM], m_rinfo[PSET_SPHERE_MEDIUM], 0.25f);
        CreateSphereParticleSet(m_pset[PSET_SPHERE_LARGE],  m_rinfo[PSET_SPHERE_LARGE],  0.5f);
        CreateBulletParticleSet(m_pset[PSET_SPHERE_BULLET], m_rinfo[PSET_SPHERE_BULLET]);
    }
    {
        m_models[MODEL_UNITQUAD]    = ModelInfo(I3D_QUADS, VA_SCREEN_QUAD, IBO_NULL, 4);
        m_models[MODEL_UNITCUBE]    = ModelInfo(I3D_QUADS, VA_UNIT_CUBE, IBO_NULL, 24);
        m_models[MODEL_UNITSPHERE]  = ModelInfo(I3D_QUADS, VA_UNIT_SPHERE, IBO_UNITSPHERE, (unitsphere_div.y-1)*(unitsphere_div.x)*4);
    }

    return true;
}

void GraphicResourceManager::finalize()
{
    for(uint32 i=0; i<_countof(m_shader); ++i)  { if(m_shader[i]) { atmSafeRelease( m_shader[i] ); } }
    for(uint32 i=0; i<_countof(m_rt); ++i)      { if(m_rt[i]) { atmSafeRelease( m_rt[i] ); } }
    for(uint32 i=0; i<_countof(m_ubo); ++i)     { if(m_ubo[i]) { atmSafeRelease( m_ubo[i] ); } }
    for(uint32 i=0; i<_countof(m_ibo); ++i)     { if(m_ibo[i]) { atmSafeRelease( m_ibo[i] ); } }
    for(uint32 i=0; i<_countof(m_vbo); ++i)     { if(m_vbo[i]) { atmSafeRelease( m_vbo[i] ); } }
    for(uint32 i=0; i<_countof(m_va); ++i)      { if(m_va[i]) { atmSafeRelease( m_va[i] ); } }
    for(uint32 i=0; i<_countof(m_tex2d); ++i)   { if(m_tex2d[i]) { atmSafeRelease( m_tex2d[i] ); } }
    for(uint32 i=0; i<_countof(m_tex1d); ++i)   { if(m_tex1d[i]) { atmSafeRelease( m_tex1d[i] ); } }
    for(uint32 i=0; i<_countof(m_sampler); ++i) { if(m_sampler[i]) { atmSafeRelease( m_sampler[i] ); } }
    for(uint32 i=0; i<_countof(m_depth_states); ++i) { if(m_depth_states[i]) { atmSafeRelease( m_depth_states[i] ); } }
    for(uint32 i=0; i<_countof(m_blend_states); ++i) { if(m_blend_states[i]) { atmSafeRelease( m_blend_states[i] ); } }
    atmSafeRelease(m_title_font);
    atmSafeRelease(m_font);
}

GraphicResourceManager::GraphicResourceManager()
    : m_flag_exit(false)
{
#ifdef atm_enable_ShaderLiveEdit
    m_glsl_modified = false;
    m_glsl_watcher.setName("GLSL Watcher");
    m_glsl_watcher.run( std::bind(&GraphicResourceManager::watchGLSLFiles, this) );
#endif // atm_enable_ShaderLiveEdit
}

GraphicResourceManager::~GraphicResourceManager()
{
    m_flag_exit = true;
#ifdef atm_enable_ShaderLiveEdit
    m_glsl_watcher.join();
#endif // atm_enable_ShaderLiveEdit
}

void GraphicResourceManager::update()
{
#ifdef atm_enable_ShaderLiveEdit
    if(m_glsl_modified) {
        m_glsl_modified = false;
        istPrint("recompiling shaders...");
        for(size_t i=0; i<_countof(m_shader); ++i) {
            if(m_shader[i] && m_shader[i]->needsRecompile()) {
                m_shader[i]->recompile();
            }
        }
        istPrint("done.\n");
    }
#endif // atm_enable_ShaderLiveEdit
}

#ifdef atm_enable_ShaderLiveEdit


void GraphicResourceManager::watchGLSLFiles()
{
    const char glsl_dir[] = "shader";
    char abs_path[MAX_PATH];
    ::GetFullPathNameA(glsl_dir, MAX_PATH, abs_path, nullptr);

    m_glsl_notifier = ::FindFirstChangeNotificationA(abs_path, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);
    while(!m_flag_exit) {
        if(::WaitForSingleObject(m_glsl_notifier, 1000)==WAIT_OBJECT_0) {
            ist::ExecVCTool("msbuild shader.vcxproj");
            m_glsl_modified = true;
            ::FindCloseChangeNotification(m_glsl_notifier);
            m_glsl_notifier = ::FindFirstChangeNotificationA(glsl_dir, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);
        }
    }
}
#endif // atm_enable_ShaderLiveEdit


} // namespace atm

#ifndef __atomic_Graphics_ResourceID__
#define __atomic_Graphics_ResourceID__
namespace atomic {

const size_t ATOMIC_MAX_CHARACTERS = 1024;

enum SH_RID {
    SH_GBUFFER_FLUID,
    SH_GBUFFER_RIGID,
    SH_DIRECTIONALLIGHT,
    SH_POINTLIGHT,
    SH_BLOOM_LUMINANCE,
    SH_BLOOM_HBLUR,
    SH_BLOOM_VBLUR,
    SH_BLOOM_COMPOSITE,
    SH_NORMAL_HBLUR,
    SH_NORMAL_VBLUR,
    SH_OUTPUT,
    SH_END,
};

enum RT_RID {
    RT_GBUFFER,
    RT_DEFERRED,
    RT_GAUSS0,
    RT_GAUSS1,
    RT_END,
};

enum TEX2D_RID {
    TEX2D_RANDOM,
    TEX2D_END,
};

enum VA_RID {
    VA_SCREEN_QUAD,
    VA_BLOOM_LUMINANCE_QUADS,
    VA_BLOOM_BLUR_QUADS,
    VA_BLOOM_COMPOSITE_QUAD,
    VA_UNIT_CUBE,
    VA_UNIT_SPHERE,
    VA_FRACTION_CUBE,
    VA_END,
};

enum VBO_RID {
    VBO_SCREEN_QUAD,
    VBO_BLOOM_LUMINANCE_QUADS,
    VBO_BLOOM_BLUR_QUADS,
    VBO_BLOOM_COMPOSITE_QUAD,
    VBO_UNIT_CUBE,
    VBO_UNIT_SPHERE,
    VBO_FRACTION_CUBE,

    VBO_FLUID_PARTICLES,
    VBO_RIGID_PARTICLES,
    VBO_POINTLIGHT_INSTANCES,
    VBO_DIRECTIONALLIGHT_INSTANCES,
    VBO_END,
};

enum IBO_RID {
    IBO_SPHERE,
    IBO_END,
};

enum UBO_RID {
    UBO_RENDER_STATES,
    UBO_BLOOM_STATES,
    UBO_END,
};

enum CB_RID {
    CB_CLASS_CUBE,
    CB_CLASS_SPHERE,
    CB_CHARACTER_INSTANCE,
    CB_END,
};

} // namespace atomic
#endif //__atomic_Graphics_ResourceID__

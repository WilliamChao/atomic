#include "Common.h"
const vec2 u_ScreenSize = vec2(512.0, 256.0);
const vec2 u_RcpScreenSize = vec2(1.0,1.0)/u_ScreenSize;
const vec2 u_HalfPixel = u_RcpScreenSize*0.5;

#ifdef GLSL_VS
ia_out(GLSL_POSITION)           vec2 ia_VertexPosition;
ia_out(GLSL_TEXCOORD0)          vec2 ia_VertexTexcoord0;
ia_out(GLSL_TEXCOORD1)          vec2 ia_VertexTexcoord1;
ia_out(GLSL_TEXCOORD2)          vec2 ia_VertexTexcoord2;
ia_out(GLSL_TEXCOORD3)          vec2 ia_VertexTexcoord3;
#endif
#if defined(GLSL_VS) || defined(GLSL_PS)
vs_out vec2 vs_Texcoord;
vs_out vec2 vs_TexcoordMin;
vs_out vec2 vs_TexcoordMax;
vs_out vec2 vs_Texcoords[4];
#endif

#if defined(GLSL_VS)

#if defined(GLSL_BLOOM_LUMINANCE)
void main()
{
    vs_Texcoord = ia_VertexTexcoord0*u_RS.ScreenTexcoord;
    vs_Texcoord.y *= u_RS.AspectRatio;
    gl_Position = vec4(ia_VertexPosition, 0.0, 1.0);
}

#elif defined(GLSL_BLOOM_HBLUR) || defined(GLSL_BLOOM_VBLUR)

void main()
{
    vs_TexcoordMin = ia_VertexTexcoord1+u_HalfPixel;
    vs_TexcoordMax = ia_VertexTexcoord2-u_HalfPixel;
    vs_TexcoordMax.y *= u_RS.RcpAspectRatio;
    vs_Texcoord = ia_VertexTexcoord0;
    gl_Position = vec4(ia_VertexPosition, 0.0, 1.0);
}

#elif defined(GLSL_BLOOM_COMPOSITE)

void main()
{
    vec2 adjast = vec2(1.0, u_RS.RcpAspectRatio);
    vs_Texcoords[0] = ia_VertexTexcoord0 * adjast;
    vs_Texcoords[1] = ia_VertexTexcoord1 * adjast;
    vs_Texcoords[2] = ia_VertexTexcoord2 * adjast;
    vs_Texcoords[3] = ia_VertexTexcoord3 * adjast;
    gl_Position = vec4(ia_VertexPosition, 0.0, 1.0);
}

#endif



#elif defined(GLSL_PS)

uniform vec2 u_TexcoordMin;
uniform vec2 u_TexcoordMax;
const float Weight[5] = float[5](0.05, 0.09, 0.12, 0.16, 0.16);

ps_out(0) vec4 ps_FragColor;


vec4 Luminance()
{
    vec4 color = texture(u_ColorBuffer, vs_Texcoord);
    vec4 glow = texture(u_GlowBuffer, vs_Texcoord);
    color = max(color-vec4(0.6), vec4(0.0))*2.5;
    color += glow;
    color.w += 0.5;
    return color;
}

vec4 HorizontalBlur()
{
    vec4 color = vec4(0.0);
    vec2 gap = vec2(u_RS.RcpScreenSize.x*3.7, 0.0);
    color += texture(u_ColorBuffer, clamp(vs_Texcoord - gap*4.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[0];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord - gap*3.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[1];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord - gap*2.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[2];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord - gap*1.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[3];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord          , vs_TexcoordMin, vs_TexcoordMax)) * Weight[4];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord + gap*1.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[3];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord + gap*2.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[2];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord + gap*3.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[1];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord + gap*4.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[0];
    return color;
}

vec4 VerticalBlur()
{
    vec4 color = vec4(0.0);
    vec2 gap = vec2(0.0, u_RS.RcpScreenSize.y);
    color += texture(u_ColorBuffer, clamp(vs_Texcoord - gap*4.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[0];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord - gap*3.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[1];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord - gap*2.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[2];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord - gap*1.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[3];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord          , vs_TexcoordMin, vs_TexcoordMax)) * Weight[4];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord + gap*1.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[3];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord + gap*2.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[2];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord + gap*3.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[1];
    color += texture(u_ColorBuffer, clamp(vs_Texcoord + gap*4.0, vs_TexcoordMin, vs_TexcoordMax)) * Weight[0];
    return color;
}

vec4 Composite()
{
    const float weight = 0.1;
    vec4 col =
           (texture(u_ColorBuffer, vs_Texcoords[0]) * weight)
         + (texture(u_ColorBuffer, vs_Texcoords[1]) * weight)
         + (texture(u_ColorBuffer, vs_Texcoords[2]) * weight)
         + (texture(u_ColorBuffer, vs_Texcoords[3]) * weight);
    return vec4(col.rgb, 1.0);
}


void main()
{
#if defined(GLSL_BLOOM_LUMINANCE)
    ps_FragColor = Luminance();
#elif defined(GLSL_BLOOM_HBLUR)
    ps_FragColor = HorizontalBlur();
#elif defined(GLSL_BLOOM_VBLUR)
    ps_FragColor = VerticalBlur();
#elif defined(GLSL_BLOOM_COMPOSITE)
    ps_FragColor = Composite();
#endif
}

#endif

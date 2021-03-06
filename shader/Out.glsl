#include "Common.h"

#ifdef GLSL_VS
ia_out(GLSL_POSITION)           vec2 ia_VertexPosition;
ia_out(GLSL_TEXCOORD0)          vec2 ia_VertexTexcoord0;
#endif
#if defined(GLSL_VS) || defined(GLSL_PS)
vs_out vec2 vs_Texcoord;
#endif

#if defined(GLSL_VS)


void main(void)
{
    vs_Texcoord = ia_VertexTexcoord0 * u_RS.ScreenTexcoord;
    gl_Position = vec4(ia_VertexPosition, 0.0, 1.0);
}

#elif defined(GLSL_PS)

ps_out(0) vec4 ps_FragColor;

void main()
{
    ps_FragColor = texture(u_ColorBuffer, vs_Texcoord);
}

#endif



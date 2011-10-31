// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#define _SCL_SECURE_NO_WARNINGS

#define GLM_FORCE_INLINE    // glm �̃C�����C������ SSE ��
#define GLM_FORCE_SSE2      //
#define GLM_GTX_simd_vec4   //
#define GLM_GTX_simd_mat4   //

#define GLEW_STATIC // glew �� static link ����

#include <stdio.h>
#include <GL/glew.h>
#include <GL/wglew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <windows.h>
#include <mmsystem.h>
#include <xnamath.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <EASTL/algorithm.h>
#include <EASTL/sort.h>
#include <EASTL/vector.h>
#include <EASTL/list.h>
#include <EASTL/set.h>
#include <EASTL/map.h>
#include <EASTL/string.h>
#include <iostream>

namespace stl = eastl;

#include <windows.h>
#include <windowsx.h>

#ifdef IST_DIRECTX
    #include <D3D11.h>
    #include <D3DX11.h>
    #include <D3DX10.h>

    #pragma comment(lib, "d3d11.lib")
    #pragma comment(lib, "d3dx11.lib")
    #pragma comment(lib, "d3dx10.lib")
#endif // IST_DIREXTX


// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B

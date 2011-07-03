#ifndef __atomic_Types__
#define __atomic_Types__

#include "ist/ist.h"
using namespace ist::graphics;

namespace atomic
{

typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

typedef float   float32;
typedef double  float64;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat2;
using glm::mat3;
using glm::mat4;


// __m128 �𒼐� eastl::vector �Ƃ��Ɋi�[����ƁA
// �A���C�����g�� 16 �ɑ����ĂȂ��A�h���X�� SSE �̃R�s�[���߂ŃA�N�Z�X���ăN���b�V�������肷��B
// �Ȃ̂� eastl::vector �Ɋi�[����Ƃ��͉��L�̍\���̂ő�p���A�L���X�g�łȂ�Ƃ�����B
struct __declspec(align(16)) quadword { char c[16]; };



using ist::Task;
using ist::TaskScheduler;


typedef unsigned int id_t;
enum {
    CHR_FRACTION    = 0x80000000,
    CHR_BULLET      = 0x40000000,
    CHR_GROUND      = 0x20000000,
    INVALID_ID = 0xffffffff,
};




typedef stl::allocator PermanentAllocator;
//typedef stack_allocator< stack_allocator<stl::allocator> > FrameScopedAllocator; // ���Ƃł������
typedef stl::allocator FrameScopedAllocator;



} // namespace atomic
#endif // __atomic_Types__

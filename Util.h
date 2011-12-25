#ifndef __atomic_Util__
#define __atomic_Util__

struct sphRigidSphere;
struct sphRigidBox;

namespace atomic {

    vec4 GenRotateAxis();
    void CreateRigidSphere(sphRigidSphere &o, EntityHandle h, const vec4& pos, float32 r);
    void CreateRigidBox(sphRigidBox &o, EntityHandle h, const mat4& t, const vec4 &size);

} // namespace atomic

namespace glm {
namespace detail {

// glm �̒��Ɏ��̂��Ȃ��悤�Ȃ̂Łc
__forceinline fvec4SIMD operator* (fmat4x4SIMD const & M, fvec4SIMD const & V)
{
    // Splat x,y,z and w
    fvec4SIMD vTempX = _mm_shuffle_ps(V.Data, V.Data, _MM_SHUFFLE(0,0,0,0));
    fvec4SIMD vTempY = _mm_shuffle_ps(V.Data, V.Data, _MM_SHUFFLE(1,1,1,1));
    fvec4SIMD vTempZ = _mm_shuffle_ps(V.Data, V.Data, _MM_SHUFFLE(2,2,2,2));
    fvec4SIMD vTempW = _mm_shuffle_ps(V.Data, V.Data, _MM_SHUFFLE(3,3,3,3));
    // Mul by the matrix
    vTempX = vTempX * M.Data[0];
    vTempY = vTempY * M.Data[1];
    vTempZ = vTempZ * M.Data[2];
    vTempW = vTempW * M.Data[3];
    // Add them all together
    vTempX = vTempX + vTempY;
    vTempZ = vTempZ + vTempW;
    vTempX = vTempX + vTempZ;
    return vTempX;
}

} // namespace detail
} // namespace glm

#endif // __atomic_Util__

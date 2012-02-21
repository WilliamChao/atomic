#ifndef __ist_Math_FrustumCulling_h__
#define __ist_Math_FrustumCulling_h__
namespace ist {

typedef __m128 simdvec4_t;

struct FrustumPlanes
{
private:
    simdvec4_t m_data[6];

public:

    // view projection matrix ���� frustum ���\������ 6 �� plane ���Z�o���܂�
    // MatrixType �� XMMATRIX �� glm::mat4x4 ��z��
    template<class MatrixType>
    void constructFromViewProjectionMatrix(const MatrixType &mat, bool normalize=true);

    simdvec4_t& operator[](int i) { return m_data[i]; }
    const simdvec4_t& operator[](int i) const { return m_data[i]; }
};

struct AxisAlignedBoundingBox
{
private:
    simdvec4_t m_data[2];

public:
    // �S�Ă� simdvec4_t �̈����� w �v�f�� 1.0f �ł���K�v������܂�

    AxisAlignedBoundingBox() { m_data[0]=m_data[1]=_mm_set1_ps(0.0f); }
    AxisAlignedBoundingBox(const simdvec4_t v) { m_data[0]=m_data[1]=v; }
    AxisAlignedBoundingBox(const simdvec4_t v_min, const simdvec4_t v_max) { m_data[0]=v_min; m_data[1]=v_max; }

    void addPoint(const simdvec4_t v);

    simdvec4_t& operator[](int i) { return m_data[i]; }
    const simdvec4_t& operator[](int i) const { return m_data[i]; }
};
typedef AxisAlignedBoundingBox AABB;

// true �Ȃ王�E���Afalse �Ȃ王�E�O
bool TestFrustumAABB(const FrustumPlanes &frustum, const AABB &aabb);



} // namespace ist

#include "FrustumCulling.inl"

#endif // __ist_Math_FrustumCulling_h__

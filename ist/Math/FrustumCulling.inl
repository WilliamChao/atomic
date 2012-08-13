﻿#ifndef __ist_Math_FrustumCulling_inl__
#define __ist_Math_FrustumCulling_inl__
namespace ist {

// _mm_set_ps() は wzyx の順に格納される (little endian) ため、xyzw の順に格納するものを用意
inline simdvec4_t simdvec4_set( float _1, float _2, float _3, float _4 )
{
    return _mm_set_ps(_4, _3, _2, _1);
}

inline simdvec4_t min( simdvec4_t v1, simdvec4_t v2 )
{
    return _mm_min_ps(v1, v2);
}

inline simdvec4_t max( simdvec4_t v1, simdvec4_t v2 )
{
    return _mm_max_ps(v1, v2);
}

inline simdvec4_t greater( simdvec4_t v1, simdvec4_t v2 )
{
    return _mm_cmpgt_ps(v1, v2);
}

inline simdvec4_t select( simdvec4_t v1, simdvec4_t v2, simdvec4_t c )
{
    simdvec4_t t1 = _mm_andnot_ps(c, v1);
    simdvec4_t t2 = _mm_and_ps(v2, c);
    return _mm_or_ps(t1, t2);
}

inline simdvec4_t dot( simdvec4_t v1, simdvec4_t v2 )
{
    simdvec4_t t2 = v2;
    simdvec4_t t  = _mm_mul_ps(v1,t2);
    t2  = _mm_shuffle_ps(t2,t, _MM_SHUFFLE(1,0,0,0));
    t2  = _mm_add_ps(t2,t);
    t   = _mm_shuffle_ps(t,t2, _MM_SHUFFLE(0,3,0,0));
    t   = _mm_add_ps(t,t2);
    return _mm_shuffle_ps(t,t, _MM_SHUFFLE(2,2,2,2));
}

inline simdvec4_t plane_normalize_est( simdvec4_t p )
{
    simdvec4_t d = _mm_mul_ps(p,p);
    simdvec4_t t = _mm_shuffle_ps(d,d,_MM_SHUFFLE(2,1,2,1));
    d = _mm_add_ss(d,t);
    t = _mm_shuffle_ps(t,t,_MM_SHUFFLE(1,1,1,1));
    d = _mm_add_ss(d,t);
    d = _mm_shuffle_ps(d,d,_MM_SHUFFLE(0,0,0,0));
    d = _mm_rsqrt_ps(d);
    d = _mm_mul_ps(d,p);
    return d;
}


template<class MatrixType>
inline void FrustumPlanes::constructFromViewProjectionMatrix(const MatrixType &mat, bool normalize)
{
    typedef float (mat4_t)[4][4];
    const mat4_t &m = reinterpret_cast<const mat4_t&>(mat);

    // Left clipping plane
    m_data[0] = simdvec4_set(
        m[0][3] + m[0][0],
        m[1][3] + m[1][0],
        m[2][3] + m[2][0],
        m[3][3] + m[3][0] );

    // Right clipping plane
    m_data[1] = simdvec4_set(
        m[0][3] - m[0][0],
        m[1][3] - m[1][0],
        m[2][3] - m[2][0],
        m[3][3] - m[3][0] );

    // Top clipping plane
    m_data[2] = simdvec4_set(
        m[0][3] - m[0][1],
        m[1][3] - m[1][1],
        m[2][3] - m[2][1],
        m[3][3] - m[3][1] );

    // Bottom clipping plane
    m_data[3] = simdvec4_set(
        m[0][3] + m[0][1],
        m[1][3] + m[1][1],
        m[2][3] + m[2][1],
        m[3][3] + m[3][1] );

    // Near clipping plane
    m_data[4] = simdvec4_set(
        m[0][2],
        m[1][2],
        m[2][2],
        m[3][2] );

    // Far clipping plane
    m_data[5] = simdvec4_set(
        m[0][3] - m[0][2],
        m[1][3] - m[1][2],
        m[2][3] - m[2][2],
        m[3][3] - m[3][2] );

    if(normalize) {
        m_data[0] = plane_normalize_est(m_data[0]);
        m_data[1] = plane_normalize_est(m_data[1]);
        m_data[2] = plane_normalize_est(m_data[2]);
        m_data[3] = plane_normalize_est(m_data[3]);
        m_data[4] = plane_normalize_est(m_data[4]);
        m_data[5] = plane_normalize_est(m_data[5]);
    }
}


inline void AABB::addPoint(const simdvec4_t v)
{
    m_data[0] = min(m_data[0], v);
    m_data[1] = max(m_data[1], v);
}




inline bool TestFrustumAABB(const FrustumPlanes &frustum, const AABB &aabb)
{
    const simdvec4_t zero = _mm_set1_ps(0.0f);
    const simdvec4_t aabb_min = aabb[0];
    const simdvec4_t aabb_max = aabb[1];

    for(int i=0; i<6; ++i) {
        const simdvec4_t plane  = frustum[i];
        const simdvec4_t gt_zero= greater(plane, zero);
        const simdvec4_t pos    = select(aabb_min, aabb_max, gt_zero);
        const simdvec4_t r      = dot(pos, plane);

        __declspec(align(16)) float rv[4];
        _mm_store_ss(rv, r);
        if(rv[0] < 0.0f) { return false; }
    }
    return true;
}

} // namespace ist
#endif // __ist_Math_FrustumCulling_inl__

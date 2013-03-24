﻿#ifndef ist_Math_Misc_h
#define ist_Math_Misc_h

namespace ist {

/// メンバ関数ポインタとかは普通のキャストが効かないので union で強引にキャスト
template<class To, class From>
inline To force_cast(From v)
{
    union {
        From from;
        To to;
    } u = {v};
    return u.to;
}

template<class T>
inline T clamp(T v, T minmum, T maximum)
{
    return stl::min<T>(stl::max<T>(v, minmum), maximum);
}

template<class T>
inline T div_ceil(T v, T d)
{
    return v/d + (v%d==0 ? 0 : 1);
}


template<class F, class CharT>
inline void separate_scan(CharT *str, CharT separator, const F &f)
{
    for(;;) {
        size_t l = 0;
        while(str[l]!=separator && str[l]!=0) { ++l; }
        f(str, l);
        if(str[l]==separator) { str+=l+1; }
        else { break; }
    }
}

} // namespace ist

#endif // ist_Math_Misc_h

/**
 * Copyright 2014-2017 Andreas Schäfer
 * Copyright 2015 Kurt Kanzenbach
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FLAT_ARRAY_DETAIL_SHORT_VEC_AVX_DOUBLE_16_HPP
#define FLAT_ARRAY_DETAIL_SHORT_VEC_AVX_DOUBLE_16_HPP

#if (LIBFLATARRAY_WIDEST_VECTOR_ISA == LIBFLATARRAY_AVX) || (LIBFLATARRAY_WIDEST_VECTOR_ISA == LIBFLATARRAY_AVX2)

#include <libflatarray/detail/sqrt_reference.hpp>
#include <libflatarray/detail/short_vec_helpers.hpp>
#include <libflatarray/config.h>
#include <libflatarray/short_vec_base.hpp>

// disable certain warnings from system headers when compiling with
// Microsoft Visual Studio:
#ifdef _MSC_BUILD
#pragma warning( push )
#pragma warning( disable : 4514 )
#endif

#include <immintrin.h>
#ifdef LIBFLATARRAY_WITH_CPP14
#include <initializer_list>
#endif

#ifdef _MSC_BUILD
#pragma warning( pop )
#endif

namespace LibFlatArray {

template<typename CARGO, std::size_t ARITY>
class short_vec;

#ifdef __ICC
// disabling this warning as implicit type conversion is exactly our goal here:
#pragma warning push
#pragma warning (disable: 2304)
#endif

template<>
class short_vec<double, 16> : public short_vec_base<double, 16>
{
public:
    static const std::size_t ARITY = 16;
    typedef short_vec<double, 16> mask_type;
    typedef short_vec_strategy::avx strategy;

    template<typename _CharT, typename _Traits>
    friend std::basic_ostream<_CharT, _Traits>& operator<<(
        std::basic_ostream<_CharT, _Traits>& __os,
        const short_vec<double, 16>& vec);

    inline
    short_vec(const double data = 0) :
        val{_mm256_broadcast_sd(&data),
            _mm256_broadcast_sd(&data),
            _mm256_broadcast_sd(&data),
            _mm256_broadcast_sd(&data)}
    {}

    inline
    short_vec(const double *data)
    {
        load(data);
    }

    inline
    short_vec(
        const __m256d& val1,
        const __m256d& val2,
        const __m256d& val3,
        const __m256d& val4) :
        val{val1,
            val2,
            val3,
            val4}
    {}

#ifdef LIBFLATARRAY_WITH_CPP14
    inline
    short_vec(const std::initializer_list<double>& il)
    {
        const double *ptr = static_cast<const double *>(&(*il.begin()));
        load(ptr);
    }
#endif

    inline
    bool any() const
    {
        __m256d buf0 = _mm256_or_pd(
            _mm256_or_pd(val[ 0], val[ 1]),
            _mm256_or_pd(val[ 2], val[ 3]));
        return (0 == _mm256_testz_si256(
                    _mm256_castpd_si256(buf0),
                    _mm256_castpd_si256(buf0)));
    }

    inline
    double operator[](int i) const
    {
        __m256d buf0 = val[i >> 2];
        i &= 3;

        __m128d buf1;
        if (i < 2) {
            buf1 = _mm256_extractf128_pd(buf0, 0);
        } else {
            buf1 = _mm256_extractf128_pd(buf0, 1);
        }

        i &= 1;

        if (i == 0) {
            return _mm_cvtsd_f64(buf1);
        }

        buf1 = _mm_shuffle_pd(buf1, buf1, 1);
        return _mm_cvtsd_f64(buf1);
    }

    inline
    void operator-=(const short_vec<double, 16>& other)
    {
        val[ 0] = _mm256_sub_pd(val[ 0], other.val[ 0]);
        val[ 1] = _mm256_sub_pd(val[ 1], other.val[ 1]);
        val[ 2] = _mm256_sub_pd(val[ 2], other.val[ 2]);
        val[ 3] = _mm256_sub_pd(val[ 3], other.val[ 3]);
    }

    inline
    short_vec<double, 16> operator-(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_sub_pd(val[ 0], other.val[ 0]),
            _mm256_sub_pd(val[ 1], other.val[ 1]),
            _mm256_sub_pd(val[ 2], other.val[ 2]),
            _mm256_sub_pd(val[ 3], other.val[ 3]));
    }

    inline
    void operator+=(const short_vec<double, 16>& other)
    {
        val[ 0] = _mm256_add_pd(val[ 0], other.val[ 0]);
        val[ 1] = _mm256_add_pd(val[ 1], other.val[ 1]);
        val[ 2] = _mm256_add_pd(val[ 2], other.val[ 2]);
        val[ 3] = _mm256_add_pd(val[ 3], other.val[ 3]);
    }

    inline
    short_vec<double, 16> operator+(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_add_pd(val[ 0], other.val[ 0]),
            _mm256_add_pd(val[ 1], other.val[ 1]),
            _mm256_add_pd(val[ 2], other.val[ 2]),
            _mm256_add_pd(val[ 3], other.val[ 3]));
    }

    inline
    void operator*=(const short_vec<double, 16>& other)
    {
        val[ 0] = _mm256_mul_pd(val[ 0], other.val[ 0]);
        val[ 1] = _mm256_mul_pd(val[ 1], other.val[ 1]);
        val[ 2] = _mm256_mul_pd(val[ 2], other.val[ 2]);
        val[ 3] = _mm256_mul_pd(val[ 3], other.val[ 3]);
    }

    inline
    short_vec<double, 16> operator*(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_mul_pd(val[ 0], other.val[ 0]),
            _mm256_mul_pd(val[ 1], other.val[ 1]),
            _mm256_mul_pd(val[ 2], other.val[ 2]),
            _mm256_mul_pd(val[ 3], other.val[ 3]));
    }

    inline
    void operator/=(const short_vec<double, 16>& other)
    {
        val[ 0] = _mm256_div_pd(val[ 0], other.val[ 0]);
        val[ 1] = _mm256_div_pd(val[ 1], other.val[ 1]);
        val[ 2] = _mm256_div_pd(val[ 2], other.val[ 2]);
        val[ 3] = _mm256_div_pd(val[ 3], other.val[ 3]);
    }

    inline
    short_vec<double, 16> operator/(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_div_pd(val[ 0], other.val[ 0]),
            _mm256_div_pd(val[ 1], other.val[ 1]),
            _mm256_div_pd(val[ 2], other.val[ 2]),
            _mm256_div_pd(val[ 3], other.val[ 3]));
    }

    inline
    short_vec<double, 16> operator<(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_cmp_pd(val[ 0], other.val[ 0], _CMP_LT_OS),
            _mm256_cmp_pd(val[ 1], other.val[ 1], _CMP_LT_OS),
            _mm256_cmp_pd(val[ 2], other.val[ 2], _CMP_LT_OS),
            _mm256_cmp_pd(val[ 3], other.val[ 3], _CMP_LT_OS));
    }

    inline
    short_vec<double, 16> operator<=(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_cmp_pd(val[ 0], other.val[ 0], _CMP_LE_OS),
            _mm256_cmp_pd(val[ 1], other.val[ 1], _CMP_LE_OS),
            _mm256_cmp_pd(val[ 2], other.val[ 2], _CMP_LE_OS),
            _mm256_cmp_pd(val[ 3], other.val[ 3], _CMP_LE_OS));
    }

    inline
    short_vec<double, 16> operator==(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_cmp_pd(val[ 0], other.val[ 0], _CMP_EQ_OQ),
            _mm256_cmp_pd(val[ 1], other.val[ 1], _CMP_EQ_OQ),
            _mm256_cmp_pd(val[ 2], other.val[ 2], _CMP_EQ_OQ),
            _mm256_cmp_pd(val[ 3], other.val[ 3], _CMP_EQ_OQ));
    }

    inline
    short_vec<double, 16> operator>(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_cmp_pd(val[ 0], other.val[ 0], _CMP_GT_OS),
            _mm256_cmp_pd(val[ 1], other.val[ 1], _CMP_GT_OS),
            _mm256_cmp_pd(val[ 2], other.val[ 2], _CMP_GT_OS),
            _mm256_cmp_pd(val[ 3], other.val[ 3], _CMP_GT_OS));
    }

    inline
    short_vec<double, 16> operator>=(const short_vec<double, 16>& other) const
    {
        return short_vec<double, 16>(
            _mm256_cmp_pd(val[ 0], other.val[ 0], _CMP_GE_OS),
            _mm256_cmp_pd(val[ 1], other.val[ 1], _CMP_GE_OS),
            _mm256_cmp_pd(val[ 2], other.val[ 2], _CMP_GE_OS),
            _mm256_cmp_pd(val[ 3], other.val[ 3], _CMP_GE_OS));
    }

    inline
    short_vec<double, 16> sqrt() const
    {
        return short_vec<double, 16>(
            _mm256_sqrt_pd(val[ 0]),
            _mm256_sqrt_pd(val[ 1]),
            _mm256_sqrt_pd(val[ 2]),
            _mm256_sqrt_pd(val[ 3]));
    }

    inline
    void load(const double *data)
    {
        val[ 0] = _mm256_loadu_pd(data +  0);
        val[ 1] = _mm256_loadu_pd(data +  4);
        val[ 2] = _mm256_loadu_pd(data +  8);
        val[ 3] = _mm256_loadu_pd(data + 12);
    }

    inline
    void load_aligned(const double *data)
    {
        SHORTVEC_ASSERT_ALIGNED(data, 32);
        val[ 0] = _mm256_load_pd(data +  0);
        val[ 1] = _mm256_load_pd(data +  4);
        val[ 2] = _mm256_load_pd(data +  8);
        val[ 3] = _mm256_load_pd(data + 12);
    }

    inline
    void store(double *data) const
    {
        _mm256_storeu_pd(data +  0, val[ 0]);
        _mm256_storeu_pd(data +  4, val[ 1]);
        _mm256_storeu_pd(data +  8, val[ 2]);
        _mm256_storeu_pd(data + 12, val[ 3]);
    }

    inline
    void store_aligned(double *data) const
    {
        SHORTVEC_ASSERT_ALIGNED(data, 32);
        _mm256_store_pd(data +  0, val[ 0]);
        _mm256_store_pd(data +  4, val[ 1]);
        _mm256_store_pd(data +  8, val[ 2]);
        _mm256_store_pd(data + 12, val[ 3]);
    }

    inline
    void store_nt(double *data) const
    {
        SHORTVEC_ASSERT_ALIGNED(data, 32);
        _mm256_stream_pd(data +  0, val[ 0]);
        _mm256_stream_pd(data +  4, val[ 1]);
        _mm256_stream_pd(data +  8, val[ 2]);
        _mm256_stream_pd(data + 12, val[ 3]);
    }

#ifdef __AVX2__
    inline
    void gather(const double *ptr, const int *offsets)
    {
        __m128i indices;
        indices = _mm_loadu_si128(reinterpret_cast<const __m128i *>(offsets));
        val[ 0]   = _mm256_i32gather_pd(ptr, indices, 8);
        indices = _mm_loadu_si128(reinterpret_cast<const __m128i *>(offsets + 4));
        val[ 1]   = _mm256_i32gather_pd(ptr, indices, 8);
        indices = _mm_loadu_si128(reinterpret_cast<const __m128i *>(offsets + 8));
        val[ 2]   = _mm256_i32gather_pd(ptr, indices, 8);
        indices = _mm_loadu_si128(reinterpret_cast<const __m128i *>(offsets + 12));
        val[ 3]   = _mm256_i32gather_pd(ptr, indices, 8);
    }
#else
    inline
    void gather(const double *ptr, const int *offsets)
    {
        val[ 0] = _mm256_set_pd(
            *(ptr + offsets[ 3]),
            *(ptr + offsets[ 2]),
            *(ptr + offsets[ 1]),
            *(ptr + offsets[ 0]));

        val[ 1] = _mm256_set_pd(
            *(ptr + offsets[ 7]),
            *(ptr + offsets[ 6]),
            *(ptr + offsets[ 5]),
            *(ptr + offsets[ 4]));

        val[ 2] = _mm256_set_pd(
            *(ptr + offsets[11]),
            *(ptr + offsets[10]),
            *(ptr + offsets[ 9]),
            *(ptr + offsets[ 8]));

        val[ 3] = _mm256_set_pd(
            *(ptr + offsets[15]),
            *(ptr + offsets[14]),
            *(ptr + offsets[13]),
            *(ptr + offsets[12]));
    }
#endif

    inline
    void scatter(double *ptr, const int *offsets) const
    {
        __m128d tmp;
        tmp = _mm256_extractf128_pd(val[ 0], 0);
        _mm_storel_pd(ptr + offsets[0], tmp);
        _mm_storeh_pd(ptr + offsets[1], tmp);
        tmp = _mm256_extractf128_pd(val[ 0], 1);
        _mm_storel_pd(ptr + offsets[2], tmp);
        _mm_storeh_pd(ptr + offsets[3], tmp);
        tmp = _mm256_extractf128_pd(val[ 1], 0);
        _mm_storel_pd(ptr + offsets[4], tmp);
        _mm_storeh_pd(ptr + offsets[5], tmp);
        tmp = _mm256_extractf128_pd(val[ 1], 1);
        _mm_storel_pd(ptr + offsets[6], tmp);
        _mm_storeh_pd(ptr + offsets[7], tmp);
        tmp = _mm256_extractf128_pd(val[ 2], 0);
        _mm_storel_pd(ptr + offsets[8], tmp);
        _mm_storeh_pd(ptr + offsets[9], tmp);
        tmp = _mm256_extractf128_pd(val[ 2], 1);
        _mm_storel_pd(ptr + offsets[10], tmp);
        _mm_storeh_pd(ptr + offsets[11], tmp);
        tmp = _mm256_extractf128_pd(val[ 3], 0);
        _mm_storel_pd(ptr + offsets[12], tmp);
        _mm_storeh_pd(ptr + offsets[13], tmp);
        tmp = _mm256_extractf128_pd(val[ 3], 1);
        _mm_storel_pd(ptr + offsets[14], tmp);
        _mm_storeh_pd(ptr + offsets[15], tmp);
    }

    inline
    void blend(const mask_type& mask, const short_vec<double, 16>& other)
    {
        val[ 0] = _mm256_blendv_pd(val[ 0], other.val[ 0], mask.val[ 0]);
        val[ 1] = _mm256_blendv_pd(val[ 1], other.val[ 1], mask.val[ 1]);
        val[ 2] = _mm256_blendv_pd(val[ 2], other.val[ 2], mask.val[ 2]);
        val[ 3] = _mm256_blendv_pd(val[ 3], other.val[ 3], mask.val[ 3]);
    }

private:
    __m256d val[4];
};

inline
void operator<<(double *data, const short_vec<double, 16>& vec)
{
    vec.store(data);
}

#ifdef __ICC
#pragma warning pop
#endif

inline
short_vec<double, 16> sqrt(const short_vec<double, 16>& vec)
{
    return vec.sqrt();
}

template<typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os,
           const short_vec<double, 16>& vec)
{
    const double *data1 = reinterpret_cast<const double *>(&vec.val[ 0]);
    const double *data2 = reinterpret_cast<const double *>(&vec.val[ 1]);
    const double *data3 = reinterpret_cast<const double *>(&vec.val[ 2]);
    const double *data4 = reinterpret_cast<const double *>(&vec.val[ 3]);

    __os << "["  << data1[0] << ", " << data1[1] << ", " << data1[2] << ", " << data1[3]
         << ", " << data2[0] << ", " << data2[1] << ", " << data2[2] << ", " << data2[3]
         << ", " << data3[0] << ", " << data3[1] << ", " << data3[2] << ", " << data3[3]
         << ", " << data4[0] << ", " << data4[1] << ", " << data4[2] << ", " << data4[3]
         << "]";
    return __os;
}

}

#endif

#endif

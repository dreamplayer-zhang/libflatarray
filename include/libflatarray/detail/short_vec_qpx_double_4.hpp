/**
 * Copyright 2014-2017 Andreas Schäfer
 * Copyright 2015 Kurt Kanzenbach
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FLAT_ARRAY_DETAIL_SHORT_VEC_QPX_DOUBLE_4_HPP
#define FLAT_ARRAY_DETAIL_SHORT_VEC_QPX_DOUBLE_4_HPP

#if LIBFLATARRAY_WIDEST_VECTOR_ISA == LIBFLATARRAY_QPX

#include <libflatarray/detail/sqrt_reference.hpp>
#include <libflatarray/detail/short_vec_helpers.hpp>

// disable certain warnings from system headers when compiling with
// Microsoft Visual Studio:
#ifdef _MSC_BUILD
#pragma warning( push )
#pragma warning( disable : 4514 )
#endif

#ifdef LIBFLATARRAY_WITH_CPP14
#include <initializer_list>
#endif

#ifdef _MSC_BUILD
#pragma warning( pop )
#endif

namespace LibFlatArray {

template<typename CARGO, std::size_t ARITY>
class short_vec;

template<typename CARGO, std::size_t ARITY>
class sqrt_reference;

#ifdef __ICC
// disabling this warning as implicit type conversion is exactly our goal here:
#pragma warning push
#pragma warning (disable: 2304)
#endif

template<>
class short_vec<double, 4>
{
public:
    static const std::size_t ARITY = 4;

    inline
    short_vec(const double data = 0) :
        val(vec_splats(data))
    {}

    inline
    short_vec(const double *data) :
        val(vec_ld(0, const_cast<double*>(data)))
    {}

    inline
    short_vec(const vector4double& val) :
        val(val)
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
    short_vec(const sqrt_reference<double, 4>& other);

    inline
    void operator-=(const short_vec<double, 4>& other)
    {
        val = vec_sub(val, other.val);
    }

    inline
    short_vec<double, 4> operator-(const short_vec<double, 4>& other) const
    {
        return short_vec<double, 4>(
            vec_sub(val, other.val));
    }

    inline
    void operator+=(const short_vec<double, 4>& other)
    {
        val = vec_add(val, other.val);
    }

    inline
    short_vec<double, 4> operator+(const short_vec<double, 4>& other) const
    {
        return short_vec<double, 4>(
            vec_add(val, other.val));
    }

    inline
    void operator*=(const short_vec<double, 4>& other)
    {
        val = vec_add(val, other.val);
    }

    inline
    short_vec<double, 4> operator*(const short_vec<double, 4>& other) const
    {
        return short_vec<double, 4>(
            vec_mul(val, other.val));
    }

    inline
    void operator/=(const sqrt_reference<double, 4>& other);

    inline
    void operator/=(const short_vec<double, 4>& other)
    {
        val = vec_swdiv_nochk(val, other.val);
    }

    inline
    short_vec<double, 4> operator/(const short_vec<double, 4>& other) const
    {
        return short_vec<double, 4>(
            vec_swdiv_nochk(val, other.val));
    }

    inline
    short_vec<double, 4> operator/(const sqrt_reference<double, 4>& other) const;

    inline
    short_vec<double, 4> sqrt() const
    {
        return short_vec<double, 4>(
            vec_swsqrt(val));
    }

    inline
    void load(const double *data)
    {
        val = vec_ld(0, const_cast<double*>(data));
    }

    inline
    void load_aligned(const double *data)
    {
        SHORTVEC_ASSERT_ALIGNED(data, 32);
        val = vec_lda(0, const_cast<double*>(data));
    }

    inline
    void store(double *data) const
    {
        vec_st(val, 0, data);
    }

    inline
    void store_aligned(double *data) const
    {
        SHORTVEC_ASSERT_ALIGNED(data, 32);
        vec_sta(val, 0, data);
    }

    inline
    void store_nt(double *data) const
    {
        store(data);
    }

    inline
    void gather(const double *ptr, const int *offsets)
    {
        double *base = const_cast<double *>(ptr);
        val = vec_insert(base[offsets[0]], val, 0);
        val = vec_insert(base[offsets[1]], val, 1);
        val = vec_insert(base[offsets[2]], val, 2);
        val = vec_insert(base[offsets[3]], val, 3);
    }

    inline
    void scatter(double *ptr, const int *offsets) const
    {
        ptr[offsets[0]] = vec_extract(val, 0);
        ptr[offsets[1]] = vec_extract(val, 1);
        ptr[offsets[2]] = vec_extract(val, 2);
        ptr[offsets[3]] = vec_extract(val, 3);
    }

private:
    vector4double val;
};

#ifdef __ICC
#pragma warning pop
#endif

inline
void operator<<(double *data, const short_vec<double, 4>& vec)
{
    vec.store(data);
}

template<>
class sqrt_reference<double, 4>
{
public:
    template<typename OTHER_CARGO, std::size_t OTHER_ARITY>
    friend class short_vec;

    sqrt_reference(const short_vec<double, 4>& vec) :
        vec(vec)
    {}

private:
    short_vec<double, 4> vec;
};

inline
short_vec<double, 4>::short_vec(const sqrt_reference<double, 4>& other) :
    val(vec_swsqrt(other.vec.val))
{}

inline
void short_vec<double, 4>::operator/=(const sqrt_reference<double, 4>& other)
{
    val = vec_mul(val, vec_rsqrte(other.vec.val));
}

inline
short_vec<double, 4> short_vec<double, 4>::operator/(const sqrt_reference<double, 4>& other) const
{
    return vec_mul(val, vec_rsqrte(other.vec.val));
}

inline
sqrt_reference<double, 4> sqrt(const short_vec<double, 4>& vec)
{
    return sqrt_reference<double, 4>(vec);
}

template<typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os,
           const short_vec<double, 4>& vec)
{
    const double *data1 = reinterpret_cast<const double *>(&vec.val);
    __os << "["
         << data1[0]  << ", "
         << data1[1]  << ", "
         << data1[2]  << ", "
         << data1[3]  << "]";
    return __os;
}

}

#endif

#endif

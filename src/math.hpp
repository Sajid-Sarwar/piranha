/* Copyright 2009-2016 Francesco Biscani (bluescarni@gmail.com)

This file is part of the Piranha library.

The Piranha library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The Piranha library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the Piranha library.  If not,
see https://www.gnu.org/licenses/. */

#ifndef PIRANHA_MATH_HPP
#define PIRANHA_MATH_HPP

#include <algorithm>
#include <boost/numeric/conversion/cast.hpp>
#include <cmath>
#include <complex>
#include <cstdarg>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "detail/sfinae_types.hpp"
#include "exceptions.hpp"
#include "is_key.hpp"
#include "symbol_set.hpp"
#include "type_traits.hpp"

namespace piranha
{

/// Math namespace.
/**
 * Namespace for general-purpose mathematical functions.
 */
namespace math
{

/// Default functor for the implementation of piranha::math::is_zero().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. The default implementation defines a call
 * operator which is enabled only if the argument type is constructible from the C++ \p int type and \p T is equality
 * comparable.
 */
template <typename T, typename = void>
struct is_zero_impl {
private:
    // NOTE: the equality comparable requirement already implies that the return type of
    // the comparison must be convertible to bool.
    template <typename U>
    using enabler =
        typename std::enable_if<std::is_constructible<U, int>::value && is_equality_comparable<U>::value, int>::type;

public:
    /// Call operator.
    /**
     * \note
     * This operator is enabled only if \p U is constructible from \p int and
     * equality-comparable.
     *
     * The operator will compare \p x to an instance of \p U constructed from the literal 0.
     *
     * @param x argument to be tested.
     *
     * @return \p true if \p x is zero, \p false otherwise.
     *
     * @throws unspecified any exception thrown by the construction or comparison of instances of type \p U, or
     * by the conversion of the result of the comparison to \p bool.
     */
    template <typename U, enabler<U> = 0>
    bool operator()(const U &x) const
    {
        return x == U(0);
    }
};
} // namespace math

namespace detail
{

// Enabler for math::is_zero().
template <typename T>
using math_is_zero_enabler = typename std::enable_if<
    std::is_convertible<decltype(math::is_zero_impl<T>{}(std::declval<const T &>())), bool>::value, int>::type;
} // namespace detail

namespace math
{

/// Zero test.
/**
 * \note
 * This function is enabled only if <tt>is_zero_impl<T>{}(x)</tt> is a well-formed expression returning
 * a type implicitly convertible to \p bool.
 *
 * Test if value is zero. The actual implementation of this function is in the piranha::math::is_zero_impl functor's
 * call operator. The body of this function is equivalent to:
 * @code
 * return is_zero_impl<T>{}(x);
 * @endcode
 *
 * @param x value to be tested.
 *
 * @return \p true if value is zero, \p false otherwise.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::is_zero_impl functor or by
 * the conversion of the result to \p bool.
 */
template <typename T, detail::math_is_zero_enabler<T> = 0>
inline bool is_zero(const T &x)
{
    return is_zero_impl<T>{}(x);
}
} // namespace math

namespace detail
{

// Enabler for the std complex specialisation of is_zero.
template <typename T>
using math_is_zero_std_complex_enabler =
    typename std::enable_if<std::is_same<T, std::complex<float>>::value || std::is_same<T, std::complex<double>>::value
                            || std::is_same<T, std::complex<long double>>::value>::type;
} // namespace detail

namespace math
{

/// Specialisation of the piranha::math::is_zero() functor for C++ complex floating-point types.
/**
 * This specialisation is enabled if \p T is an <tt>std::complex</tt> of a C++ floating-point type.
 */
template <typename T>
struct is_zero_impl<T, detail::math_is_zero_std_complex_enabler<T>> {
    /// Call operator.
    /**
     * The operator will test separately the real and imaginary parts of the complex argument.
     *
     * @param c argument to be tested.
     *
     * @return \p true if \p c is zero, \p false otherwise.
     */
    bool operator()(const T &c) const
    {
        return is_zero(c.real()) && is_zero(c.imag());
    }
};

/// Default functor for the implementation of piranha::math::is_unitary().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. The default implementation defines a call
 * operator which is enabled only if the argument type is constructible from the C++ \p int type and \p T is equality
 * comparable.
 */
template <typename T, typename = void>
struct is_unitary_impl {
private:
    template <typename U>
    using enabler =
        typename std::enable_if<std::is_constructible<U, int>::value && is_equality_comparable<U>::value, int>::type;

public:
    /// Call operator.
    /**
     * \note
     * This template method is enabled only if \p U can be constructed from \p int and \p U is
     * equality comparable.
     *
     * The operator will compare \p x to an instance of \p U constructed from the literal 1.
     *
     * @param x argument to be tested.
     *
     * @return \p true if \p x is equal to 1, \p false otherwise.
     *
     * @throws unspecified any exception thrown by the construction or comparison of instances of type \p U or by
     * the conversion of the result to \p bool.
     */
    template <typename U, enabler<U> = 0>
    bool operator()(const U &x) const
    {
        return x == U(1);
    }
};
} // namespace math

namespace detail
{

// Enabler for piranha::math::is_unitary().
template <typename T>
using math_is_unitary_enabler = typename std::enable_if<
    std::is_convertible<decltype(math::is_unitary_impl<T>{}(std::declval<const T &>())), bool>::value, int>::type;
} // namespace detail

namespace math
{

/// Unitary test.
/**
 * \note
 * This function is enabled only if <tt>is_unitary_impl<T>{}(x)</tt> is a valid expression, returning a type
 * which is implicitly convertible to \p bool.
 *
 * Test if value is equal to 1. The actual implementation of this function is in the piranha::math::is_unitary_impl
 * functor's
 * call operator. The body of this function is equivalent to:
 * @code
 * return is_unitary_impl<T>{}(x);
 * @endcode
 *
 * @param x value to be tested.
 *
 * @return \p true if value is equal to 1, \p false otherwise.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::is_unitary_impl functor, or by
 * the conversion of the result to \p bool.
 */
template <typename T, detail::math_is_unitary_enabler<T> = 0>
inline bool is_unitary(const T &x)
{
    return is_unitary_impl<T>{}(x);
}

/// Default functor for the implementation of piranha::math::negate().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will
 * assign to the input value its negation.
 */
template <typename T, typename = void>
struct negate_impl {
private:
    template <typename U>
    using negate_t = decltype(std::declval<U &>() = -std::declval<U &>());
    template <typename U>
    static void negate_imple(U &x, const std::true_type &)
    {
        x = static_cast<U>(-x);
    }
    template <typename U>
    static void negate_imple(U &x, const std::false_type &)
    {
        x = -x;
    }

public:
    /// Generic call operator.
    /**
     * \note
     * This operator is enabled only if the expression <tt>x = -x</tt> is well-formed.
     *
     * The body of the operator is equivalent to:
     * @code
     * x = -x;
     * @endcode
     *
     * @param x value to be negated.
     *
     * @throws unspecified any exception resulting from the in-place negation or assignment of \p x.
     */
    template <typename U, enable_if_t<is_detected<negate_t, U>::value, int> = 0>
    void operator()(U &x) const
    {
        negate_imple(x, std::is_integral<U>{});
    }
};
} // namespace math

namespace detail
{

// Enabler for math::negate().
template <typename T>
using math_negate_enabler = typename std::enable_if<
    conjunction<negation<std::is_const<T>>, true_tt<decltype(math::negate_impl<T>{}(std::declval<T &>()))>>::value,
    int>::type;
} // namespace detail

namespace math
{

/// In-place negation.
/**
 * \note
 * This function is enabled only if \p T is not const and if the expression <tt>negate_impl<T>{}(x)</tt> is valid.
 *
 * Negate value in-place. The actual implementation of this function is in the piranha::math::negate_impl functor's
 * call operator. The body of this function is equivalent to:
 * @code
 * negate_impl<T>{}(x);
 * @endcode
 * The result of the call operator of piranha::math::negate_impl is ignored.
 *
 * @param x value to be negated.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::negate_impl.
 */
template <typename T, detail::math_negate_enabler<T> = 0>
inline void negate(T &x)
{
    negate_impl<T>{}(x);
}

/// Default functor for the implementation of piranha::math::multiply_accumulate().
/**
 * This functor can be specialised via the \p std::enable_if mechanism.
 */
template <typename T, typename = void>
struct multiply_accumulate_impl {
private:
    // NOTE: as usual, we check the expression against const ref arguments.
    template <typename U>
    using addmul_t = decltype(std::declval<U &>() += std::declval<const U &>() * std::declval<const U &>());
    template <typename U>
    using enabler = enable_if_t<is_detected<addmul_t, U>::value, int>;

public:
    /// Call operator.
    /**
     * \note
     * This call operator is enabled only if the expression <tt>x += y * z</tt> is well-formed.
     *
     * The body of the operator is equivalent to:
     * @code
     * x += y * z;
     * @endcode
     *
     * @param x target value for accumulation.
     * @param y first argument.
     * @param z second argument.
     *
     * @throws unspecified any exception resulting from in-place addition or
     * binary multiplication on the operands.
     */
    template <typename U, enabler<U> = 0>
    void operator()(U &x, const U &y, const U &z) const
    {
        x += y * z;
    }
};

#if defined(FP_FAST_FMA) && defined(FP_FAST_FMAF) && defined(FP_FAST_FMAL)

inline namespace impl
{

// Enabler for the fast floating-point implementation of multiply_accumulate().
template <typename T>
using math_multiply_accumulate_float_enabler = enable_if_t<std::is_floating_point<T>::value>;
} // namespace impl

/// Specialisation of the implementation of piranha::math::multiply_accumulate() for floating-point types.
/**
 * This functor is enabled only if the macros \p FP_FAST_FMA, \p FP_FAST_FMAF and \p FP_FAST_FMAL are defined.
 */
template <typename T>
struct multiply_accumulate_impl<T, math_multiply_accumulate_float_enabler<T>> {
    /// Call operator.
    /**
     * This implementation will use the \p std::fma() function.
     *
     * @param x target value for accumulation.
     * @param y first argument.
     * @param z second argument.
     */
    void operator()(T &x, const T &y, const T &z) const
    {
        x = std::fma(y, z, x);
    }
};

#endif
} // namespace math

inline namespace impl
{

// Enabler for multiply_accumulate.
template <typename T>
using math_multiply_accumulate_t = decltype(
    math::multiply_accumulate_impl<T>{}(std::declval<T &>(), std::declval<const T &>(), std::declval<const T &>()));

template <typename T>
using math_multiply_accumulate_enabler = enable_if_t<is_detected<math_multiply_accumulate_t, T>::value, int>;
} // namespace impl

namespace math
{

/// Multiply-accumulate.
/**
 * \note
 * This function is enabled only if the expression <tt>multiply_accumulate_impl<T>{}(x, y, z)</tt> is valid.
 *
 * This function will set \p x to <tt>x + y * z</tt>. The actual implementation of this function is in the
 * piranha::math::multiply_accumulate_impl functor's call operator. The body of this function is equivalent to:
 * @code
 * multiply_accumulate_impl<T>{}(x, y, z);
 * @endcode
 *
 * @param x target value for accumulation.
 * @param y first argument.
 * @param z second argument.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::multiply_accumulate_impl.
 */
template <typename T, math_multiply_accumulate_enabler<T> = 0>
inline void multiply_accumulate(T &x, const T &y, const T &z)
{
    multiply_accumulate_impl<T>{}(x, y, z);
}

/// Default functor for the implementation of piranha::math::cos().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename Enable = void>
struct cos_impl {
};

/// Specialisation of the piranha::math::cos() functor for floating-point types.
/**
 * This specialisation is activated when \p T is a C++ floating-point type.
 * The result will be computed via the standard <tt>std::cos()</tt> function.
 */
template <typename T>
struct cos_impl<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    /// Call operator.
    /**
     * The cosine will be computed via <tt>std::cos()</tt>.
     *
     * @param x argument.
     *
     * @return cosine of \p x.
     */
    T operator()(const T &x) const
    {
        return std::cos(x);
    }
};

/// Specialisation of the piranha::math::cos() functor for integral types.
/**
 * This specialisation is activated when \p T is an integral type.
 */
template <typename T>
struct cos_impl<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    /// Call operator.
    /**
     * @param x argument.
     *
     * @return cosine of \p x.
     *
     * @throws std::invalid_argument if the argument is not zero.
     */
    T operator()(const T &x) const
    {
        if (x == T(0)) {
            return T(1);
        }
        piranha_throw(std::invalid_argument, "cannot compute the cosine of a non-zero integral");
    }
};
} // namespace math

namespace detail
{

// Type for the result of math::cos().
template <typename T>
using math_cos_type_ = decltype(math::cos_impl<T>{}(std::declval<const T &>()));

template <typename T>
using math_cos_type = typename std::enable_if<is_returnable<math_cos_type_<T>>::value, math_cos_type_<T>>::type;
} // namespace detail

namespace math
{

/// Cosine.
/**
 * \note
 * This function is enabled only if the expression <tt>cos_impl<T>{}(x)</tt> is valid, returning
 * a type which satisfies piranha::is_returnable.
 *
 * Returns the cosine of \p x. The actual implementation of this function is in the piranha::math::cos_impl functor's
 * call operator. The body of this function is equivalent to:
 * @code
 * return cos_impl<T>{}(x);
 * @endcode
 *
 * @param x cosine argument.
 *
 * @return cosine of \p x.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::cos_impl functor.
 */
template <typename T>
inline detail::math_cos_type<T> cos(const T &x)
{
    return cos_impl<T>{}(x);
}

/// Default functor for the implementation of piranha::math::sin().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename Enable = void>
struct sin_impl {
};

/// Specialisation of the piranha::math::sin() functor for floating-point types.
/**
 * This specialisation is activated when \p T is a C++ floating-point type.
 * The result will be computed via the standard <tt>std::sin()</tt> function.
 */
template <typename T>
struct sin_impl<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    /// Call operator.
    /**
     * The sine will be computed via <tt>std::sin()</tt>.
     *
     * @param x argument.
     *
     * @return sine of \p x.
     */
    T operator()(const T &x) const
    {
        return std::sin(x);
    }
};

/// Specialisation of the piranha::math::sin() functor for integral types.
/**
 * This specialisation is activated when \p T is an integral type.
 */
template <typename T>
struct sin_impl<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    /// Call operator.
    /**
     * @param x argument.
     *
     * @return sine of \p x.
     *
     * @throws std::invalid_argument if the argument is not zero.
     */
    T operator()(const T &x) const
    {
        if (x == T(0)) {
            return T(0);
        }
        piranha_throw(std::invalid_argument, "cannot compute the sine of a non-zero integral");
    }
};
} // namespace math

namespace detail
{

// Type for the result of math::sin().
template <typename T>
using math_sin_type_ = decltype(math::sin_impl<T>{}(std::declval<const T &>()));

template <typename T>
using math_sin_type = typename std::enable_if<is_returnable<math_sin_type_<T>>::value, math_sin_type_<T>>::type;
} // namespace detail

namespace math
{

/// Sine.
/**
 * \note
 * This function is enabled only if the expression <tt>sin_impl<T>{}(x)</tt> is valid, returning
 * a type which satisfies piranha::is_returnable.
 *
 * Returns the sine of \p x. The actual implementation of this function is in the piranha::math::sin_impl functor's
 * call operator. The body of this function is equivalent to:
 * @code
 * return sin_impl<T>{}(x);
 * @endcode
 *
 * @param x sine argument.
 *
 * @return sine of \p x.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::sin_impl functor.
 */
template <typename T>
inline detail::math_sin_type<T> sin(const T &x)
{
    return sin_impl<T>{}(x);
}

/// Default functor for the implementation of piranha::math::partial().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename Enable = void>
struct partial_impl {
};

/// Specialisation of the piranha::math::partial() functor for arithmetic types.
/**
 * This specialisation is activated when \p T is a C++ arithmetic type.
 * The result will be zero.
 */
template <typename T>
struct partial_impl<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
    /// Call operator.
    /**
     * @return an instance of \p T constructed from zero.
     */
    T operator()(const T &, const std::string &) const
    {
        return T(0);
    }
};
} // namespace math

namespace detail
{

// Return type for math::partial().
template <typename T>
using math_partial_type_
    = decltype(math::partial_impl<T>{}(std::declval<const T &>(), std::declval<const std::string &>()));

template <typename T>
using math_partial_type =
    typename std::enable_if<is_returnable<math_partial_type_<T>>::value, math_partial_type_<T>>::type;
} // namespace detail

namespace math
{

/// Partial derivative.
/**
 * \note
 * This function is enabled only if the expression <tt>partial_impl<T>{}(x,str)</tt> is valid, returning a type that
 * satisfies piranha::is_returnable.
 *
 * Return the partial derivative of \p x with respect to the symbolic quantity named \p str. The actual
 * implementation of this function is in the piranha::math::partial_impl functor. The body of this function
 * is equivalent to:
 * @code
 * return partial_impl<T>{}(x,str);
 * @endcode
 *
 * @param x argument for the partial derivative.
 * @param str name of the symbolic quantity with respect to which the derivative will be computed.
 *
 * @return partial derivative of \p x with respect to \p str.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::partial_impl.
 */
template <typename T>
inline detail::math_partial_type<T> partial(const T &x, const std::string &str)
{
    return partial_impl<T>{}(x, str);
}

/// Default functor for the implementation of piranha::math::integrate().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename Enable = void>
struct integrate_impl {
};
} // namespace math

namespace detail
{

// Return type for math::integrate().
template <typename T>
using math_integrate_type_
    = decltype(math::integrate_impl<T>{}(std::declval<const T &>(), std::declval<const std::string &>()));

template <typename T>
using math_integrate_type =
    typename std::enable_if<is_returnable<math_integrate_type_<T>>::value, math_integrate_type_<T>>::type;
} // namespace detail

namespace math
{

/// Integration.
/**
 * \note
 * This function is enabled only if the expression <tt>integrate_impl<T>{}(x,str)</tt> is valid, returning a type that
 * satisfies piranha::is_returnable.
 *
 * Return the antiderivative of \p x with respect to the symbolic quantity named \p str. The actual
 * implementation of this function is in the piranha::math::integrate_impl functor. The body of this function
 * is equivalent to:
 * @code
 * return integrate_impl<T>{}(x,str);
 * @endcode
 *
 * @param x argument for the integration.
 * @param str name of the symbolic quantity with respect to which the integration will be computed.
 *
 * @return antiderivative of \p x with respect to \p str.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::integrate_impl.
 */
template <typename T>
inline detail::math_integrate_type<T> integrate(const T &x, const std::string &str)
{
    return integrate_impl<T>{}(x, str);
}

/// Default functor for the implementation of piranha::math::evaluate().
/**
 * This functor should be specialised via the \p std::enable_if mechanism.
 */
template <typename T, typename U, typename Enable = void>
struct evaluate_impl {
private:
    template <typename V>
    using enabler = typename std::enable_if<std::is_copy_constructible<V>::value && is_returnable<V>::value, int>::type;

public:
    /// Call operator.
    /**
     * \note
     * This operator is enabled only if \p V is copy-constructible and if it satisfies piranha::is_returnable.
     *
     * The default behaviour is to return the input value \p x unchanged.
     *
     * @param x evaluation argument.
     *
     * @return copy of \p x.
     *
     * @throws unspecified any exception thrown by the invoked copy constructor.
     */
    template <typename V, enabler<V> = 0>
    V operator()(const V &x, const std::unordered_map<std::string, U> &) const
    {
        return x;
    }
};
} // namespace math

namespace detail
{

// Return type for math::evaluate().
template <typename T, typename U>
using math_evaluate_type_ = decltype(
    math::evaluate_impl<T, U>{}(std::declval<const T &>(), std::declval<const std::unordered_map<std::string, U> &>()));

template <typename T, typename U>
using math_evaluate_type =
    typename std::enable_if<is_returnable<math_evaluate_type_<T, U>>::value, math_evaluate_type_<T, U>>::type;
} // namespace detail

namespace math
{

/// Evaluation.
/**
 * \note
 * This function is enabled only if <tt>evaluate_impl<T,U>{}(x,dict)</tt> is a valid expression, returning
 * a type which satisfies piranha::is_returnable.
 *
 * Evaluation is the simultaneous substitution of all symbolic arguments in an expression. The input dictionary \p dict
 * specifies the quantity (value) that will be susbstituted for each argument (key), represented as a string.
 * The actual implementation of this function is in the piranha::math::evaluate_impl functor.
 * The body of this function is equivalent to:
 * @code
 * return evaluate_impl<T,U>{}(x,dict);
 * @endcode
 *
 * @param x quantity that will be evaluated.
 * @param dict dictionary that will be used to perform the substitution.
 *
 * @return \p x evaluated according to \p dict.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::evaluate_impl.
 */
template <typename U, typename T>
inline detail::math_evaluate_type<T, U> evaluate(const T &x, const std::unordered_map<std::string, U> &dict)
{
    return evaluate_impl<T, U>{}(x, dict);
}

/// Default functor for the implementation of piranha::math::subs().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename U, typename Enable = void>
struct subs_impl {
};
} // namespace math

namespace detail
{

// Return type for math::subs().
template <typename T, typename U>
using math_subs_type_ = decltype(
    math::subs_impl<T, U>{}(std::declval<const T &>(), std::declval<const std::string &>(), std::declval<const U &>()));

template <typename T, typename U>
using math_subs_type =
    typename std::enable_if<is_returnable<math_subs_type_<T, U>>::value, math_subs_type_<T, U>>::type;
} // namespace detail

namespace math
{

/// Substitution.
/**
 * \note
 * This function is enabled only if <tt>subs_impl<T,U>{}(x,name,y)</tt> is a valid expression, returning
 * a type which satisfies piranha::is_returnable.
 *
 * Substitute a symbolic variable with a generic object.
 * The actual implementation of this function is in the piranha::math::subs_impl functor.
 * The body of this function is equivalent to:
 * @code
 * return subs_impl<T,U>{}(x,name,y);
 * @endcode
 *
 * @param x quantity that will be subject to substitution.
 * @param name name of the symbolic variable that will be substituted.
 * @param y object that will substitute the variable.
 *
 * @return \p x after substitution  of \p name with \p y.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::subs_impl.
 */
template <typename T, typename U>
inline detail::math_subs_type<T, U> subs(const T &x, const std::string &name, const U &y)
{
    return subs_impl<T, U>{}(x, name, y);
}

/// Default functor for the implementation of piranha::math::t_subs().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename U, typename V, typename = void>
struct t_subs_impl {
};
} // namespace math

namespace detail
{

// Return type for math::t_subs().
template <typename T, typename U, typename V>
using math_t_subs_type_
    = decltype(math::t_subs_impl<T, U, V>{}(std::declval<const T &>(), std::declval<const std::string &>(),
                                            std::declval<const U &>(), std::declval<const V &>()));

template <typename T, typename U, typename V>
using math_t_subs_type =
    typename std::enable_if<is_returnable<math_t_subs_type_<T, U, V>>::value, math_t_subs_type_<T, U, V>>::type;
} // namespace detail

namespace math
{

/// Trigonometric substitution.
/**
 * \note
 * This function is enabled only if <tt>t_subs_impl<T,U,V>{}(x,name,c,s)</tt> is a valid expression, returning
 * a type which satisfies piranha::is_returnable.
 *
 * Substitute the cosine and sine of a symbolic variable with generic objects.
 * The actual implementation of this function is in the piranha::math::t_subs_impl functor.
 * The body of this function is equivalent to:
 * @code
 * return t_subs_impl<T,U,V>{}(x,name,c,s);
 * @endcode
 *
 * @param x quantity that will be subject to substitution.
 * @param name name of the symbolic variable that will be substituted.
 * @param c object that will substitute the cosine of the variable.
 * @param s object that will substitute the sine of the variable.
 *
 * @return \p x after substitution of cosine and sine of \p name with \p c and \p s.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_subs_impl.
 */
template <typename T, typename U, typename V>
inline detail::math_t_subs_type<T, U, V> t_subs(const T &x, const std::string &name, const U &c, const V &s)
{
    return t_subs_impl<T, U, V>{}(x, name, c, s);
}

/// Default functor for the implementation of piranha::math::abs().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename Enable = void>
struct abs_impl {
};
} // namespace math

namespace detail
{

template <typename T>
using abs_arith_enabler = typename std::enable_if<(std::is_signed<T>::value && std::is_integral<T>::value)
                                                  || (std::is_unsigned<T>::value && std::is_integral<T>::value)
                                                  || std::is_floating_point<T>::value>::type;
}

namespace math
{

/// Specialisation of the piranha::math::abs() functor for signed and unsigned integer types, and floating-point types.
/**
 * This specialisation is activated when \p T is a signed or unsigned integer type, or a floating-point type.
 * The result will be computed via \p std::abs() for floating-point and signed integer types,
 * while for unsigned integer types it will be the input value unchanged.
 */
template <typename T>
struct abs_impl<T, detail::abs_arith_enabler<T>> {
private:
    template <typename U>
    static U impl(const U &x, typename std::enable_if<std::is_floating_point<U>::value>::type * = nullptr)
    {
        return std::abs(x);
    }
    // NOTE: use decltype here so, in the remote case we are dealing with an extended integer types (for which
    // std::abs()
    // will not exist and cast to long long might be lossy), the overload will be discarded.
    template <typename U>
    static auto impl(const U &x,
                     typename std::enable_if<std::is_integral<U>::value && std::is_signed<U>::value>::type * = nullptr)
        -> decltype(static_cast<U>(std::abs(static_cast<long long>(x))))
    {
        // Cast to long long in order to avoid conversion derps when dealing with chars.
        return static_cast<U>(std::abs(static_cast<long long>(x)));
    }
    template <typename U>
    static U impl(const U &x,
                  typename std::enable_if<std::is_integral<U>::value && std::is_unsigned<U>::value>::type * = nullptr)
    {
        return x;
    }

public:
    /// Call operator.
    /**
     * @param x input parameter.
     *
     * @return absolute value of \p x.
     */
    auto operator()(const T &x) const -> decltype(impl(x))
    {
        return impl(x);
    }
};

/// Absolute value.
/**
 * The actual implementation of this function is in the piranha::math::abs_impl functor.
 *
 * @param x quantity whose absolute value will be calculated.
 *
 * @return absolute value of \p x.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::abs_impl.
 */
template <typename T>
inline auto abs(const T &x) -> decltype(abs_impl<T>()(x))
{
    return abs_impl<T>()(x);
}
} // namespace math

/// Type trait to detect the presence of the piranha::math::is_zero() function.
/**
 * The type trait will be \p true if piranha::math::is_zero() can be successfully called on instances of \p T.
 */
template <typename T>
class has_is_zero : detail::sfinae_types
{
    typedef typename std::decay<T>::type Td;
    template <typename T1>
    static auto test(const T1 &t) -> decltype(math::is_zero(t), void(), yes());
    static no test(...);
    static const bool implementation_defined = std::is_same<decltype(test(std::declval<Td>())), yes>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

// Static init.
template <typename T>
const bool has_is_zero<T>::value;

/// Type trait to detect the presence of the piranha::math::negate function.
/**
 * The type trait will be \p true if piranha::math::negate can be successfully called on instances of \p T
 * stripped of reference qualifiers, \p false otherwise.
 */
template <typename T>
class has_negate : detail::sfinae_types
{
    template <typename T1>
    static auto test(T1 &t) -> decltype(math::negate(t), void(), yes());
    static no test(...);
    static const bool implementation_defined = std::is_same<decltype(test(std::declval<T &>())), yes>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

template <typename T>
const bool has_negate<T>::value;

namespace detail
{

#if !defined(PIRANHA_DOXYGEN_INVOKED)

// Type definition and type checking for the output of Poisson brackets.
template <typename T>
using pbracket_type_tmp = decltype(math::partial(std::declval<const T &>(), std::string())
                                   * math::partial(std::declval<const T &>(), std::string()));

template <typename T, typename = void>
struct pbracket_type_ {
};

template <typename T>
struct pbracket_type_<
    T, typename std::enable_if<std::is_same<decltype(std::declval<const pbracket_type_tmp<T> &>()
                                                     + std::declval<const pbracket_type_tmp<T> &>()),
                                            pbracket_type_tmp<T>>::value
                               && std::is_same<decltype(std::declval<const pbracket_type_tmp<T> &>()
                                                        - std::declval<const pbracket_type_tmp<T> &>()),
                                               pbracket_type_tmp<T>>::value
                               && std::is_constructible<pbracket_type_tmp<T>, int>::value
                               && std::is_assignable<pbracket_type_tmp<T> &, pbracket_type_tmp<T>>::value>::type> {
    using type = pbracket_type_tmp<T>;
};

// The final typedef.
template <typename T>
using pbracket_type = typename pbracket_type_<T>::type;

#endif
} // namespace detail

namespace math
{

/// Poisson bracket.
/**
 * \note
 * This template function is enabled only if \p T is differentiable and the arithmetic operations needed to compute the
 * brackets
 * are supported by the types involved in the computation.
 *
 * The Poisson bracket of \p f and \p g with respect to the list of momenta \p p_list and coordinates \p q_list
 * is defined as:
 * \f[
 * \left\{f,g\right\} = \sum_{i=1}^{N}
 * \left[
 * \frac{\partial f}{\partial q_{i}} \frac{\partial g}{\partial p_{i}} -
 * \frac{\partial f}{\partial p_{i}} \frac{\partial g}{\partial q_{i}}
 * \right],
 * \f]
 * where \f$ p_i \f$ and \f$ q_i \f$ are the elements of \p p_list and \p q_list.
 *
 * @param f first argument.
 * @param g second argument.
 * @param p_list list of the names of momenta.
 * @param q_list list of the names of coordinates.
 *
 * @return the poisson bracket of \p f and \p g with respect to \p p_list and \p q_list.
 *
 * @throws std::invalid_argument if the sizes of \p p_list and \p q_list differ or if
 * \p p_list or \p q_list contain duplicate entries.
 * @throws unspecified any exception thrown by piranha::math::partial() or by the invoked arithmetic operators,
 * constructors and assignment operators.
 */
template <typename T>
inline detail::pbracket_type<T> pbracket(const T &f, const T &g, const std::vector<std::string> &p_list,
                                         const std::vector<std::string> &q_list)
{
    using return_type = detail::pbracket_type<T>;
    if (p_list.size() != q_list.size()) {
        piranha_throw(std::invalid_argument, "the number of coordinates is different from the number of momenta");
    }
    if (std::unordered_set<std::string>(p_list.begin(), p_list.end()).size() != p_list.size()) {
        piranha_throw(std::invalid_argument, "the list of momenta contains duplicate entries");
    }
    if (std::unordered_set<std::string>(q_list.begin(), q_list.end()).size() != q_list.size()) {
        piranha_throw(std::invalid_argument, "the list of coordinates contains duplicate entries");
    }
    return_type retval = return_type(0);
    for (decltype(p_list.size()) i = 0u; i < p_list.size(); ++i) {
        // NOTE: could use multadd/sub here, if we implement it for series.
        retval = retval + partial(f, q_list[i]) * partial(g, p_list[i]);
        retval = retval - partial(f, p_list[i]) * partial(g, q_list[i]);
    }
    return retval;
}
} // namespace math

/// Detect piranha::math::pbracket().
/**
 * The type trait will be \p true if piranha::math::pbracket() can be used on instances of type \p T,
 * \p false otherwise.
 */
template <typename T>
class has_pbracket : detail::sfinae_types
{
    using v_string = std::vector<std::string>;
    template <typename T1>
    static auto test(const T1 &x)
        -> decltype(math::pbracket(x, x, std::declval<v_string const &>(), std::declval<v_string const &>()), void(),
                    yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<T>())), yes>::value;
};

template <typename T>
const bool has_pbracket<T>::value;

namespace detail
{

template <typename T>
using is_canonical_enabler = typename std::enable_if<has_pbracket<T>::value && has_is_zero<pbracket_type<T>>::value
                                                         && std::is_constructible<pbracket_type<T>, int>::value
                                                         && is_equality_comparable<pbracket_type<T>>::value,
                                                     int>::type;

template <typename T>
inline bool is_canonical_impl(const std::vector<T const *> &new_p, const std::vector<T const *> &new_q,
                              const std::vector<std::string> &p_list, const std::vector<std::string> &q_list)
{
    using p_type = decltype(math::pbracket(*new_q[0], *new_p[0], p_list, q_list));
    if (p_list.size() != q_list.size()) {
        piranha_throw(std::invalid_argument, "the number of coordinates is different from the number of momenta");
    }
    if (new_p.size() != new_q.size()) {
        piranha_throw(std::invalid_argument,
                      "the number of new coordinates is different from the number of new momenta");
    }
    if (p_list.size() != new_p.size()) {
        piranha_throw(std::invalid_argument, "the number of new momenta is different from the number of momenta");
    }
    if (std::unordered_set<std::string>(p_list.begin(), p_list.end()).size() != p_list.size()) {
        piranha_throw(std::invalid_argument, "the list of momenta contains duplicate entries");
    }
    if (std::unordered_set<std::string>(q_list.begin(), q_list.end()).size() != q_list.size()) {
        piranha_throw(std::invalid_argument, "the list of coordinates contains duplicate entries");
    }
    const auto size = new_p.size();
    for (decltype(new_p.size()) i = 0u; i < size; ++i) {
        for (decltype(new_p.size()) j = 0u; j < size; ++j) {
            // NOTE: no need for actually doing computations when i == j.
            if (i != j && !math::is_zero(math::pbracket(*new_p[i], *new_p[j], p_list, q_list))) {
                return false;
            }
            if (i != j && !math::is_zero(math::pbracket(*new_q[i], *new_q[j], p_list, q_list))) {
                return false;
            }
            // Poisson bracket needs to be zero for i != j, one for i == j.
            // NOTE: cast from bool to int is always 0 or 1.
            if (math::pbracket(*new_q[i], *new_p[j], p_list, q_list) != p_type(static_cast<int>(i == j))) {
                return false;
            }
        }
    }
    return true;
}
} // namespace detail

namespace math
{

/// Check if a transformation is canonical.
/**
 * \note
 * This function is enabled only if all the following requirements are met:
 * - \p T satisfies piranha::has_pbracket,
 * - the output type of piranha::has_pbracket for \p T satisfies piranha::has_is_zero, it is constructible from \p int
 *   and it is equality comparable.
 *
 * This function will check if a transformation of Hamiltonian momenta and coordinates is canonical using the Poisson
 * bracket test.
 * The transformation is expressed as two separate collections of objects, \p new_p and \p new_q, representing the new
 * momenta
 * and coordinates as functions of the old momenta \p p_list and \p q_list.
 *
 * @param new_p list of objects representing the new momenta.
 * @param new_q list of objects representing the new coordinates.
 * @param p_list list of names of the old momenta.
 * @param q_list list of names of the old coordinates.
 *
 * @return \p true if the transformation is canonical, \p false otherwise.
 *
 * @throws std::invalid_argument if the sizes of the four input arguments are not the same or if either \p p_list or \p
 * q_list
 * contain duplicate entries.
 * @throws unspecified any exception thrown by:
 * - piranha::math::pbracket(),
 * - construction and comparison of objects of the type returned by piranha::math::pbracket(),
 * - piranha::math::is_zero(),
 * - memory errors in standard containers.
 */
template <typename T, detail::is_canonical_enabler<T> = 0>
inline bool transformation_is_canonical(const std::vector<T> &new_p, const std::vector<T> &new_q,
                                        const std::vector<std::string> &p_list, const std::vector<std::string> &q_list)
{
    std::vector<T const *> pv, qv;
    std::transform(new_p.begin(), new_p.end(), std::back_inserter(pv), [](const T &p) { return &p; });
    std::transform(new_q.begin(), new_q.end(), std::back_inserter(qv), [](const T &q) { return &q; });
    return detail::is_canonical_impl(pv, qv, p_list, q_list);
}

// clang-format off
/// Check if a transformation is canonical (alternative overload).
/**
 * @param new_p list of objects representing the new momenta.
 * @param new_q list of objects representing the new coordinates.
 * @param p_list list of names of the old momenta.
 * @param q_list list of names of the old coordinates.
 *
 * @return the output of transformation_is_canonical(const std::vector<T> &, const std::vector<T> &, const std::vector<std::string> &, const std::vector<std::string> &).
 *
 * @throws unspecified any exception thrown by transformation_is_canonical(const std::vector<T> &, const std::vector<T> &, const std::vector<std::string> &, const std::vector<std::string> &).
 */
// clang-format on
template <typename T, detail::is_canonical_enabler<T> = 0>
inline bool transformation_is_canonical(std::initializer_list<T> new_p, std::initializer_list<T> new_q,
                                        const std::vector<std::string> &p_list, const std::vector<std::string> &q_list)
{
    std::vector<T const *> pv, qv;
    std::transform(new_p.begin(), new_p.end(), std::back_inserter(pv), [](const T &p) { return &p; });
    std::transform(new_q.begin(), new_q.end(), std::back_inserter(qv), [](const T &q) { return &q; });
    return detail::is_canonical_impl(pv, qv, p_list, q_list);
}

/// Default functor for the implementation of piranha::math::degree().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 *
 * Note that the implementation of this functor requires two overloaded call operators, one for the unary form
 * of piranha::math::degree() (the total degree), the other for the binary form of piranha::math::degree()
 * (the partial degree).
 */
template <typename T, typename Enable = void>
struct degree_impl {
};

/// Total degree.
/**
 * Return the total degree (as in polynomial degree).
 *
 * The actual implementation of this function is in the piranha::math::degree_impl functor.
 *
 * @param x object whose degree will be computed.
 *
 * @return total degree.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::degree_impl.
 */
template <typename T>
inline auto degree(const T &x) -> decltype(degree_impl<T>()(x))
{
    return degree_impl<T>()(x);
}

/// Partial degree.
/**
 * Return the partial degree (as in polynomial degree, but only a set of variables is considered in the computation).
 *
 * The actual implementation of this function is in the piranha::math::degree_impl functor.
 *
 * @param x object whose partial degree will be computed.
 * @param names names of the variables that will be considered in the computation.
 *
 * @return partial degree.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::degree_impl.
 */
template <typename T>
inline auto degree(const T &x, const std::vector<std::string> &names) -> decltype(degree_impl<T>()(x, names))
{
    return degree_impl<T>()(x, names);
}

/// Default functor for the implementation of piranha::math::ldegree().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 *
 * Note that the implementation of this functor requires two overloaded call operators, one for the unary form
 * of piranha::math::ldegree() (the total low degree), the other for the binary form of piranha::math::ldegree()
 * (the partial low degree).
 */
template <typename T, typename Enable = void>
struct ldegree_impl {
};

/// Total low degree.
/**
 * Return the total low degree (as in polynomial low degree).
 *
 * The actual implementation of this function is in the piranha::math::ldegree_impl functor.
 *
 * @param x object whose low degree will be computed.
 *
 * @return total low degree.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::ldegree_impl.
 */
template <typename T>
inline auto ldegree(const T &x) -> decltype(ldegree_impl<T>()(x))
{
    return ldegree_impl<T>()(x);
}

/// Partial low degree.
/**
 * Return the partial low degree (as in polynomial low degree, but only a set of variables is considered in the
 * computation).
 *
 * The actual implementation of this function is in the piranha::math::ldegree_impl functor.
 *
 * @param x object whose partial low degree will be computed.
 * @param names names of the variables that will be considered in the computation.
 *
 * @return partial low degree.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::ldegree_impl.
 */
template <typename T>
inline auto ldegree(const T &x, const std::vector<std::string> &names) -> decltype(ldegree_impl<T>()(x, names))
{
    return ldegree_impl<T>()(x, names);
}

/// Default functor for the implementation of piranha::math::t_degree().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 *
 * Note that the implementation of this functor requires two overloaded call operators, one for the unary form
 * of piranha::math::t_degree() (the total trigonometric degree), the other for the binary form of
 * piranha::math::t_degree()
 * (the partial trigonometric degree).
 */
template <typename T, typename Enable = void>
struct t_degree_impl {
};

/// Total trigonometric degree.
/**
 * A type exposing a trigonometric degree property, in analogy with the concept of polynomial degree,
 * should be a linear combination of real or complex trigonometric functions. For instance, the Poisson series
 * \f[
 * 2\cos\left(3x+y\right) + 3\cos\left(2x-y\right)
 * \f]
 * has a trigonometric degree of 3+1=4.
 *
 * The actual implementation of this function is in the piranha::math::t_degree_impl functor.
 *
 * @param x object whose trigonometric degree will be computed.
 *
 * @return total trigonometric degree.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_degree_impl.
 */
template <typename T>
inline auto t_degree(const T &x) -> decltype(t_degree_impl<T>()(x))
{
    return t_degree_impl<T>()(x);
}

/// Partial trigonometric degree.
/**
 * The partial trigonometric degree is the trigonometric degree when only certain variables are considered in
 * the computation.
 *
 * The actual implementation of this function is in the piranha::math::t_degree_impl functor.
 *
 * @param x object whose trigonometric degree will be computed.
 * @param names names of the variables that will be considered in the computation of the degree.
 *
 * @return partial trigonometric degree.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_degree_impl.
 *
 * @see piranha::math::t_degree().
 */
template <typename T>
inline auto t_degree(const T &x, const std::vector<std::string> &names) -> decltype(t_degree_impl<T>()(x, names))
{
    return t_degree_impl<T>()(x, names);
}

/// Default functor for the implementation of piranha::math::t_ldegree().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 *
 * Note that the implementation of this functor requires two overloaded call operators, one for the unary form
 * of piranha::math::t_ldegree() (the total trigonometric low degree), the other for the binary form of
 * piranha::math::t_ldegree()
 * (the partial trigonometric low degree).
 */
template <typename T, typename Enable = void>
struct t_ldegree_impl {
};

/// Total trigonometric low degree.
/**
 * A type exposing a trigonometric low degree property, in analogy with the concept of polynomial low degree,
 * should be a linear combination of real or complex trigonometric functions. For instance, the Poisson series
 * \f[
 * 2\cos\left(3x+y\right) + 3\cos\left(2x-y\right)
 * \f]
 * has a trigonometric low degree of 2-1=1.
 *
 * The actual implementation of this function is in the piranha::math::t_ldegree_impl functor.
 *
 * @param x object whose trigonometric low degree will be computed.
 *
 * @return total trigonometric low degree.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_ldegree_impl.
 */
template <typename T>
inline auto t_ldegree(const T &x) -> decltype(t_ldegree_impl<T>()(x))
{
    return t_ldegree_impl<T>()(x);
}

/// Partial trigonometric low degree.
/**
 * The partial trigonometric low degree is the trigonometric low degree when only certain variables are considered in
 * the computation.
 *
 * The actual implementation of this function is in the piranha::math::t_ldegree_impl functor.
 *
 * @param x object whose trigonometric low degree will be computed.
 * @param names names of the variables that will be considered in the computation of the degree.
 *
 * @return partial trigonometric low degree.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_ldegree_impl.
 *
 * @see piranha::math::t_ldegree().
 */
template <typename T>
inline auto t_ldegree(const T &x, const std::vector<std::string> &names) -> decltype(t_ldegree_impl<T>()(x, names))
{
    return t_ldegree_impl<T>()(x, names);
}

/// Default functor for the implementation of piranha::math::t_order().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 *
 * Note that the implementation of this functor requires two overloaded call operators, one for the unary form
 * of piranha::math::t_order() (the total trigonometric order), the other for the binary form of
 * piranha::math::t_order()
 * (the partial trigonometric order).
 */
template <typename T, typename Enable = void>
struct t_order_impl {
};

/// Total trigonometric order.
/**
 * A type exposing a trigonometric order property should be a linear combination of real or complex trigonometric
 * functions.
 * The order is computed in a way similar to the trigonometric degree, with the key difference that the absolute values
 * of
 * the trigonometric degrees of each variable are considered in the computation. For instance, the Poisson series
 * \f[
 * 2\cos\left(3x+y\right) + 3\cos\left(2x-y\right)
 * \f]
 * has a trigonometric order of abs(3)+abs(1)=4.
 *
 * The actual implementation of this function is in the piranha::math::t_order_impl functor.
 *
 * @param x object whose trigonometric order will be computed.
 *
 * @return total trigonometric order.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_order_impl.
 */
template <typename T>
inline auto t_order(const T &x) -> decltype(t_order_impl<T>()(x))
{
    return t_order_impl<T>()(x);
}

/// Partial trigonometric order.
/**
 * The partial trigonometric order is the trigonometric order when only certain variables are considered in
 * the computation.
 *
 * The actual implementation of this function is in the piranha::math::t_order_impl functor.
 *
 * @param x object whose trigonometric order will be computed.
 * @param names names of the variables that will be considered in the computation of the order.
 *
 * @return partial trigonometric order.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_order_impl.
 *
 * @see piranha::math::t_order().
 */
template <typename T>
inline auto t_order(const T &x, const std::vector<std::string> &names) -> decltype(t_order_impl<T>()(x, names))
{
    return t_order_impl<T>()(x, names);
}

/// Default functor for the implementation of piranha::math::t_lorder().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 *
 * Note that the implementation of this functor requires two overloaded call operators, one for the unary form
 * of piranha::math::t_lorder() (the total trigonometric low order), the other for the binary form of
 * piranha::math::t_lorder()
 * (the partial trigonometric low order).
 */
template <typename T, typename Enable = void>
struct t_lorder_impl {
};

/// Total trigonometric low order.
/**
 * A type exposing a trigonometric low order property should be a linear combination of real or complex trigonometric
 * functions.
 * The low order is computed in a way similar to the trigonometric low degree, with the key difference that the absolute
 * values of
 * the trigonometric degrees of each variable are considered in the computation. For instance, the Poisson series
 * \f[
 * 2\cos\left(3x+y\right) + 3\cos\left(2x-y\right)
 * \f]
 * has a trigonometric low order of abs(2)+abs(1)=3.
 *
 * The actual implementation of this function is in the piranha::math::t_lorder_impl functor.
 *
 * @param x object whose trigonometric low order will be computed.
 *
 * @return total trigonometric low order.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_lorder_impl.
 */
template <typename T>
inline auto t_lorder(const T &x) -> decltype(t_lorder_impl<T>()(x))
{
    return t_lorder_impl<T>()(x);
}

/// Partial trigonometric low order.
/**
 * The partial trigonometric low order is the trigonometric low order when only certain variables are considered in
 * the computation.
 *
 * The actual implementation of this function is in the piranha::math::t_lorder_impl functor.
 *
 * @param x object whose trigonometric low order will be computed.
 * @param names names of the variables that will be considered in the computation of the order.
 *
 * @return partial trigonometric low order.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::t_lorder_impl.
 *
 * @see piranha::math::t_lorder().
 */
template <typename T>
inline auto t_lorder(const T &x, const std::vector<std::string> &names) -> decltype(t_lorder_impl<T>()(x, names))
{
    return t_lorder_impl<T>()(x, names);
}

/// Implementation of the piranha::math::truncate_degree() functor.
/**
 * The default implementation does not define any call operator.
 */
template <typename T, typename U, typename = void>
struct truncate_degree_impl {
};
} // namespace math

namespace detail
{

// Enablers for the degree truncation methods.
template <typename T, typename U>
using truncate_degree_enabler = typename std::enable_if<
    std::is_same<decltype(math::truncate_degree_impl<T, U>()(std::declval<const T &>(), std::declval<const U &>())),
                 T>::value,
    int>::type;

template <typename T, typename U>
using truncate_pdegree_enabler = typename std::enable_if<
    std::is_same<decltype(math::truncate_degree_impl<T, U>()(std::declval<const T &>(), std::declval<const U &>(),
                                                             std::declval<const std::vector<std::string> &>())),
                 T>::value,
    int>::type;
} // namespace detail

namespace math
{

/// Truncation based on the total degree.
/**
 * This method is used to eliminate from the input argument \p x all the parts
 * whose total degree is greater than \p max_degree.
 *
 * The actual implementation of this function is in the piranha::math::truncate_degree_impl functor.
 *
 * The body of this function is equivalent to:
@code
return truncate_degree_impl<T,U>()(x,max_degree);
@endcode
 * The call operator of piranha::math::truncate_degree_impl is required to return type \p T, otherwise
 * this function will be disabled.
 *
 * @param x object which will be subject to truncation.
 * @param max_degree maximum allowed total degree in the output.
 *
 * @return the truncated counterpart of \p x.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::truncate_degree_impl.
 */
template <typename T, typename U, detail::truncate_degree_enabler<T, U> = 0>
inline T truncate_degree(const T &x, const U &max_degree)
{
    return truncate_degree_impl<T, U>()(x, max_degree);
}

/// Truncation based on the partial degree.
/**
 * This method is used to eliminate from the input argument \p x all the parts
 * whose partial degree is greater than \p max_degree.
 *
 * The actual implementation of this function is in the piranha::math::truncate_degree_impl functor.
 *
 * The body of this function is equivalent to:
@code
return truncate_degree_impl<T,U>()(x,max_degree,names);
@endcode
 * The call operator of piranha::math::truncate_degree_impl is required to return type \p T, otherwise
 * this function will be disabled.
 *
 * @param x object which will be subject to truncation.
 * @param max_degree maximum allowed partial degree in the output.
 * @param names names of the variables that will be considered in the computation of the partial degree.
 *
 * @return the truncated counterpart of \p x.
 *
 * @throws unspecified any exception thrown by the call operator of piranha::math::truncate_degree_impl.
 */
template <typename T, typename U, detail::truncate_pdegree_enabler<T, U> = 0>
inline T truncate_degree(const T &x, const U &max_degree, const std::vector<std::string> &names)
{
    return truncate_degree_impl<T, U>()(x, max_degree, names);
}
} // namespace math

/// Type trait to detect if types can be used in piranha::math::truncate_degree().
/**
 * The type trait will be true if instances of types \p T and \p U can be used as arguments of
 * piranha::math::truncate_degree()
 * (both in the binary and ternary version of the function).
 */
template <typename T, typename U>
class has_truncate_degree : detail::sfinae_types
{
    template <typename T1, typename U1>
    static auto test1(const T1 &t, const U1 &u) -> decltype(math::truncate_degree(t, u), void(), yes());
    static no test1(...);
    template <typename T1, typename U1>
    static auto test2(const T1 &t, const U1 &u)
        -> decltype(math::truncate_degree(t, u, std::declval<const std::vector<std::string> &>()), void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1(std::declval<T>(), std::declval<U>())), yes>::value
                              && std::is_same<decltype(test2(std::declval<T>(), std::declval<U>())), yes>::value;
};

template <typename T, typename U>
const bool has_truncate_degree<T, U>::value;

/// Type trait for differentiable types.
/**
 * The type trait will be \p true if piranha::math::partial() can be successfully called on instances of
 * type \p T, \p false otherwise.
 */
template <typename T>
class is_differentiable : detail::sfinae_types
{
    template <typename U>
    static auto test(const U &u) -> decltype(math::partial(u, ""), void(), yes());
    static no test(...);
    static const bool implementation_defined = std::is_same<decltype(test(std::declval<T>())), yes>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

// Static init.
template <typename T>
const bool is_differentiable<T>::value;

namespace detail
{

// A key is differentiable if the type returned by the partial method
// is a pair (sometype,key).
template <typename Key, typename Pair>
struct is_differential_key_pair {
    static const bool value = false;
};

template <typename Key, typename PairFirst>
struct is_differential_key_pair<Key, std::pair<PairFirst, Key>> {
    static const bool value = true;
};
} // namespace detail

/// Type trait to detect differentiable keys.
/**
 * This type trait will be \p true if \p Key is a key type providing a const method <tt>partial()</tt> taking a const
 * instance of
 * piranha::symbol_set::positions and a const instance of piranha::symbol_set as input, and returning an
 * <tt>std::pair</tt> of
 * any type and \p Key. Otherwise, the type trait will be \p false.
 * If \p Key does not satisfy piranha::is_key, a compilation error will be produced.
 *
 * The decay type of \p Key is considered in this type trait.
 */
template <typename T>
class key_is_differentiable : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    PIRANHA_TT_CHECK(is_key, Td);
    template <typename U>
    static auto test(const U &u)
        -> decltype(u.partial(std::declval<const symbol_set::positions &>(), std::declval<const symbol_set &>()));
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = detail::is_differential_key_pair<Td, decltype(test(std::declval<Td>()))>::value;
};

template <typename T>
const bool key_is_differentiable<T>::value;

/// Type trait for integrable types.
/**
 * The type trait will be \p true if piranha::math::integrate() can be successfully called on instances of
 * type \p T, \p false otherwise.
 */
template <typename T>
class is_integrable : detail::sfinae_types
{
    template <typename U>
    static auto test(const U &u) -> decltype(math::integrate(u, ""), void(), yes());
    static no test(...);
    static const bool implementation_defined = std::is_same<decltype(test(std::declval<T>())), yes>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

template <typename T>
const bool is_integrable<T>::value;

/// Type trait to detect integrable keys.
/**
 * This type trait will be \p true if \p Key is a key type providing a const method <tt>integrate()</tt> taking a const
 * instance of
 * piranha::symbol and a const instance of piranha::symbol_set as input, and returning an <tt>std::pair</tt> of
 * any type and \p Key. Otherwise, the type trait will be \p false.
 * If \p Key does not satisfy piranha::is_key, a compilation error will be produced.
 *
 * The decay type of \p Key is considered in this type trait.
 */
template <typename T>
class key_is_integrable : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    PIRANHA_TT_CHECK(is_key, Td);
    template <typename U>
    static auto test(const U &u)
        -> decltype(u.integrate(std::declval<const symbol &>(), std::declval<const symbol_set &>()));
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = detail::is_differential_key_pair<Td, decltype(test(std::declval<Td>()))>::value;
};

template <typename T>
const bool key_is_integrable<T>::value;

/// Type trait to detect if type has a degree property.
/**
 * The type trait will be true if instances of type \p T can be used as arguments of piranha::math::degree()
 * (both in the unary and binary version of the function).
 */
template <typename T>
class has_degree : detail::sfinae_types
{
    template <typename U>
    static auto test1(const U &u) -> decltype(math::degree(u), void(), yes());
    static no test1(...);
    template <typename U>
    static auto test2(const U &u)
        -> decltype(math::degree(u, std::declval<const std::vector<std::string> &>()), void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1(std::declval<T>())), yes>::value
                              && std::is_same<decltype(test2(std::declval<T>())), yes>::value;
};

// Static init.
template <typename T>
const bool has_degree<T>::value;

/// Type trait to detect if type has a low degree property.
/**
 * The type trait will be true if instances of type \p T can be used as arguments of piranha::math::ldegree()
 * (both in the unary and binary version of the function).
 */
template <typename T>
class has_ldegree : detail::sfinae_types
{
    template <typename U>
    static auto test1(const U &u) -> decltype(math::ldegree(u), void(), yes());
    static no test1(...);
    template <typename U>
    static auto test2(const U &u)
        -> decltype(math::ldegree(u, std::declval<const std::vector<std::string> &>()), void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1(std::declval<T>())), yes>::value
                              && std::is_same<decltype(test2(std::declval<T>())), yes>::value;
};

// Static init.
template <typename T>
const bool has_ldegree<T>::value;

/// Type trait to detect if type has a trigonometric degree property.
/**
 * The type trait will be true if instances of type \p T can be used as arguments of piranha::math::t_degree()
 * (both in the unary and binary version of the function).
 */
template <typename T>
class has_t_degree : detail::sfinae_types
{
    template <typename U>
    static auto test1(const U &u) -> decltype(math::t_degree(u), void(), yes());
    static no test1(...);
    template <typename U>
    static auto test2(const U &u)
        -> decltype(math::t_degree(u, std::declval<const std::vector<std::string> &>()), void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1(std::declval<T>())), yes>::value
                              && std::is_same<decltype(test2(std::declval<T>())), yes>::value;
};

// Static init.
template <typename T>
const bool has_t_degree<T>::value;

/// Type trait to detect if type has a trigonometric low degree property.
/**
 * The type trait will be true if instances of type \p T can be used as arguments of piranha::math::t_ldegree()
 * (both in the unary and binary version of the function).
 */
template <typename T>
class has_t_ldegree : detail::sfinae_types
{
    template <typename U>
    static auto test1(const U &u) -> decltype(math::t_ldegree(u), void(), yes());
    static no test1(...);
    template <typename U>
    static auto test2(const U &u)
        -> decltype(math::t_ldegree(u, std::declval<const std::vector<std::string> &>()), void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1(std::declval<T>())), yes>::value
                              && std::is_same<decltype(test2(std::declval<T>())), yes>::value;
};

// Static init.
template <typename T>
const bool has_t_ldegree<T>::value;

/// Type trait to detect if type has a trigonometric order property.
/**
 * The type trait will be true if instances of type \p T can be used as arguments of piranha::math::t_order()
 * (both in the unary and binary version of the function).
 */
template <typename T>
class has_t_order : detail::sfinae_types
{
    template <typename U>
    static auto test1(const U &u) -> decltype(math::t_order(u), void(), yes());
    static no test1(...);
    template <typename U>
    static auto test2(const U &u)
        -> decltype(math::t_order(u, std::declval<const std::vector<std::string> &>()), void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1(std::declval<T>())), yes>::value
                              && std::is_same<decltype(test2(std::declval<T>())), yes>::value;
};

// Static init.
template <typename T>
const bool has_t_order<T>::value;

/// Type trait to detect if type has a trigonometric low order property.
/**
 * The type trait will be true if instances of type \p T can be used as arguments of piranha::math::t_lorder()
 * (both in the unary and binary version of the function).
 */
template <typename T>
class has_t_lorder : detail::sfinae_types
{
    template <typename U>
    static auto test1(const U &u) -> decltype(math::t_lorder(u), void(), yes());
    static no test1(...);
    template <typename U>
    static auto test2(const U &u)
        -> decltype(math::t_lorder(u, std::declval<const std::vector<std::string> &>()), void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1(std::declval<T>())), yes>::value
                              && std::is_same<decltype(test2(std::declval<T>())), yes>::value;
};

// Static init.
template <typename T>
const bool has_t_lorder<T>::value;

/// Type trait to detect if a key type has a degree property.
/**
 * The type trait has the same meaning as piranha::has_degree, but it's meant for use with key types.
 * It will test the presence of two <tt>degree()</tt> const methods, the first one accepting a const instance of
 * piranha::symbol_set, the second one a const instance of piranha::symbol_set::positions and a const instance of
 * piranha::symbol_set.
 *
 * \p Key must satisfy piranha::is_key.
 */
template <typename Key>
class key_has_degree : detail::sfinae_types
{
    PIRANHA_TT_CHECK(is_key, Key);
    // NOTE: here it works (despite the difference in constness with the use below) because none of the two versions
    // of test1() is an exact match, and the conversion in constness has a higher priority than the ellipsis conversion.
    // For more info:
    // http://cpp0x.centaur.ath.cx/over.ics.rank.html
    template <typename T>
    static auto test1(const T *t) -> decltype(t->degree(std::declval<const symbol_set &>()), void(), yes());
    static no test1(...);
    template <typename T>
    static auto test2(const T *t)
        -> decltype(t->degree(std::declval<const symbol_set::positions &>(), std::declval<const symbol_set &>()),
                    void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1((Key *)nullptr)), yes>::value
                              && std::is_same<decltype(test2((Key *)nullptr)), yes>::value;
};

template <typename Key>
const bool key_has_degree<Key>::value;

/// Type trait to detect if a key type has a low degree property.
/**
 * The type trait has the same meaning as piranha::has_ldegree, but it's meant for use with key types.
 * It will test the presence of two <tt>ldegree()</tt> const methods, the first one accepting a const instance of
 * piranha::symbol_set, the second one a const instance of piranha::symbol_set::positions and a const instance of
 * piranha::symbol_set.
 *
 * \p Key must satisfy piranha::is_key.
 */
template <typename Key>
class key_has_ldegree : detail::sfinae_types
{
    PIRANHA_TT_CHECK(is_key, Key);
    template <typename T>
    static auto test1(const T *t) -> decltype(t->ldegree(std::declval<const symbol_set &>()), void(), yes());
    static no test1(...);
    template <typename T>
    static auto test2(const T *t)
        -> decltype(t->ldegree(std::declval<const symbol_set::positions &>(), std::declval<const symbol_set &>()),
                    void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1((Key *)nullptr)), yes>::value
                              && std::is_same<decltype(test2((Key *)nullptr)), yes>::value;
};

template <typename Key>
const bool key_has_ldegree<Key>::value;

/// Type trait to detect if a key type has a trigonometric degree property.
/**
 * The type trait has the same meaning as piranha::has_t_degree, but it's meant for use with key types.
 * It will test the presence of two <tt>t_degree()</tt> const methods, the first one accepting a const instance of
 * piranha::symbol_set, the second one a const instance of piranha::symbol_set::positions and a const instance of
 * piranha::symbol_set.
 *
 * \p Key must satisfy piranha::is_key.
 */
template <typename Key>
class key_has_t_degree : detail::sfinae_types
{
    PIRANHA_TT_CHECK(is_key, Key);
    template <typename T>
    static auto test1(T const *t) -> decltype(t->t_degree(std::declval<const symbol_set &>()), void(), yes());
    static no test1(...);
    template <typename T>
    static auto test2(T const *t)
        -> decltype(t->t_degree(std::declval<const symbol_set::positions &>(), std::declval<const symbol_set &>()),
                    void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1((Key const *)nullptr)), yes>::value
                              && std::is_same<decltype(test2((Key const *)nullptr)), yes>::value;
};

// Static init.
template <typename T>
const bool key_has_t_degree<T>::value;

/// Type trait to detect if a key type has a trigonometric low degree property.
/**
 * The type trait has the same meaning as piranha::has_t_ldegree, but it's meant for use with key types.
 * It will test the presence of two <tt>t_ldegree()</tt> const methods, the first one accepting a const instance of
 * piranha::symbol_set, the second one a const instance of piranha::symbol_set::positions and a const instance of
 * piranha::symbol_set.
 *
 * \p Key must satisfy piranha::is_key.
 */
template <typename Key>
class key_has_t_ldegree : detail::sfinae_types
{
    PIRANHA_TT_CHECK(is_key, Key);
    template <typename T>
    static auto test1(T const *t) -> decltype(t->t_ldegree(std::declval<const symbol_set &>()), void(), yes());
    static no test1(...);
    template <typename T>
    static auto test2(T const *t)
        -> decltype(t->t_ldegree(std::declval<const symbol_set::positions &>(), std::declval<const symbol_set &>()),
                    void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1((Key const *)nullptr)), yes>::value
                              && std::is_same<decltype(test2((Key const *)nullptr)), yes>::value;
};

// Static init.
template <typename T>
const bool key_has_t_ldegree<T>::value;

/// Type trait to detect if a key type has a trigonometric order property.
/**
 * The type trait has the same meaning as piranha::has_t_order, but it's meant for use with key types.
 * It will test the presence of two <tt>t_order()</tt> const methods, the first one accepting a const instance of
 * piranha::symbol_set, the second one a const instance of piranha::symbol_set::positions and a const instance of
 * piranha::symbol_set.
 *
 * \p Key must satisfy piranha::is_key.
 */
template <typename Key>
class key_has_t_order : detail::sfinae_types
{
    PIRANHA_TT_CHECK(is_key, Key);
    template <typename T>
    static auto test1(T const *t) -> decltype(t->t_order(std::declval<const symbol_set &>()), void(), yes());
    static no test1(...);
    template <typename T>
    static auto test2(T const *t)
        -> decltype(t->t_order(std::declval<const symbol_set::positions &>(), std::declval<const symbol_set &>()),
                    void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1((Key const *)nullptr)), yes>::value
                              && std::is_same<decltype(test2((Key const *)nullptr)), yes>::value;
};

// Static init.
template <typename T>
const bool key_has_t_order<T>::value;

/// Type trait to detect if a key type has a trigonometric low order property.
/**
 * The type trait has the same meaning as piranha::has_t_lorder, but it's meant for use with key types.
 * It will test the presence of two <tt>t_lorder()</tt> const methods, the first one accepting a const instance of
 * piranha::symbol_set, the second one a const instance of piranha::symbol_set::positions and a const instance of
 * piranha::symbol_set.
 *
 * \p Key must satisfy piranha::is_key.
 */
template <typename Key>
class key_has_t_lorder : detail::sfinae_types
{
    PIRANHA_TT_CHECK(is_key, Key);
    template <typename T>
    static auto test1(T const *t) -> decltype(t->t_lorder(std::declval<const symbol_set &>()), void(), yes());
    static no test1(...);
    template <typename T>
    static auto test2(T const *t)
        -> decltype(t->t_lorder(std::declval<const symbol_set::positions &>(), std::declval<const symbol_set &>()),
                    void(), yes());
    static no test2(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test1((Key const *)nullptr)), yes>::value
                              && std::is_same<decltype(test2((Key const *)nullptr)), yes>::value;
};

// Static init.
template <typename T>
const bool key_has_t_lorder<T>::value;

/// Type trait to detect the presence of the piranha::math::is_unitary() function.
/**
 * The type trait will be \p true if piranha::math::is_unitary() can be successfully called on instances of \p T.
 */
template <typename T>
class has_is_unitary : detail::sfinae_types
{
    typedef typename std::decay<T>::type Td;
    template <typename T1>
    static auto test(const T1 &t) -> decltype(math::is_unitary(t), void(), yes());
    static no test(...);
    static const bool implementation_defined = std::is_same<decltype(test(std::declval<Td>())), yes>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

template <typename T>
const bool has_is_unitary<T>::value;

/// Type trait to detect the presence of the piranha::math::subs function.
/**
 * The type trait will be \p true if piranha::math::subs can be successfully called on instances
 * of type \p T, with an instance of type \p U as substitution argument.
 */
template <typename T, typename U>
class has_subs : detail::sfinae_types
{
    typedef typename std::decay<T>::type Td;
    typedef typename std::decay<U>::type Ud;
    template <typename T1, typename U1>
    static auto test(const T1 &t, const U1 &u)
        -> decltype(math::subs(t, std::declval<std::string const &>(), u), void(), yes());
    static no test(...);
    static const bool implementation_defined
        = std::is_same<decltype(test(std::declval<Td>(), std::declval<Ud>())), yes>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

// Static init.
template <typename T, typename U>
const bool has_subs<T, U>::value;

/// Type trait to detect the presence of the piranha::math::t_subs function.
/**
 * The type trait will be \p true if piranha::math::t_subs can be successfully called on instances
 * of type \p T, with instances of type \p U and \p V as substitution arguments.
 */
template <typename T, typename U, typename V>
class has_t_subs : detail::sfinae_types
{
    typedef typename std::decay<T>::type Td;
    typedef typename std::decay<U>::type Ud;
    typedef typename std::decay<V>::type Vd;
    template <typename T1, typename U1, typename V1>
    static auto test(const T1 &t, const U1 &u, const V1 &v)
        -> decltype(math::t_subs(t, std::declval<std::string const &>(), u, v), void(), yes());
    static no test(...);
    static const bool implementation_defined
        = std::is_same<decltype(test(std::declval<Td>(), std::declval<Ud>(), std::declval<Vd>())), yes>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

// Static init.
template <typename T, typename U, typename V>
const bool has_t_subs<T, U, V>::value;

/// Type trait to detect the presence of the trigonometric substitution method in keys.
/**
 * This type trait will be \p true if the decay type of \p Key provides a const method <tt>t_subs()</tt> accepting as
 * const parameters a string,
 * an instance of \p T, an instance of \p U and an instance of piranha::symbol_set. The return value of the method must
 * be an <tt>std::vector</tt>
 * of pairs in which the second type must be \p Key itself. The <tt>t_subs()</tt> represents the substitution of a
 * symbol with its cosine
 * and sine passed as instances of \p T and \p U respectively.
 *
 * The decay type of \p Key must satisfy piranha::is_key.
 */
template <typename Key, typename T, typename U>
class key_has_t_subs : detail::sfinae_types
{
    typedef typename std::decay<Key>::type Keyd;
    typedef typename std::decay<T>::type Td;
    typedef typename std::decay<U>::type Ud;
    PIRANHA_TT_CHECK(is_key, Keyd);
    template <typename Key1, typename T1, typename U1>
    static auto test(const Key1 &k, const T1 &t, const U1 &u)
        -> decltype(k.t_subs(std::declval<const std::string &>(), t, u, std::declval<const symbol_set &>()));
    static no test(...);
    template <typename T1>
    struct check_result_type {
        static const bool value = false;
    };
    template <typename Res>
    struct check_result_type<std::vector<std::pair<Res, Keyd>>> {
        static const bool value = true;
    };

public:
    /// Value of the type trait.
    static const bool value
        = check_result_type<decltype(test(std::declval<Keyd>(), std::declval<Td>(), std::declval<Ud>()))>::value;
};

// Static init.
template <typename Key, typename T, typename U>
const bool key_has_t_subs<Key, T, U>::value;

/// Type trait to detect the presence of the substitution method in keys.
/**
 * This type trait will be \p true if the decay type of \p Key provides a const method <tt>subs()</tt> accepting as
 * const parameters a string,
 * an instance of \p T and an instance of piranha::symbol_set. The return value of the method must be an
 * <tt>std::vector</tt>
 * of pairs in which the second type must be \p Key itself. The <tt>subs()</tt> method represents the substitution of a
 * symbol with
 * an instance of type \p T.
 *
 * The decay type of \p Key must satisfy piranha::is_key.
 */
template <typename Key, typename T>
class key_has_subs : detail::sfinae_types
{
    typedef typename std::decay<Key>::type Keyd;
    typedef typename std::decay<T>::type Td;
    PIRANHA_TT_CHECK(is_key, Keyd);
    template <typename Key1, typename T1>
    static auto test(const Key1 &k, const T1 &t)
        -> decltype(k.subs(std::declval<const std::string &>(), t, std::declval<const symbol_set &>()));
    static no test(...);
    template <typename T1>
    struct check_result_type {
        static const bool value = false;
    };
    template <typename Res>
    struct check_result_type<std::vector<std::pair<Res, Keyd>>> {
        static const bool value = true;
    };

public:
    /// Value of the type trait.
    static const bool value = check_result_type<decltype(test(std::declval<Keyd>(), std::declval<Td>()))>::value;
};

// Static init.
template <typename Key, typename T>
const bool key_has_subs<Key, T>::value;

/// Type trait to detect the availability of piranha::math::multiply_accumulate().
/**
 * This type trait will be \p true if piranha::math::multiply_accumulate() can be called with arguments of type \p T,
 * \p false otherwise.
 */
template <typename T>
class has_multiply_accumulate
{
    template <typename U>
    using multiply_accumulate_t = decltype(
        math::multiply_accumulate(std::declval<U &>(), std::declval<const U &>(), std::declval<const U &>()));
    static const bool implementation_defined = is_detected<multiply_accumulate_t, T>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

// Static init.
template <typename T>
const bool has_multiply_accumulate<T>::value;

/// Type trait to detect the availability of piranha::math::evaluate.
/**
 * This type trait will be \p true if piranha::math::evaluate can be called with arguments of type \p T and \p U,
 * \p false otherwise.
 */
template <typename T, typename U>
class is_evaluable : detail::sfinae_types
{
    template <typename T2, typename U2>
    static auto test(const T2 &t, const std::unordered_map<std::string, U2> &dict)
        -> decltype(math::evaluate(t, dict), void(), yes());
    static no test(...);
    static const bool implementation_defined
        = std::is_same<decltype(test(std::declval<T>(), std::declval<std::unordered_map<std::string, U>>())),
                       yes>::value;

public:
    /// Value of the type trait.
    static const bool value = implementation_defined;
};

template <typename T, typename U>
const bool is_evaluable<T, U>::value;

/// Type trait to detect evaluable keys.
/**
 * This type trait will be \p true if \p Key is a key type providing a const method <tt>evaluate()</tt> taking a const
 * instance of
 * piranha::symbol_set::positions_map of \p T and a const instance of piranha::symbol_set as input, \p false otherwise.
 * If \p Key does not satisfy piranha::is_key, a compilation error will be produced.
 *
 * The decay type of \p Key is considered in this type trait.
 */
template <typename Key, typename T>
class key_is_evaluable : detail::sfinae_types
{
    typedef typename std::decay<Key>::type Keyd;
    PIRANHA_TT_CHECK(is_key, Keyd);
    template <typename Key1, typename T1>
    static auto test(const Key1 &k, const symbol_set::positions_map<T1> &pmap)
        -> decltype(k.evaluate(pmap, std::declval<const symbol_set &>()), void(), yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value
        = std::is_same<decltype(test(std::declval<Keyd>(), std::declval<symbol_set::positions_map<T>>())), yes>::value;
};

template <typename Key, typename T>
const bool key_is_evaluable<Key, T>::value;

/// Type trait to detect piranha::math::sin().
/**
 * The type trait will be \p true if piranha::math::sin() can be used on instances of type \p T,
 * \p false otherwise.
 */
template <typename T>
class has_sine : detail::sfinae_types
{
    template <typename T1>
    static auto test(const T1 &x) -> decltype(math::sin(x), void(), yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<T>())), yes>::value;
};

template <typename T>
const bool has_sine<T>::value;

/// Type trait to detect piranha::math::cos().
/**
 * The type trait will be \p true if piranha::math::cos() can be used on instances of type \p T,
 * \p false otherwise.
 */
template <typename T>
class has_cosine : detail::sfinae_types
{
    template <typename T1>
    static auto test(const T1 &x) -> decltype(math::cos(x), void(), yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<T>())), yes>::value;
};

template <typename T>
const bool has_cosine<T>::value;

/// Detect piranha::math::transformation_is_canonical().
/**
 * The type trait will be \p true if piranha::math::transformation_is_canonical() can be used on instances of type \p T,
 * \p false otherwise.
 */
template <typename T>
class has_transformation_is_canonical : detail::sfinae_types
{
    using v_string = std::vector<std::string>;
    template <typename T1>
    static auto test(const T1 &) -> decltype(math::transformation_is_canonical(std::declval<std::vector<T1> const &>(),
                                                                               std::declval<std::vector<T1> const &>(),
                                                                               std::declval<v_string const &>(),
                                                                               std::declval<v_string const &>()),
                                             void(), yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<T>())), yes>::value;
};

template <typename T>
const bool has_transformation_is_canonical<T>::value;

namespace math
{

/// Default functor for the implementation of piranha::math::add3().
/**
 * This functor should be specialised via the \p std::enable_if mechanism.
 */
template <typename T, typename Enable = void>
struct add3_impl {
    /// Call operator.
    /**
     * \note
     * This operator is enabled only if the expression <tt>a = b + c</tt> is well-formed.
     *
     * This operator will return the result of the expression <tt>a = b + c</tt>.
     *
     * @param a the return value.
     * @param b the first operand.
     * @param c the second operand.
     *
     * @return <tt>a = b + c</tt>.
     *
     * @throws unspecified any exception thrown by the invoked binary and/or assignment operators
     * of \p U.
     */
    template <typename U>
    auto operator()(U &a, const U &b, const U &c) const -> decltype(a = b + c)
    {
        return a = b + c;
    }
};

/// Specialisation of the piranha::math::add3() functor for integral types.
/**
 * This specialisation is activated when \p T is an integral type.
 */
template <typename T>
struct add3_impl<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    /// Call operator.
    /**
     * This operator will return the expression <tt>a = static_cast<T>(b + c)</tt>, with <tt>b + c</tt>
     * forcibly cast back to \p T in order to avoid compiler warnings with short integral types.
     *
     * @param a the return value.
     * @param b the first operand.
     * @param c the second operand.
     *
     * @return <tt>a = static_cast<T>(b + c)</tt>.
     */
    T &operator()(T &a, const T &b, const T &c) const
    {
        return a = static_cast<T>(b + c);
    }
};

/// Ternary addition.
/**
 * This function will set \p a to <tt>b + c</tt>. The actual implementation of this function is in the
 * piranha::math::add3_impl functor's
 * call operator.
 *
 * @param a the return value.
 * @param b the first operand.
 * @param c the second operand.
 *
 * @return the value returned by the call operator of piranha::math::add3_impl.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::add3_impl functor.
 */
template <typename T>
inline auto add3(T &a, const T &b, const T &c) -> decltype(add3_impl<T>()(a, b, c))
{
    return add3_impl<T>()(a, b, c);
}

/// Default functor for the implementation of piranha::math::sub3().
/**
 * This functor should be specialised via the \p std::enable_if mechanism.
 */
template <typename T, typename Enable = void>
struct sub3_impl {
    /// Call operator.
    /**
     * \note
     * This operator is enabled only if the expression <tt>a = b - c</tt> is well-formed.
     *
     * This operator will return the result of the expression <tt>a = b - c</tt>.
     *
     * @param a the return value.
     * @param b the first operand.
     * @param c the second operand.
     *
     * @return <tt>a = b - c</tt>.
     *
     * @throws unspecified any exception thrown by the invoked binary and/or assignment operators
     * of \p U.
     */
    template <typename U>
    auto operator()(U &a, const U &b, const U &c) const -> decltype(a = b - c)
    {
        return a = b - c;
    }
};

/// Specialisation of the piranha::math::sub3() functor for integral types.
/**
 * This specialisation is activated when \p T is an integral type.
 */
template <typename T>
struct sub3_impl<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    /// Call operator.
    /**
     * This operator will return the expression <tt>a = static_cast<T>(b - c)</tt>, with <tt>b - c</tt>
     * forcibly cast back to \p T in order to avoid compiler warnings with short integral types.
     *
     * @param a the return value.
     * @param b the first operand.
     * @param c the second operand.
     *
     * @return <tt>a = static_cast<T>(b - c)</tt>.
     */
    T &operator()(T &a, const T &b, const T &c) const
    {
        return a = static_cast<T>(b - c);
    }
};

/// Ternary subtraction.
/**
 * This function will set \p a to <tt>b - c</tt>. The actual implementation of this function is in the
 * piranha::math::sub3_impl functor's
 * call operator.
 *
 * @param a the return value.
 * @param b the first operand.
 * @param c the second operand.
 *
 * @return the value returned by the call operator of piranha::math::sub3_impl.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::sub3_impl functor.
 */
template <typename T>
inline auto sub3(T &a, const T &b, const T &c) -> decltype(sub3_impl<T>()(a, b, c))
{
    return sub3_impl<T>()(a, b, c);
}

/// Default functor for the implementation of piranha::math::mul3().
/**
 * This functor should be specialised via the \p std::enable_if mechanism.
 */
template <typename T, typename Enable = void>
struct mul3_impl {
    /// Call operator.
    /**
     * \note
     * This operator is enabled only if the expression <tt>a = b * c</tt> is well-formed.
     *
     * This operator will return the result of the expression <tt>a = b * c</tt>.
     *
     * @param a the return value.
     * @param b the first operand.
     * @param c the second operand.
     *
     * @return <tt>a = b * c</tt>.
     *
     * @throws unspecified any exception thrown by the invoked binary and/or assignment operators
     * of \p U.
     */
    template <typename U>
    auto operator()(U &a, const U &b, const U &c) const -> decltype(a = b * c)
    {
        return a = b * c;
    }
};

/// Specialisation of the piranha::math::mul3() functor for integral types.
/**
 * This specialisation is activated when \p T is an integral type.
 */
template <typename T>
struct mul3_impl<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    /// Call operator.
    /**
     * This operator will return the expression <tt>a = static_cast<T>(b * c)</tt>, with <tt>b * c</tt>
     * forcibly cast back to \p T in order to avoid compiler warnings with short integral types.
     *
     * @param a the return value.
     * @param b the first operand.
     * @param c the second operand.
     *
     * @return <tt>a = static_cast<T>(b * c)</tt>.
     */
    T &operator()(T &a, const T &b, const T &c) const
    {
        return a = static_cast<T>(b * c);
    }
};

/// Ternary multiplication.
/**
 * This function will set \p a to <tt>b * c</tt>. The actual implementation of this function is in the
 * piranha::math::mul3_impl functor's
 * call operator.
 *
 * @param a the return value.
 * @param b the first operand.
 * @param c the second operand.
 *
 * @return the value returned by the call operator of piranha::math::mul3_impl.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::mul3_impl functor.
 */
template <typename T>
inline auto mul3(T &a, const T &b, const T &c) -> decltype(mul3_impl<T>()(a, b, c))
{
    return mul3_impl<T>()(a, b, c);
}

/// Default functor for the implementation of piranha::math::div3().
/**
 * This functor should be specialised via the \p std::enable_if mechanism.
 */
template <typename T, typename Enable = void>
struct div3_impl {
    /// Call operator.
    /**
     * \note
     * This operator is enabled only if the expression <tt>a = b / c</tt> is well-formed.
     *
     * This operator will return the result of the expression <tt>a = b / c</tt>.
     *
     * @param a the return value.
     * @param b the first operand.
     * @param c the second operand.
     *
     * @return <tt>a = b / c</tt>.
     *
     * @throws unspecified any exception thrown by the invoked binary and/or assignment operators
     * of \p U.
     */
    template <typename U>
    auto operator()(U &a, const U &b, const U &c) const -> decltype(a = b / c)
    {
        return a = b / c;
    }
};

/// Specialisation of the piranha::math::div3() functor for integral types.
/**
 * This specialisation is activated when \p T is an integral type.
 */
template <typename T>
struct div3_impl<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    /// Call operator.
    /**
     * This operator will return the expression <tt>a = static_cast<T>(b / c)</tt>, with <tt>b / c</tt>
     * forcibly cast back to \p T in order to avoid compiler warnings with short integral types.
     *
     * @param a the return value.
     * @param b the first operand.
     * @param c the second operand.
     *
     * @return <tt>a = static_cast<T>(b / c)</tt>.
     */
    T &operator()(T &a, const T &b, const T &c) const
    {
        return a = static_cast<T>(b / c);
    }
};

/// Ternary division.
/**
 * This function will set \p a to <tt>b / c</tt>. The actual implementation of this function is in the
 * piranha::math::div3_impl functor's
 * call operator.
 *
 * @param a the return value.
 * @param b the first operand.
 * @param c the second operand.
 *
 * @return the value returned by the call operator of piranha::math::div3_impl.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::div3_impl functor.
 */
template <typename T>
inline auto div3(T &a, const T &b, const T &c) -> decltype(div3_impl<T>()(a, b, c))
{
    return div3_impl<T>()(a, b, c);
}

/// Exception to signal an inexact division.
struct inexact_division final : std::invalid_argument {
    /// Default constructor.
    explicit inexact_division() : std::invalid_argument("inexact division") {}
};

/// Default functor for the implementation of piranha::math::divexact().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename = void>
struct divexact_impl {
};

/// Exact division.
/**
 * This method will write into \p a the exact result of <tt>b / c</tt>. The actual implementation of this function is in
 * the piranha::math::divexact_impl functor's call operator. The implementation should throw an instance of
 * piranha::math::inexact_division if the division is not exact.
 *
 * @param a the return value.
 * @param b the first operand.
 * @param c the second operand.
 *
 * @return the value returned by the call operator of piranha::math::divexact_impl.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::divexact_impl functor.
 */
template <typename T>
inline auto divexact(T &a, const T &b, const T &c) -> decltype(divexact_impl<T>()(a, b, c))
{
    return divexact_impl<T>()(a, b, c);
}
} // namespace math

namespace detail
{

// Greatest common divisor using the euclidean algorithm.
// NOTE: this can yield negative values, depending on the signs
// of a and b. Supports C++ integrals and mp_integer.
// NOTE: using this with C++ integrals unchecked on ranges can result in undefined
// behaviour.
template <typename T>
inline T gcd_euclidean(T a, T b)
{
    while (true) {
        if (math::is_zero(a)) {
            return b;
        }
        b %= a;
        if (math::is_zero(b)) {
            return a;
        }
        a %= b;
    }
}
} // namespace detail

namespace math
{

/// Default functor for the implementation of piranha::math::gcd().
/**
 * This functor should be specialised via the \p std::enable_if mechanism. Default implementation will not define
 * the call operator, and will hence result in a compilation error when used.
 */
template <typename T, typename U, typename = void>
struct gcd_impl {
};

/// Implementation of piranha::math::gcd() for integral types.
/**
 * This specialisation is enabled when \p T and \p U are C++ integral types.
 */
template <typename T, typename U>
struct gcd_impl<T, U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value>::type> {
    /// The promoted type of T and U.
    using p_type = decltype(std::declval<const T &>() + std::declval<const U &>());
    /// Call operator.
    /**
     * The GCD will be computed via the euclidean algorithm. No overflow check is performed during
     * the computation.
     *
     * @param a the first operand.
     * @param b the second operand.
     *
     * @return the GCD of \p a and \p b.
     */
    p_type operator()(const T &a, const U &b) const
    {
        return detail::gcd_euclidean(static_cast<p_type>(a), static_cast<p_type>(b));
    }
};

/// GCD.
/**
 * This function will return the GCD of \p a and \p b. The actual implementation of this function is in the
 * piranha::math::gcd_impl functor's
 * call operator.
 *
 * @param a the first operand.
 * @param b the second operand.
 *
 * @return the value returned by the call operator of piranha::math::gcd_impl.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::gcd_impl functor.
 */
template <typename T, typename U>
inline auto gcd(const T &a, const U &b) -> decltype(gcd_impl<T, U>()(a, b))
{
    return gcd_impl<T, U>()(a, b);
}

/// Default functor for the implementation of piranha::math::gcd3().
/**
 * This functor should be specialised via the \p std::enable_if mechanism.
 */
template <typename T, typename = void>
struct gcd3_impl {
    /// Call operator.
    /**
     * \note
     * This operator is enabled only if the expression <tt>out = math::gcd(a,b)</tt> is well-formed.
     *
     * @param out the output value.
     * @param a the first operand.
     * @param b the second operand.
     *
     * @return <tt>out = math::gcd(a,b)</tt>.
     *
     * @throws unspecified any exception thrown by piranha::math::gcd() or the invoked
     * assignment operator.
     */
    template <typename T1>
    auto operator()(T1 &out, const T1 &a, const T1 &b) const -> decltype(out = math::gcd(a, b))
    {
        return out = math::gcd(a, b);
    }
};

/// Specialisation of the piranha::math::gcd3() functor for integral types.
/**
 * This specialisation is enabled when \p T is a C++ integral type.
 */
template <typename T>
struct gcd3_impl<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    /// Call operator.
    /**
     * This call operator will forcibly cast back to \p T the result of piranha::math::gcd().
     *
     * @param out the output value.
     * @param a the first operand.
     * @param b the second operand.
     *
     * @return <tt>out = static_cast<T>(math::gcd(a,b))</tt>.
     */
    T &operator()(T &out, const T &a, const T &b) const
    {
        return out = static_cast<T>(math::gcd(a, b));
    }
};

/// Ternary GCD.
/**
 * This function will write the GCD of \p a and \p b into \p out. The actual implementation of this function is in the
 * piranha::math::gcd3_impl functor's
 * call operator.
 *
 * @param out the output value.
 * @param a the first operand.
 * @param b the second operand.
 *
 * @return the value returned by the call operator of piranha::math::gcd3_impl.
 *
 * @throws unspecified any exception thrown by the call operator of the piranha::math::gcd3_impl functor.
 */
template <typename T>
inline auto gcd3(T &out, const T &a, const T &b) -> decltype(gcd3_impl<T>()(out, a, b))
{
    return gcd3_impl<T>()(out, a, b);
}
} // namespace math

/// Detect piranha::math::add3().
/**
 * The type trait will be \p true if piranha::math::add3() can be used on instances of the decay type of \p T,
 * \p false otherwise.
 */
template <typename T>
class has_add3 : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    template <typename T1>
    static auto test(const T1 &)
        -> decltype(math::add3(std::declval<T1 &>(), std::declval<const T1 &>(), std::declval<const T1 &>()), void(),
                    yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<Td>())), yes>::value;
};

template <typename T>
const bool has_add3<T>::value;

/// Detect piranha::math::sub3().
/**
 * The type trait will be \p true if piranha::math::sub3() can be used on instances of the decay type of \p T,
 * \p false otherwise.
 */
template <typename T>
class has_sub3 : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    template <typename T1>
    static auto test(const T1 &)
        -> decltype(math::sub3(std::declval<T1 &>(), std::declval<const T1 &>(), std::declval<const T1 &>()), void(),
                    yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<Td>())), yes>::value;
};

template <typename T>
const bool has_sub3<T>::value;

/// Detect piranha::math::mul3().
/**
 * The type trait will be \p true if piranha::math::mul3() can be used on instances of the decay type of \p T,
 * \p false otherwise.
 */
template <typename T>
class has_mul3 : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    template <typename T1>
    static auto test(const T1 &)
        -> decltype(math::mul3(std::declval<T1 &>(), std::declval<const T1 &>(), std::declval<const T1 &>()), void(),
                    yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<Td>())), yes>::value;
};

template <typename T>
const bool has_mul3<T>::value;

/// Detect piranha::math::div3().
/**
 * The type trait will be \p true if piranha::math::div3() can be used on instances of the decay type of \p T,
 * \p false otherwise.
 */
template <typename T>
class has_div3 : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    template <typename T1>
    static auto test(const T1 &)
        -> decltype(math::div3(std::declval<T1 &>(), std::declval<const T1 &>(), std::declval<const T1 &>()), void(),
                    yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<Td>())), yes>::value;
};

template <typename T>
const bool has_div3<T>::value;

/// Detect piranha::math::gcd().
/**
 * The type trait will be \p true if piranha::math::gcd() can be used on instances of the decay types of \p T and \p U,
 * \p false otherwise.
 */
template <typename T, typename U = T>
class has_gcd : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    using Ud = typename std::decay<U>::type;
    template <typename T1, typename U1>
    static auto test(const T1 &a, const U1 &b) -> decltype(math::gcd(a, b), void(), yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<Td>(), std::declval<Ud>())), yes>::value;
};

template <typename T, typename U>
const bool has_gcd<T, U>::value;

/// Detect piranha::math::gcd3().
/**
 * The type trait will be \p true if piranha::math::gcd3() can be used on instances of the decay type of \p T,
 * \p false otherwise.
 */
template <typename T>
class has_gcd3 : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    template <typename T1>
    static auto test(const T1 &)
        -> decltype(math::gcd3(std::declval<T1 &>(), std::declval<const T1 &>(), std::declval<const T1 &>()), void(),
                    yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<Td>())), yes>::value;
};

template <typename T>
const bool has_gcd3<T>::value;

/// Detect piranha::math::divexact().
/**
 * The type trait will be \p true if piranha::math::divexact() can be used on instances of the decay type of \p T,
 * \p false otherwise.
 */
template <typename T>
class has_exact_division : detail::sfinae_types
{
    using Td = typename std::decay<T>::type;
    template <typename T1>
    static auto test(const T1 &)
        -> decltype(math::divexact(std::declval<T1 &>(), std::declval<const T1 &>(), std::declval<const T1 &>()),
                    void(), yes());
    static no test(...);

public:
    /// Value of the type trait.
    static const bool value = std::is_same<decltype(test(std::declval<Td>())), yes>::value;
};

template <typename T>
const bool has_exact_division<T>::value;
} // namespace piranha

#endif

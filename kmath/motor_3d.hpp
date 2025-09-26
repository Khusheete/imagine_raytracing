// Copyright © 2025 Souchet Ferdinand (aka. Khusheete)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#pragma once


#include "euclidian_flat_3d.hpp"
#include "utils.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "rotor_3d.hpp"

#include <cmath>


namespace kmath {

  template<Number T>
  struct _Motor3 {
    T s, e23, e31, e12, e0123, e01, e02, e03;

  public:
    _Motor3(): _Motor3(IDENTITY) {}
    _Motor3(const T s, const T e23, const T e31, const T e12, const T e0123, const T e01, const T e02, const T e03): s(s), e23(e23), e31(e31), e12(e12), e0123(e0123), e01(e01), e02(e02), e03(e03) {}
    _Motor3(const _Rotor3<T> &real, const _Rotor3<T> &dual): s(real.s), e23(real.e23), e31(real.e31), e12(real.e12), e0123(dual.s), e01(dual.e23), e02(dual.e31), e03(dual.e12) {}


    static inline _Motor3<T> from_axis_angle(const _Vec3<T> &axis, const T angle) {
      return _Motor3<T>(
        _Rotor3<T>::from_axis_angle(axis, angle),
        _Rotor3<T>::ZERO
      );
    }
    

    static inline _Motor3<T> from_translation(const _Vec3<T> &translation) {
      return _Motor3<T>(
        _Rotor3<T>::IDENTITY,
        _Rotor3<T>((T)0.0, -((T)0.5) * translation)
      );
    }


    static inline _Motor3<T> from_rotor(const _Rotor3<T> &rotation) {
      return _Motor3<T>(rotation, _Rotor3<T>::ZERO);
    }


    static inline _Motor3<T> from_rotor_translation(const _Rotor3<T> &rotation, const _Vec3<T> &translation) {
      _Rotor3<T> trans((T)0.0, (T)0.5 * translation);
      // If we call I the pseudoscalar e0123:
      // M = Motor_translation * Motor_rotation
      //   = (1 + I trans) * rotation
      //   = rotation + I trans * rotation
      //
      // The grade 2 elements of trans * rotation have a sign flip and the scalar part does not.
      return _Motor3<T>(rotation, reverse(trans * rotation));
    }


    static _Motor3<T> from_axis_angle_translation(const _Vec3<T> &axis, const T angle, const _Vec3<T> &translation) {
      _Rotor3<T> rot = _Rotor3<T>::from_axis_angle(axis, angle);
      _Rotor3<T> trans((T)0.0, (T)0.5 * translation);
      return _Motor3<T>(rot, reverse(trans * rot));
    }


    static _Motor3<T> from_screw_coordinates(const _Vec3<T> &direction, const _Vec3<T> &moment, const T angle, const T translation) {
      if (!is_approx_zero(angle)) {
        T cos_a = std::cos(angle / 2.0);
        T sin_a = std::sin(angle / 2.0);
        return _Motor3<T>(
          _Rotor3<T>(cos_a, sin_a * direction),
          _Rotor3<T>((T)-0.5 * translation * sin_a, sin_a * moment + (T)(0.5 * translation * cos_a) * direction)
        );
      } else {
        return _Motor3<T>::from_translation(translation * direction);
      }
    }

  public:
    static const _Motor3<T> ZERO;
    static const _Motor3<T> IDENTITY;
  };


  template<Number T>
  const _Motor3<T> _Motor3<T>::ZERO = _Motor3<T>(_Rotor3<T>::ZERO, _Rotor3<T>::ZERO);
  template<Number T>
  const _Motor3<T> _Motor3<T>::IDENTITY = _Motor3<T>(_Rotor3<T>::IDENTITY, _Rotor3<T>::ZERO);


  // ===========================
  // = Motor specific function =
  // ===========================


  template<Number T>
  inline const _Rotor3<T> &get_real_part(const _Motor3<T> &m) {
    return *reinterpret_cast<const _Rotor3<T>*>(&m);
  }


  template<Number T>
  inline const _Rotor3<T> &get_dual_part(const _Motor3<T> &m) {
    return *(1 + reinterpret_cast<const _Rotor3<T>*>(&m));
  }


  template<Number T>
  inline _Rotor3<T> get_rotor(const _Motor3<T> &m) {
    return get_real_part(m);
  }


  template<Number T>
  inline _Vec3<T> get_translation(const _Motor3<T> &m) {
    const _Rotor3<T> &real = get_real_part(m);
    const _Rotor3<T> &dual = get_dual_part(m);
    _Rotor3<T> translation = (T)2.0 * reverse(dual) * reverse(real); // See _Motor3<T>::from_rotor_translation
    // Don't negate the vanishing part, as it was already negated by not doing -reverse(dual)
    return _Vec3<T>(translation.e23, translation.e31, translation.e12);
  }


  // A motor is simple when its grade 4 part is null
  template<Number T>
  inline bool is_simple(const _Motor3<T> &m) {
    return is_approx_zero(m.e0132);
  }


  // The fast square root returns the motor that does half the transformation as `m` modulo a positive factor
  template<Number T>
  inline _Motor3<T> fast_sqrt(const _Motor3<T> &m) {
    T scaling = (T)1.0 + m.s;
    T half_g4 = (T)0.5 * m.e0123;
    return _Motor3<T>(
      scaling * scaling,
      scaling * m.e23,
      scaling * m.e31,
      scaling * m.e12,
      scaling * m.e0123 - m.s * half_g4,
      scaling * m.e01 + m.e23 * half_g4,
      scaling * m.e02 + m.e31 * half_g4,
      scaling * m.e03 + m.e12 * half_g4
    );
  }


  template<Number T>
  _Motor3<T> sqrt(const _Motor3<T> &m) {
    if (m.s >= 0.0) {
      T num = (T)2.0 * ((T)1.0 + m.s);
      T g4 = m.e0123 / num;
      return _Motor3<T>(
        (T)1.0 + m.s,
        m.e23,
        m.e31,
        m.e12,
        m.e0123 - m.s * g4,
        m.e01 + m.e23 * g4,
        m.e02 + m.e31 * g4,
        m.e03 + m.e12 * g4
      ) / std::sqrt(num);
    } else {
      T num = (T)2.0 * ((T)1.0 - m.s);
      T g4 = m.e0123 / num;
      return _Motor3<T>(
        (T)1.0 - m.s,
        -m.e23,
        -m.e31,
        -m.e12,
        -m.e0123 - m.s * g4,
        -m.e01 + m.e23 * g4,
        -m.e02 + m.e31 * g4,
        -m.e03 + m.e12 * g4
      ) / std::sqrt(num);
    }
  }


  template<Number T>
  void to_screw_coordinates(const _Motor3<T> &m, _Vec3<T> &direction, _Vec3<T> &moment, T &angle, T &translation) {
    angle = 2.0 * std::acos(m.s);
    if (!is_approx_zero(angle)) {
      T inv_sin_a = std::sin(0.5 * angle);
      direction = inv_sin_a * _Vec3<T>(m.e23, m.e31, m.e12);
      translation = -2.0 * m.s * inv_sin_a;
      moment = inv_sin_a * _Vec3<T>(m.e01, m.e02, m.e03) - ((T)0.5) * translation * m.s * inv_sin_a * direction;
    } else {
      direction = get_translation(m);
      translation = length(direction);
      if (!is_approx_zero(translation)) { // Normalize translation direction
        direction /= translation;
      } else {
        direction = _Vec3<T>::ZERO;
      }
      moment = _Vec3<T>::INF;
    }
  }


  template<Number T>
  inline _Motor3<T> reverse(const _Motor3<T> &m) {
    return _Motor3<T>(
      m.s    , -m.e23, -m.e31, -m.e12,
      m.e0123, -m.e01, -m.e02, -m.e03
    );
  }


  template<Number T>
  inline T magnitude_squared(const _Motor3<T> &m) {
    return m.s * m.s + m.e23 * m.e23 + m.e31 * m.e31 + m.e12 * m.e12;
  }


  template<Number T>
  inline T magnitude(const _Motor3<T> &m) {
    return std::sqrt(magnitude_squared(m));
  }


  template<Number T>
  inline T vanishing_magnitude_squared(const _Motor3<T> &m) {
    return m.e0123 * m.e0123 + m.e01 * m.e01 + m.e02 * m.e02 + m.e03 * m.e03;
  }


  template<Number T>
  inline T vanishing_magnitude(const _Motor3<T> &m) {
    return std::sqrt(vanishing_magnitude_squared(m));
  }
  

  template<Number T>
  inline _Motor3<T> inverse(const _Motor3<T> &m) {
    return reverse(m) / magnitude_squared(m);
  }


  template<Number T>
  inline _Motor3<T> normalized(const _Motor3<T> &m) {
    return m / magnitude(m);
  }


  // Exponentiate a line/screw to create a motor to which it is invariant.
  template<Number T>
  _Motor3<T> exp(const _Line3<T> &b) {
    const T r = magnitude_squared(b);
    if (is_square_approx_zero(r)) {
      // When the bivector is ideal, the degree 2 and higher are null
      return _Motor3<T>(
        _Rotor3<T>::IDENTITY,
        _Rotor3<T>((T)0.0, b.e01, b.e02, b.e03)
      );
    }

    // A general bivector times it's reverse is a sturdy number:
    // 
    // S = b23^2 + b31^2 + b12^2 - 2 ( b23 * b01 + b31 * b02 + b12 * b03 ) e0123
    // 
    // We've already got the real part (r), and (half) the pseudo-scalar part is:
    const T ps = - b.e23 * b.e01 - b.e31 * b.e02 - b.e12 * b.e03;

    // This sturdy number allows for normalizing the bivector so that we can
    // express it as the sum of two commutating bivectors b = alpha l + beta lL.
    // With this, we'll have: exp(b) = exp(alpha L) exp(beta IL).
    //
    // Let's take the square root of S:
    //
    // S^0.5 = sqrt(r) + ps / sqrt(r) e0123
    //
    // (This formula is why we calculated half the pseudo-scalar part of S)
    //
    // Let's call the real part u and the pseudo-scalar part v:
    const T u = std::sqrt(r);
    const T v = ps / u;

    // And to normalize the bivector, we need the inverse square root of S:
    //
    // S^-0.5 = 1 / sqrt(r) - ps / r^(3/2) e0123
    const T inv_u = (T)1.0 / u;
    const T inv_v = -v / r;

    // Here is b normalized: l = sqrt(S) \ b
    const _Line3<T> l = _Line3<T>(
      inv_u * b.e23,
      inv_u * b.e31,
      inv_u * b.e12,
      inv_u * b.e01 - inv_v * b.e23,
      inv_u * b.e02 - inv_v * b.e31,
      inv_u * b.e03 - inv_v * b.e12
    );

    // Now, there's why we've done all this. We know that:
    //
    // b = S l = (u + vI) l = u l + v lI
    //
    // And l dot lI = 0 since lI is on the ideal plane. Therefore l and lI commute.
    // Moreover, l * reverse(l) = -1 since it has been normalized. We thus have:
    //
    // exp(b) = exp(ul) exp(vlI)
    //        = (cos u + sin u l) (1 + vlI)
    //        = cos u + sin u l + v cos u l I - v sin u I
    const T cosu = std::cos(u);
    const T sinu = std::sin(u);
    const T vcosu = v * cosu;

    return _Motor3<T>(
      cosu,
      sinu * l.e23,
      sinu * l.e31,
      sinu * l.e12,
      - v * sinu,
      sinu * l.e01 - vcosu * l.e23,
      sinu * l.e02 - vcosu * l.e31,
      sinu * l.e03 - vcosu * l.e12
    );
  }


  // Take the log map of m to create the corresponding screw from the Lie group.
  // Note that we don't have exp(log(m)) = m. But exp(log(m)) represents the same transformation as m.
  template<Number T>
  _Line3<T> log(const _Motor3<T> &m) {
    const T r = length(_Vec3<T>(m.e23, m.e31, m.e12));
    if (is_approx_zero(r)) {
      // When this motor is a pure translation, the line is a vanishing line that is easy to compute
      return _Line3<T>(
        (T)0.0,
        (T)0.0,
        (T)0.0,
        m.e01,
        m.e02,
        m.e03
      );
    }

    // Extract the norm of the bivector part of m. We'll normalize <m>2 in the same way
    // we normalized the bivector in the exponential. This decompositions gives the
    // logarithm almost directly, as it's exp will be this versor.
    const T ps = - m.e23 * m.e01 - m.e31 * m.e02 - m.e12 * m.e03;

    // S^0.5 = u + vI
    const T u = std::sqrt(r);
    const T v = ps / u;

    // S^(-0.5) = inv_u + inv_v I
    const T inv_u = (T)1.0 / u;
    const T inv_v = -v / r;

    // Writing m, we have:
    //
    // m = alpha exp(B)                        ; B is the bivector that we are searching for, let's say it's normalization gives: (a + bI)l
    //   = alpha ( cos a + sin a l + b cos a l I - b sin a I )
    //   = alpha cos a + alpha (sin a + b cos a I) l - alpha b sin a I
    //
    // And since the normalized part of <m>2 is invariant:
    // 
    // m = s + alpha <m>2 + m0123 I
    //   = s + (u + vI)l + m0123 I
    //
    // So by identification:
    //
    // s     = alpha cos a
    // m0123 = - alpha b sin a
    // u     = alpha sin a
    // v     = alpha b cos a
    //
    // Therefore (provided that it is defined):
    //
    // a = atan2(u, s) = &tan2(-m0123, v)
    // b = v / s = -m0123 / u
    //
    // Though m0123 and v might be both null (in which case, the result might be incorrect)

    // Here is <m>2 normalized: l = sqrt(S) \ m
    const _Line3<T> l = _Line3<T>(
      inv_u * m.e23,
      inv_u * m.e31,
      inv_u * m.e12,
      inv_u * m.e01 - inv_v * m.e23,
      inv_u * m.e02 - inv_v * m.e31,
      inv_u * m.e03 - inv_v * m.e12
    );

    const T a = std::atan2(u, m.s);
    const T b = (is_approx_zero(m.s))? -m.e0123 / u : v / m.s;

    return _Line3<T>(
      a * l.e23,
      a * l.e31,
      a * l.e12,
      a * l.e01 - b * l.e23,
      a * l.e02 - b * l.e31,
      a * l.e03 - b * l.e12
    );
  }


  template<Number T>
  inline _Motor3<T> pow(const _Motor3<T> &m, T power) {
    return exp(power * log(m));
  }


  template<Number T>
  inline _Mat4<T> as_transform(const _Motor3<T> &m) {
    _Rotor3<T> rotor = get_rotor(m);
    _Vec3<T> translation = get_translation(m);
    return as_transform(rotor, translation);
  }


  // ===================
  // = Motor operators =
  // ===================


  template<Number T>
  inline _Motor3<T> operator+(const _Motor3<T> &a, const _Motor3<T> &b) {
    _Motor3<T> r(a);
    r += b;
    return r;
  }


  template<Number T>
  inline _Motor3<T> &operator+=(_Motor3<T> &a, const _Motor3<T> &b) {
    a.s     += b.s;
    a.e23   += b.e23;
    a.e31   += b.e31;
    a.e12   += b.e12;
    a.e0123 += b.e0123;
    a.e01   += b.e01;
    a.e02   += b.e02;
    a.e03   += b.e03;
    return a;
  }


  template<Number T>
  inline _Motor3<T> operator-(const _Motor3<T> &a, const _Motor3<T> &b) {
    _Motor3<T> r(a);
    r -= b;
    return r;
  }


  template<Number T>
  inline _Motor3<T> &operator-=(_Motor3<T> &a, const _Motor3<T> &b) {
    a.s     -= b.s;
    a.e23   -= b.e23;
    a.e31   -= b.e31;
    a.e12   -= b.e12;
    a.e0123 -= b.e0123;
    a.e01   -= b.e01;
    a.e02   -= b.e02;
    a.e03   -= b.e03;
    return a;
  }


  template<Number T>
  inline _Motor3<T> operator-(const _Motor3<T> &a) {
    return _Motor3<T>(
      -a.s, -a.e23, -a.e31, -a.e12, -a.e0123, -a.e01, -a.e02, -a.e03
    );
  }


  template<Number T>
  inline _Motor3<T> operator*(const T a, const _Motor3<T> &b) {
    _Motor3<T> r(b);
    r *= a;
    return r;
  }


  template<Number T>
  inline _Motor3<T> operator*(const _Motor3<T> &b, const T a) {
    return a * b;
  }


  template<Number T>
  inline _Motor3<T> &operator*=(_Motor3<T> &a, const T b) {
    a.s     *= b;
    a.e23   *= b;
    a.e31   *= b;
    a.e12   *= b;
    a.e0123 *= b;
    a.e01   *= b;
    a.e02   *= b;
    a.e03   *= b;
    return a;
  }


  template<Number T>
  inline _Motor3<T> operator/(const _Motor3<T> &a, const T b) {
    _Motor3<T> r(a);
    r /= b;
    return r;
  }


  template<Number T>
  inline _Motor3<T> &operator/=(_Motor3<T> &a, const T b) {
    a.s     /= b;
    a.e23   /= b;
    a.e31   /= b;
    a.e12   /= b;
    a.e0123 /= b;
    a.e01   /= b;
    a.e02   /= b;
    a.e03   /= b;
    return a;
  }


  template<Number T>
  inline _Motor3<T> operator*(const _Motor3<T> &a, const _Motor3<T> &b) {
    return _Motor3<T>(
      b.s * a.s - b.e23 * a.e23 - a.e31 * b.e31 - b.e12 * a.e12,
      a.s * b.e23 + b.s * a.e23 - a.e31 * b.e12 + b.e31 * a.e12,
      a.s * b.e31 + b.s * a.e31 + a.e23 * b.e12 - b.e23 * a.e12,
      a.s * b.e12 + b.s * a.e12 - a.e23 * b.e31 + b.e23 * a.e31,

      a.e01 * b.e23 + a.e02 * b.e31 + a.e03 * b.e12 + b.e01 * a.e23 + b.e02 * a.e31 + b.e03 * a.e12 + a.s * b.e0123 + b.s * a.e0123,
      a.s * b.e01 + b.s * a.e01 + a.e03 * b.e31 - a.e02 * b.e12 + b.e02 * a.e12 - b.e03 * a.e31 - a.e23 * b.e0123 - b.e23 * a.e0123,
      a.s * b.e02 + b.s * a.e02 + a.e01 * b.e12 - a.e03 * b.e23 - b.e01 * a.e12 + b.e03 * a.e23 - a.e31 * b.e0123 - b.e31 * a.e0123,
      a.s * b.e03 + b.s * a.e03 - a.e01 * b.e31 + a.e02 * b.e23 + b.e01 * a.e31 - b.e02 * a.e23 - a.e12 * b.e0123 - b.e12 * a.e0123
    );
  }


  template<Number T>
  inline _Motor3<T> &operator*=(_Motor3<T> &a, const _Motor3<T> &b) {
    a = a * b;
    return a;
  }
  

  // ===========================
  // = Interpolation functions =
  // ===========================


  template<Number T>
  _Motor3<T> seplerp(const _Motor3<T> &a, const _Motor3<T> &b, const T t) {
    _Vec3<T> a_trans = get_translation(a);
    _Vec3<T> b_trans = get_translation(b);
    _Rotor3<T> a_rot = get_rotor(a);
    _Rotor3<T> b_rot = get_rotor(b);
    return _Motor3<T>::from_rotor_translation(slerp(a_rot, b_rot, t), lerp(a_trans, b_trans, t));
  }


  template<Number T>
  _Motor3<T> sclerp(const _Motor3<T> &a, const _Motor3<T> &b, const T t) {
    _Motor3<T> delta = reverse(a) * b;
    _Vec3<T> direction, moment;
    T angle, translation;
    to_screw_coordinates(delta, direction, moment, angle, translation);
    return a * _Motor3<T>::from_screw_coordinates(direction, moment, t * angle, t * translation);
  }


  template<Number T>
  _Motor3<T> lielerp(const _Motor3<T> &a, const _Motor3<T> &b, const T t) {
    return a * pow(reverse(a) * b, t);
  }


  template<Number T>
  _Motor3<T> kenlerp(const _Motor3<T> &a, const _Motor3<T> &b, const T t, const T beta) {
    _Motor3<T> sc_res = sclerp(a, b, t);
    _Rotor3<T> sc_rot = get_rotor(sc_res);
    _Vec3<T> sc_trans = get_translation(sc_res);
    _Motor3<T> sep_res = seplerp(a, b, t);
    _Rotor3<T> sep_rot = get_rotor(sep_res);
    _Vec3<T> sep_trans = get_translation(sep_res);
    return _Motor3<T>::from_rotor_translation(slerp(sc_rot, sep_rot, beta), lerp(sc_trans, sep_trans, beta));
  }
  

  // ===================
  // = Transformations =
  // ===================


  template<Number T>
  _Plane3<T> transform(const _Plane3<T> &a, const _Motor3<T> &m) {
    return _Plane3<T>(
      - (T)2.0 * a.e1 * m.e02 * m.e12 - (T)2.0 * a.e2 * m.e03 * m.e23 - (T)2.0 * a.e3 * m.e01 * m.e31 + a.e0 * m.s * m.s + a.e0 * m.e23 * m.e23 + a.e0 * m.e31 * m.e31 + a.e0 * m.e12 * m.e12 + (T)2.0 * a.e1 * m.s * m.e01 + (T)2.0 * a.e2 * m.s * m.e02 + (T)2.0 * a.e3 * m.s * m.e03 + (T)2.0 * a.e1 * m.e0123 * m.e23 + (T)2.0 * a.e2 * m.e0123 * m.e31 + (T)2.0 * a.e3 * m.e0123 * m.e12 + (T)2.0 * a.e1 * m.e03 * m.e31 + (T)2.0 * a.e2 * m.e12 * m.e01 + (T)2.0 * a.e3 * m.e02 * m.e23,
      - a.e1 * m.e12 * m.e12 - a.e1 * m.e31 * m.e31 + a.e1 * m.s * m.s + a.e1 * m.e23 * m.e23 + (T)2.0 * a.e2 * m.e12 * m.s + (T)2.0 * a.e2 * m.e23 * m.e31 - (T)2.0 * a.e3 * m.s * m.e31 + (T)2.0 * a.e3 * m.e12 * m.e23,
      - a.e2 * m.e23 * m.e23 - a.e2 * m.e12 * m.e12 + a.e2 * m.s * m.s + a.e2 * m.e31 * m.e31 + (T)2.0 * a.e3 * m.s * m.e23 + (T)2.0 * a.e3 * m.e12 * m.e31 - (T)2.0 * a.e1 * m.s * m.e12 + (T)2.0 * a.e1 * m.e23 * m.e31,
      - a.e3 * m.e23 * m.e23 - a.e3 * m.e31 * m.e31 + a.e3 * m.s * m.s + a.e3 * m.e12 * m.e12 + (T)2.0 * a.e1 * m.s * m.e31 + (T)2.0 * a.e1 * m.e12 * m.e23 - (T)2.0 * a.e2 * m.s * m.e23 + (T)2.0 * a.e2 * m.e12 * m.e31
    );
  }
  

  template<Number T>
  _Line3<T> transform(const _Line3<T> &a, const _Motor3<T> &m) {
    return _Line3<T>(
      - a.e23 * m.e31 * m.e31 - a.e23 * m.e12 * m.e12 + a.e23 * m.e23 * m.e23 + a.e23 * m.s * m.s + (T)2.0 * a.e31 * m.s * m.e12 - (T)2.0 * a.e12 * m.s * m.e31 + (T)2.0 * a.e31 * m.e23 * m.e31 + (T)2.0 * a.e12 * m.e12 * m.e23,
      - a.e31 * m.e23 * m.e23 - m.e12 * m.e12 * a.e31 + a.e31 * m.e31 * m.e31 + m.s * m.s * a.e31 - (T)2.0 * a.e23 * m.s * m.e12 + (T)2.0 * a.e12 * m.s * m.e23 + (T)2.0 * a.e12 * m.e12 * m.e31 + (T)2.0 * a.e23 * m.e23 * m.e31,
      - a.e12 * m.e23 * m.e23 - a.e12 * m.e31 * m.e31 + a.e12 * m.e12 * m.e12 + a.e12 * m.s * m.s + (T)2.0 * a.e23 * m.s * m.e31 - (T)2.0 * a.e31 * m.s * m.e23 + (T)2.0 * a.e23 * m.e12 * m.e23 + (T)2.0 * a.e31 * m.e12 * m.e31,
      - a.e01 * m.e31 * m.e31 - a.e01 * m.e12 * m.e12 + a.e01 * m.e23 * m.e23 + a.e01 * m.s * m.s - (T)2.0 * a.e12 * m.s * m.e02 - (T)2.0 * a.e03 * m.s * m.e31 - (T)2.0 * a.e23 * m.s * m.e0123 - (T)2.0 * a.e23 * m.e31 * m.e02 - (T)2.0 * a.e23 * m.e12 * m.e03 - (T)2.0 * a.e31 * m.e12 * m.e0123 + (T)2.0 * a.e31 * m.s * m.e03 + (T)2.0 * a.e02 * m.s * m.e12 + (T)2.0 * a.e03 * m.e12 * m.e23 + (T)2.0 * a.e02 * m.e23 * m.e31 + (T)2.0 * a.e31 * m.e01 * m.e31 + (T)2.0 * a.e31 * m.e23 * m.e02 + (T)2.0 * a.e12 * m.e03 * m.e23 + (T)2.0 * a.e12 * m.e12 * m.e01 + (T)2.0 * a.e23 * m.e23 * m.e01 + (T)2.0 * a.e12 * m.e0123 * m.e31,
      - a.e02 * m.e23 * m.e23 - a.e02 * m.e12 * m.e12 + a.e02 * m.e31 * m.e31 + a.e02 * m.s * m.s + (T)2.0 * a.e12 * m.s * m.e01 - (T)2.0 * a.e01 * m.s * m.e12 - (T)2.0 * a.e23 * m.s * m.e03 - (T)2.0 * a.e31 * m.e23 * m.e01 - (T)2.0 * a.e31 * m.e12 * m.e03 - (T)2.0 * a.e31 * m.s * m.e0123 - (T)2.0 * a.e12 * m.e0123 * m.e23 + (T)2.0 * a.e03 * m.s * m.e23 + (T)2.0 * a.e12 * m.e12 * m.e02 + (T)2.0 * a.e23 * m.e01 * m.e31 + (T)2.0 * a.e31 * m.e31 * m.e02 + (T)2.0 * a.e03 * m.e12 * m.e31 + (T)2.0 * a.e23 * m.e23 * m.e02 + (T)2.0 * a.e01 * m.e23 * m.e31 + (T)2.0 * a.e12 * m.e03 * m.e31 + (T)2.0 * a.e23 * m.e12 * m.e0123,
      - a.e03 * m.e23 * m.e23 - a.e03 * m.e31 * m.e31 + a.e03 * m.e12 * m.e12 + a.e03 * m.s * m.s - (T)2.0 * a.e02 * m.s * m.e23 - (T)2.0 * a.e31 * m.s * m.e01 + (T)2.0 * a.e23 * m.s * m.e02 - (T)2.0 * a.e12 * m.e23 * m.e01 - (T)2.0 * a.e12 * m.e31 * m.e02 - (T)2.0 * a.e12 * m.s * m.e0123 - (T)2.0 * a.e23 * m.e0123 * m.e31 + (T)2.0 * a.e01 * m.s * m.e31 + (T)2.0 * a.e23 * m.e12 * m.e01 + (T)2.0 * a.e31 * m.e12 * m.e02 + (T)2.0 * a.e12 * m.e12 * m.e03 + (T)2.0 * a.e01 * m.e12 * m.e23 + (T)2.0 * a.e02 * m.e12 * m.e31 + (T)2.0 * a.e23 * m.e03 * m.e23 + (T)2.0 * a.e31 * m.e03 * m.e31 + (T)2.0 * a.e31 * m.e0123 * m.e23
    );
  }


  template<Number T>
  _Point3<T> transform(const _Point3<T> &a, const _Motor3<T> &m) {
    return _Point3<T>(
      - a.e032 * m.e31 * m.e31 - a.e032 * m.e12 * m.e12 + a.e032 * m.e23 * m.e23 + a.e032 * m.s * m.s - (T)2.0 * a.e021 * m.e31 * m.s - (T)2.0 * a.e123 * m.e01 * m.s - (T)2.0 * a.e123 * m.e02 * m.e12 - (T)2.0 * a.e123 * m.e0123 * m.e23 + (T)2.0 * a.e013 * m.e12 * m.s + (T)2.0 * a.e021 * m.e23 * m.e12 + (T)2.0 * a.e013 * m.e23 * m.e31 + (T)2.0 * a.e123 * m.e31 * m.e03,
      - a.e013 * m.e12 * m.e12 - a.e013 * m.e23 * m.e23 + a.e013 * m.e31 * m.e31 + a.e013 * m.s * m.s - (T)2.0 * a.e032 * m.e12 * m.s - (T)2.0 * a.e123 * m.e02 * m.s - (T)2.0 * a.e123 * m.e23 * m.e03 - (T)2.0 * a.e123 * m.e0123 * m.e31 + (T)2.0 * a.e021 * m.e23 * m.s + (T)2.0 * a.e032 * m.e23 * m.e31 + (T)2.0 * a.e021 * m.e31 * m.e12 + (T)2.0 * a.e123 * m.e01 * m.e12,
      - a.e021 * m.e23 * m.e23 - a.e021 * m.e31 * m.e31 + a.e021 * m.e12 * m.e12 + a.e021 * m.s * m.s - (T)2.0 * a.e013 * m.e23 * m.s - (T)2.0 * a.e123 * m.e03 * m.s - (T)2.0 * a.e123 * m.e01 * m.e31 - (T)2.0 * a.e123 * m.e0123 * m.e12 + (T)2.0 * a.e032 * m.e31 * m.s + (T)2.0 * a.e032 * m.e23 * m.e12 + (T)2.0 * a.e013 * m.e31 * m.e12 + (T)2.0 * a.e123 * m.e23 * m.e02,
      + a.e123 * (m.e23 * m.e23 + m.e31 * m.e31 + m.e12 * m.e12 + m.s * m.s)
    );
  }


  template<Number T>
  _Vec3<T> transform_point(const _Vec3<T> &a, const _Motor3<T> &m) {
    T norm = m.s * m.s + m.e23 * m.e23 + m.e31 * m.e31 + m.e12 * m.e12;
    return _Vec3<T>(
      + a.x * m.e23 * m.e23 - a.x * m.e31 * m.e31 - a.x * m.e12 * m.e12 + a.x * m.s * m.s + (T)2.0 * a.y * m.e23 * m.e31 + (T)2.0 * a.y * m.s * m.e12 + (T)2.0 * a.z * m.e23 * m.e12 - (T)2.0 * a.z * m.s * m.e31 - (T)2.0 * m.e01 * m.s - (T)2.0 * m.e02 * m.e12 + (T)2.0 * m.e03 * m.e31 - (T)2.0 * m.e0123 * m.e23,
      - a.y * m.e23 * m.e23 + a.y * m.e31 * m.e31 - a.y * m.e12 * m.e12 + a.y * m.s * m.s + (T)2.0 * a.z * m.s * m.e23 + (T)2.0 * a.x * m.e23 * m.e31 + (T)2.0 * a.z * m.e31 * m.e12 - (T)2.0 * a.x * m.s * m.e12 + (T)2.0 * m.e01 * m.e12 - (T)2.0 * m.e02 * m.s - (T)2.0 * m.e03 * m.e23 - (T)2.0 * m.e0123 * m.e31,
      - a.z * m.e23 * m.e23 - a.z * m.e31 * m.e31 + a.z * m.e12 * m.e12 + a.z * m.s * m.s + (T)2.0 * a.x * m.e23 * m.e12 + (T)2.0 * a.x * m.e31 * m.s - (T)2.0 * a.y * m.e23 * m.s + (T)2.0 * a.y * m.e31 * m.e12 - (T)2.0 * m.e01 * m.e31 + (T)2.0 * m.e02 * m.e23 - (T)2.0 * m.e03 * m.s - (T)2.0 * m.e0123 * m.e12
    ) / norm;
  }


  template<Number T>
  _Vec3<T> transform_direction(const _Vec3<T> &a, const _Motor3<T> &m) {
    return _Vec3<T>(
      + a.x * m.e23 * m.e23 - a.x * m.e31 * m.e31 - a.x * m.e12 * m.e12 + a.x * m.s * m.s + (T)2.0 * a.y * m.e31 * m.e23 + (T)2.0 * a.y * m.e12 * m.s - (T)2.0 * a.z * m.e31 * m.s + (T)2.0 * a.z * m.e12 * m.e23,
      - a.y * m.e23 * m.e23 + a.y * m.e31 * m.e31 - a.y * m.e12 * m.e12 + a.y * m.s * m.s + (T)2.0 * a.z * m.s * m.e23 + (T)2.0 * a.z * m.e31 * m.e12 + (T)2.0 * a.x * m.e31 * m.e23 - (T)2.0 * a.x * m.s * m.e12,
      - a.z * m.e23 * m.e23 - a.z * m.e31 * m.e31 + a.z * m.e12 * m.e12 + a.z * m.s * m.s + (T)2.0 * a.x * m.e12 * m.e23 + (T)2.0 * a.x * m.s * m.e31 - (T)2.0 * a.y * m.s * m.e23 + (T)2.0 * a.y * m.e31 * m.e12
    );
  }


  // ========================
  // = Flat multiplications =
  // ========================


  template<Number T>
  _Motor3<T> operator*(const _Plane3<T> &a, const _Plane3<T> &b) {
    return _Motor3<T>(
      b.e1 * a.e1 + b.e2 * a.e2 + b.e3 * a.e3,
      a.e2 * b.e3 - b.e2 * a.e3,
      b.e1 * a.e3 - b.e3 * a.e1,
      b.e2 * a.e1 - a.e2 * b.e1,
      (T)0.0,
      a.e0 * b.e1 - a.e1 * b.e0,
      a.e0 * b.e2 - a.e2 * b.e0,
      a.e0 * b.e3 - b.e0 * a.e3
    );
  }


  template<Number T>
  _Motor3<T> operator*(const _Line3<T> &a, const _Line3<T> &b) {
    return _Motor3<T>(
      - a.e31 * b.e31 - b.e23 * a.e23 - a.e12 * b.e12,
      a.e12 * b.e31 - a.e31 * b.e12,
      b.e12 * a.e23 - a.e12 * b.e23,
      b.e23 * a.e31 - a.e23 * b.e31,
      a.e12 * b.e03 + b.e23 * a.e01 + a.e02 * b.e31 + a.e31 * b.e02 + b.e01 * a.e23 + a.e03 * b.e12,
      a.e12 * b.e02 + a.e03 * b.e31 - a.e02 * b.e12 - b.e03 * a.e31,
      b.e12 * a.e01 + b.e03 * a.e23 - a.e12 * b.e01 - a.e03 * b.e23,
      b.e23 * a.e02 + b.e01 * a.e31 - a.e01 * b.e31 - b.e02 * a.e23
    );
  }


  template<Number T>
  _Motor3<T> operator*(const _Point3<T> &a, const _Point3<T> &b) {
    return _Motor3<T>(
      - a.e123 * b.e123,
      (T)0.0,
      (T)0.0,
      (T)0.0,
      (T)0.0,
      a.e032 * b.e123 - b.e032 * a.e123,
      a.e013 * b.e123 - a.e123 * b.e013,
      a.e021 * b.e123 - b.e021 * a.e123
    );
  }


  template<Number T>
  inline _Motor3<T> operator/(const _Plane3<T> &a, const _Plane3<T> &b) {
    return a * reverse(b);
  }


  template<Number T>
  inline _Motor3<T> operator/(const _Line3<T> &a, const _Line3<T> &b) {
    return a * reverse(b);
  }


  template<Number T>
  inline _Motor3<T> operator/(const _Point3<T> &a, const _Point3<T> &b) {
    return a * reverse(b);
  }


  // ================
  // = Type aliases =
  // ================


  typedef _Motor3<float> Motor3;
  typedef _Motor3<double> Motor3d;
}

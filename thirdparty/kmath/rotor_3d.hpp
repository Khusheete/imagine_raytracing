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


#include "vector.hpp"
#include "matrix.hpp"
#include "euclidian_flat_3d.hpp"

#include <cmath>


namespace kmath {


  // Technically, this struct does not represent all rotor, since it can only
  // represent real rotations around the origin, ie. a rotation arount a line
  // going through the origin. A general rotor represents a rotation around a
  // general line. When this line is a vanishing line, the rotation around it
  // is actually a translation. The representation of a general rotor would
  // need to be represented in a basis such as:
  // 
  // (1, e23, e31, e12, e0123, e01, e02, e03)
  //
  // Which is exactly the basis of a general motor. Furthermore translations are
  // easely represented by a _Vec3, so this is the object you should use to
  // store a translation transformation. If you need a PGA object to represent
  // a general rotor, you should use the _Motor3 type (which is a bit more
  // general as it may be any rigid transformation).
  template<Number T>
  struct _Rotor3 {
    T s, e23, e31, e12;

  public:
    _Rotor3(): _Rotor3(IDENTITY) {}
    _Rotor3(T s, T e23, T e31, T e12): s(s), e23(e23), e31(e31), e12(e12) {}
    _Rotor3(T s, const _Vec3<T> &v): s(s), e23(v.x), e31(v.y), e12(v.z) {}


    static _Rotor3<T> from_axis_angle(const _Vec3<T> &axis, const T angle) {
      return _Rotor3<T>(
        (T)std::cos((T)0.5 * angle),
        - (T)std::sin((T)0.5 * angle) * axis
      );
    }

  public:
    static const _Rotor3<T> ZERO;
    static const _Rotor3<T> IDENTITY;
  };


  template<Number T>
  const _Rotor3<T> _Rotor3<T>::ZERO = _Rotor3<T>((T)0.0, (T)0.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Rotor3<T> _Rotor3<T>::IDENTITY = _Rotor3<T>((T)1.0, (T)0.0, (T)0.0, (T)0.0);


  // ===========================
  // = Rotor specific function =
  // ===========================


  template<Number T>
  inline _Rotor3<T> reverse(const _Rotor3<T> &r) {
    return _Rotor3<T>(
      r.s, -r.e23, -r.e31, -r.e12
    );
  }


  template<Number T>
  inline T length_squared(const _Rotor3<T> &r) {
    return r.s * r.s + r.e23 * r.e23 + r.e31 * r.e31 + r.e12 * r.e12;
  }


  template<Number T>
  inline T length(const _Rotor3<T> &r) {
    return std::sqrt(length_squared(r));
  }


  template<Number T>
  inline _Rotor3<T> normalized(const _Rotor3<T> &r) {
    return r / length(r);
  }


  template<Number T>
  inline _Rotor3<T> inverse(const _Rotor3<T> &r) {
    return reverse(r) / length_squared(r);
  }


  template<Number T>
  inline _Vec3<T> get_direction(const _Rotor3<T> &r) {
    return -_Vec3<T>(r.e23, r.e31, r.e12);
  }


  template<Number T>
  _Rotor3<T> exp(const _Rotor3<T> &r) {
    T len_v = length(get_direction(r));
    T exp_w = std::exp(r.s);

    if (is_approx_zero(len_v)) return _Rotor3<T>(exp_w, exp_w * get_direction(r));

    return _Rotor3<T>(
      exp_w * std::cos(len_v),
      (exp_w * std::sin(len_v) / len_v) * get_direction(r)
    );
  }


  template<Number T>
  _Rotor3<T> log(const _Rotor3<T> &r) {
    T len = length(r);
    T len_v = length(_Vec3<T>(r.e23, r.e31, r.e12));

    if (is_approx_zero(len_v)) return _Rotor3<T>(std::log(len), _Vec3<T>::ZERO);
  
    return _Rotor3<T>(
      std::log(len),
      (std::acos(r.s / len) / len_v) * get_direction(r)
    );
  }


  template<Number T>
  inline _Rotor3<T> pow(const _Rotor3<T> &r, const T power) {
    return exp(power * log(r));
  }


  // ===================
  // = Rotor operators =
  // ===================

  
  template<Number T>
  inline _Rotor3<T> slerp(const _Rotor3<T> &a, const _Rotor3<T> &b, const T t) {
    return a * pow(reverse(a) * b, t);
  }


  template<Number T>
  inline _Rotor3<T> operator+(const _Rotor3<T> &a, const _Rotor3<T> &b) {
    _Rotor3<T> r(a);
    r += b;
    return r;
  }


  template<Number T>
  inline _Rotor3<T> &operator+=(_Rotor3<T> &a, const _Rotor3<T> &b) {
    a.s   += b.s;
    a.e23 += b.e23;
    a.e31 += b.e31;
    a.e12 += b.e12;
    return a;
  }


  template<Number T>
  inline _Rotor3<T> operator-(const _Rotor3<T> &a, const _Rotor3<T> &b) {
    _Rotor3<T> r(a);
    r -= b;
    return r;
  }


  template<Number T>
  inline _Rotor3<T> &operator-=(_Rotor3<T> &a, const _Rotor3<T> &b) {
    a.s   -= b.s;
    a.e23 -= b.e23;
    a.e31 -= b.e31;
    a.e12 -= b.e12;
    return a;    
  }


  template<Number T>
  inline _Rotor3<T> operator-(const _Rotor3<T> &a) {
    return _Rotor3<T>(-a.s, -a.e23, -a.e31, -a.e12);
  }


  template<Number T>
  inline _Rotor3<T> operator*(const T a, const _Rotor3<T> &b) {
    _Rotor3<T> r(b);
    r *= a;
    return r;
  }


  template<Number T>
  inline _Rotor3<T> operator*(const _Rotor3<T> &a, const T b) {
    return b * a;
  }


  template<Number T>
  inline _Rotor3<T> &operator*=(_Rotor3<T> &a, const T b) {
    a.s   *= b;
    a.e23 *= b;
    a.e31 *= b;
    a.e12 *= b;
    return a;
  }


  template<Number T>
  inline _Rotor3<T> operator/(const _Rotor3<T> &a, const T b) {
    _Rotor3<T> r(a);
    r /= b;
    return r;
  }


  template<Number T>
  inline _Rotor3<T> &operator/=(_Rotor3<T> &a, const T b) {
    a.s   /= b;
    a.e23 /= b;
    a.e31 /= b;
    a.e12 /= b;
    return a;
  }


  template<Number T>
  inline _Rotor3<T> operator*(const _Rotor3<T> &a, const _Rotor3<T> &b) {
    return _Rotor3<T>(
      b.s * a.s - b.e23 * a.e23 - a.e31 * b.e31 - b.e12 * a.e12,
      a.s * b.e23 + b.s * a.e23 - a.e31 * b.e12 + b.e31 * a.e12,
      a.s * b.e31 + b.s * a.e31 + a.e23 * b.e12 - b.e23 * a.e12,
      a.s * b.e12 + b.s * a.e12 - a.e23 * b.e31 + b.e23 * a.e31
    );
  }


  template<Number T>
  inline _Rotor3<T> &operator*=(_Rotor3<T> &a, const _Rotor3<T> &b) {
    a = a * b;
    return a;
  }


  // ============
  // = As basis =
  // ============


  template<Number T>
  inline _Vec3<T> get_x_basis_vector(const _Rotor3<T> &r) {
    return _Vec3<T>(
      r.e23 * r.e23 - r.e31 * r.e31 - r.e12 * r.e12 + r.s * r.s,
      (T)2.0 * (r.e23 * r.e31 - r.e12 * r.s),
      (T)2.0 * (r.e23 * r.e12 + r.e31 * r.s)
    );
  }


  template<Number T>
  inline _Vec3<T> get_y_basis_vector(const _Rotor3<T> &r) {
    return _Vec3<T>(
      (T)2.0 * (r.e23 * r.e31 + r.e12 * r.s),
      -r.e23 * r.e23 + r.e31 * r.e31 - r.e12 * r.e12 + r.s * r.s,
      (T)2.0 * (r.e31 * r.e12 - r.e23 * r.s)
    );
  }


  template<Number T>
  inline _Vec3<T> get_z_basis_vector(const _Rotor3<T> &r) {
    return _Vec3<T>(
      (T)2.0 * (r.e23 * r.e12 - r.e31 * r.s),
      (T)2.0 * (r.e31 * r.e12 + r.e23 * r.s),
      -r.e23 * r.e23 - r.e31 * r.e31 + r.e12 * r.e12 + r.s * r.s
    );
  }


  template<Number T>
  inline _Mat3<T> as_basis(const _Rotor3<T> &r) {
    return _Mat3<T>(
      get_x_basis_vector(r),
      get_y_basis_vector(r),
      get_z_basis_vector(r)
    );
  }


  template<Number T>
  inline _Mat4<T> as_transform(const _Rotor3<T> &rotor, const _Vec3<T> &translation) {
    return _Mat4<T>::from_basis(as_basis(rotor), translation);
  }


  // ===================
  // = Transformations =
  // ===================
  

  template<Number T>
  inline _Plane3<T> transform(const _Plane3<T> &a, const _Rotor3<T> &r) {
    return _Plane3<T>(
      - r.e12 * r.e12 * a.e1 + (T)2.0 * r.e12 * r.s * a.e2 + r.s * r.s * a.e1 + (T)2.0 * a.e3 * r.e12 * r.e23 - r.e31 * r.e31 * a.e1 + (T)2.0 * a.e2 * r.e31 * r.e23 - (T)2.0 * a.e3 * r.s * r.e31 + a.e1 * r.e23 * r.e23,
      (T)2.0 * r.e31 * a.e1 * r.e23 - r.e12 * r.e12 * a.e2 - a.e2 * r.e23 * r.e23 - (T)2.0 * r.e12 * r.s * a.e1 + (T)2.0 * a.e3 * r.s * r.e23 + r.s * r.s * a.e2 + (T)2.0 * a.e3 * r.e12 * r.e31 + a.e2 * r.e31 * r.e31,
      (T)2.0 * r.e12 * a.e1 * r.e23 + (T)2.0 * r.e12 * a.e2 * r.e31 - a.e3 * r.e31 * r.e31 + (T)2.0 * r.s * r.e31 * a.e1 + a.e3 * r.s * r.s + a.e3 * r.e12 * r.e12 - a.e3 * r.e23 * r.e23 - (T)2.0 * r.s * a.e2 * r.e23,
      a.e0 * r.e23 * r.e23 + a.e0 * r.s * r.s + a.e0 * r.e12 * r.e12 + a.e0 * r.e31 * r.e31
    );
  }
  

  template<Number T>
  inline _Line3<T> transform(const _Line3<T> &a, const _Rotor3<T> &r) {
    return _Line3<T>(
      r.s  * r.s * a.e23 + (T)2.0 * r.e12 * a.e12 * r.e23 - r.e12  * r.e12 * a.e23 + (T)2.0 * r.e12 * a.e31 * r.s - (T)2.0 * r.s * r.e31 * a.e12 + (T)2.0 * a.e31 * r.e31 * r.e23 + a.e23 * r.e23  * r.e23 - r.e31  * r.e31 * a.e23,
      - (T)2.0 * r.e12 * r.s * a.e23 - a.e31 * r.e23  * r.e23 + (T)2.0 * r.e31 * a.e23 * r.e23 + (T)2.0 * r.e12 * r.e31 * a.e12 - r.e12  * r.e12 * a.e31 + (T)2.0 * r.s * a.e12 * r.e23 + a.e31 * r.e31  * r.e31 + a.e31 * r.s  * r.s,
      (T)2.0 * r.e12 * a.e31 * r.e31 + (T)2.0 * r.e12 * a.e23 * r.e23 - r.e31  * r.e31 * a.e12 + r.s  * r.s * a.e12 - a.e12 * r.e23  * r.e23 + r.e12  * r.e12 * a.e12 - (T)2.0 * a.e31 * r.s * r.e23 + (T)2.0 * r.s * r.e31 * a.e23,
      a.e01 * r.e23  * r.e23 + (T)2.0 * a.e02 * r.e31 * r.e23 + (T)2.0 * r.e12 * a.e03 * r.e23 - (T)2.0 * r.s * r.e31 * a.e03 + (T)2.0 * a.e02 * r.e12 * r.s - a.e01 * r.e31  * r.e31 + r.s  * r.s * a.e01 - r.e12  * r.e12 * a.e01,
      (T)2.0 * r.e12 * r.e31 * a.e03 + (T)2.0 * r.s * a.e03 * r.e23 + (T)2.0 * a.e01 * r.e31 * r.e23 + a.e02 * r.e31  * r.e31 - a.e02 * r.e12  * r.e12 + a.e02 * r.s  * r.s - a.e02 * r.e23  * r.e23 - (T)2.0 * r.e12 * r.s * a.e01,
      - r.e31  * r.e31 * a.e03 + (T)2.0 * a.e02 * r.e12 * r.e31 + (T)2.0 * r.s * a.e01 * r.e31 + r.e12  * r.e12 * a.e03 - (T)2.0 * a.e02 * r.s * r.e23 + r.s  * r.s * a.e03 + (T)2.0 * r.e12 * a.e01 * r.e23 - a.e03 * r.e23  * r.e23
    );
  }


  template<Number T>
  inline _Point3<T> transform(const _Point3<T> &a, const _Rotor3<T> &r) {
    return _Point3<T>(
      a.e032 * r.e23 * r.e23 - (T)2.0 * r.e31 * a.e021 * r.s + (T)2.0 * a.e021 * r.e23 * r.e12 - a.e032 * r.e31 * r.e31 + (T)2.0 * a.e013 * r.e12 * r.s - a.e032 * r.e12 * r.e12 + a.e032 * r.s * r.s + (T)2.0 * r.e31 * r.e23 * a.e013,
      (T)2.0 * a.e032 * r.e31 * r.e23 + (T)2.0 * a.e021 * r.e23 * r.s - a.e013 * r.e12 * r.e12 + a.e013 * r.s * r.s + r.e31 * r.e31 * a.e013 - (T)2.0 * a.e032 * r.e12 * r.s + (T)2.0 * r.e31 * a.e021 * r.e12 - r.e23 * r.e23 * a.e013,
      - a.e021 * r.e23 * r.e23 - (T)2.0 * r.e23 * a.e013 * r.s + (T)2.0 * a.e032 * r.e23 * r.e12 - r.e31 * r.e31 * a.e021 + a.e021 * r.e12 * r.e12 + a.e021 * r.s * r.s + (T)2.0 * a.e032 * r.e31 * r.s + (T)2.0 * r.e31 * a.e013 * r.e12,
      r.e23 * r.e23 * a.e123 + r.e12 * r.e12 * a.e123 + r.s * r.s * a.e123 + r.e31 * r.e31 * a.e123
    );
  }


  template<Number T>
  inline _Vec3<T> transform(const _Vec3<T> &a, const _Rotor3<T> &r) {
    return _Vec3<T>(
      (T)2.0 * r.e23 * r.e31 * a.y + (T)2.0 * a.y * r.e12 * r.s + (T)2.0 * a.z * r.e23 * r.e12 - a.x * r.e31 * r.e31 - (T)2.0 * a.z * r.e31 * r.s + r.e23 * r.e23 * a.x - a.x * r.e12 * r.e12 + a.x * r.s * r.s,
      - r.e23 * r.e23 * a.y + (T)2.0 * a.z * r.e23 * r.s + (T)2.0 * a.z * r.e31 * r.e12 + r.e31 * r.e31 * a.y - a.y * r.e12 * r.e12 - (T)2.0 * a.x * r.e12 * r.s + a.y * r.s * r.s + (T)2.0 * r.e23 * a.x * r.e31,
      - a.z * r.e23 * r.e23 + (T)2.0 * r.e23 * a.x * r.e12 + a.z * r.e12 * r.e12 - (T)2.0 * r.e23 * a.y * r.s + (T)2.0 * a.x * r.e31 * r.s + a.z * r.s * r.s - a.z * r.e31 * r.e31 + (T)2.0 * r.e31 * a.y * r.e12
    );
  }


  // ================
  // = Type aliases =
  // ================
  

  typedef _Rotor3<float> Rotor3;
  typedef _Rotor3<double> Rotor3d;
}

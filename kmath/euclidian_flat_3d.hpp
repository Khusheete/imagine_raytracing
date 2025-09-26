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


#include "utils.hpp"
#include "vector.hpp"
#include <cmath>


namespace kmath {


  // =======================
  // = Struct declarations =
  // =======================


  template<Number T>
  struct _Plane3 {
    T e1, e2, e3, e0;    

  public:
    _Plane3(): _Plane3((T)0.0, (T)0.0, (T)0.0, (T)0.0) {}
    _Plane3(T e1, T e2, T e3, T e0): e1(e1), e2(e2), e3(e3), e0(e0) {}


    static inline _Plane3<T> plane(const T a, const T b, const T c, const T d) {
      return _Plane3<T>(a, b, c, -d);
    }


    static inline _Plane3<T> plane(const _Vec3<T> &normal, const T distance) {
      return _Plane3<T>(normal.x, normal.y, normal.z, -distance);
    }


    static inline _Plane3<T> vanishing_plane(const T delta) {
      return _Plane3<T>((T)0.0, (T)0.0, (T)0.0, -delta);
    }


  public:
    static const _Plane3<T> VANISHING_PLANE;
    static const _Plane3<T> YZ;
    static const _Plane3<T> ZX;
    static const _Plane3<T> XY;
  };


  template<Number T> const _Plane3<T> _Plane3<T>::VANISHING_PLANE = _Plane3<T>((T)0.0, (T)0.0, (T)0.0, (T)-1.0);
  template<Number T> const _Plane3<T> _Plane3<T>::YZ = _Plane3<T>((T)1.0, (T)0.0, (T)0.0, (T)0.0);
  template<Number T> const _Plane3<T> _Plane3<T>::ZX = _Plane3<T>((T)0.0, (T)1.0, (T)0.0, (T)0.0);
  template<Number T> const _Plane3<T> _Plane3<T>::XY = _Plane3<T>((T)0.0, (T)0.0, (T)1.0, (T)0.0);
  

  // Note that this type is meant to represent a geometric line.
  // But it is in fact a bivector, which in general in 3D represents a screw.
  // Operations on lines in this header always result in a line, but other operations
  // (such as a motor logarithm) may result in a value of type _Line3<T> that represents
  // such a general bivector.
  //
  // The set of general bivectors forms the Lie group of the motor Lie algebra (ie. rigid euclidian
  // motions). The exponential and logarithm maps are used to to and from the Lie group and algebra.
  // The Lie bracket is the commutator product between two lines.
  template<Number T>
  struct _Line3 {
    T e23, e31, e12, e01, e02, e03;

  public:
    _Line3(): _Line3((T)0.0, (T)0.0, (T)0.0, (T)0.0, (T)0.0, (T)0.0) {}
    _Line3(const T e23, const T e31, const T e12, const T e01, const T e02, const T e03): e23(e23), e31(e31), e12(e12), e01(e01), e02(e02), e03(e03) {}


    static inline _Line3<T> line(const _Vec3<T> direction, const _Vec3<T> point) {
      return _Line3<T>(
        direction.x,
        direction.y,
        direction.z,
        point.y * direction.z - point.z * direction.y,
        point.z * direction.x - point.x * direction.z,
        point.x * direction.y - point.y * direction.x
      );
    }


    static inline _Line3<T> line(const T dx, const T dy, const T dz, const T px, const T py, const T pz) {
      return _Line3<T>(
        dx,
        dy,
        dz,
        py * dz - pz * dy,
        pz * dx - px * dz,
        px * dy - py * dx
      );
    }


    static inline _Line3<T> line(const _Vec3<T> direction) {
      return _Line3<T>(
        direction.x,
        direction.y,
        direction.z,
        (T)0.0,
        (T)0.0,
        (T)0.0
      );
    }


    static inline _Line3<T> line(const T dx, const T dy, const T dz) {
      return _Line3<T>(
        dx,
        dy,
        dz,
        (T)0.0,
        (T)0.0,
        (T)0.0
      );
    }


    static inline _Line3<T> vanishing_line(const _Vec3<T> direction) {
      return _Line3<T>(
        (T)0.0, (T)0.0, (T)0.0,
        -direction.x, -direction.y, -direction.z
      );
    }


    static inline _Line3<T> vanishing_line(const T dx, const T dy, const T dz) {
      return _Line3<T>(
        (T)0.0, (T)0.0, (T)0.0,
        -dx, -dy, -dz
      );
    }

    
    static inline _Line3<T> from_plucker(const _Vec3<T> direction, const _Vec3<T> moment) {
      return _Line3<T>(
        direction.x, direction.y, direction.z,
        moment.x, moment.y, moment.z      
      );
    }

    
    static inline _Line3<T> from_plucker(const T dx, const T dy, const T dz, const T mx, const T my, const T mz) {
      return _Line3<T>(
        dx, dy, dz,
        mx, my, mz      
      );
    }
  };


  template<Number T>
  struct _Point3 {
    T e032, e013, e021, e123;

  public:
    static inline _Point3<T> point(const _Vec3<T> &p) {
      return _Point3<T>(
        p.x, p.y, p.z, (T)1.0
      );
    }


    static inline _Point3<T> point(const T x, const T y, const T z) {
      return _Point3<T>(
        x, y, z, (T)1.0
      );
    }


    static inline _Point3<T> direction(const _Vec3<T> &d) {
      return _Point3<T>(
        -d.x, -d.y, -d.z, (T)0.0
      );
    }


    static inline _Point3<T> direction(const T x, const T y, const T z) {
      return _Point3<T>(
        -x, -y, -z, (T)0.0
      );
    }

  public:
    static const _Point3<T> ZERO;
    static const _Point3<T> ORIGIN;
    static const _Point3<T> X_DIR;
    static const _Point3<T> Y_DIR;
    static const _Point3<T> Z_DIR;
  };


  template<Number T>
  const _Point3<T> _Point3<T>::ZERO   = _Point3<T>((T)0.0, (T)0.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Point3<T> _Point3<T>::ORIGIN  = _Point3<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0);
  template<Number T>
  const _Point3<T> _Point3<T>::X_DIR = _Point3<T>((T)1.0, (T)0.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Point3<T> _Point3<T>::Y_DIR  = _Point3<T>((T)0.0, (T)1.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Point3<T> _Point3<T>::Z_DIR  = _Point3<T>((T)0.0, (T)0.0, (T)1.0, (T)0.0);


  // =============================
  // = Used function declaration =
  // =============================
  

  template<Number T>
  inline _Point3<T> meet(const _Plane3<T> &plane, const _Line3<T> &line);
  template<Number T>
  inline _Point3<T> meet(const _Line3<T> &line, const _Plane3<T> &plane);

  template<Number T>
  inline _Plane3<T> join(const _Line3<T> &line, const _Point3<T> &point);
  template<Number T>
  inline _Plane3<T> join(const _Point3<T> &point, const _Line3<T> &line);


  // ===================
  // = Plane functions =
  // ===================
  

  template<Number T>
  inline bool is_vanishing(const _Plane3<T> &a) {
    return is_square_approx_zero(magnitude_squared(a));
  }


  template<Number T>
  inline T magnitude_squared(const _Plane3<T> &a) {
    return a.e1 * a.e1 + a.e2 * a.e2 + a.e3 * a.e3;
  }
  

  template<Number T>
  inline T vanishing_magnitude_squared(const _Plane3<T> &a) {
    return a.e0 * a.e0;
  }


  template<Number T>
  inline T magnitude(const _Plane3<T> &a) {
    return std::sqrt(magnitude_squared(a));
  }
  

  template<Number T>
  inline T vanishing_magnitude(const _Plane3<T> &a) {
    return std::abs(a.e0);
  }


  template<Number T>
  inline _Plane3<T> normalized(const _Plane3<T> &a) {
    if (!is_vanishing(a)) {
      return a / magnitude(a);
    } else {
      return _Plane3<T>(
        0.0, 0.0, 0.0, -1.0
      );
    }
  }


  template<Number T>
  inline _Line3<T> meet(const _Plane3<T> &a, const _Plane3<T> &b) {
    return _Line3<T>(
      a.e2 * b.e3 - a.e3 * b.e2,
      a.e3 * b.e1 - a.e1 * b.e3,
      a.e1 * b.e2 - a.e2 * b.e1,
      a.e0 * b.e1 - a.e1 * b.e0,
      a.e0 * b.e2 - a.e2 * b.e0,
      a.e0 * b.e3 - a.e3 * b.e0    
    );
  }


  template<Number T>
  inline _Point3<T> meet(const _Plane3<T> &a, const _Plane3<T> &b, const _Plane3<T> &c) {
    return meet(meet(a, b), c);
  }


  template<Number T>
  inline T inner(const _Plane3<T> &a, const _Plane3<T> &b) {
    return a.e1 * b.e1 + a.e2 * b.e2 + a.e3 * b.e3;
  }


  template<Number T>
  inline _Point3<T> dual(const _Plane3<T> &p) {
    return _Point3<T>(p.e1, p.e2, p.e3, (T)0.0);
  }


  template<Number T>
  inline _Plane3<T> reverse(const _Plane3<T> &p) {
    return p;
  }


  template<Number T>
  inline _Plane3<T> inverse(const _Plane3<T> &p) {
    return reverse(p) / magnitude_squared(p);
  }


  // ===================
  // = Plane operators =
  // ===================


  template<Number T>
  inline _Plane3<T> operator+(const _Plane3<T> &a, const _Plane3<T> &b) {
    _Plane3<T> res(a);
    res += b;
    return res;
  }


  template<Number T>
  inline _Plane3<T> &operator+=(_Plane3<T> &a, const _Plane3<T> &b) {
    a.e1 += b.e1;
    a.e2 += b.e2;
    a.e3 += b.e3;
    a.e0 += b.e0;
    return a;
  }


  template<Number T>
  inline _Plane3<T> operator-(const _Plane3<T> &a, const _Plane3<T> &b) {
    _Plane3<T> res(a);
    res -= b;
    return res;
  }


  template<Number T>
  inline _Plane3<T> &operator-=(_Plane3<T> &a, const _Plane3<T> &b) {
    a.e1 -= b.e1;
    a.e2 -= b.e2;
    a.e3 -= b.e3;
    a.e0 -= b.e0;
    return a;
  }


  template<Number T>
  inline _Plane3<T> operator-(const _Plane3<T> &a){
    return _Plane3<T>(
      -a.e1,
      -a.e2,
      -a.e3,
      -a.e0
    );
  }


  template<Number T>
  inline _Plane3<T> operator*(const T a, const _Plane3<T> &b) {
    _Plane3<T> res(b);
    res.e1 = a * res.e1;
    res.e2 = a * res.e2;
    res.e3 = a * res.e3;
    res.e0 = a * res.e0;
    return res;
  }


  template<Number T>
  inline _Plane3<T> operator*(const _Plane3<T> &a, const T b) {
    _Plane3<T> res(a);
    res.e1 = res.e1 * b;
    res.e2 = res.e2 * b;
    res.e3 = res.e3 * b;
    res.e0 = res.e0 * b;
    return res;
  }


  template<Number T>
  inline _Plane3<T> &operator*=(_Plane3<T> &a, const T b) {
    a = a * b;
    return a;
  }


  template<Number T>
  inline _Plane3<T> operator/(const _Plane3<T> &a, const T b) {
    _Plane3<T> res(a);
    res /= b;
    return res;
  }


  template<Number T>
  inline _Plane3<T> &operator/=(_Plane3<T> &a, const T b) {
    a.e1 /= b;
    a.e2 /= b;
    a.e3 /= b;
    a.e0 /= b;
    return a;
  }


  // ==================
  // = Line functions =
  // ==================


  template<Number T>
  inline bool is_vanishing(const _Line3<T> &a) {
    return is_square_approx_zero(magnitude_squared(a));
  }
  

  template<Number T>
  inline T magnitude_squared(const _Line3<T> &a) {
    return a.e23 * a.e23 + a.e31 * a.e31 + a.e12 * a.e12;
  }
  

  template<Number T>
  inline T vanishing_magnitude_squared(const _Line3<T> &a) {
    return a.e01 * a.e01 + a.e02 * a.e02 + a.e03 * a.e03;
  }


  template<Number T>
  inline T magnitude(const _Line3<T> &a) {
    return std::sqrt(magnitude_squared(a));
  }
  

  template<Number T>
  inline T vanishing_magnitude(const _Line3<T> &a) {
    return std::sqrt(vanishing_magnitude_squared(a));
  }


  // This normalization usually normalizes the translation part of the screw represented by
  // this _Line3 object. If it is a line through the origin (meaning that it represents a
  // rotation around the origin), the rotation part will be normalized.
  //
  // TODO: check if this makes sense
  template<Number T>
  inline _Line3<T> normalized(const _Line3<T> &a) {
    const T vm2 = vanishing_magnitude_squared(a);
    if (!is_square_approx_zero(vm2)) {
      return a / std::sqrt(vm2);
    } else {
      return a / magnitude(a);
    }
  }


  template<Number T>
  inline T inner(const _Line3<T> &a, const _Line3<T> &b) {
    return -(a.e23 * b.e23 + a.e31 * b.e31 + a.e12 * b.e12);
  }


  template<Number T>
  inline T join(const _Line3<T> &a, const _Line3<T> &b) {
    return (
      a.e23 * b.e01
      + a.e31 * b.e02
      + a.e12 * b.e03
      + a.e01 * b.e23
      + a.e02 * b.e31
      + a.e03 * b.e12
    );
  }


  template<Number T>
  inline T meet(const _Line3<T> &a, const _Line3<T> &b) {
    return (
      a.e23 * b.e01
      + a.e31 * b.e02
      + a.e12 * b.e03
      + a.e01 * b.e23
      + a.e02 * b.e31
      + a.e03 * b.e12
    );
  }


  template<Number T>
  inline _Line3<T> reverse(const _Line3<T> &l) {
    return -l;
  }


  template<Number T>
  inline _Line3<T> inverse(const _Line3<T> &l) {
    return reverse(l) / magnitude_squared(l);
  }


  // template<Number T>
  // inline T get_angle(const _Line3<T> &a);


  // ==================
  // = Line operators =
  // ==================


  template<Number T>
  inline _Line3<T> operator+(const _Line3<T> &a, const _Line3<T> &b) {
    _Line3<T> res(a);
    res += b;
    return res;
  }


  template<Number T>
  inline _Line3<T> &operator+=(_Line3<T> &a, const _Line3<T> &b) {
    a.e23 += b.e23;
    a.e31 += b.e31;
    a.e12 += b.e12;
    a.e01 += b.e01;
    a.e02 += b.e02;
    a.e03 += b.e03;
    return a;
  }


  template<Number T>
  inline _Line3<T> operator-(const _Line3<T> &a, const _Line3<T> &b) {
    _Line3<T> res(a);
    res -= b;
    return res;
  }


  template<Number T>
  inline _Line3<T> &operator-=(_Line3<T> &a, const _Line3<T> &b) {
    a.e23 -= b.e23;
    a.e31 -= b.e31;
    a.e12 -= b.e12;
    a.e01 -= b.e01;
    a.e02 -= b.e02;
    a.e03 -= b.e03;
    return a;
  }


  template<Number T>
  inline _Line3<T> operator-(const _Line3<T> &a){
    return _Line3<T>(
      -a.e23,
      -a.e31,
      -a.e12,
      -a.e01,
      -a.e02,
      -a.e03
    );
  }


  template<Number T>
  inline _Line3<T> operator*(const T a, const _Line3<T> &b) {
    _Line3<T> res(b);
    res.e23 = a * res.e23;
    res.e31 = a * res.e31;
    res.e12 = a * res.e12;
    res.e01 = a * res.e01;
    res.e02 = a * res.e02;
    res.e03 = a * res.e03;
    return res;
  }


  template<Number T>
  inline _Line3<T> operator*(const _Line3<T> &a, const T b) {
    _Line3<T> res(a);
    res.e23 = res.e23 * b;
    res.e31 = res.e31 * b;
    res.e12 = res.e12 * b;
    res.e01 = res.e01 * b;
    res.e02 = res.e02 * b;
    res.e03 = res.e03 * b;
    return res;
  }


  template<Number T>
  inline _Line3<T> &operator*=(_Line3<T> &a, const T b) {
    a = a * b;
    return a;
  }


  template<Number T>
  inline _Line3<T> operator/(const _Line3<T> &a, const T b) {
    _Line3<T> res(a);
    res /= b;
    return res;
  }


  template<Number T>
  inline _Line3<T> &operator/=(_Line3<T> &a, const T b) {
    a.e23 /= b;
    a.e31 /= b;
    a.e12 /= b;
    a.e01 /= b;
    a.e02 /= b;
    a.e03 /= b;
    return a;
  }


  // ===================
  // = Point functions =
  // ===================


  template<Number T>
  inline _Vec3<T> as_vector(const _Point3<T> &a) {
    if (!is_vanishing(a)) {
      return _Vec3<T>(
        a.e032 / a.e123,
        a.e013 / a.e123,
        a.e021 / a.e123      
      );
    } else {
      return _Vec3<T>(
        -a.e032,
        -a.e013,
        -a.e021
      );
    }
  }
  

  template<Number T>
  inline T magnitude_squared(const _Point3<T> &a) {
    return a.e123 * a.e123;
  }
  

  template<Number T>
  inline T vanishing_magnitude_squared(const _Point3<T> &a) {
    return a.e032 * a.e032 + a.e013 * a.e013 + a.e021 * a.e021;
  }


  template<Number T>
  inline T magnitude(const _Point3<T> &a) {
    return std::abs(a.e123);
  }
  

  template<Number T>
  inline T vanishing_magnitude(const _Point3<T> &a) {
    return std::sqrt(vanishing_magnitude_squared(a));
  }


  template<Number T>
  inline bool is_vanishing(const _Point3<T> &a) {
    return is_approx_zero(a.e123);
  }


  template<Number T>
  inline _Point3<T> normalized(const _Point3<T> &a) {
    if (!is_vanishing(a)) {
      return a / a.e123;
    } else {
      return a / vanishing_magnitude(a);
    }
  }


  template<Number T>
  inline _Line3<T> join(const _Point3<T> &a, const _Point3<T> &b) {
    return _Line3<T>(
      a.e032 * b.e123 - a.e123 * b.e032,
      a.e013 * b.e123 - a.e123 * b.e013,
      a.e021 * b.e123 - a.e123 * b.e021,
      a.e021 * b.e013 - a.e013 * b.e021,
      a.e032 * b.e021 - a.e021 * b.e032,
      a.e013 * b.e032 - a.e032 * b.e013
    );
  }


  template<Number T>
  inline _Plane3<T> join(const _Point3<T> &a, const _Point3<T> &b, const _Point3<T> &c) {
    return join(join(a, b), c);
  }


  template<Number T>
  inline T inner(const _Point3<T> &a, const _Point3<T> &b) {
    return - a.e123 * b.e123;
  }


  template<Number T>
  inline _Point3<T> reverse(const _Point3<T> &p) {
    return -p;
  }


  template<Number T>
  inline _Point3<T> inverse(const _Point3<T> &p) {
    return reverse(p) / magnitude_squared(p);
  }


  // ===================
  // = Point operators =
  // ===================


  template<Number T>
  inline _Point3<T> operator+(const _Point3<T> &a, const _Point3<T> &b) {
    _Point3<T> res(a);
    res += b;
    return res;
  }


  template<Number T>
  inline _Point3<T> &operator+=(_Point3<T> &a, const _Point3<T> &b) {
    a.e123 += b.e123;
    a.e032 += b.e032;
    a.e013 += b.e013;
    a.e021 += b.e021;
    return a;
  }


  template<Number T>
  inline _Point3<T> operator-(const _Point3<T> &a, const _Point3<T> &b) {
    _Point3<T> res(a);
    res -= b;
    return res;
  }


  template<Number T>
  inline _Point3<T> &operator-=(_Point3<T> &a, const _Point3<T> &b) {
    a.e123 -= b.e123;
    a.e032 -= b.e032;
    a.e013 -= b.e013;
    a.e021 -= b.e021;
    return a;
  }


  template<Number T>
  inline _Point3<T> operator-(const _Point3<T> &a){
    return _Point3<T>(
      -a.e032,
      -a.e013,
      -a.e021,
      -a.e123
    );
  }


  template<Number T>
  inline _Point3<T> operator*(const T a, const _Point3<T> &b) {
    _Point3<T> res(b);
    res.e123 = a * res.e123;
    res.e032 = a * res.e032;
    res.e013 = a * res.e013;
    res.e021 = a * res.e021;
    return res;
  }


  template<Number T>
  inline _Point3<T> operator*(const _Point3<T> &a, const T b) {
    _Point3<T> res(a);
    res.e123 = res.e123 * b;
    res.e032 = res.e032 * b;
    res.e013 = res.e013 * b;
    res.e021 = res.e021 * b;
    return res;
  }


  template<Number T>
  inline _Point3<T> &operator*=(_Point3<T> &a, const T b) {
    a = a * b;
    return a;
  }


  template<Number T>
  inline _Point3<T> operator/(const _Point3<T> &a, const T b) {
    _Point3<T> res(a);
    res /= b;
    return res;
  }


  template<Number T>
  inline _Point3<T> &operator/=(_Point3<T> &a, const T b) {
    a.e123 /= b;
    a.e032 /= b;
    a.e013 /= b;
    a.e021 /= b;
    return a;
  }
  

  // ========================
  // = Plane-line functions =
  // ========================
  

  template<Number T>
  inline _Point3<T> meet(const _Plane3<T> &plane, const _Line3<T> &line) {
    return _Point3<T>(
      plane.e2 * line.e03 - plane.e3 * line.e02 - plane.e0 * line.e23,
      plane.e3 * line.e01 - plane.e1 * line.e03 - plane.e0 * line.e31,
      plane.e1 * line.e02 - plane.e2 * line.e01 - plane.e0 * line.e12,
      plane.e1 * line.e23 + plane.e2 * line.e31 + plane.e3 * line.e12
    );
  }


  template<Number T>
  inline _Point3<T> meet(const _Line3<T> &line, const _Plane3<T> &plane) {
    return meet(plane, line);
  }


  template<Number T>
  inline _Plane3<T> inner(const _Plane3<T> &plane, const _Line3<T> &line) {
    return _Plane3<T>(
      plane.e3 * line.e31 - plane.e2 * line.e12,
      plane.e1 * line.e12 - plane.e3 * line.e23,
      plane.e2 * line.e23 - plane.e1 * line.e31,
      -(plane.e1 * line.e01 + plane.e2 * line.e02 + plane.e3 * line.e03)
    );
  }


  template<Number T>
  inline _Plane3<T> inner(const _Line3<T> &line, const _Plane3<T> &plane) {
    return -inner(plane, line);
  }


  template<Number T>
  inline bool is_on(const _Line3<T> &line, const _Plane3<T> &plane) {
    return is_approx_zero(inner(plane, line)); // TODO: check
  }


  // ========================
  // = Line-point functions =
  // ========================


  template<Number T>
  inline _Plane3<T> join(const _Line3<T> &line, const _Point3<T> &point) {
    return _Plane3<T>(
      line.e01 * point.e123 + line.e31 * point.e021 - line.e12 * point.e013,
      line.e02 * point.e123 + line.e12 * point.e032 - line.e23 * point.e021,
      line.e03 * point.e123 + line.e23 * point.e013 - line.e31 * point.e032,
      -line.e01 * point.e032 - line.e02 * point.e013 - line.e03 * point.e021
    );
  }


  template<Number T>
  inline _Plane3<T> join(const _Point3<T> &point, const _Line3<T> &line) {
    return join(line, point);
  }


  template<Number T>
  inline _Plane3<T> inner(const _Line3<T> &line, const _Point3<T> &point) {
    return _Plane3<T>(
       - line.e23 * point.e123,
       - line.e31 * point.e123,
       - line.e12 * point.e123,
      line.e23 * point.e032 + line.e31 * point.e013 + line.e12 * point.e021
    );
  }


  template<Number T>
  inline _Plane3<T> inner(const _Point3<T> &point, const _Line3<T> &line) {
    return inner(line, point);
  }


  template<Number T>
  inline bool is_on(const _Point3<T> &point, const _Line3<T> &line) {
    return is_approx_zero(inner(line, point));
  }


  // =========================
  // = Plane-point functions =
  // =========================


  template<Number T>
  inline T meet(const _Plane3<T> &plane, const _Point3<T> &point) {
    return (
      plane.e0 * point.e123
      + plane.e1 * point.e032
      + plane.e2 * point.e013
      + plane.e3 * point.e021
    );
  }


  template<Number T>
  inline T meet(const _Point3<T> &point, const _Plane3<T> &plane) {
    return -meet(plane, point);
  }
  

  template<Number T>
  inline T join(const _Plane3<T> &plane, const _Point3<T> &point) {
    return (
      - plane.e0 * point.e123
      - plane.e1 * point.e032
      - plane.e2 * point.e013
      - plane.e3 * point.e021
    );
  }


  template<Number T>
  inline T join(const _Point3<T> &point, const _Plane3<T> &plane) {
    return -join(plane, point);
  }
  

  template<Number T>
  inline _Line3<T> inner(const _Plane3<T> &plane, const _Point3<T> &point) {
    return _Line3<T>(
      plane.e1 * point.e123,
      plane.e2 * point.e123,
      plane.e3 * point.e123,
      plane.e3 * point.e013 - plane.e2 * point.e021,
      plane.e1 * point.e021 - plane.e3 * point.e032,
      plane.e2 * point.e032 - plane.e1 * point.e013
    );
  }


  template<Number T>
  inline _Line3<T> inner(const _Point3<T> &point, const _Plane3<T> &plane) {
    return inner(plane, point);
  }



  template<Number T>
  inline bool is_on(const _Point3<T> &point, const _Plane3<T> &plane) {
    return is_approx_zero(inner(plane, point));
  }


  // ============================
  // = Projections & regections =
  // ============================


  // Fast projection gives a projection modulo a positive factor
  template<Number T>
  inline _Plane3<T> fast_project(const _Plane3<T> &a, const _Point3<T> &b) {
    return inner(inner(a, b), b);
  }


  // Fast projection gives a projection modulo a positive factor
  template<Number T>
  inline _Line3<T> fast_project(const _Line3<T> &a, const _Point3<T> &b) {
    return inner(inner(a, b), b);
  }


  // Fast projection gives a projection modulo a positive factor
  template<Number T>
  inline _Line3<T> fast_project(const _Line3<T> &a, const _Plane3<T> &b) {
    return meet(inner(a, b), b);
  }


  // Fast projection gives a projection modulo a positive factor
  template<Number T>
  inline _Plane3<T> fast_project(const _Plane3<T> &a, const _Line3<T> &b) {
    return inner(inner(a, b), b);
  }


  // Fast projection gives a projection modulo a positive factor
  template<Number T>
  inline _Point3<T> fast_project(const _Point3<T> &a, const _Plane3<T> &b) {
    return meet(inner(a, b), b);
  }


  // Fast projection gives a projection modulo a positive factor
  template<Number T>
  inline _Point3<T> fast_project(const _Point3<T> &a, const _Line3<T> &b) {
    return meet(inner(a, b), b);
  }


  template<Number T>
  inline _Plane3<T> project(const _Plane3<T> &a, const _Point3<T> &b) {
    return inner(inner(a, b), inverse(b));
  }


  template<Number T>
  inline _Line3<T> project(const _Line3<T> &a, const _Point3<T> &b) {
    return inner(inner(a, b), inverse(b));
  }


  template<Number T>
  inline _Line3<T> project(const _Line3<T> &a, const _Plane3<T> &b) {
    return meet(inner(a, b), inverse(b));
  }


  template<Number T>
  inline _Line3<T> project(const _Plane3<T> &a, const _Line3<T> &b) {
    return inner(inner(a, b), inverse(b));
  }


  template<Number T>
  inline _Point3<T> project(const _Point3<T> &a, const _Plane3<T> &b) {
    return meet(inner(a, b), inverse(b));
  }


  template<Number T>
  inline _Point3<T> project(const _Point3<T> &a, const _Line3<T> &b) {
    return meet(inner(a, b), inverse(b));
  }


  // Fast rejection gives a rejection modulo a positive factor
  template<Number T>
  inline _Plane3<T> fast_reject(const _Plane3<T> &a, const _Point3<T> &b) {
    return _Plane3<T>((T)0.0, (T)0.0, (T)0.0, -meet(a, b) * b.e123);
  }


  // Fast rejection gives a rejection modulo a positive factor
  template<Number T>
  inline _Line3<T> fast_reject(const _Line3<T> &a, const _Plane3<T> &b) {
    return inner(meet(a, b), b);
  }


  // Fast rejection gives a rejection modulo a positive factor
  template<Number T>
  inline _Plane3<T> fast_reject(const _Plane3<T> &a, const _Line3<T> &b) {
    return inner(meet(a, b), b);
  }


  // Fast rejection gives a rejection modulo a positive factor
  template<Number T>
  inline _Point3<T> fast_reject(const _Point3<T> &a, const _Plane3<T> &b) {
    return (-meet(a, b)) * _Point3<T>(b.e1, b.e2, b.e3, (T)0.0);
  }


  template<Number T>
  inline _Plane3<T> reject(const _Plane3<T> &a, const _Point3<T> &b) {
    return inner(meet(a, b), inverse(b));
  }


  template<Number T>
  inline _Line3<T> reject(const _Line3<T> &a, const _Plane3<T> &b) {
    return inner(meet(a, b), inverse(b));
  }


  template<Number T>
  inline _Plane3<T> reject(const _Plane3<T> &a, const _Line3<T> &b) {
    return inner(meet(a, b), inverse(b));
  }


  template<Number T>
  inline _Point3<T> reject(const _Point3<T> &a, const _Plane3<T> &b) {
    return meet(meet(a, b), inverse(b));
  }


  // ===============
  // = Reflections =
  // ===============


  template<Number T>
  _Point3<T> fast_reflect(const _Point3<T> &a, const _Plane3<T> &b) {
    return _Point3<T>(
      b.e1 * b.e1 * a.e032 + (T)2.0 * a.e013 * b.e2 * b.e1 + (T)2.0 * a.e021 * b.e1 * b.e3 + (T)2.0 * a.e123 * b.e0 * b.e1 - a.e032 * b.e2 * b.e2 - a.e032 * b.e3 * b.e3,
      a.e013 * b.e2 * b.e2 + (T)2.0 * a.e032 * b.e2 * b.e1 + (T)2.0 * a.e021 * b.e3 * b.e2 + (T)2.0 * a.e123 * b.e2 * b.e0 - a.e013 * b.e3 * b.e3 - a.e013 * b.e1 * b.e1,
      a.e021 * b.e3 * b.e3 + (T)2.0 * a.e032 * b.e1 * b.e3 + (T)2.0 * a.e013 * b.e2 * b.e3 + (T)2.0 * a.e123 * b.e3 * b.e0 - a.e021 * b.e1 * b.e1 - a.e021 * b.e2 * b.e2,
      - a.e123 * (b.e1 * b.e1 + b.e2 * b.e2 + b.e3 * b.e3)
    );
  }


  template<Number T>
  _Point3<T> fast_reflect(const _Point3<T> &a, const _Line3<T> &b) {
    return _Point3<T>(
      - (T)2.0 * a.e021 * b.e23 * b.e12 - a.e032 * b.e23 * b.e23 + (T)2.0 * b.e02 * b.e12 * a.e123 + a.e032 * b.e12 * b.e12 + a.e032 * b.e31 * b.e31 - (T)2.0 * b.e31 * b.e03 * a.e123 - (T)2.0 * b.e31 * b.e23 * a.e013,
      - (T)2.0 * a.e032 * b.e31 * b.e23 + b.e23 * b.e23 * a.e013 + a.e013 * b.e12 * b.e12 - (T)2.0 * b.e01 * b.e12 * a.e123 - b.e31 * b.e31 * a.e013 + (T)2.0 * b.e23 * b.e03 * a.e123 - (T)2.0 * b.e31 * a.e021 * b.e12,
      a.e021 * b.e23 * b.e23 - (T)2.0 * a.e032 * b.e23 * b.e12 - (T)2.0 * b.e02 * b.e23 * a.e123 + b.e31 * b.e31 * a.e021 - (T)2.0 * b.e31 * a.e013 * b.e12 - a.e021 * b.e12 * b.e12 + (T)2.0 * b.e31 * b.e01 * a.e123,
      - b.e31 * b.e31 * a.e123 - b.e23 * b.e23 * a.e123 - b.e12 * b.e12 * a.e123
    );
  }


  template<Number T>
  _Point3<T> fast_reflect(const _Point3<T> &a, const _Point3<T> &b) {
    return _Point3<T>(
      + (T)2.0 * a.e123 * b.e123 * b.e032 - a.e032 * b.e123 * b.e123,
      + (T)2.0 * a.e123 * b.e123 * b.e013 - a.e013 * b.e123 * b.e123,
      + (T)2.0 * a.e123 * b.e123 * b.e021 - a.e021 * b.e123 * b.e123,
      + a.e123 * b.e123 * b.e123
    );
  }


  template<Number T>
  _Line3<T> fast_reflect(const _Line3<T> &a, const _Plane3<T> &b) {
    return _Line3<T>(
      - b.e3 * b.e3 * a.e23 + (T)2.0 * b.e3 * b.e1 * a.e12 - a.e23 * b.e2 * b.e2 + (T)2.0 * b.e1 * b.e2 * a.e31 + b.e1 * b.e1 * a.e23,
      + b.e2 * b.e2 * a.e31 + (T)2.0 * b.e3 * a.e12 * b.e2 - b.e3 * b.e3 * a.e31 + (T)2.0 * b.e1 * a.e23 * b.e2 - b.e1 * b.e1 * a.e31,
      (T)2.0 * b.e3 * b.e2 * a.e31 - b.e1 * b.e1 * a.e12 + b.e3 * b.e3 * a.e12 - a.e12 * b.e2 * b.e2 + (T)2.0 * b.e3 * b.e1 * a.e23,
      - (T)2.0 * b.e3 * b.e0 * a.e31 - (T)2.0 * b.e1 * a.e02 * b.e2 - a.e01 * b.e1 * b.e1 - (T)2.0 * b.e3 * b.e1 * a.e03 + b.e3 * b.e3 * a.e01 + a.e01 * b.e2 * b.e2 + (T)2.0 * a.e12 * b.e0 * b.e2,
      - (T)2.0 * b.e1 * a.e12 * b.e0 + b.e1 * b.e1 * a.e02 - a.e02 * b.e2 * b.e2 - (T)2.0 * b.e3 * a.e03 * b.e2 - (T)2.0 * a.e01 * b.e1 * b.e2 + (T)2.0 * b.e3 * a.e23 * b.e0 + b.e3 * b.e3 * a.e02,
      + a.e03 * b.e2 * b.e2 - b.e3 * b.e3 * a.e03 + b.e1 * b.e1 * a.e03 - (T)2.0 * b.e3 * a.e02 * b.e2 + (T)2.0 * b.e1 * b.e0 * a.e31 - (T)2.0 * b.e3 * a.e01 * b.e1 - (T)2.0 * a.e23 * b.e0 * b.e2
    );
  }


  template<Number T>
  _Line3<T> fast_reflect(const _Line3<T> &a, const _Line3<T> &b) {
    return _Line3<T>(
       + a.e23 * b.e31 * b.e31 + a.e23 * b.e12 * b.e12 - a.e23 * b.e23 * b.e23 - (T)2.0 * a.e12 * b.e12 * b.e23 - (T)2.0 * a.e31 * b.e31 * b.e23,
       + a.e31 * b.e23 * b.e23 + a.e31 * b.e12 * b.e12 - a.e31 * b.e31 * b.e31 - (T)2.0 * a.e23 * b.e31 * b.e23 - (T)2.0 * a.e12 * b.e31 * b.e12,
       + a.e12 * b.e23 * b.e23 + a.e12 * b.e31 * b.e31 - a.e12 * b.e12 * b.e12 - (T)2.0 * a.e31 * b.e31 * b.e12 - (T)2.0 * a.e23 * b.e12 * b.e23,
       + b.e31 * b.e31 * a.e01 + b.e12 * b.e12 * a.e01 - a.e01 * b.e23 * b.e23 - (T)2.0 * a.e23 * b.e01 * b.e23 - (T)2.0 * a.e31 * b.e01 * b.e31 - (T)2.0 * a.e12 * b.e01 * b.e12 - (T)2.0 * a.e31 * b.e02 * b.e23 + (T)2.0 * a.e23 * b.e02 * b.e31 - (T)2.0 * a.e12 * b.e03 * b.e23 + (T)2.0 * a.e23 * b.e03 * b.e12 - (T)2.0 * a.e02 * b.e31 * b.e23 - (T)2.0 * a.e03 * b.e12 * b.e23,
       + a.e02 * b.e23 * b.e23 + a.e02 * b.e12 * b.e12 - a.e02 * b.e31 * b.e31 - (T)2.0 * a.e23 * b.e01 * b.e31 + (T)2.0 * a.e31 * b.e01 * b.e23 - (T)2.0 * a.e31 * b.e02 * b.e31 - (T)2.0 * a.e12 * b.e02 * b.e12 - (T)2.0 * a.e23 * b.e02 * b.e23 + (T)2.0 * a.e31 * b.e03 * b.e12 - (T)2.0 * a.e12 * b.e03 * b.e31 - (T)2.0 * a.e01 * b.e31 * b.e23 - (T)2.0 * a.e03 * b.e31 * b.e12,
       + a.e03 * b.e23 * b.e23 + a.e03 * b.e31 * b.e31 - a.e03 * b.e12 * b.e12 + (T)2.0 * a.e12 * b.e01 * b.e23 - (T)2.0 * a.e23 * b.e01 * b.e12 + (T)2.0 * a.e12 * b.e02 * b.e31 - (T)2.0 * a.e31 * b.e02 * b.e12 - (T)2.0 * a.e23 * b.e03 * b.e23 - (T)2.0 * a.e31 * b.e03 * b.e31 - (T)2.0 * a.e12 * b.e03 * b.e12 - (T)2.0 * a.e01 * b.e12 * b.e23 - (T)2.0 * a.e02 * b.e31 * b.e12
    );
  }


  template<Number T>
  _Line3<T> fast_reflect(const _Line3<T> &a, const _Point3<T> &b) {
    return _Line3<T>(
      - a.e23 * b.e123 * b.e123,
      - a.e31 * b.e123 * b.e123,
      - a.e12 * b.e123 * b.e123,
      + a.e01 * b.e123 * b.e123 + (T)2.0 * a.e31 * b.e021 * b.e123 - (T)2.0 * a.e12 * b.e123 * b.e013,
      + a.e02 * b.e123 * b.e123 + (T)2.0 * a.e12 * b.e123 * b.e032 - (T)2.0 * a.e23 * b.e021 * b.e123,
      + a.e03 * b.e123 * b.e123 + (T)2.0 * a.e23 * b.e123 * b.e013 - (T)2.0 * a.e31 * b.e123 * b.e032
    );
  }


  template<Number T>
  _Plane3<T> fast_reflect(const _Plane3<T> &a, const _Plane3<T> &b) {
    return _Plane3<T>(
      a.e1 * b.e2 * b.e2 + a.e1 * b.e3 * b.e3 - a.e1 * b.e1 * b.e1 - (T)2.0 * a.e3 * b.e1 * b.e3 - (T)2.0 * a.e2 * b.e1 * b.e2,
      a.e2 * b.e1 * b.e1 + a.e2 * b.e3 * b.e3 - a.e2 * b.e2 * b.e2 - (T)2.0 * a.e3 * b.e2 * b.e3 - (T)2.0 * a.e1 * b.e1 * b.e2,
      a.e3 * b.e2 * b.e2 + a.e3 * b.e1 * b.e1 - a.e3 * b.e3 * b.e3 - (T)2.0 * a.e2 * b.e2 * b.e3 - (T)2.0 * a.e1 * b.e1 * b.e3,
      a.e0 * b.e1 * b.e1 + a.e0 * b.e2 * b.e2 + a.e0 * b.e3 * b.e3 - (T)2.0 * a.e2 * b.e2 * b.e0 - (T)2.0 * a.e3 * b.e3 * b.e0 - (T)2.0 * a.e1 * b.e1 * b.e0
    );
  }


  template<Number T>
  _Plane3<T> fast_reflect(const _Plane3<T> &a, const _Line3<T> &b) {
    return _Plane3<T>(
      - (T)2.0 * a.e3 * b.e23 * b.e12 - b.e23 * b.e23 * a.e1 + b.e12 * b.e12 * a.e1 - (T)2.0 * b.e23 * a.e2 * b.e31 + b.e31 * b.e31 * a.e1,
      b.e23 * b.e23 * a.e2 + b.e12 * b.e12 * a.e2 - (T)2.0 * b.e23 * b.e31 * a.e1 - (T)2.0 * a.e3 * b.e12 * b.e31 - a.e2 * b.e31 * b.e31,
      a.e3 * b.e23 * b.e23 + a.e3 * b.e31 * b.e31 - a.e3 * b.e12 * b.e12 - (T)2.0 * b.e23 * b.e12 * a.e1 - (T)2.0 * b.e12 * a.e2 * b.e31,
      - a.e0 * b.e31 * b.e31 - (T)2.0 * b.e12 * a.e2 * b.e01 - a.e0 * b.e23 * b.e23 + (T)2.0 * b.e23 * b.e03 * a.e2 + (T)2.0 * b.e12 * b.e02 * a.e1 - (T)2.0 * b.e03 * b.e31 * a.e1 - (T)2.0 * a.e3 * b.e23 * b.e02 - a.e0 * b.e12 * b.e12 + (T)2.0 * a.e3 * b.e31 * b.e01
    );
  }


  template<Number T>
  _Plane3<T> fast_reflect(const _Plane3<T> &a, const _Point3<T> &b) {
    return _Plane3<T>(
      + a.e1 * b.e123 * b.e123,
      + a.e2 * b.e123 * b.e123,
      + a.e3 * b.e123 * b.e123,
      - a.e0 * b.e123 * b.e123 - 2 * a.e1 * b.e123 * b.e032 - 2 * a.e2 * b.e123 * b.e013 - 2 * a.e3 * b.e123 * b.e021
    );
  }


  template<typename A, typename B>
  inline A reflect(const A &a, const B &b) {
    return fast_reflect(a, b) / magnitude_squared(b);
  }


  // ========================
  // = Comparison functions =
  // ========================


  template<Number T>
  inline bool is_approx_zero(const _Plane3<T> &a) {
    return is_approx_zero(*reinterpret_cast<const _Vec4<T>*>(&a));
  }
  

  template<Number T>
  inline bool is_approx_zero(const _Line3<T> &a) {
    return is_approx_zero(*reinterpret_cast<const _Vec3<T>*>(&a)) && is_approx_zero(*(1 + reinterpret_cast<const _Vec3<T>*>(&a)));
  }


  template<Number T>
  inline bool is_approx_zero(const _Point3<T> &a) {
    return is_approx_zero(*reinterpret_cast<const _Vec4<T>*>(&a));
  }


  // ================
  // = Type aliases =
  // ================


  typedef _Plane3<float> Plane3;
  typedef _Line3<float> Line3;
  typedef _Point3<float> Point3;

  typedef _Plane3<double> Plane3d;
  typedef _Line3<double> Line3d;
  typedef _Point3<double> Point3d;
}

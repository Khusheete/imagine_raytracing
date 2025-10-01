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


#include "concepts.hpp"
#include <cmath>


namespace kmath {


  // ===========
  // = Vector2 =
  // ===========
  

  template<Number T>
  struct _Vec2 {
    T x, y;

  public:
    _Vec2(): _Vec2(ZERO) {}
    _Vec2(const T x, const T y): x(x), y(y) {}

  public:
    inline T &operator[](const size_t index) { return reinterpret_cast<T*>(this)[index]; }
    inline const T &operator[](const size_t index) const { return reinterpret_cast<const T*>(this)[index]; }

  public:
    static const _Vec2<T> ZERO;
    static const _Vec2<T> ONE;
    static const _Vec2<T> INF;
    static const _Vec2<T> X;
    static const _Vec2<T> Y;
  };


  template<Number T>
  const _Vec2<T> _Vec2<T>::ZERO = _Vec2<T>((T)0.0, (T)0.0);
  template<Number T>
  const _Vec2<T> _Vec2<T>::ONE = _Vec2<T>((T)1.0, (T)1.0);
  template<Number T>
  const _Vec2<T> _Vec2<T>::INF = _Vec2<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());
  template<Number T>
  const _Vec2<T> _Vec2<T>::X = _Vec2<T>((T)1.0, (T)0.0);
  template<Number T>
  const _Vec2<T> _Vec2<T>::Y = _Vec2<T>((T)0.0, (T)1.0);


  // =====================
  // = Vector2 Functions =
  // =====================


  template<Number T>
  inline T length_squared(const _Vec2<T> &v) {
    return v.x * v.x + v.y * v.y;
  }
  

  template<Number T>
  inline T length(const _Vec2<T> &v) {
    return std::sqrt(length_squared(v));
  }


  template<Number T>
  inline T distance_squared(const _Vec2<T> &a, const _Vec2<T> &b) {
    return length_squared(b - a);
  }


  template<Number T>
  inline T distance(const _Vec2<T> &a, const _Vec2<T> &b) {
    return length(b - a);
  }
  

  template<Number T>
  inline _Vec2<T> normalized(const _Vec2<T> &v) {
    return v / length(v);
  }
    

  template<Number T>
  inline T dot(const _Vec2<T> &a, const _Vec2<T> &b) {
    return a.x * b.x + a.y * b.y;
  }


  // TOOD: FIXME
  template<Number T>
  inline _Vec2<T> apply(const _Vec2<T> &a, T (*op)(const T)) {
    return _Vec2<T>(
      op(a.x),
      op(a.y)
    );
  }


  template<Number T>
  inline _Vec2<T> apply(const _Vec2<T> &a, const _Vec2<T> &b, T (*op)(const T, const T)) {
    return _Vec2<T>(
      op(a.x, b.x),
      op(a.y, b.y)
    );
  }


  template<Number T>
  inline _Vec2<T> min(const _Vec2<T> &a, const _Vec2<T> &b) {
    return _Vec2<T>(
      std::min(a.x, b.x),
      std::min(a.y, b.y)
    );
  }


  template<Number T>
  inline _Vec2<T> max(const _Vec2<T> &a, const _Vec2<T> &b) {
    return _Vec2<T>(
      std::max(a.x, b.x),
      std::max(a.y, b.y)
    );
  }


  // =====================
  // = Vector2 Operators =
  // =====================


  template<Number T>
  inline _Vec2<T> operator+(const _Vec2<T> &a, const _Vec2<T> &b) {
    _Vec2<T> res(a);
    res += b;
    return res;
  }


  template<Number T>
  inline _Vec2<T> &operator+=(_Vec2<T> &a, const _Vec2<T> &b) {
    a.x += b.x;
    a.y += b.y;
    return a;
  }


  template<Number T>
  inline _Vec2<T> operator-(const _Vec2<T> &a, const _Vec2<T> &b) {
    _Vec2<T> res(a);
    res -= b;
    return res;
  }


  template<Number T>
  inline _Vec2<T> &operator-=(_Vec2<T> &a, const _Vec2<T> &b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
  }


  template<Number T>
  inline _Vec2<T> operator-(const _Vec2<T> &a) {
    return _Vec2<T>(
      -a.x, -a.y
    );
  }


  template<Number T>
  inline _Vec2<T> operator*(const _Vec2<T> &a, const _Vec2<T> &b) {
    _Vec2<T> res(a);
    res *= b;
    return res;
  }


  template<Number T>
  inline _Vec2<T> &operator*=(_Vec2<T> &a, const _Vec2<T> &b) {
    a.x *= b.x;
    a.y *= b.y;
    return a;
  }


  template<Number T>
  inline _Vec2<T> operator*(const T s, const _Vec2<T> &v) {
    return _Vec2<T>(
      s * v.x,
      s * v.y
    );
  }


  template<Number T>
  inline _Vec2<T> operator*(const _Vec2<T> &v, const T s) {
    return _Vec2<T>(
      v.x * s,
      v.y * s    
    );
  }


  template<Number T>
  inline _Vec2<T> &operator*=(_Vec2<T> &v, const T s) {
    v = v * s;
    return v;
  }


  template<Number T>
  inline _Vec2<T> operator/(const _Vec2<T> &v, const T s) {
    _Vec2<T> res(v);
    res /= s;
    return res;
  }


  template<Number T>
  inline _Vec2<T> &operator/=(_Vec2<T> &v, const T s) {
    const T scale = (T)1.0 / s;
    return v *= scale;
  }


  // ===========
  // = Vector3 =
  // ===========
  

  template<Number T>
  struct _Vec3 {
    T x, y, z;

  public:
    _Vec3(): _Vec3(ZERO) {}
    _Vec3(const T x, const T y, const T z): x(x), y(y), z(z) {}
    _Vec3(const _Vec2<T> &a, const T b): x(a.x), y(a.y), z(b) {}
    _Vec3(const T a, const _Vec2<T> &b): x(a), y(b.x), z(b.y) {}

  public:
    inline T &operator[](const size_t index) { return reinterpret_cast<T*>(this)[index]; }
    inline const T &operator[](const size_t index) const { return reinterpret_cast<const T*>(this)[index]; }

  public:
    static const _Vec3<T> ZERO;
    static const _Vec3<T> ONE;
    static const _Vec3<T> INF;
    static const _Vec3<T> X;
    static const _Vec3<T> Y;
    static const _Vec3<T> Z;
  };


  template<Number T>
  const _Vec3<T> _Vec3<T>::ZERO = _Vec3<T>((T)0.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Vec3<T> _Vec3<T>::ONE = _Vec3<T>((T)1.0, (T)1.0, (T)1.0);
  template<Number T>
  const _Vec3<T> _Vec3<T>::INF = _Vec3<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());
  template<Number T>
  const _Vec3<T> _Vec3<T>::X = _Vec3<T>((T)1.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Vec3<T> _Vec3<T>::Y = _Vec3<T>((T)0.0, (T)1.0, (T)0.0);
  template<Number T>
  const _Vec3<T> _Vec3<T>::Z = _Vec3<T>((T)0.0, (T)0.0, (T)1.0);


  // =====================
  // = Vector3 Functions =
  // =====================


  template<Number T>
  inline T length_squared(const _Vec3<T> &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
  }
  

  template<Number T>
  inline T length(const _Vec3<T> &v) {
    return std::sqrt(length_squared(v));
  }


  template<Number T>
  inline T distance_squared(const _Vec3<T> &a, const _Vec3<T> &b) {
    return length_squared(b - a);
  }


  template<Number T>
  inline T distance(const _Vec3<T> &a, const _Vec3<T> &b) {
    return length(b - a);
  }
  

  template<Number T>
  inline _Vec3<T> normalized(const _Vec3<T> &v) {
    return v / length(v);
  }
    

  template<Number T>
  inline T dot(const _Vec3<T> &a, const _Vec3<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }


  template<Number T>
  inline _Vec3<T> cross(const _Vec3<T> &a, const _Vec3<T> &b) {
    return _Vec3<T>(
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x    
    );
  }


  template<Number T>
  inline _Vec2<T> homogeneous_projection(const _Vec3<T> &v) {
    return _Vec2<T>(v.x, v.y, v.z) / v.w;
  }


  template<Number T>
  inline _Vec3<T> apply(const _Vec3<T> &a, T (*op)(const T)) {
    return _Vec3<T>(
      op(a.x),
      op(a.y),
      op(a.z)
    );
  }


  template<Number T>
  inline _Vec3<T> apply(const _Vec3<T> &a, const _Vec3<T> &b, T (*op)(const T, const T)) {
    return _Vec3<T>(
      op(a.x, b.x),
      op(a.y, b.y),
      op(a.z, b.z)
    );
  }


  template<Number T>
  inline _Vec3<T> min(const _Vec3<T> &a, const _Vec3<T> &b) {
    return _Vec3<T>(
      std::min(a.x, b.x),
      std::min(a.y, b.y),
      std::min(a.z, b.z)
    );
  }


  template<Number T>
  inline _Vec3<T> max(const _Vec3<T> &a, const _Vec3<T> &b) {
    return _Vec3<T>(
      std::max(a.x, b.x),
      std::max(a.y, b.y),
      std::max(a.z, b.z)
    );
  }
  

  // =====================
  // = Vector3 Operators =
  // =====================


  template<Number T>
  inline _Vec3<T> operator+(const _Vec3<T> &a, const _Vec3<T> &b) {
    _Vec3<T> res(a);
    res += b;
    return res;
  }


  template<Number T>
  inline _Vec3<T> &operator+=(_Vec3<T> &a, const _Vec3<T> &b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
  }


  template<Number T>
  inline _Vec3<T> operator-(const _Vec3<T> &a, const _Vec3<T> &b) {
    _Vec3<T> res(a);
    res -= b;
    return res;
  }


  template<Number T>
  inline _Vec3<T> &operator-=(_Vec3<T> &a, const _Vec3<T> &b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
  }


  template<Number T>
  inline _Vec3<T> operator-(const _Vec3<T> &a) {
    return _Vec3<T>(
      -a.x, -a.y, -a.z    
    );
  }


  template<Number T>
  inline _Vec3<T> operator*(const _Vec3<T> &a, const _Vec3<T> &b) {
    _Vec3<T> res(a);
    res *= b;
    return res;
  }


  template<Number T>
  inline _Vec3<T> &operator*=(_Vec3<T> &a, const _Vec3<T> &b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    return a;
  }


  template<Number T>
  inline _Vec3<T> operator*(const T s, const _Vec3<T> &v) {
    return _Vec3<T>(
      s * v.x,
      s * v.y,
      s * v.z    
    );
  }


  template<Number T>
  inline _Vec3<T> operator*(const _Vec3<T> &v, const T s) {
    return _Vec3<T>(
      v.x * s,
      v.y * s,
      v.z * s    
    );
  }


  template<Number T>
  inline _Vec3<T> &operator*=(_Vec3<T> &v, const T s) {
    v = v * s;
    return v;
  }


  template<Number T>
  inline _Vec3<T> operator/(const _Vec3<T> &v, const T s) {
    _Vec3<T> res(v);
    res /= s;
    return res;
  }


  template<Number T>
  inline _Vec3<T> &operator/=(_Vec3<T> &v, const T s) {
    const T scale = (T)1.0 / s;
    return v *= scale;
  }


  // ===========
  // = Vector4 =
  // ===========


  template<Number T>
  struct _Vec4 {
    T x, y, z, w;

  public:
    _Vec4(): _Vec4(ZERO) {}
    _Vec4(const T x, const T y, const T z, const T w): x(x), y(y), z(z), w(w) {}
    _Vec4(const _Vec2<T> &a, const T b, const T c): x(a.x), y(a.y), z(b), w(c) {}
    _Vec4(const T a, const _Vec2<T> &b, const T c): x(a), y(b.x), z(b.z), w(c) {}
    _Vec4(const T a, const T b, const _Vec2<T> &c): x(a), y(b), z(c.x), w(c.y) {}
    _Vec4(const _Vec2<T> &a, const _Vec2<T> &b): x(a.x), y(a.y), z(b.x), w(b.y) {}
    _Vec4(const _Vec3<T> &a, const T b): x(a.x), y(a.y), z(a.z), w(b) {}
    _Vec4(const T a, const _Vec3<T> &b): x(a), y(b.x), z(b.y), w(b.z) {}

  public:
    inline T &operator[](const size_t index) { return reinterpret_cast<T*>(this)[index]; }
    inline const T &operator[](const size_t index) const { return reinterpret_cast<const T*>(this)[index]; }

  public:
    static const _Vec4<T> ZERO;
    static const _Vec4<T> ONE;
    static const _Vec4<T> INF;
    static const _Vec4<T> X;
    static const _Vec4<T> Y;
    static const _Vec4<T> Z;
    static const _Vec4<T> W;
  };


  template<Number T>
  const _Vec4<T> _Vec4<T>::ZERO = _Vec4<T>((T)0.0, (T)0.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Vec4<T> _Vec4<T>::ONE = _Vec4<T>((T)1.0, (T)1.0, (T)1.0, (T)1.0);
  template<Number T>
  const _Vec4<T> _Vec4<T>::INF = _Vec4<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());
  template<Number T>
  const _Vec4<T> _Vec4<T>::X = _Vec4<T>((T)1.0, (T)0.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Vec4<T> _Vec4<T>::Y = _Vec4<T>((T)0.0, (T)1.0, (T)0.0, (T)0.0);
  template<Number T>
  const _Vec4<T> _Vec4<T>::Z = _Vec4<T>((T)0.0, (T)0.0, (T)1.0, (T)0.0);
  template<Number T>
  const _Vec4<T> _Vec4<T>::W = _Vec4<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0);


  // =====================
  // = Vector4 Functions =
  // =====================


  template<Number T>
  inline T length_squared(const _Vec4<T> &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
  }
  

  template<Number T>
  inline T length(const _Vec4<T> &v) {
    return std::sqrt(length_squared(v));
  }


  template<Number T>
  inline T distance_squared(const _Vec4<T> &a, const _Vec4<T> &b) {
    return length_squared(b - a);
  }


  template<Number T>
  inline T distance(const _Vec4<T> &a, const _Vec4<T> &b) {
    return length(b - a);
  }
  

  template<Number T>
  inline _Vec4<T> normalized(const _Vec4<T> &v) {
    return v / length(v);
  }
    

  template<Number T>
  inline T dot(const _Vec4<T> &a, const _Vec4<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  }


  template<Number T>
  inline _Vec3<T> homogeneous_projection(const _Vec4<T> &v) {
    return _Vec3<T>(v.x, v.y, v.z) / v.w;
  }


  template<Number T>
  inline _Vec4<T> apply(const _Vec4<T> &a, T (*op)(const T)) {
    return _Vec4<T>(
      op(a.x),
      op(a.y),
      op(a.z),
      op(a.w)
    );
  }


  template<Number T>
  inline _Vec4<T> apply(const _Vec4<T> &a, const _Vec4<T> &b, T (*op)(const T, const T)) {
    return _Vec4<T>(
      op(a.x, b.x),
      op(a.y, b.y),
      op(a.z, b.z),
      op(a.w, b.w)
    );
  }


  template<Number T>
  inline _Vec4<T> min(const _Vec4<T> &a, const _Vec4<T> &b) {
    return _Vec4<T>(
      std::min(a.x, b.x),
      std::min(a.y, b.y),
      std::min(a.z, b.z),
      std::min(a.w, b.w)
    );
  }


  template<Number T>
  inline _Vec4<T> max(const _Vec4<T> &a, const _Vec4<T> &b) {
    return _Vec4<T>(
      std::max(a.x, b.x),
      std::max(a.y, b.y),
      std::max(a.z, b.z),
      std::max(a.w, b.w)
    );
  }
  

  // =====================
  // = Vector4 Operators =
  // =====================


  template<Number T>
  inline _Vec4<T> operator+(const _Vec4<T> &a, const _Vec4<T> &b) {
    _Vec4<T> res(a);
    res += b;
    return res;
  }


  template<Number T>
  inline _Vec4<T> &operator+=(_Vec4<T> &a, const _Vec4<T> &b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
    return a;
  }


  template<Number T>
  inline _Vec4<T> operator-(const _Vec4<T> &a, const _Vec4<T> &b) {
    _Vec4<T> res(a);
    res -= b;
    return res;
  }


  template<Number T>
  inline _Vec4<T> &operator-=(_Vec4<T> &a, const _Vec4<T> &b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    a.w -= b.w;
    return a;
  }


  template<Number T>
  inline _Vec4<T> operator-(const _Vec4<T> &a) {
    return _Vec4<T>(
      -a.x, -a.y, -a.z, -a.w    
    );
  }


  template<Number T>
  inline _Vec4<T> operator*(const _Vec4<T> &a, const _Vec4<T> &b) {
    _Vec4<T> res(a);
    res *= b;
    return res;
  }


  template<Number T>
  inline _Vec4<T> &operator*=(_Vec4<T> &a, const _Vec4<T> &b) {
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    a.w *= b.w;
    return a;
  }


  template<Number T>
  inline _Vec4<T> operator*(const T s, const _Vec4<T> &v) {
    return _Vec4<T>(
      s * v.x,
      s * v.y,
      s * v.z,
      s * v.w
    );
  }


  template<Number T>
  inline _Vec4<T> operator*(const _Vec4<T> &v, const T s) {
    return _Vec4<T>(
      v.x * s,
      v.y * s,
      v.z * s,
      v.w * s
    );
  }


  template<Number T>
  inline _Vec4<T> &operator*=(_Vec4<T> &v, const T s) {
    v = v * s;
    return v;
  }


  template<Number T>
  inline _Vec4<T> operator/(const _Vec4<T> &v, const T s) {
    _Vec4<T> res(v);
    res /= s;
    return res;
  }


  template<Number T>
  inline _Vec4<T> &operator/=(_Vec4<T> &v, const T s) {
    const T scale = (T)1.0 / s;
    return v *= scale;
  }



  // ================
  // = Type aliases =
  // ================

  typedef _Vec2<float> Vec2;
  typedef _Vec2<double> Vec2d;
  typedef _Vec2<int> Vec2i;
  typedef _Vec2<long> Vec2l;

  typedef _Vec3<float> Vec3;
  typedef _Vec3<double> Vec3d;
  typedef _Vec3<int> Vec3i;
  typedef _Vec3<long> Vec3l;

  typedef _Vec4<float> Vec4;
  typedef _Vec4<double> Vec4d;
  typedef _Vec4<int> Vec4i;
  typedef _Vec4<long> Vec4l;
}

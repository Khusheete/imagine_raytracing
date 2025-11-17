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

#include <cmath>


namespace kmath {

  constexpr const char* PGA3D_BASIS[] = {
    "1"   ,
    "e0"  , "e1"  , "e2"  , "e3"  ,
    "e01" , "e02" , "e03" , "e12" , "e31", "e23",
    "e021", "e013", "e032", "e123",
    "e0123"
  };


  template<Number T>
  class _Mvec3 {
  public:
    enum class Basis : size_t {
      s = 0,
      e0   , e1  , e2  , e3  ,
      e01  , e02 , e03 , e12 , e31, e23,
      e021 , e013, e032, e123,
      e0123
    };

  
  public:
    inline _Mvec3<T> grade(const int g) const {
      _Mvec3<T> res;
      switch (g) {
      case 0:
        res[Basis::s] = (*this)[Basis::s];
        break;
      case 1:
        res[Basis::e0] = (*this)[Basis::e0];
        res[Basis::e1] = (*this)[Basis::e1];
        res[Basis::e2] = (*this)[Basis::e2];
        res[Basis::e3] = (*this)[Basis::e3];
        break;
      case 2:
        res[Basis::e01] = (*this)[Basis::e01];
        res[Basis::e02] = (*this)[Basis::e02];
        res[Basis::e03] = (*this)[Basis::e03];
        res[Basis::e23] = (*this)[Basis::e23];
        res[Basis::e31] = (*this)[Basis::e31];
        res[Basis::e12] = (*this)[Basis::e12];
        break;
      case 3:
        res[Basis::e032] = (*this)[Basis::e032];
        res[Basis::e013] = (*this)[Basis::e013];
        res[Basis::e021] = (*this)[Basis::e021];
        res[Basis::e123] = (*this)[Basis::e123];
        break;
      case 4:
        res[Basis::e0123] = res[Basis::e0123];
        break;
      }
      return res;
    }


    // Hodge dual
    inline _Mvec3<T> hdual() const {
      _Mvec3<T> res;
      res[0]  = data[15];
      res[1]  = data[14];
      res[2]  = data[13];
      res[3]  = data[12];
      res[4]  = data[11];
      res[5]  = data[10];
      res[6]  = data[9];
      res[7]  = data[8];
      res[8]  = data[7];
      res[9]  = data[6];
      res[10] = data[5];
      res[11] = data[4];
      res[12] = data[3];
      res[13] = data[2];
      res[14] = data[1];
      res[15] = data[0];
      return res;
    }


    // Reverse
    inline _Mvec3<T> rev() const {
      _Mvec3<T> res;
      res[0]  = data[0];
      res[1]  = data[1];
      res[2]  = data[2];
      res[3]  = data[3];
      res[4]  = data[4];
      res[5]  = -data[5];
      res[6]  = -data[6];
      res[7]  = -data[7];
      res[8]  = -data[8];
      res[9]  = -data[9];
      res[10] = -data[10];
      res[11] = -data[11];
      res[12] = -data[12];
      res[13] = -data[13];
      res[14] = -data[14];
      res[15] = data[15];
      return res;
    }


    // Clifford conjugate
    inline _Mvec3<T> conj() const {
      _Mvec3<T> res;
      res[0]  = data[0];
      res[1]  = -data[1];
      res[2]  = -data[2];
      res[3]  = -data[3];
      res[4]  = -data[4];
      res[5]  = -data[5];
      res[6]  = -data[6];
      res[7]  = -data[7];
      res[8]  = -data[8];
      res[9]  = -data[9];
      res[10] = -data[10];
      res[11] = data[11];
      res[12] = data[12];
      res[13] = data[13];
      res[14] = data[14];
      res[15] = data[15];
      return res;
    }


    inline T norm() const {
      return std::sqrt(norm_squared());
    }


    inline T inorm() const {
      return std::sqrt(inorm_squared());
    }


    inline T norm_squared() const {
      return (*this * this->rev())[0];
    }


    inline T inorm_squared() const {
      return hdual().norm_squared();
    }


    inline _Mvec3<T> plane_normalize() const {
      T norm = length(kmath::_Vec3<T>((*this)[Basis::e1], (*this)[Basis::e2], (*this)[Basis::e3]));
      return (*this) / norm;
    }


    inline _Mvec3<T> line_normalize() const {
      T norm = length(kmath::_Vec3<T>((*this)[Basis::e23], (*this)[Basis::e31], (*this)[Basis::e12]));
      return (*this) / norm;
    }


    inline _Mvec3<T> vanishing_line_normalize() const {
      T norm = length(kmath::_Vec3<T>((*this)[Basis::e01], (*this)[Basis::e02], (*this)[Basis::e03]));
      return (*this) / norm;
    }


    inline _Mvec3<T> point_normalize() const {
      return (*this) / (*this)[Basis::e123];
    }


    inline T &operator[](size_t idx) { return data[idx]; }
    inline const T &operator[](size_t idx) const { return data[idx]; }
    inline T &operator[](Basis idx) { return data[(size_t)idx]; }
    inline const T &operator[](Basis idx) const { return data[(size_t)idx]; }

  public:
    static _Mvec3<T> plane(const T a, const T b, const T c, const T d) {
      _Mvec3<T> res;
      res[Basis::e0] = -d;
      res[Basis::e1] = a;
      res[Basis::e2] = b;
      res[Basis::e3] = c;
      return res;
    }


    static _Mvec3<T> vanishing_plane(const T d) {
      return _Mvec3<T>(d, 1);
    }


    static _Mvec3<T> line(const T ux, const T uy, const T uz) {
      _Mvec3<T> res;
      res[Basis::e23] = ux;
      res[Basis::e31] = uy;
      res[Basis::e12] = uz;
      return res;
    }


    static _Mvec3<T> line_at(const T ux, const T uy, const T uz, const T px, const T py, const T pz) {
      _Mvec3<T> res;
      res[Basis::e01] = py * uz - pz * uy;
      res[Basis::e02] = pz * ux - px * uz;
      res[Basis::e03] = px * uy - py * ux;
      res[Basis::e23] = ux;
      res[Basis::e31] = uy;
      res[Basis::e12] = uz;
      return res;
    }


    static _Mvec3<T> line_plucker(const T ux, const T uy, const T uz, const T mx, const T my, const T mz) {
      _Mvec3<T> res;
      res[Basis::e23] = ux;
      res[Basis::e31] = uy;
      res[Basis::e12] = uz;
      res[Basis::e01] = mx;
      res[Basis::e02] = my;
      res[Basis::e03] = mz;
      return res;
    }


    static _Mvec3<T> vanishing_line(const T ux, const T uy, const T uz) {
      _Mvec3<T> res;
      res[Basis::e01] = ux;
      res[Basis::e02] = uy;
      res[Basis::e03] = uz;
      return res;
    }


    static _Mvec3<T> line(const _Vec3<T> &u) {
      return line(u.x, u.y, u.z);
    }


    static _Mvec3<T> line_at(const _Vec3<T> &u, const _Vec3<T> &pos) {
      return line_at(u.x, u.y, u.z, pos.x, pos.y, pos.z);
    }


    static _Mvec3<T> vanishing_line(const _Vec3<T> &u) {
      return vanishing_line(u.x, u.y, u.z);
    }


    static _Mvec3<T> point(const T x, const T y, const T z) {
      _Mvec3<T> res;
      res[Basis::e123] = (T)1.0;
      res[Basis::e032] = x;
      res[Basis::e013] = y;
      res[Basis::e021] = z;
      return res;
    }


    static _Mvec3<T> direction(const T x, const T y, const T z) {
      _Mvec3<T> res;
      res[Basis::e032] = x;
      res[Basis::e013] = y;
      res[Basis::e021] = z;
      return res;
    }


    static _Mvec3<T> point(const _Vec3<T> &pos) {
      return point(pos.x, pos.y, pos.z);
    }


    static _Mvec3<T> direction(const _Vec3<T> &dir) {
      return direction(dir.x, dir.y, dir.z);
    }


    _Mvec3() {}

    _Mvec3(const T values[16]) {
      data[0]  = values[0];
      data[1]  = values[1];
      data[2]  = values[2];
      data[3]  = values[3];
      data[4]  = values[4];
      data[5]  = values[5];
      data[6]  = values[6];
      data[7]  = values[7];
      data[8]  = values[8];
      data[9]  = values[9];
      data[10] = values[10];
      data[11] = values[11];
      data[12] = values[12];
      data[13] = values[13];
      data[14] = values[14];
      data[15] = values[15];
    }

    _Mvec3(const T val, const size_t idx) {
      data[idx] = val;
    }


    _Mvec3(const T val, const Basis idx) {
      data[idx] = val;
    }


  public:
    static const _Mvec3<T> ZERO;  
    static const _Mvec3<T> ONE;
    static const _Mvec3<T> PSEUDOSCALAR;
    static const _Mvec3<T> INF_PLANE;

    static const _Mvec3<T> e0;
    static const _Mvec3<T> e1;
    static const _Mvec3<T> e2;
    static const _Mvec3<T> e3;
    static const _Mvec3<T> e01;
    static const _Mvec3<T> e02;
    static const _Mvec3<T> e03;
    static const _Mvec3<T> e12;
    static const _Mvec3<T> e31;
    static const _Mvec3<T> e23;
    static const _Mvec3<T> e021;
    static const _Mvec3<T> e013;
    static const _Mvec3<T> e032;
    static const _Mvec3<T> e123;
    static const _Mvec3<T> e0123;

  private:
      T data[16] = {0};
  };


  // Constants
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::ZERO = _Mvec3<T>();
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::PSEUDOSCALAR = _Mvec3<T>((T)1.0, 0);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::INF_PLANE = _Mvec3<T>(-(T)1.0, 0);

  template<Number T>
  const _Mvec3<T> _Mvec3<T>::ONE = _Mvec3<T>((T)1.0, 0);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e0 = _Mvec3<T>((T)1.0, 1);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e1 = _Mvec3<T>((T)1.0, 2);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e2 = _Mvec3<T>((T)1.0, 3);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e3 = _Mvec3<T>((T)1.0, 4);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e01 = _Mvec3<T>((T)1.0, 5);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e02 = _Mvec3<T>((T)1.0, 6);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e03 = _Mvec3<T>((T)1.0, 7);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e12 = _Mvec3<T>((T)1.0, 8);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e31 = _Mvec3<T>((T)1.0, 9);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e23 = _Mvec3<T>((T)1.0, 10);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e021 = _Mvec3<T>((T)1.0, 11);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e013 = _Mvec3<T>((T)1.0, 12);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e032 = _Mvec3<T>((T)1.0, 13);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e123 = _Mvec3<T>((T)1.0, 14);
  template<Number T>
  const _Mvec3<T> _Mvec3<T>::e0123 = _Mvec3<T>((T)1.0, 15);


  // Geometric product
  template<Number T>
  _Mvec3<T> operator*(const _Mvec3<T> &a, const _Mvec3<T> &b) {
    _Mvec3<T> res;
    res[0]  = b[0]  * a[0] + b[2]  * a[2] + b[3]  * a[3] + b[4]  * a[4] - b[8]  * a[8] - b[9]  * a[9] - b[10] * a[10] - b[14] * a[14];
    res[1]  = b[1]  * a[0] + b[0]  * a[1] - b[5]  * a[2] - b[6]  * a[3] - b[7]  * a[4] + b[2]  * a[5] + b[3]  * a[6]  + b[4]  * a[7] + b[11] * a[8] + b[12] * a[9] + b[13] * a[10] + b[8]  * a[11] + b[9]  * a[12] + b[10] * a[13] + b[15] * a[14] - b[14] * a[15];
    res[2]  = b[2]  * a[0] + b[0]  * a[2] - b[8]  * a[3] + b[9]  * a[4] + b[3]  * a[8] - b[4]  * a[9] - b[14] * a[10] - b[10] * a[14];
    res[3]  = b[3]  * a[0] + b[8]  * a[2] + b[0]  * a[3] - b[10] * a[4] - b[2]  * a[8] - b[14] * a[9] + b[4]  * a[10] - b[9]  * a[14];
    res[4]  = b[4]  * a[0] - b[9]  * a[2] + b[10] * a[3] + b[0]  * a[4] - b[14] * a[8] + b[2]  * a[9] - b[3]  * a[10] - b[8]  * a[14];
    res[5]  = b[5]  * a[0] + b[2]  * a[1] - b[1]  * a[2] - b[11] * a[3] + b[12] * a[4] + b[0]  * a[5] - b[8]  * a[6]  + b[9]  * a[7] + b[6]  * a[8] - b[7]  * a[9] - b[15] * a[10] - b[3]  * a[11] + b[4]  * a[12] + b[14] * a[13] - b[13] * a[14] - b[10] * a[15];
    res[6]  = b[6]  * a[0] + b[3]  * a[1] + b[11] * a[2] - b[1]  * a[3] - b[13] * a[4] + b[8]  * a[5] + b[0]  * a[6]  - b[10] * a[7] - b[5]  * a[8] - b[15] * a[9] + b[7]  * a[10] + b[2]  * a[11] + b[14] * a[12] - b[4]  * a[13] - b[12] * a[14] - b[9]  * a[15];
    res[7]  = b[7]  * a[0] + b[4]  * a[1] - b[12] * a[2] + b[13] * a[3] - b[1]  * a[4] - b[9]  * a[5] + b[10] * a[6]  + b[0]  * a[7] - b[15] * a[8] + b[5]  * a[9] - b[6]  * a[10] + b[14] * a[11] - b[2]  * a[12] + b[3]  * a[13] - b[11] * a[14] - b[8]  * a[15];
    res[8]  = b[8]  * a[0] + b[3]  * a[2] - b[2]  * a[3] + b[14] * a[4] + b[0]  * a[8] + b[10] * a[9] - b[9]  * a[10] + b[4]  * a[14];
    res[9]  = b[9]  * a[0] - b[4]  * a[2] + b[14] * a[3] + b[2]  * a[4] - b[10] * a[8] + b[0]  * a[9] + b[8]  * a[10] + b[3]  * a[14];
    res[10] = b[10] * a[0] + b[14] * a[2] + b[4]  * a[3] - b[3]  * a[4] + b[9]  * a[8] - b[8]  * a[9] + b[0]  * a[10] + b[2]  * a[14];
    res[11] = b[11] * a[0] - b[8]  * a[1] + b[6]  * a[2] - b[5]  * a[3] + b[15] * a[4] - b[3]  * a[5] + b[2]  * a[6]  - b[14] * a[7] - b[1]  * a[8] + b[13] * a[9] - b[12] * a[10] + b[0]  * a[11] + b[10] * a[12] - b[9]  * a[13] + b[7]  * a[14] - b[4]  * a[15];
    res[12] = b[12] * a[0] - b[9]  * a[1] - b[7]  * a[2] + b[15] * a[3] + b[5]  * a[4] + b[4]  * a[5] - b[14] * a[6]  - b[2]  * a[7] - b[13] * a[8] - b[1]  * a[9] + b[11] * a[10] - b[10] * a[11] + b[0]  * a[12] + b[8]  * a[13] + b[6]  * a[14] - b[3]  * a[15];
    res[13] = b[13] * a[0] - b[10] * a[1] + b[15] * a[2] + b[7]  * a[3] - b[6]  * a[4] - b[14] * a[5] - b[4]  * a[6]  + b[3]  * a[7] + b[12] * a[8] - b[11] * a[9] - b[1]  * a[10] + b[9]  * a[11] - b[8]  * a[12] + b[0]  * a[13] + b[5]  * a[14] - b[2]  * a[15];
    res[14] = b[14] * a[0] + b[10] * a[2] + b[9]  * a[3] + b[8]  * a[4] + b[4]  * a[8] + b[3]  * a[9] + b[2]  * a[10] + b[0]  * a[14];
    res[15] = b[15] * a[0] + b[14] * a[1] + b[13] * a[2] + b[12] * a[3] + b[11] * a[4] + b[10] * a[5] + b[9]  * a[6]  + b[8]  * a[7] + b[7]  * a[8] + b[6]  * a[9] + b[5]  * a[10] - b[4]  * a[11] - b[3]  * a[12] - b[2]  * a[13] - b[1]  * a[14] + b[0]  * a[15];
    return res;
  }


  // Outer product
  template<Number T>
  _Mvec3<T> operator&(const _Mvec3<T> &a, const _Mvec3<T> &b) {
    _Mvec3<T> res;
    res[0]  = b[0]  * a[0];
    res[1]  = b[1]  * a[0] + b[0]  * a[1];
    res[2]  = b[2]  * a[0] + b[0]  * a[2];
    res[3]  = b[3]  * a[0] + b[0]  * a[3];
    res[4]  = b[4]  * a[0] + b[0]  * a[4];
    res[5]  = b[5]  * a[0] + b[2]  * a[1] - b[1]  * a[2] + b[0]  * a[5];
    res[6]  = b[6]  * a[0] + b[3]  * a[1] - b[1]  * a[3] + b[0]  * a[6];
    res[7]  = b[7]  * a[0] + b[4]  * a[1] - b[1]  * a[4] + b[0]  * a[7];
    res[8]  = b[8]  * a[0] + b[3]  * a[2] - b[2]  * a[3] + b[0]  * a[8];
    res[9]  = b[9]  * a[0] - b[4]  * a[2] + b[2]  * a[4] + b[0]  * a[9];
    res[10] = b[10] * a[0] + b[4]  * a[3] - b[3]  * a[4] + b[0]  * a[10];
    res[11] = b[11] * a[0] - b[8]  * a[1] + b[6]  * a[2] - b[5]  * a[3] - b[3]  * a[5] + b[2]  * a[6] - b[1] * a[8]  + b[0] * a[11];
    res[12] = b[12] * a[0] - b[9]  * a[1] - b[7]  * a[2] + b[5]  * a[4] + b[4]  * a[5] - b[2]  * a[7] - b[1] * a[9]  + b[0] * a[12];
    res[13] = b[13] * a[0] - b[10] * a[1] + b[7]  * a[3] - b[6]  * a[4] - b[4]  * a[6] + b[3]  * a[7] - b[1] * a[10] + b[0] * a[13];
    res[14] = b[14] * a[0] + b[10] * a[2] + b[9]  * a[3] + b[8]  * a[4] + b[4]  * a[8] + b[3]  * a[9] + b[2] * a[10] + b[0] * a[14];
    res[15] = b[15] * a[0] + b[14] * a[1] + b[13] * a[2] + b[12] * a[3] + b[11] * a[4] + b[10] * a[5] + b[9] * a[6]  + b[8] * a[7] + b[7] * a[8] + b[6] * a[9] + b[5] * a[10] - b[4] * a[11] - b[3] * a[12] - b[2] * a[13] - b[1] * a[14] + b[0] * a[15];
    return res;    
  }


  // Regressive product
  template<Number T>
  _Mvec3<T> operator|(const _Mvec3<T> &a, const _Mvec3<T> &b) {
    _Mvec3<T> res;    
    res[15] = 1 * (a[15] * b[15]);
    res[14] = a[14] * b[15] + a[15] * b[14];
    res[13] = a[13] * b[15] + a[15] * b[13];
    res[12] = a[12] * b[15] + a[15] * b[12];
    res[11] = a[11] * b[15] + a[15] * b[11];
    res[10] = a[10] * b[15] + a[13] * b[14] - a[14] * b[13] + a[15] * b[10];
    res[9]  = a[9]  * b[15] + a[12] * b[14] - a[14] * b[12] + a[15] * b[9];
    res[8]  = a[8]  * b[15] + a[11] * b[14] - a[14] * b[11] + a[15] * b[8];
    res[7]  = a[7]  * b[15] + a[12] * b[13] - a[13] * b[12] + a[15] * b[7];
    res[6]  = a[6]  * b[15] - a[11] * b[13] + a[13] * b[11] + a[15] * b[6];
    res[5]  = a[5]  * b[15] + a[11] * b[12] - a[12] * b[11] + a[15] * b[5];
    res[4]  = a[4]  * b[15] + a[7]  * b[14] - a[9]  * b[13] + a[10] * b[12] + a[12] * b[10] - a[13] * b[9]  + a[14] * b[7] + a[15] * b[4];
    res[3]  = a[3]  * b[15] + a[6]  * b[14] + a[8]  * b[13] - a[10] * b[11] - a[11] * b[10] + a[13] * b[8]  + a[14] * b[6] + a[15] * b[3];
    res[2]  = a[2]  * b[15] + a[5]  * b[14] - a[8]  * b[12] + a[9]  * b[11] + a[11] * b[9]  - a[12] * b[8]  + a[14] * b[5] + a[15] * b[2];
    res[1]  = a[1]  * b[15] - a[5]  * b[13] - a[6]  * b[12] - a[7]  * b[11] - a[11] * b[7]  - a[12] * b[6]  - a[13] * b[5] + a[15] * b[1];
    res[0]  = a[0]  * b[15] - a[1]  * b[14] - a[2]  * b[13] - a[3]  * b[12] - a[4]  * b[11] + a[5]  * b[10] + a[6]  * b[9] + a[7]  * b[8] + a[8] * b[7] + a[9] * b[6] + a[10] * b[5] + a[11] * b[4] + a[12] * b[3] + a[13] * b[2] + a[14] * b[1] + a[15] * b[0];
    return res;
  }


  // Inner product
  template<Number T>
  _Mvec3<T> operator||(const _Mvec3<T> &a, const _Mvec3<T> &b) {
    _Mvec3<T> res;
    res[0]  = b[0]  * a[0] + b[2]  * a[2] + b[3]  * a[3]  + b[4]  * a[4] - b[8]  * a[8]  - b[9]  * a[9]  - b[10] * a[10] - b[14] * a[14];
    res[1]  = b[1]  * a[0] + b[0]  * a[1] - b[5]  * a[2]  - b[6]  * a[3] - b[7]  * a[4]  + b[2]  * a[5]  + b[3]  * a[6]  + b[4]  * a[7] + b[11] * a[8] + b[12] * a[9] + b[13] * a[10] + b[8] * a[11] + b[9] * a[12] + b[10] * a[13] + b[15] * a[14] - b[14] * a[15];
    res[2]  = b[2]  * a[0] + b[0]  * a[2] - b[8]  * a[3]  + b[9]  * a[4] + b[3]  * a[8]  - b[4]  * a[9]  - b[14] * a[10] - b[10] * a[14];
    res[3]  = b[3]  * a[0] + b[8]  * a[2] + b[0]  * a[3]  - b[10] * a[4] - b[2]  * a[8]  - b[14] * a[9]  + b[4]  * a[10] - b[9]  * a[14];
    res[4]  = b[4]  * a[0] - b[9]  * a[2] + b[10] * a[3]  + b[0]  * a[4] - b[14] * a[8]  + b[2]  * a[9]  - b[3]  * a[10] - b[8]  * a[14];
    res[5]  = b[5]  * a[0] - b[11] * a[3] + b[12] * a[4]  + b[0]  * a[5] - b[15] * a[10] - b[3]  * a[11] + b[4]  * a[12] - b[10] * a[15];
    res[6]  = b[6]  * a[0] + b[11] * a[2] - b[13] * a[4]  + b[0]  * a[6] - b[15] * a[9]  + b[2]  * a[11] - b[4]  * a[13] - b[9]  * a[15];
    res[7]  = b[7]  * a[0] - b[12] * a[2] + b[13] * a[3]  + b[0]  * a[7] - b[15] * a[8]  - b[2]  * a[12] + b[3]  * a[13] - b[8]  * a[15];
    res[8]  = b[8]  * a[0] + b[14] * a[4] + b[0]  * a[8]  + b[4]  * a[14];
    res[9]  = b[9]  * a[0] + b[14] * a[3] + b[0]  * a[9]  + b[3]  * a[14];
    res[10] = b[10] * a[0] + b[14] * a[2] + b[0]  * a[10] + b[2]  * a[14];
    res[11] = b[11] * a[0] + b[15] * a[4] + b[0]  * a[11] - b[4]  * a[15];
    res[12] = b[12] * a[0] + b[15] * a[3] + b[0]  * a[12] - b[3]  * a[15];
    res[13] = b[13] * a[0] + b[15] * a[2] + b[0]  * a[13] - b[2]  * a[15];
    res[14] = b[14] * a[0] + b[0]  * a[14];
    res[15] = b[15] * a[0] + b[0]  * a[15];
    return res;
  }


  // Multivector addition
  template<Number T>
  inline _Mvec3<T> operator+(const _Mvec3<T> &a, const _Mvec3<T> &b) {
    _Mvec3<T> res;
    res[0]  = a[0]  + b[0];
    res[1]  = a[1]  + b[1];
    res[2]  = a[2]  + b[2];
    res[3]  = a[3]  + b[3];
    res[4]  = a[4]  + b[4];
    res[5]  = a[5]  + b[5];
    res[6]  = a[6]  + b[6];
    res[7]  = a[7]  + b[7];
    res[8]  = a[8]  + b[8];
    res[9]  = a[9]  + b[9];
    res[10] = a[10] + b[10];
    res[11] = a[11] + b[11];
    res[12] = a[12] + b[12];
    res[13] = a[13] + b[13];
    res[14] = a[14] + b[14];
    res[15] = a[15] + b[15];
    return res;    
  }


  // Multivector subtraction
  template<Number T>
  inline _Mvec3<T> operator-(const _Mvec3<T> &a, const _Mvec3<T> &b) {
    _Mvec3<T> res;
    res[0]  = a[0]  - b[0];
    res[1]  = a[1]  - b[1];
    res[2]  = a[2]  - b[2];
    res[3]  = a[3]  - b[3];
    res[4]  = a[4]  - b[4];
    res[5]  = a[5]  - b[5];
    res[6]  = a[6]  - b[6];
    res[7]  = a[7]  - b[7];
    res[8]  = a[8]  - b[8];
    res[9]  = a[9]  - b[9];
    res[10] = a[10] - b[10];
    res[11] = a[11] - b[11];
    res[12] = a[12] - b[12];
    res[13] = a[13] - b[13];
    res[14] = a[14] - b[14];
    res[15] = a[15] - b[15];
    return res;    
  }


  // Multivector opposite
  template<Number T>
  inline _Mvec3<T> operator-(const _Mvec3<T> &a) {
    _Mvec3<T> res;
    res[0]  = -a[0];
    res[1]  = -a[1];
    res[2]  = -a[2];
    res[3]  = -a[3];
    res[4]  = -a[4];
    res[5]  = -a[5];
    res[6]  = -a[6];
    res[7]  = -a[7];
    res[8]  = -a[8];
    res[9]  = -a[9];
    res[10] = -a[10];
    res[11] = -a[11];
    res[12] = -a[12];
    res[13] = -a[13];
    res[14] = -a[14];
    res[15] = -a[15];
    return res;    
  }


  // Scalar/multivector multiplication
  template<Number T>
  inline _Mvec3<T> operator*(const T a, const _Mvec3<T> &b) {
    _Mvec3<T> res;
    res[0]  = a * b[0];
    res[1]  = a * b[1];
    res[2]  = a * b[2];
    res[3]  = a * b[3];
    res[4]  = a * b[4];
    res[5]  = a * b[5];
    res[6]  = a * b[6];
    res[7]  = a * b[7];
    res[8]  = a * b[8];
    res[9]  = a * b[9];
    res[10] = a * b[10];
    res[11] = a * b[11];
    res[12] = a * b[12];
    res[13] = a * b[13];
    res[14] = a * b[14];
    res[15] = a * b[15];
    return res;    
  }


  // Multivector/scalar multiplication
  template<Number T>
  inline _Mvec3<T> operator*(const _Mvec3<T> &a, const T b) {
    _Mvec3<T> res;
    res[0]  = b * a[0];
    res[1]  = b * a[1];
    res[2]  = b * a[2];
    res[3]  = b * a[3];
    res[4]  = b * a[4];
    res[5]  = b * a[5];
    res[6]  = b * a[6];
    res[7]  = b * a[7];
    res[8]  = b * a[8];
    res[9]  = b * a[9];
    res[10] = b * a[10];
    res[11] = b * a[11];
    res[12] = b * a[12];
    res[13] = b * a[13];
    res[14] = b * a[14];
    res[15] = b * a[15];
    return res;    
  }


  // Multivector/scalar division
  template<Number T>
  inline _Mvec3<T> operator/(const _Mvec3<T> &a, const T b) {
    _Mvec3<T> res;
    res[0]  = a[0]  / b;
    res[1]  = a[1]  / b;
    res[2]  = a[2]  / b;
    res[3]  = a[3]  / b;
    res[4]  = a[4]  / b;
    res[5]  = a[5]  / b;
    res[6]  = a[6]  / b;
    res[7]  = a[7]  / b;
    res[8]  = a[8]  / b;
    res[9]  = a[9]  / b;
    res[10] = a[10] / b;
    res[11] = a[11] / b;
    res[12] = a[12] / b;
    res[13] = a[13] / b;
    res[14] = a[14] / b;
    res[15] = a[15] / b;
    return res;    
  }


  // Scalar/multivector addition
  template<Number T>
  inline _Mvec3<T> operator+(const T a, const _Mvec3<T> &b) {
    _Mvec3<T> res;
    res[0]  = a + b[0];
    res[1]  = b[1];
    res[2]  = b[2];
    res[3]  = b[3];
    res[4]  = b[4];
    res[5]  = b[5];
    res[6]  = b[6];
    res[7]  = b[7];
    res[8]  = b[8];
    res[9]  = b[9];
    res[10] = b[10];
    res[11] = b[11];
    res[12] = b[12];
    res[13] = b[13];
    res[14] = b[14];
    res[15] = b[15];
    return res;    
  }


  // Multivector/scalar addition
  template<Number T>
  inline _Mvec3<T> operator+(const _Mvec3<T> &a, const T b) {
    _Mvec3<T> res;
    res[0]  = b + a[0];
    res[1]  = a[1];
    res[2]  = a[2];
    res[3]  = a[3];
    res[4]  = a[4];
    res[5]  = a[5];
    res[6]  = a[6];
    res[7]  = a[7];
    res[8]  = a[8];
    res[9]  = a[9];
    res[10] = a[10];
    res[11] = a[11];
    res[12] = a[12];
    res[13] = a[13];
    res[14] = a[14];
    res[15] = a[15];
    return res;    
  }


  // Scalar/multivector subtraction
  template<Number T>
  inline _Mvec3<T> operator-(const T a, const _Mvec3<T> &b) {
    _Mvec3<T> res;
    res[0]  = a - b[0];
    res[1]  = - b[1];
    res[2]  = - b[2];
    res[3]  = - b[3];
    res[4]  = - b[4];
    res[5]  = - b[5];
    res[6]  = - b[6];
    res[7]  = - b[7];
    res[8]  = - b[8];
    res[9]  = - b[9];
    res[10] = - b[10];
    res[11] = - b[11];
    res[12] = - b[12];
    res[13] = - b[13];
    res[14] = - b[14];
    res[15] = - b[15];
    return res;    
  }


  // Multivector/scalar subtraction
  template<Number T>
  inline _Mvec3<T> operator-(const _Mvec3<T> &a, const T b) {
    _Mvec3<T> res;
    res[0]  = a[0] - b;
    res[1]  = a[1];
    res[2]  = a[2];
    res[3]  = a[3];
    res[4]  = a[4];
    res[5]  = a[5];
    res[6]  = a[6];
    res[7]  = a[7];
    res[8]  = a[8];
    res[9]  = a[9];
    res[10] = a[10];
    res[11] = a[11];
    res[12] = a[12];
    res[13] = a[13];
    res[14] = a[14];
    res[15] = a[15];
    return res;    
  }


  // ================
  // = Type aliases =
  // ================

  typedef _Mvec3<float> Mvec3;
  typedef _Mvec3<double> Mvec3d;
}

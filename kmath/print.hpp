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


#include "matrix.hpp"
#include "vector.hpp"
#include "euclidian_flat_3d.hpp"
#include "rotor_3d.hpp"
#include "motor_3d.hpp"
#include "pga_3d.hpp"

#include <ostream>


namespace kmath {
  using namespace std;


  // ===========
  // = Vectors =
  // ===========


  template<Number T>
  ostream &operator<<(ostream &stream, const _Vec2<T> &o) {
    stream << "Vec2(" << o.x << ", " << o.y << ")";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Vec3<T> &o) {
    stream << "Vec3(" << o.x << ", " << o.y << ", " << o.z << ")";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Vec4<T> &o) {
    stream << "Vec4(" << o.x << ", " << o.y << ", " << o.z << ", " << o.w << ")";
    return stream;
  }


  // ============
  // = Matrices =
  // ============


  template<Number T>
  ostream &operator<<(ostream &stream, const _Mat2<T> &o) {
    stream << "Mat2(" << o.x.x << ", " << o.x.y << "; ";
    stream << o.y.x << ", " << o.y.y << ")";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Mat3<T> &o) {
    stream << "Mat3(" << o.x.x << ", " << o.x.y << ", " << o.x.z << "; ";
    stream << o.y.x << ", " << o.y.y << ", " << o.y.z << "; ";
    stream << o.z.x << ", " << o.z.y << ", " << o.z.z << ")";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Mat4<T> &o) {
    stream << "Mat4(" << o.x.x << ", " << o.x.y << ", " << o.x.z << ", " << o.x.w << "; ";
    stream << o.y.x << ", " << o.y.y << ", " << o.y.z << ", " << o.y.w << "; ";
    stream << o.z.x << ", " << o.z.y << ", " << o.z.z << ", " << o.z.w << "; ";
    stream << o.w.x << ", " << o.w.y << ", " << o.w.z << ", " << o.w.w << ")";
    return stream;
  }


  // =====================
  // = 3D PGA primitives =
  // =====================


  template<Number T>
  ostream &operator<<(ostream &stream, const _Plane3<T> &o) {
    stream << o.e1 << " e1 + " << o.e2 << " e2 + " << o.e3 << " e3 + " << o.e0 << " e0";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Line3<T> &o) {
    if (!is_vanishing(o)) {
      stream << o.e23 << " e23 + " << o.e31 << " e31 + " << o.e12 << " e12 + ";
    }
    stream << o.e01 << " e01 + " << o.e02 << " e02 + " << o.e03 << " e03";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Point3<T> &o) {
    stream << o.e032 << " e032 + " << o.e013 << " e013 + " << o.e021 << " e021 + " << o.e123 << " e123";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Rotor3<T> &o) {
    stream << o.s << " + " << o.e23 << " e23 + " << o.e31 << " e31 + " << o.e12 << " e12";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Motor3<T> &o) {
    stream << o.s << " + " << o.e23 << " e23 + " << o.e31 << " e31 + " << o.e12 << " e12 + ";
    stream << o.e0123 << " e0123 + " << o.e01 << " e01 + " << o.e02 << " e02 + " << o.e03 << " e03";
    return stream;
  }


  template<Number T>
  ostream &operator<<(ostream &stream, const _Mvec3<T> &o) {
    stream << o[_Mvec3<T>::Basis::s] << " + " << o[_Mvec3<T>::Basis::e0] << " e0 + " << o[_Mvec3<T>::Basis::e1] << " e1 + " << o[_Mvec3<T>::Basis::e2] << " e2 + " << o[_Mvec3<T>::Basis::e3] << " e3 + ";
    stream << o[_Mvec3<T>::Basis::e23] << " e23 + " << o[_Mvec3<T>::Basis::e31] << " e31 + " << o[_Mvec3<T>::Basis::e12] << " e12 + ";
    stream << o[_Mvec3<T>::Basis::e01] << " e01 + " << o[_Mvec3<T>::Basis::e02] << " e02 + " << o[_Mvec3<T>::Basis::e03] << " e03 + ";
    stream << o[_Mvec3<T>::Basis::e032] << " e032 + " << o[_Mvec3<T>::Basis::e013] << " e013 + " << o[_Mvec3<T>::Basis::e021] << " e021 + " << o[_Mvec3<T>::Basis::e123] << " e123 + ";
    stream << o[_Mvec3<T>::Basis::e0123] << " e0123";
    return stream;
  }
}

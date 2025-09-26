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
#include "private/defines.hpp"
#include "vector.hpp"

#include <cmath>


namespace kmath {

  template<typename T>
  inline bool is_approx_zero(const T &a) {
    return length_squared(a) < KMATH_EPSILON2;
  }


  template<>
  inline bool is_approx_zero<double>(const double &a) {
    return std::abs(a) < KMATH_EPSILON;
  }


  template<>
  inline bool is_approx_zero<float>(const float &a) {
    return std::abs(a) < KMATH_EPSILON;
  }


  template<>
  inline bool is_approx_zero<int>(const int &a) {
    return a == 0;
  }


  template<>
  inline bool is_approx_zero<long>(const long &a) {
    return a == 0;
  }


  template<Number T>
  requires Orderable<T>
  inline bool is_square_approx_zero(const T &a) {
    return std::abs(a) < KMATH_EPSILON2;
  }


  template<>
  inline bool is_square_approx_zero<int>(const int &a) {
    return a == 0;
  }


  template<>
  inline bool is_square_approx_zero<long>(const long &a) {
    return a == 0;
  }


  template<typename T>
  inline bool is_approx(const T &a, const T &b) {
    return is_approx_zero(b - a);
  }


  template<Number T>
  inline T inv_lerp(const T a, const T b, const T x) {
    return (x - a) / (b - a);
  }


  template<Number S, Vector<S> T>
  inline T lerp(const T &a, const T &b, const S &t) {
    return (S)(1.0 - t) * a + t * b;
  }
}

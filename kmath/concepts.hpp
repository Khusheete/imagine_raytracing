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


namespace kmath {

  template<typename N>
  concept Number = requires(N n1, N n2) {
    n1 += n2;
    n1 -= n2;
    n1 *= n2;
    n1 /= n2;
    n1 + n2;
    n1 - n2;
    n1 * n2;
    n1 / n2;
  };


  template<typename S>
  concept Orderable = requires(S a, S b) {
    a == b;
    a < b;
    a > b;
    a >= b;
    a <= b;
  };


  template<typename V, typename K>
  concept Vector = requires(V v1, V v2, K l) {
    v1 += v2;
    v1 -= v2;
    v1 *= l;
    v1 /= l;
    v1 + v2;
    v1 - v2;
    l * v1;
    v1 * l;
    v1 / l;
  };


  template<typename A, typename B>
  concept Castable = requires(A a, B b) {
    b = a;
  };


  template<typename F, typename R, typename ...Ts>
  concept Function = requires(F f, R r, Ts ...ts) {
    r = f(ts...);
  };
}

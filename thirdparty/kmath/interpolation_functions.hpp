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


#include "constants.hpp"
#include "utils.hpp"

#include <cmath>


namespace kmath::ease {
  template<Number T>
  using EasingFunction = T (*)(const T);


  namespace out {
    template<Number T>
    inline T quad(const T t) {
      const T u = (T)1.0 - t;
      return (T)1.0 - u * u;
    }


    template<Number T>
    inline T cubic(const T t) {
      const T u = (T)1.0 - t;
      return (T)1.0 - u * u * u;
    }


    template<Number T>
    inline T quart(const T t) {
      T u = (T)1.0 - t;
      return (T)1.0 - (u * u) * (u * u);
    }


    template<Number T>
    inline T quint(const T t) {
      T u = (T)1.0 - t;
      return (T)1.0 - (u * u) * (u * u) * u;
    }


    template<Number T>
    inline T sine(const T t) {
      return std::sin((T)0.5 * (T)PI * t);
    }


    template<Number T>
    inline T circ(const T t) {
      T u = (T)1.0 - t;
      return std::sqrt((T)1.0 - u * u);
    }


    template<Number T>
    inline T elastic(const T t) {
      return (t == (T)1.0)? 1.0 : std::pow((T)2.0, (T)-10.0 * t) * std::sin((t * (T)10.0 - (T)0.75) * TAU / (T)3.0) + (T)1.0;
    }


    template<Number T>
    inline T expo(const T t) {
      return (t == (T)1.0)? (T)1.0 : (T)1.0 - std::pow((T)2.0, (T)-10.0 * t);
    }


    template<Number T>
    inline T back(const T t) {
      const T a = (T)1.70158;
      const T b = a + (T)1.0;
      const T u = t - (T)1.0;
      return (T)1.0 + (b * u) * (u * u) + a * (u * u);
    }


    template<Number T>
    inline T bounce(const T t) {
      const T n = (T)7.5625;

      if (t < (T)0.3636363636363636) {
        return n * t * t;
      } else if (t < (T)0.7272727272727273) {
        const T u = t - (T)0.5454545454545455;
        return n * u * u + (T)0.75;
      } else if (t < (T)0.9090909090909091) {
        const T u = t - (T)0.8181818181818182;
        return n * u * u + (T)0.9375;
      } else {
        const T u = t - (T)0.9545454545454545;
        return n * u * u + (T)0.984375;
      }
    }
  }


  namespace in {
    template<Number T>
    inline T quad(const T t) {
      return t * t;
    }


    template<Number T>
    inline T cubic(const T t) {
      return t * t * t;
    }


    template<Number T>
    inline T quart(const T t) {
      return (t * t) * (t * t);
    }


    template<Number T>
    inline T quint(const T t) {
      return (t * t) * (t * t) * t;
    }


    template<Number T>
    inline T sine(const T t) {
      return (T)1.0 - std::cos((T)0.5 * (T)PI * t);
    }


    template<Number T>
    inline T circ(const T t) {
      return (T)1.0 - std::sqrt((T)1.0 - t * t);
    }


    template<Number T>
    inline T elastic(const T t) {
      return (t == (T)0.0)? 0.0 : -std::pow((T)2.0, (T)10.0 * t - (T)10.0) * std::sin((t * (T)10.0 - (T)10.75) * TAU / 3);
    }


    template<Number T>
    inline T expo(const T t) {
      return (t == (T)0.0)? 0.0 : std::pow((T)2.0, (T)10.0 * t - (T)10.0);
    }


    template<Number T>
    inline T back(const T t) {
      const T a = (T)1.70158;
      const T b = a + (T)1.0;
      return b * (t * t) * t - a * (t * t);
    }


    template<Number T>
    inline T bounce(const T t) {
      return (T)1.0 - out::bounce((T)1.0 - t);
    }
  }


  namespace in_out {

    namespace {
      template<Number T>
      inline T _p1(const T t) {
        return (T)-2.0 * t + (T)2.0;
      }
    }


    template<Number T>
    inline T quad(const T t) {
      return (t < (T)0.5)?
        (T)2.0 * t * t
        : (T)1.0 - (T)0.5 * _p1(t) * _p1(t);
    }


    template<Number T>
    inline T cubic(const T t) {
      return (t < (T)0.5)?
        (T)4.0 * t * t * t
        : (T)1.0 - (T)0.5 * _p1(t) * _p1(t) * _p1(t);
    }


    template<Number T>
    inline T quart(const T t) {
      return (t < (T)0.5)?
      (T)8.0 * (t * t) * (t * t)
      : (T)1.0 - (T)0.5 * (_p1(t) * _p1(t)) * (_p1(t) * _p1(t));
    }


    template<Number T>
    inline T quint(const T t) {
      return (t < (T)0.5f)?
        (T)16.0 * (t * t) * (t * t) * t
        : (T)1.0 - (T)0.5 * (_p1(t) * _p1(t)) * (_p1(t) * _p1(t)) * _p1(t);
    }


    template<Number T>
    inline T sine(const T t) {
      return (T)-0.5 * (std::cos(PI * t) - 1.0);
    }


    template<Number T>
    inline T circ(const T t) {
      return (t < (T)0.5)?
      (T)0.5 - (T)0.5 * std::sqrt((T)1.0 - (T)4.0 * t * t)
      : (T)0.5 * std::sqrt((T)1.0 - ((T)-2.0 * t + (T)2.0) * ((T)-2.0 * t + (T)2.0)) + (T)0.5;
    }


    template<Number T>
    inline T elastic(const T t) {
      const T omega = TAU / (T)4.5;
      return
        (t == (T)0.0)? 0.0 : (t == (T)1.0)? 1.0
        : (t < (T)0.5)?
        (T)-0.5 * std::pow((T)2.0, (T)20.0 * t - (T)10.0) * std::sin(((T)20.0 * t - (T)11.125) * omega)
        : (T)1.0 + (T)0.5 * std::pow((T)2.0, (T)-20.0 * t + (T)10.0) * std::sin(((T)20.0 * t - (T)11.125) * omega);
    }


    template<Number T>
    inline T expo(const T t) {
      return (t == (T)0.0)? 0.0 : (t == (T)1.0)? 1.0
        : (t < (T)0.5)?
        (T)0.5 * std::pow((T)2.0, (T)20.0 * t - (T)10.0)
        : (T)1.0 - (T)0.5 * std::pow((T)2.0, (T)-20.0 * t + (T)10.0);
    }


    template<Number T>
    inline T back(const T t) {
      const T a = (T)1.70158;
      const T b = a + (T)1.0;
      return (t < (T)0.5)?
        (T)2.0 * t * t * ((b + (T)1.0) * (T)2.0 * t - b)
        : (T)1.0 + (T)0.5 * _p1(t) * _p1(t) * (b - (b + (T)1.0) * _p1(t));
    }


    template<Number T>
    inline T bounce(const T t) {
      return (t < (T)0.5)?
        (T)0.5 - (T)0.5 * out::bounce((T)1.0 - (T)2.0 * t)
        : (T)0.5 + (T)0.5 * out::bounce((T)2.0 * t - (T)1.0);
    }
  }
}


namespace kmath {
  template<Number T, Number S>
  inline T interpolate(const T &a, const T &b, const S &t, ease::EasingFunction<S> easing) {
    return lerp(a, b, easing(t));
  }
}

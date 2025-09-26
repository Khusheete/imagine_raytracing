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


namespace kmath {
  // =========================
  // = Struct predeclaration =
  // =========================


  template<Number T>
  struct _Mat4;


  // ========
  // = Mat2 =
  // ========


  template<Number T>
  struct _Mat2 {
    _Vec2<T> x, y;

  public:
    static inline _Mat2<T> scale(const T scale) {
      return scale * _Mat2<T>::IDENTITY;
    }


    static inline _Mat2<T> scale(const T x, const T y) {
      return _Mat2<T>(
        _Vec2<T>(x     , (T)0.0),
        _Vec2<T>((T)0.0, y     )
      );
    }
    

    static inline _Mat2<T> rotation(const T angle) {
      const T cos = std::cos(angle);
      const T sin = std::sin(angle);
      return _Mat2<T>(
        _Vec2<T>(cos , sin),
        _Vec2<T>(-sin, cos)
      );
    }
    

  public:
    static const _Mat2<T> IDENTITY;
  };


  template<Number T>
  const _Mat2<T> _Mat2<T>::IDENTITY = _Mat2<T>(
    _Vec2<T>((T)1.0, (T)0.0),
    _Vec2<T>((T)0.0, (T)1.0)
  );


  template<Number T>
  inline _Mat2<T> transpose(const _Mat2<T> &m) {
    return _Mat2<T>(
      _Vec2<T>(m.x.x, m.y.x),
      _Vec2<T>(m.x.y, m.y.y)
    );
  }

  
  template<Number T>
  inline _Vec2<T> operator*(const _Mat2<T> &a, const _Vec2<T> &b) {
    return _Vec2<T>(
      a.x.x * b.x + a.y.x * b.y,
      a.x.y * b.x + a.y.x * b.y
    );
  }


  template<Number T>
  inline _Vec2<T> operator*(const _Vec2<T> &a, const _Mat2<T> &b) {
    return _Vec2<T>(
      kmath::dot(a.x, b),
      kmath::dot(a.y, b)
    );
  }


  template<Number T>
  inline _Mat2<T> operator*(const _Mat2<T> &a, const _Mat2<T> &b) {
    return _Mat2<T>(
      a * b.x,
      a * b.y
    );
  }


  template<Number T>
  inline _Mat2<T> &operator*=(_Mat2<T> &a, const _Mat2<T> &b) {
    a = a * b;
    return a;
  }


  // ========
  // = Mat3 =
  // ========


  template<Number T>
  struct _Mat3 {
    _Vec3<T> x, y, z;

  public:
    static inline _Mat3<T> from_mat4(const _Mat4<T> &mat4) {
      return _Mat3<T>(
        _Vec3<T>(mat4.x.x, mat4.x.y, mat4.x.z),
        _Vec3<T>(mat4.y.x, mat4.y.y, mat4.y.z),
        _Vec3<T>(mat4.z.x, mat4.z.y, mat4.z.z)
      );
    }


    static inline _Mat3<T> scale(const T scale) {
      return scale * _Mat3<T>::IDENTITY;
    }


    static inline _Mat3<T> scale(const T x, const T y, const T z) {
      return _Mat3<T>(
        _Vec3<T>(x     , (T)0.0, (T)0.0),
        _Vec3<T>((T)0.0, y     , (T)0.0),
        _Vec3<T>((T)0.0, (T)0.0, z     )
      );
    }


    static inline _Mat3<T> x_rotation(const T angle) {
      const T cos = std::cos(angle);
      const T sin = std::sin(angle);
      return _Mat3<T>(
        _Vec3<T>((T)1.0, (T)0.0, (T)0.0),
        _Vec3<T>((T)0.0, cos   , sin   ),
        _Vec3<T>((T)0.0, -sin  , cos   )
      );
    }


    static inline _Mat3<T> y_rotation(const T angle) {
      const T cos = std::cos(angle);
      const T sin = std::sin(angle);
      return _Mat3<T>(
        _Vec3<T>(cos   , (T)0.0, -sin  ),
        _Vec3<T>((T)0.0, (T)1.0, (T)0.0),
        _Vec3<T>(sin   , (T)0.0, cos   )
      );
    }


    static inline _Mat3<T> z_rotation(const T angle) {
      const T cos = std::cos(angle);
      const T sin = std::sin(angle);
      return _Mat3<T>(
        _Vec3<T>(cos   , sin   , (T)0.0),
        _Vec3<T>(-sin  , cos   , (T)0.0),
        _Vec3<T>((T)0.0, (T)0.0, (T)1.0)
      );
    }
    

  public:
    static const _Mat3<T> IDENTITY;
  };


  template<Number T>
  const _Mat3<T> _Mat3<T>::IDENTITY = _Mat3<T>(
    _Vec3<T>((T)1.0, (T)0.0, (T)0.0),
    _Vec3<T>((T)0.0, (T)1.0, (T)0.0),
    _Vec3<T>((T)0.0, (T)0.0, (T)1.0)
  );


  template<Number T>
  inline _Mat3<T> transpose(const _Mat3<T> &m) {
    return _Mat3<T>(
      _Vec3<T>(m.x.x, m.y.x, m.z.x),
      _Vec3<T>(m.x.y, m.y.y, m.z.y),
      _Vec3<T>(m.x.z, m.y.z, m.z.z)
    );
  }


  // The inverse for a Matrice representing a rigid transformation. (Orthogonal matrix).
  // If the matrice has non-uniform scaling, use fast_inverse_non_uniform.
  template<Number T>
  inline _Mat3<T> fast_inverse(const _Mat3<T> &m) {
    return transpose(m);
  }


  template<Number T>
  inline _Mat3<T> fast_inverse_non_uniform(const _Mat3<T> &m) {
    _Mat3<T> res(m);
    res.x /= length_squared(res.x);
    res.y /= length_squared(res.y);
    res.z /= length_squared(res.z);
    return transpose(res);
  }
  

  template<Number T>
  inline _Vec3<T> operator*(const _Mat3<T> &a, const _Vec3<T> &b) {
    return _Vec3<T>(
      a.x.x * b.x + a.y.x * b.y + a.z.x * b.z,
      a.x.y * b.x + a.y.y * b.y + a.z.y * b.z,
      a.x.z * b.x + a.y.z * b.y + a.z.z * b.z
    );
  }


  template<Number T>
  inline _Vec3<T> operator*(const _Vec3<T> &a, const _Mat3<T> &b) {
    return _Vec3<T>(
      kmath::dot(a, b.x),
      kmath::dot(a, b.y),
      kmath::dot(a, b.z)
    );
  }


  template<Number T>
  inline _Mat3<T> operator*(const _Mat3<T> &a, const _Mat3<T> &b) {
    return _Mat3<T>(
      a * b.x,
      a * b.y,
      a * b.z
    );
  }


  template<Number T>
  inline _Mat3<T> &operator*=(_Mat3<T> &a, const _Mat3<T> &b) {
    a = a * b;
    return a;
  }


  // ========
  // = Mat4 =
  // ========


  template<Number T>
  struct _Mat4 {
    _Vec4<T> x, y, z, w;

  public:

    inline static _Mat4<T> from_basis(const _Mat3<T> &basis, const _Vec3<T> &position = _Vec3<T>::ZERO) {
      return _Mat4<T>(
      _Vec4<T>(basis.x.x , basis.x.y , basis.x.z , (T)0.0),
      _Vec4<T>(basis.y.x , basis.y.y , basis.y.z , (T)0.0),
      _Vec4<T>(basis.z.x , basis.z.y , basis.z.z , (T)0.0),
      _Vec4<T>(position.x, position.y, position.z, (T)1.0)
      );
    }


  public:
    static inline _Mat4<T> scale(const T scale) {
      return scale * _Mat4<T>::IDENTITY;
    }


    static inline _Mat4<T> scale(const T x, const T y, const T z, const T w = (T)1.0) {
      return _Mat4<T>(
        _Vec4<T>(x     , (T)0.0, (T)0.0, (T)0.0),
        _Vec4<T>((T)0.0, y     , (T)0.0, (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, z     , (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, (T)0.0, w     )
      );
    }


    static inline _Mat4<T> x_rotation(const T angle) {
      const T cos = std::cos(angle);
      const T sin = std::sin(angle);
      return _Mat4<T>(
        _Vec4<T>((T)1.0, (T)0.0, (T)0.0, (T)0.0),
        _Vec4<T>((T)0.0, cos   , sin   , (T)0.0),
        _Vec4<T>((T)0.0, -sin  , cos   , (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0)
      );
    }


    static inline _Mat4<T> y_rotation(const T angle) {
      const T cos = std::cos(angle);
      const T sin = std::sin(angle);
      return _Mat4<T>(
        _Vec4<T>(cos   , (T)0.0, -sin  , (T)0.0),
        _Vec4<T>((T)0.0, (T)1.0, (T)0.0, (T)0.0),
        _Vec4<T>(sin   , (T)0.0, cos   , (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0)
      );
    }


    static inline _Mat4<T> z_rotation(const T angle) {
      const T cos = std::cos(angle);
      const T sin = std::sin(angle);
      return _Mat4<T>(
        _Vec4<T>(cos   , sin   , (T)0.0, (T)0.0),
        _Vec4<T>(-sin  , cos   , (T)0.0, (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, (T)1.0, (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0)
      );
    }


    static inline _Mat4<T> translation(const _Vec3<T> &translation) {
      return _Mat4<T>(
        _Vec4<T>((T)1.0, (T)0.0, (T)0.0, translation.x),
        _Vec4<T>((T)0.0, (T)1.0, (T)0.0, translation.y),
        _Vec4<T>((T)0.0, (T)0.0, (T)1.0, translation.z),
        _Vec4<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0       )
      );
    }

    
    // Creates an orthogonal projection matrix with a right handed ndc that goes from negative one to one.
    // Note that the near plane is mapped to -1 and the far plane is mapped to 1.
    static inline _Mat4<T> orthogonal_rh_no_ndc(const T near, const T far, const T width, const T height) {
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>((T)2.0 / width, (T)0.0         , (T)0.0                    , (T)0.0                           ),
        _Vec4<T>((T)0.0        , (T)2.0 / height, (T)0.0                    , (T)0.0                           ),
        _Vec4<T>((T)0.0        , (T)0.0         , (far + near) * inv_nf_dist, (T)2.0 * near * far * inv_nf_dist),
        _Vec4<T>((T)0.0        , (T)0.0         , (T)0.0                    , (T)1.0                           )
      );
    }


    // Creates an orthogonal projection matrix with a left handed ndc that goes from negative one to one.
    // Note that the near plane is mapped to -1 and the far plane is mapped to 1.
    static inline _Mat4<T> orthogonal_lh_no_ndc(const T near, const T far, const T width, const T height) {
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>((T)2.0 / width, (T)0.0         , (T)0.0                , (T)0.0                      ),
        _Vec4<T>((T)0.0        , (T)2.0 / height, (T)0.0                , (T)0.0                      ),
        _Vec4<T>((T)0.0        , (T)0.0         , - (T)2.0 * inv_nf_dist, - (far + near) * inv_nf_dist),
        _Vec4<T>((T)0.0        , (T)0.0         , (T)0.0                , (T)1.0                      )
      );
    }

    
    // Creates a perspective projection matrix with a right handed ndc that goes from zero to one.
    // Note that the near plane is mapped to 1 and the far plane is mapped to 0.
    // v_fov is the vertical field of view in rad.
    static inline _Mat4<T> perspective_rh_zo_ndc_vfov(const T near, const T far, const T v_fov, const T aspect_ratio) {
      const T half_fov = (T)0.5 * v_fov;
      const T iw_v = std::cos(half_fov) / std::sin(half_fov);
      const T iw_h = iw_v / aspect_ratio;
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>(iw_h  , (T)0.0,   (T)0.0                  , (T)0.0),
        _Vec4<T>((T)0.0, iw_v  ,   (T)0.0                  , (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, - near * inv_nf_dist      , (T)1.0),
        _Vec4<T>((T)0.0, (T)0.0, - near * far * inv_nf_dist, (T)0.0)
      );
    }


    // Creates a perspective projection matrix with a right handed ndc that goes from zero to one.
    // Note that the near plane is mapped to 1 and the far plane is mapped to 0.
    // h_fov is the horizontal field of view in rad.
    static inline _Mat4<T> perspective_rh_zo_ndc_hfov(const T near, const T far, const T v_fov, const T aspect_ratio) {
      const T half_fov = (T)0.5 * v_fov;
      const T iw_h = std::cos(half_fov) / std::sin(half_fov);
      const T iw_v = iw_h * aspect_ratio;
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>(iw_h  , (T)0.0,   (T)0.0                  , (T)0.0),
        _Vec4<T>((T)0.0, iw_v  ,   (T)0.0                  , (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, - near * inv_nf_dist      , (T)1.0),
        _Vec4<T>((T)0.0, (T)0.0, - near * far * inv_nf_dist, (T)0.0)
      );
    }


    // Creates a perspective projection matrix with a right handed ndc that goes from zero to one.
    // Note that the near plane is mapped to 1 and the far plane is mapped to 0.
    // v_fov is the vertical field of view in rad.
    static inline _Mat4<T> perspective_lh_zo_ndc_vfov(const T near, const T far, const T v_fov, const T aspect_ratio) {
      const T half_fov = (T)0.5 * v_fov;
      const T iw_v = std::cos(half_fov) / std::sin(half_fov);
      const T iw_h = iw_v / aspect_ratio;
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>(iw_h  , (T)0.0,   (T)0.0                , (T)0.0  ),
        _Vec4<T>((T)0.0, iw_v  ,   (T)0.0                , (T)0.0  ),
        _Vec4<T>((T)0.0, (T)0.0, near * inv_nf_dist      , - (T)1.0),
        _Vec4<T>((T)0.0, (T)0.0, near * far * inv_nf_dist, (T)0.0  )
      );
    }


    // Creates a perspective projection matrix with a right handed ndc that goes from zero to one.
    // Note that the near plane is mapped to 1 and the far plane is mapped to 0.
    // h_fov is the horizontal field of view in rad.
    static inline _Mat4<T> perspective_lh_zo_ndc_hfov(const T near, const T far, const T v_fov, const T aspect_ratio) {
      const T half_fov = (T)0.5 * v_fov;
      const T iw_h = std::cos(half_fov) / std::sin(half_fov);
      const T iw_v = iw_h * aspect_ratio;
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>(iw_h  , (T)0.0,   (T)0.0                , (T)0.0  ),
        _Vec4<T>((T)0.0, iw_v  ,   (T)0.0                , (T)0.0  ),
        _Vec4<T>((T)0.0, (T)0.0, near * inv_nf_dist      , - (T)1.0),
        _Vec4<T>((T)0.0, (T)0.0, near * far * inv_nf_dist, (T)0.0  )
      );
    }


    // Creates a perspective projection matrix with a right handed ndc that goes from negative one to one.
    // Note that the near plane is mapped to -1 and the far plane is mapped to 1.
    // v_fov is the vertical field of view in rad.
    static inline _Mat4<T> perspective_rh_no_ndc_vfov(const T near, const T far, const T v_fov, const T aspect_ratio) {
      const T half_fov = (T)0.5 * v_fov;
      const T iw_v = std::cos(half_fov) / std::sin(half_fov);
      const T iw_h = iw_v / aspect_ratio;
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>(iw_h  , (T)0.0,   (T)0.0                           , (T)0.0),
        _Vec4<T>((T)0.0, iw_v  ,   (T)0.0                           , (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, (far + near) * inv_nf_dist         , (T)1.0),
        _Vec4<T>((T)0.0, (T)0.0, - (T)2.0 * near * far * inv_nf_dist, (T)0.0)
      );
    }


    // Creates a perspective projection matrix with a right handed ndc that goes from negative one to one.
    // Note that the near plane is mapped to -1 and the far plane is mapped to 1.
    // h_fov is the horizontal field of view in rad.
    static inline _Mat4<T> perspective_rh_no_ndc_hfov(const T near, const T far, const T h_fov, const T aspect_ratio) {
      const T half_fov = (T)0.5 * h_fov;
      const T iw_h = std::cos(half_fov) / std::sin(half_fov);
      const T iw_v = iw_h * aspect_ratio;
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>(iw_h  , (T)0.0,   (T)0.0                           , (T)0.0),
        _Vec4<T>((T)0.0, iw_v  ,   (T)0.0                           , (T)0.0),
        _Vec4<T>((T)0.0, (T)0.0, (far + near) * inv_nf_dist         , (T)1.0),
        _Vec4<T>((T)0.0, (T)0.0, - (T)2.0 * near * far * inv_nf_dist, (T)0.0)
      );
    }

    
    // Creates a perspective projection matrix with a left handed ndc that goes from negative one to one.
    // Note that the near plane is mapped to -1 and the far plane is mapped to 1.
    // v_fov is the vertical field of view in rad.
    static inline _Mat4<T> perspective_lh_no_ndc_vfov(const T near, const T far, const T v_fov, const T aspect_ratio) {
      const T half_fov = (T)0.5 * v_fov;
      const T iw_v = std::cos(half_fov) / std::sin(half_fov);
      const T iw_h = iw_v / aspect_ratio;
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>(iw_h  , (T)0.0,   (T)0.0                           , (T)0.0  ),
        _Vec4<T>((T)0.0, iw_v  ,   (T)0.0                           , (T)0.0  ),
        _Vec4<T>((T)0.0, (T)0.0, - (far + near) * inv_nf_dist       , - (T)1.0),
        _Vec4<T>((T)0.0, (T)0.0, - (T)2.0 * near * far * inv_nf_dist, (T)0.0  )
      );
    }


    // Creates a perspective projection matrix with a left handed ndc that goes from negative one to one.
    // Note that the near plane is mapped to -1 and the far plane is mapped to 1.
    // h_fov is the horizontal field of view in rad.
    static inline _Mat4<T> perspective_lh_no_ndc_hfov(const T near, const T far, const T h_fov, const T aspect_ratio) {
      const T half_fov = (T)0.5 * h_fov;
      const T iw_h = std::cos(half_fov) / std::sin(half_fov);
      const T iw_v = iw_h * aspect_ratio;
      const T inv_nf_dist = (T)1.0 / (far - near);
      return _Mat4<T>(
        _Vec4<T>(iw_h  , (T)0.0,   (T)0.0                           , (T)0.0  ),
        _Vec4<T>((T)0.0, iw_v  ,   (T)0.0                           , (T)0.0  ),
        _Vec4<T>((T)0.0, (T)0.0, - (far + near) * inv_nf_dist       , - (T)1.0),
        _Vec4<T>((T)0.0, (T)0.0, - (T)2.0 * near * far * inv_nf_dist, (T)0.0  )
      );
    }

  public:
    static const _Mat4<T> IDENTITY;
  };


  template<Number T>
  const _Mat4<T> _Mat4<T>::IDENTITY = _Mat4<T>(
    _Vec4<T>((T)1.0, (T)0.0, (T)0.0, (T)0.0),
    _Vec4<T>((T)0.0, (T)1.0, (T)0.0, (T)0.0),
    _Vec4<T>((T)0.0, (T)0.0, (T)1.0, (T)0.0),
    _Vec4<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0)
  );


  template<Number T>
  inline _Mat4<T> transpose(const _Mat4<T> &m) {
    return _Mat4<T>(
      _Vec4<T>(m.x.x, m.y.x, m.z.x, m.w.x),
      _Vec4<T>(m.x.y, m.y.y, m.z.y, m.w.y),
      _Vec4<T>(m.x.z, m.y.z, m.z.z, m.w.z),
      _Vec4<T>(m.x.w, m.y.w, m.z.w, m.w.w)
    );
  }


  template<Number T>
  inline _Vec4<T> operator*(const _Mat4<T> &a, const _Vec4<T> &b) {
    return _Vec4(
      a.x.x * b.x + a.y.x * b.y + a.z.x * b.z + a.w.x * b.w,
      a.x.y * b.x + a.y.y * b.y + a.z.y * b.z + a.w.y * b.w,
      a.x.z * b.x + a.y.z * b.y + a.z.z * b.z + a.w.z * b.w,
      a.x.w * b.x + a.y.w * b.y + a.z.w * b.z + a.w.w * b.w
    );
  }


  template<Number T>
  inline _Vec4<T> operator*(const _Vec4<T> &a, const _Mat4<T> &b) {
    return _Vec4(
      kmath::dot(a, b.x),
      kmath::dot(a, b.y),
      kmath::dot(a, b.z),
      kmath::dot(a, b.w)
    );
  }


  template<Number T>
  inline _Mat4<T> operator*(const _Mat4<T> &a, const _Mat4<T> &b) {
    return _Mat4<T>(
      a * b.x,
      a * b.y,
      a * b.z,
      a * b.w
    );
  }


  template<Number T>
  inline _Mat4<T> &operator*=(_Mat4<T> &a, const _Mat4<T> &b) {
    a = a * b;
    return a;
  }


  // ================
  // = Type aliases =
  // ================


  typedef _Mat2<float> Mat2;
  typedef _Mat2<double> Mat2d;

  typedef _Mat3<float> Mat3;
  typedef _Mat3<double> Mat3d;

  typedef _Mat4<float> Mat4;
  typedef _Mat4<double> Mat4d;
  

}

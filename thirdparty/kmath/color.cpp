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


#include "color.hpp"
#include "vector.hpp"
#include "constants.hpp"

#include <cmath>
#include <limits>


namespace kmath {
  // ==============================
  // = Functions for calculations =
  // ==============================
  
  
  typedef _Vec2<float> LC;
  typedef _Vec2<float> ST;
  typedef _Vec3<float> Cs;


  inline float toe(const float x) {
    constexpr float k1 = 0.206f;
    constexpr float k2 = 0.03f;
    constexpr float k3 = (1.0f + k1) / (1.0f + k2);
    return 0.5f * (k3 * x - k1 + std::sqrt((k3 * x - k1) * (k3 * x - k1) + 4 * k2 * k3 * x));
  }


  inline float toe_inv(const float x) {
    constexpr float k1 = 0.206f;
    constexpr float k2 = 0.03f;
    constexpr float k3 = (1.0f + k1) / (1.0f + k2);
    return (x * x + k1 * x) / (k3 * (x + k2));
  }


  inline ST to_ST(const LC &cusp) {
    float L = cusp.x;
    float C = cusp.y;
    return ST(C / L, C / (1.0f - L));
  }


  float compute_max_saturation(const float a, const float b) {
  	float k0, k1, k2, k3, k4, wl, wm, ws;

  	if (-1.88170328f * a - 0.80936493f * b > 1) {
  		k0 = +1.19086277f; k1 = +1.76576728f; k2 = +0.59662641f; k3 = +0.75515197f; k4 = +0.56771245f;
  		wl = +4.0767416621f; wm = -3.3077115913f; ws = +0.2309699292f;
  	} else if (1.81444104f * a - 1.19445276f * b > 1) {
  		k0 = +0.73956515f; k1 = -0.45954404f; k2 = +0.08285427f; k3 = +0.12541070f; k4 = +0.14503204f;
  		wl = -1.2684380046f; wm = +2.6097574011f; ws = -0.3413193965f;
  	} else {
  		k0 = +1.35733652f; k1 = -0.00915799f; k2 = -1.15130210f; k3 = -0.50559606f; k4 = +0.00692167f;
  		wl = -0.0041960863f; wm = -0.7034186147f; ws = +1.7076147010f;
  	}

  	float S = k0 + k1 * a + k2 * b + k3 * a * a + k4 * a * b;

  	float k_l = +0.3963377774f * a + 0.2158037573f * b;
  	float k_m = -0.1055613458f * a - 0.0638541728f * b;
  	float k_s = -0.0894841775f * a - 1.2914855480f * b;

  	{
  		float l_ = 1.0f + S * k_l;
  		float m_ = 1.0f + S * k_m;
  		float s_ = 1.0f + S * k_s;

  		float l = l_ * l_ * l_;
  		float m = m_ * m_ * m_;
  		float s = s_ * s_ * s_;

  		float l_dS = 3.0f * k_l * l_ * l_;
  		float m_dS = 3.0f * k_m * m_ * m_;
  		float s_dS = 3.0f * k_s * s_ * s_;

  		float l_dS2 = 6.0f * k_l * k_l * l_;
  		float m_dS2 = 6.0f * k_m * k_m * m_;
  		float s_dS2 = 6.0f * k_s * k_s * s_;

  		float f = wl * l + wm * m + ws * s;
  		float f1 = wl * l_dS + wm * m_dS + ws * s_dS;
  		float f2 = wl * l_dS2 + wm * m_dS2 + ws * s_dS2;

  		S = S - f * f1 / (f1 * f1 - 0.5f * f * f2);
  	}

  	return S;
  }


  inline LC find_cusp(const float a, const float b) {
    float S_cusp = compute_max_saturation(a, b);
    Lrgb rgb_at_max = oklab_to_lrgb(OkLab(1.0, S_cusp * a, S_cusp * b));
    float L_cusp = std::cbrt(1.0f / std::max(std::max(rgb_at_max.x, rgb_at_max.y), rgb_at_max.z));
    float C_cusp = L_cusp * S_cusp;
    return LC(L_cusp, C_cusp);
  }


  inline ST get_ST_mid(const float a_, const float b_) {
  	float S = 0.11516993f + 1.0f / (+7.44778970f + 4.15901240f * b_ + a_ * (-2.19557347f + 1.75198401f * b_ + a_ * (-2.13704948f - 10.02301043f * b_ + a_ * (-4.24894561f + 5.38770819f * b_ + 4.69891013f * a_))));
  	float T = 0.11239642f + 1.0f / (+1.61320320f - 0.68124379f * b_ + a_ * (+0.40370612f + 0.90148123f * b_ + a_ * (-0.27087943f + 0.61223990f * b_ + a_ * (+0.00299215f - 0.45399568f * b_ - 0.14661872f * a_))));
  	return ST(S, T);
  }



  float find_gamut_intersection(const float a, const float b, const float L1, const float C1, const float L0, const LC &cusp) {
  	float t;

  	if (((L1 - L0) * cusp.y - (cusp.x - L0) * C1) <= 0.0f) {
  		t = cusp.y * L0 / (C1 * cusp.x + cusp.y * (L0 - L1));
  	} else {
  		t = cusp.y * (L0 - 1.0f) / (C1 * (cusp.x - 1.0f) + cusp.y * (L0 - L1));

  		{
  			float dL = L1 - L0;
  			float dC = C1;

  			float k_l = +0.3963377774f * a + 0.2158037573f * b;
  			float k_m = -0.1055613458f * a - 0.0638541728f * b;
  			float k_s = -0.0894841775f * a - 1.2914855480f * b;

  			float l_dt = dL + dC * k_l;
  			float m_dt = dL + dC * k_m;
  			float s_dt = dL + dC * k_s;


  			{
  				float L = L0 * (1.0f - t) + t * L1;
  				float C = t * C1;

  				float l_ = L + C * k_l;
  				float m_ = L + C * k_m;
  				float s_ = L + C * k_s;

  				float l = l_ * l_ * l_;
  				float m = m_ * m_ * m_;
  				float s = s_ * s_ * s_;

  				float ldt = 3 * l_dt * l_ * l_;
  				float mdt = 3 * m_dt * m_ * m_;
  				float sdt = 3 * s_dt * s_ * s_;

  				float ldt2 = 6 * l_dt * l_dt * l_;
  				float mdt2 = 6 * m_dt * m_dt * m_;
  				float sdt2 = 6 * s_dt * s_dt * s_;

  				float r = 4.0767416621f  * l    - 3.3077115913f * m    + 0.2309699292f * s - 1;
  				float r1 = 4.0767416621f * ldt  - 3.3077115913f * mdt  + 0.2309699292f * sdt;
  				float r2 = 4.0767416621f * ldt2 - 3.3077115913f * mdt2 + 0.2309699292f * sdt2;

  				float u_r = r1 / (r1 * r1 - 0.5f * r * r2);
  				float t_r = -r * u_r;

  				float g = -1.2684380046f  * l    + 2.6097574011f * m    - 0.3413193965f * s - 1;
  				float g1 = -1.2684380046f * ldt  + 2.6097574011f * mdt  - 0.3413193965f * sdt;
  				float g2 = -1.2684380046f * ldt2 + 2.6097574011f * mdt2 - 0.3413193965f * sdt2;

  				float u_g = g1 / (g1 * g1 - 0.5f * g * g2);
  				float t_g = -g * u_g;

  				float b = -0.0041960863f  * l - 0.7034186147f    * m    + 1.7076147010f * s - 1;
  				float b1 = -0.0041960863f * ldt - 0.7034186147f  * mdt  + 1.7076147010f * sdt;
  				float b2 = -0.0041960863f * ldt2 - 0.7034186147f * mdt2 + 1.7076147010f * sdt2;

  				float u_b = b1 / (b1 * b1 - 0.5f * b * b2);
  				float t_b = -b * u_b;

  				t_r = u_r >= 0.0f ? t_r : std::numeric_limits<float>::infinity();
  				t_g = u_g >= 0.0f ? t_g : std::numeric_limits<float>::infinity();
  				t_b = u_b >= 0.0f ? t_b : std::numeric_limits<float>::infinity();

  				t += std::min(t_r, std::min(t_g, t_b));
  			}
  		}
  	}

  	return t;
  }


  inline float find_gamut_intersection(float a, float b, float L1, float C1, float L0) {
  	// Find the cusp of the gamut triangle
  	LC cusp = find_cusp(a, b);
  	return find_gamut_intersection(a, b, L1, C1, L0, cusp);
  }


  Cs get_Cs(float L, float a_, float b_) {
  	LC cusp = find_cusp(a_, b_);

  	float C_max = find_gamut_intersection(a_, b_, L, 1, L, cusp);
  	ST ST_max = to_ST(cusp);

  	float k = C_max / std::min((L * ST_max.x), (1 - L) * ST_max.y);

  	float C_mid;
  	{
  		ST ST_mid = get_ST_mid(a_, b_);

  		float C_a = L * ST_mid.x;
  		float C_b = (1.0f - L) * ST_mid.y;
  		C_mid = 0.9f * k * std::sqrt(std::sqrt(1.0f / (1.0f / (C_a * C_a * C_a * C_a) + 1.0f / (C_b * C_b * C_b * C_b))));
  	}

  	float C_0;
  	{
  		float C_a = L * 0.4f;
  		float C_b = (1.0f - L) * 0.8f;

  		C_0 = std::sqrt(1.0f / (1.0f / (C_a * C_a) + 1.0f / (C_b * C_b)));
  	}

  	return Cs(C_0, C_mid, C_max);
  }


  // ===================
  // = OkLab functions =
  // ===================


  OkLab xyz_to_oklab(const XYZD65 &xyz) {
    const Mat3 M1(
      Vec3(+0.8189330101f, +0.0329845436f, +0.0482003018f),
      Vec3(+0.3618667424f, +0.9293118715f, +0.2643662691f),
      Vec3(-0.1288597137f, +0.0361456387f, +0.6338517070f)
    );
    const Mat3 M2(
      Vec3(+0.2104542553f, +1.9779984951f, +0.0259040371f),
      Vec3(+0.7936177850f, -2.4285922050f, +0.7827717662f),
      Vec3(-0.0040720468f, +0.4505937099f, -0.8086757660f)
    );
    Vec3 lms = M1 * xyz;
    lms = Vec3(
      std::cbrt(lms.x),
      std::cbrt(lms.y),
      std::cbrt(lms.z)
    );
    return M2 * lms;
  }


  XYZD65 oklab_to_xyz(const OkLab &lab) {
    const Mat3 M2(
      Vec3(+0.9999999984505197f , +1.0000000088817607f , +1.0000000546724108f),
      Vec3(+0.39633779217376786f, -0.10556134232365633f, -0.08948418209496574f),
      Vec3(+0.21580375806075877f, -0.06385417477170588f, -1.2914855378640917f)
    );
    const Mat3 M1(
      Vec3(+1.2270138511035211f, -0.0405801784232806f, -0.0763812845057069f),
      Vec3(-0.5577999806518222f, +1.11225686961683f  , -0.4214819784180127f),
      Vec3(+0.2812561489664678f, -0.0716766786656012f, +1.5861632204407947f)
    );
    Vec3 lms = M2 * lab;
    lms = lms * lms * lms;
    return M1 * lms;
  }
  

  Lrgb oklab_to_lrgb(const OkLab &lab) {
    const Mat3 M2(
      Vec3(+1.0000000000f, +1.0000000000f, +1.0000000000f),
      Vec3(+0.3963377774f, -0.1055613458f, -0.0894841775f),
      Vec3(+0.2158037573f, -0.0638541728f, -1.2914855480f)
    );
    const Mat3 M1(
      Vec3(+4.0767416621f, -1.2684380046f, -0.0041960863f),
      Vec3(-3.3077115913f, +2.6097574011f, -0.7034186147f),
      Vec3(+0.2309699292f, -0.3413193965f, +1.7076147010f)
    );
    Vec3 lms = M2 * lab;
    lms = lms * lms * lms;
    return M1 * lms;
  }


  OkLab lrgb_to_oklab(const Lrgb &rgb) {
    const Mat3 M1(
      Vec3(+0.4122214708f, +0.2119034982f, +0.0883024619f),
      Vec3(+0.5363325363f, +0.6806995451f, +0.2817188376f),
      Vec3(+0.0514459929f, +0.1073969566f, +0.6299787005f)
    );
    const Mat3 M2(
      Vec3(+0.2104542553f, +1.9779984951f, +0.0259040371f),
      Vec3(+0.7936177850f, -2.4285922050f, +0.7827717662f),
      Vec3(-0.0040720468f, +0.4505937099f, -0.8086757660f)
    );
    Vec3 lms = M1 * rgb;
    lms = Vec3(
      std::cbrt(lms.x),
      std::cbrt(lms.y),
      std::cbrt(lms.z)
    );
    return M2 * lms;
  }


  // ==================
  // = OkHsv function =
  // ==================


  OkLab okhsv_to_oklab(const OkHsv &hsv) {
    float h = hsv.x;
  	float s = hsv.y;
  	float v = hsv.z;

  	float a_ = std::cos(2.0f * PI * h);
  	float b_ = std::sin(2.0f * PI * h);

  	LC cusp = find_cusp(a_, b_);
  	ST ST_max = to_ST(cusp);
  	float S_max = ST_max.x;
  	float T_max = ST_max.y;
  	float S_0 = 0.5f;
  	float k = 1.0 - S_0 / S_max;

  	// first we compute L and V as if the gamut is a perfect triangle:

  	// L, C when v==1:
  	float L_v = 1     - s * S_0 / (S_0 + T_max - T_max * k * s);
  	float C_v = s * T_max * S_0 / (S_0 + T_max - T_max * k * s);

  	float L = v * L_v;
  	float C = v * C_v;

  	// then we compensate for both toe and the curved top part of the triangle:
  	float L_vt = toe_inv(L_v);
  	float C_vt = C_v * L_vt / L_v;

  	float L_new = toe_inv(L);
  	C = C * L_new / L;
  	L = L_new;

  	Lrgb rgb_scale = oklab_to_lrgb(Lrgb(L_vt, a_ * C_vt, b_ * C_vt));
  	float scale_L = std::cbrt(1.0f / std::max(std::max(rgb_scale.x, rgb_scale.y), std::max(rgb_scale.z, 0.0f)));

  	L = L * scale_L;
  	C = C * scale_L;

  	return OkLab(L, C * a_, C * b_);
  }


  OkHsv oklab_to_okhsv(const OkLab &lab) {
  	float C = std::sqrt(lab.y * lab.y + lab.z * lab.z);
  	float a_ = lab.y / C;
  	float b_ = lab.z / C;

  	float L = lab.x;
  	float h = 0.5f + 0.5f * std::atan2(-lab.z, -lab.y) / PI;

  	LC cusp = find_cusp(a_, b_);
  	ST ST_max = to_ST(cusp);
  	float S_max = ST_max.x;
  	float T_max = ST_max.y;
  	float S_0 = 0.5f;
  	float k = 1 - S_0 / S_max;

  	// first we find L_v, C_v, L_vt and C_vt

  	float t = T_max / (C + L * T_max);
  	float L_v = t * L;
  	float C_v = t * C;

  	float L_vt = toe_inv(L_v);
  	float C_vt = C_v * L_vt / L_v;

  	// we can then use these to invert the step that compensates for the toe and the curved top part of the triangle:
  	Lrgb rgb_scale = oklab_to_lrgb(Lrgb(L_vt, a_ * C_vt, b_ * C_vt));
  	float scale_L = std::cbrt(1.0f / std::max(std::max(rgb_scale.x, rgb_scale.y), std::max(rgb_scale.z, 0.0f)));

  	L = L / scale_L;
  	C = C / scale_L;

  	C = C * toe(L) / L;
  	L = toe(L);

  	// we can now compute v and s:

  	float v = L / L_v;
  	float s = (S_0 + T_max) * C_v / ((T_max * S_0) + T_max * k * C_v);

  	return OkHsv(h, s, v);
  }


  // ===================
  // = OkHsl functions =
  // ===================


  OkLab okhsl_to_oklab(const OkHsl &hsl) {
  	float h = hsl.x;
  	float s = hsl.y;
  	float l = hsl.z;

  	if (l == 1.0f) {
  		return OkLab(1.0f, 0.0f, 0.0f);
  	} else if (l == 0.0f) {
  		return OkLab(0.0f, 0.0f, 0.0f);
  	}

  	float a_ = std::cos(2.0f * PI * h);
  	float b_ = std::sin(2.0f * PI * h);
  	float L = toe_inv(l);

  	Cs cs = get_Cs(L, a_, b_);
  	float C_0 = cs.x;
  	float C_mid = cs.y;
  	float C_max = cs.z;

      // Interpolate the three values for C so that:
      // At s=0: dC/ds = C_0, C=0
      // At s=0.8: C=C_mid
      // At s=1.0: C=C_max

  	float mid = 0.8f;
  	float mid_inv = 1.25f;

  	float C, t, k_0, k_1, k_2;

  	if (s < mid) {
  		t = mid_inv * s;

  		k_1 = mid * C_0;
  		k_2 = (1.0f - k_1 / C_mid);

  		C = t * k_1 / (1.0f - k_2 * t);
  	} else {
  		t = (s - mid)/ (1 - mid);

  		k_0 = C_mid;
  		k_1 = (1.0f - mid) * C_mid * C_mid * mid_inv * mid_inv / C_0;
  		k_2 = (1.0f - (k_1) / (C_max - C_mid));

  		C = k_0 + t * k_1 / (1.0f - k_2 * t);
  	}

  	return OkLab(L, C * a_, C * b_);
  }


  OkHsl oklab_to_okhsl(const OkLab &lab) {
  	float C = std::sqrt(lab.y * lab.y + lab.z * lab.z);
  	float a_ = lab.y / C;
  	float b_ = lab.z / C;

  	float L = lab.x;
  	float h = 0.5f + 0.5f * std::atan2(-lab.z, -lab.y) / PI;

  	Cs cs = get_Cs(L, a_, b_);
  	float C_0 = cs.x;
  	float C_mid = cs.y;
  	float C_max = cs.z;

      // Inverse of the interpolation in okhsl_to_srgb:

  	float mid = 0.8f;
  	float mid_inv = 1.25f;

  	float s;
  	if (C < C_mid) {
  		float k_1 = mid * C_0;
  		float k_2 = (1.0f - k_1 / C_mid);

  		float t = C / (k_1 + k_2 * C);
  		s = t * mid;
  	} else {
  		float k_0 = C_mid;
  		float k_1 = (1.0f - mid) * C_mid * C_mid * mid_inv * mid_inv / C_0;
  		float k_2 = (1.0f - (k_1) / (C_max - C_mid));

  		float t = (C - k_0) / (k_1 + k_2 * (C - k_0));
  		s = mid + (1.0f - mid) * t;
  	}

  	float l = toe(L);

  	return OkHsl(h, s, l);
  }
}

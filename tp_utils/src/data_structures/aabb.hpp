#pragma once


#include "thirdparty/kmath/vector.hpp"


namespace tputils {
  struct AABB {
    kmath::Vec3 begin;
    kmath::Vec3 end;
  };
}

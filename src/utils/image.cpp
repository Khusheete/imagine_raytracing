#include "image.hpp"
#include <algorithm>


Image::Image(const size_t p_width, const size_t p_height, const kmath::Lrgb p_fill)
  : data(p_width * p_height, p_fill),
  width(p_width),
  height(p_height)
{}


Image Image::copy() const {
  Image image{};
  image.data = data;
  image.width = width;
  image.height = height;
  return image;
}


void Image::write_ppm(std::ostream &p_stream) const {
  p_stream << "P3" << std::endl << width << " " << height << std::endl << 255 << std::endl;
  for (size_t i = 0; i < data.size(); i++) {
    p_stream << static_cast<int>(255.0f * std::clamp(data[i].x, 0.0f, 1.0f)) << " ";
    p_stream << static_cast<int>(255.0f * std::clamp(data[i].y, 0.0f, 1.0f)) << " ";
    p_stream << static_cast<int>(255.0f * std::clamp(data[i].z, 0.0f, 1.0f)) << "\n";
  }
  p_stream << std::endl;
}

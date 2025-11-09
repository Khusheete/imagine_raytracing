#pragma once


#include "kmath/color.hpp"
#include <ostream>
#include <vector>


class Image;


template<typename F>
concept ImageMapper = requires(F f, const Image &img, int a, int b, kmath::Lrgb color) {
  color = f(img, a, b);
};


template<typename F>
concept ColorMapper = requires(F f, kmath::Lrgb result_color, const kmath::Lrgb &input_color) {
  result_color = f(input_color);
};


class Image {
public:
  inline size_t get_width() const { return width; }
  inline size_t get_height() const { return height; }
  inline size_t get_size() const { return data.size(); }
  
  inline kmath::Lrgb &operator()(const size_t p_index) {
    return data[p_index];
  }

  const kmath::Lrgb &operator()(const size_t p_index) const {
    return data[p_index];
  }

  inline kmath::Lrgb &operator()(const size_t p_x_index, const size_t p_y_index) {
    return data[p_x_index + p_y_index * width];
  }

  const kmath::Lrgb &operator()(const size_t p_x_index, const size_t p_y_index) const {
    return data[p_x_index + p_y_index * width];
  }

  inline kmath::Lrgb &operator()(const int p_x_index, const int p_y_index) {
    return data[p_x_index + p_y_index * width];
  }

  const kmath::Lrgb &operator()(const int p_x_index, const int p_y_index) const {
    return data[p_x_index + p_y_index * width];
  }


  template<ImageMapper M>
  inline Image copy_map(M p_mapper) const {
    Image result = copy();
    for (size_t j = 0; j < height; j++) {
      for (size_t i = 0; i < width; i++) {
        result(i, j) = p_mapper(*this, i, j);
      }
    }
    return result;
  }


  template<ColorMapper M>
  inline void map(M p_mapper) {
    for (size_t i = 0; i < data.size(); i++) {
      data[i] = p_mapper(data[i]);
    }
  }


  Image copy() const;

  void write_ppm(std::ostream &p_stream) const;
  

  Image(const size_t p_width, const size_t p_height, const kmath::Lrgb p_fill = kmath::Vec3::ZERO);
  Image(Image&&) = default;
  Image &operator=(Image&&) = default;
  Image(const Image&) = delete;
  Image &operator=(const Image&) = delete;
  ~Image() = default;

private:
  Image() = default;
  
private:
  std::vector<kmath::Lrgb> data;
  size_t width;
  size_t height;
};

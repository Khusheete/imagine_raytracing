/* ------------------------------------------------------------------------------------------------------------------ *
*                                                                                                                     *
*                                                                                                                     *
*                                                /\                    ^__                                            *
*                                               /#*\  /\              /##@>                                           *
*                                              <#* *> \/         _^_  \\    _^_                                       *
*                                               \##/            /###\ \è\  /###\                                      *
*                                                \/ /\         /#####n/xx\n#####\                                     *
*                   Ferdinand                       \/         \###^##xXXx##^###/                                     *
*                        Souchet                                \#/ V¨\xx/¨V \#/                                      *
*                     (aka. @Khusheete)                          V     \c\    V                                       *
*                                                                       //                                            *
*                                                                     \o/                                             *
*             ferdinand.souchet@etu.umontpellier.fr                    v                                              *
*                                                                                                                     *
*                                                                                                                     *
*                                                                                                                     *
*                                                                                                                     *
* Copyright 2025 Ferdinand Souchet (aka. @Khusheete)                                                                  *
*                                                                                                                     *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated        *
* documentation files (the “Software”), to deal in the Software without restriction, including without limitation the *
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to     *
* permit persons to whom the Software is furnished to do so, subject to the following conditions:                     *
*                                                                                                                     *
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of    *
* the Software.                                                                                                       *
*                                                                                                                     *
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO    *
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, *
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE      *
* SOFTWARE.                                                                                                           *
*                                                                                                                     *
* ------------------------------------------------------------------------------------------------------------------ */


#pragma once


#include <algorithm>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <utility>


namespace tputils {
  
  // Would not be needed in C++23
  template<typename T>
  constexpr std::underlying_type<T>::type to_underlying(T _value)
  {
    return static_cast<typename std::underlying_type<T>::type>(_value);
  }


  template<typename T, std::size_t MAX_SIZE>
  class StackVector
  {
  public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = value_type*;
    using const_iterator = const value_type*;

  public:

    inline constexpr size_type size() const
    {
      return element_count;
    }

    inline constexpr size_type max_size() const
    {
      return MAX_SIZE;
    }

    inline constexpr size_type capacity() const
    {
      return MAX_SIZE;
    }

    inline constexpr reference operator[](const size_type _index)
    {
      return elements[_index];
    }

    inline constexpr const_reference operator[](const size_type _index) const
    {
      return (*const_cast<StackVector<value_type, MAX_SIZE>*>(this))[_index];
    }

    inline constexpr iterator begin()
    {
      return &elements[0];
    }

    inline constexpr const_iterator begin() const
    {
      return &elements[0];
    }

    inline constexpr iterator end()
    {
      return &elements[0] + element_count;
    }

    inline constexpr const_iterator end() const
    {
      return &elements[0] + element_count;
    }

    inline constexpr pointer data()
    {
      return &elements[0];
    }

    inline constexpr const_pointer data() const
    {
      return &elements[0];
    }

    inline constexpr bool empty() const
    {
      return element_count == 0;
    }

    inline constexpr void push_back(const value_type &p_value)
    {
      if (size() == MAX_SIZE) throw std::length_error("Cannot push to StackVector past the maximum size.");
      elements[element_count] = p_value;
      element_count += 1;
    }

    inline constexpr void push_back(value_type &&p_value)
    {
      if (size() == MAX_SIZE) throw std::length_error("Cannot push to StackVector past the maximum size.");
      elements[element_count] = p_value;
      element_count += 1;
    }

    inline constexpr void pop_back()
    {
      element_count -= 1;
    }

    inline constexpr bool contains(const value_type &p_element) {
      for (value_type &e : elements) {
        if (e == p_element) return true;
      }
      return false;
    }

    StackVector() = default;
    ~StackVector() = default;

    StackVector(StackVector<value_type, MAX_SIZE> &&p_other)
    {
      element_count = p_other.element_count;
      for (size_type i = 0; i < element_count; i++)
      {
        elements[i] = std::move(p_other.elements[i]);
      }
      p_other.element_count = 0;
    }

    StackVector(const StackVector<value_type, MAX_SIZE> &p_other)
    {
      element_count = p_other.element_count;
      std::copy(p_other.begin(), p_other.end(), begin());
    }

    StackVector<value_type, MAX_SIZE> &operator=(StackVector<value_type, MAX_SIZE> &&p_other)
    {
      element_count = p_other.element_count;
      for (size_type i = 0; i < element_count; i++)
      {
        elements[i] = std::move(p_other.elements[i]);
      }
      p_other.element_count = 0;
      return *this;
    }

    StackVector<value_type, MAX_SIZE> &operator=(const StackVector<value_type, MAX_SIZE> &p_other)
    {
      element_count = p_other.element_count;
      std::copy(p_other.begin(), p_other.end(), begin());
    }

    // TODO: implement `insert`, `emplace`, `emplace_back`, `erase`, `at`, `back`, `front`

  private:
    value_type elements[MAX_SIZE];
    size_type element_count = 0;
  };
}

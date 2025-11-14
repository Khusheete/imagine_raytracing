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

#include <chrono>
#include <fstream>
#include <ostream>
#include <iostream>


struct Time {
  uint64_t seconds;
  uint16_t millis;
  uint16_t micros;
  uint16_t nanos;
};


class Profiler {
public:
  inline void start() {
    start_time = std::chrono::high_resolution_clock::now();
  }


  inline void dont_optimize(auto &p_value) {
    // __asm__ __volatile__ ("" : "+r,m"(p_value) : : "memory"); // Ne fonctionne pas
    std::ofstream dnull;
    dnull.open("/dev/null");
    dnull << p_value << std::endl;
    dnull.close();
  }


  inline void end() {
    end_time = std::chrono::high_resolution_clock::now();
  }


  inline uint64_t get_exec_time_seconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
  }


  inline uint64_t get_exec_time_milliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
  }


  inline uint64_t get_exec_time_microseconds() {
    return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
  }


  inline uint64_t get_exec_time_nanoseconds() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
  }


  inline Time get_exec_time() {
    return Time{
      .seconds = get_exec_time_seconds(),
      .millis  = static_cast<uint16_t>(get_exec_time_milliseconds() % 1000),
      .micros  = static_cast<uint16_t>(get_exec_time_microseconds() % 1000),
      .nanos   = static_cast<uint16_t>(get_exec_time_nanoseconds()  % 1000),
    };
  }
  
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
};


inline std::ostream &operator<<(std::ostream &p_stream, const Time &p_time) {
  bool printing = false;
  if (p_time.seconds != 0) {
    p_stream << p_time.seconds << "s ";
    printing = true;
  }
  if (printing || p_time.millis != 0) {
    p_stream << p_time.millis << "ms ";
    printing = true;
  }
  if (printing || p_time.micros != 0) {
    p_stream << p_time.micros << "\u03BCs ";
    printing = false;
  }
  p_stream << p_time.nanos << "ns";
  return p_stream;
}

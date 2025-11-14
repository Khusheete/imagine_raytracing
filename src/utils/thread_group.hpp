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


#include <atomic>
#include <barrier>
#include <functional>
#include <thread>
#include <vector>


typedef std::function<void(size_t p_thread_id, size_t p_exec_index)> ParallelFunction;


class ThreadWorkGroup {
public:
  void execute(const ParallelFunction p_func, const size_t p_begin_index, const size_t p_end_index, const size_t progress_report_interval = 100);
  void cancel();
  double get_progress() const;
  inline size_t get_done_thread_count() const { return done_thread_count; }
  inline bool is_work_done() const { return group_state != GroupState::WORKING || done_thread_count == get_thread_count(); }

  void join();
  inline size_t get_thread_count() const { return workers.size(); }

  ThreadWorkGroup(const size_t p_size);
  ~ThreadWorkGroup();

private:
  enum GroupState {
    IDLE,
    WORKING,
    EXIT,
  };

private:

  void _reset();
  
private:
  std::vector<std::thread> workers;
  std::barrier<> sync;

  ParallelFunction func;
  size_t begin_index;
  size_t index_stride;
  size_t end_index;

  size_t progress_report;

  std::atomic<size_t> progress;
  std::atomic<size_t> done_thread_count;

  std::atomic<GroupState> group_state = GroupState::IDLE;
};

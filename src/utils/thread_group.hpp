#pragma once


#include <atomic>
#include <barrier>
#include <functional>
#include <thread>
#include <vector>


typedef std::function<void(size_t)> ParallelFunction;


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

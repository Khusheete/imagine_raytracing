#include "thread_group.hpp"
#include <algorithm>
#include <cassert>
#include <thread>


void ThreadWorkGroup::execute(const ParallelFunction p_func, const size_t p_begin_index, const size_t p_end_index, const size_t progress_report_interval) {
  assert(group_state == GroupState::IDLE); // The previous job must have been joined before starting a new job.
  const size_t thread_count = get_thread_count();

  // Setup indices
  begin_index = p_begin_index;
  end_index = p_end_index;
  const size_t exec_length = p_end_index - p_begin_index;
  index_stride = (p_end_index - p_begin_index) / thread_count + ((exec_length % thread_count)? 1 : 0);

  progress_report = progress_report_interval;
  func = p_func;

  // Reset counters
  progress = 0;
  done_thread_count = 0;

  // Start execution
  group_state = GroupState::WORKING;
  sync.arrive_and_wait();
}


void ThreadWorkGroup::_reset() {
  begin_index = 0;
  end_index = 0;
  index_stride = 0;
  func = ParallelFunction();
}


void ThreadWorkGroup::cancel() {
  assert(group_state == GroupState::WORKING);
  // Cancel the current work
  group_state = GroupState::IDLE;
  // Wait for every thread to stop
  sync.arrive_and_wait();
}


double ThreadWorkGroup::get_progress() const {
  return double(progress) / (end_index - begin_index);
}


void ThreadWorkGroup::join() {
  assert(group_state == GroupState::WORKING);
  // Wait for every thread to stop
  sync.arrive_and_wait();
  group_state = GroupState::IDLE;
}


ThreadWorkGroup::ThreadWorkGroup(const size_t p_size)
  : workers(p_size),
  sync(p_size + 1)
{
  for (size_t i = 0; i < p_size; i++) {
    workers[i] = std::thread([&, i]() {
      const size_t thread_id = i;

      while (group_state != GroupState::EXIT) {
        sync.arrive_and_wait(); // Wait for something to happen

        if (group_state == GroupState::EXIT) {
          break;
        }
        
        const size_t local_begin_index = begin_index + index_stride * thread_id;
        const size_t local_end_index = std::min<size_t>(local_begin_index + index_stride, end_index);
        const size_t local_exec_count = local_end_index - local_begin_index;

        for (size_t i = 0; i < local_exec_count; i += 1) {
          const size_t exec_index = local_begin_index + i;
          func(exec_index);

          if (group_state != GroupState::WORKING) {
            break; // Cancel execution
          }

          if (progress_report && (i + 1) % progress_report == 0) {
            progress += progress_report;
          }
        }

        done_thread_count += 1;
        progress += (local_end_index - local_begin_index) % progress_report;

        sync.arrive_and_wait(); // Wait for a join
      }

      sync.arrive_and_drop();
    });
  }
}


ThreadWorkGroup::~ThreadWorkGroup() {
  group_state = GroupState::EXIT;
  sync.arrive_and_drop();
  for (std::thread &worker : workers) {
    worker.join();
  }
}

#ifndef WORKER_CREW_MANAGER_H
#define WORKER_CREW_MANAGER_H

#include <pthread.h>

#include <chrono>
#include <map>
#include <vector>

#include "eventpp/eventqueue.h"
#include "eventpp/utilities/orderedqueuelist.h"

namespace mh {

class WorkerCrewManager {
 public:
  struct Event {
    int e_id;
    int priority;
  };

  class Exception : public std::exception {
   public:
    class WorkerCrewExists;
    class FailedToSpawnWorker;
    class WorkerCrewNotFound;
  };

 private:
  struct Compare {
    template <typename T>
    bool operator()(const T &a, const T &b) const {
      return std::get<0>(a.arguments).priority >
             std::get<0>(b.arguments).priority;
    }
  };

  struct QueuePolicy {
    template <typename Item>
    using QueueList = eventpp::OrderedQueueList<Item, Compare>;

    static int getEvent(const Event &event, void *const a) {
      return event.e_id;
    }
  };

  struct WorkerCrew {
    std::vector<pthread_t> thread_ids;
    eventpp::EventQueue<int, void(const Event &, void *const), QueuePolicy>
        *worker_queue;
  };
  static std::map<std::string, WorkerCrew> workers;
  static void *worker_thread(void *);

  // Since the class is meant to be used as a static class it's constructor is
  // set to be private
  WorkerCrewManager();

 public:
  using PriorityEventQueue =
      eventpp::EventQueue<int, void(const Event &, void *const), QueuePolicy>;

  static PriorityEventQueue *create_worker_crew(int num_workers,
                                                std::string worker_name);
  static void kill_worker_crew(std::string, bool finish_remained_jobs);

  static PriorityEventQueue *get_queue(std::string);
};

// Exceptions:
class WorkerCrewManager::Exception::WorkerCrewExists
    : public WorkerCrewManager::Exception {
  virtual const char *what() const noexcept {
    return "Worker crew already exists";
  }
};

class WorkerCrewManager::Exception::FailedToSpawnWorker
    : public WorkerCrewManager::Exception {
  virtual const char *what() const noexcept {
    return "Failed to Create worker thread";
  }
};

class WorkerCrewManager::Exception::WorkerCrewNotFound
    : public WorkerCrewManager::Exception {
  virtual const char *what() const noexcept {
    return "The requested worker Crew does not exist";
  }
};

}  // namespace mh

#endif

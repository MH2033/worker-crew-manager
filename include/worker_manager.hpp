#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include <map>
#include <vector>
#include <pthread.h>
#include <chrono>
#include "eventpp/utilities/orderedqueuelist.h"
#include "eventpp/eventqueue.h"


namespace ir {
  
class WorkerManager {

public:
  struct Event {
    int e;
    int priority;
  };

private:
  struct Compare {
    template <typename T>
    bool operator() (const T & a, const T & b) const {
      return std::get<0>(a.arguments).priority > std::get<0>(b.arguments).priority;
      }
  };

  struct QueuePolicy{
    template <typename Item>
    using QueueList = eventpp::OrderedQueueList<Item, Compare >;

    static int getEvent(const Event & event, void * const a) {
      return event.e;
    }
  };

  using PriorityEventQueue = eventpp::EventQueue<int, void(const Event &, void * const), QueuePolicy>;

  struct WorkerCrew  {
    std::vector<pthread_t > thread_ids;
    PriorityEventQueue *worker_queue;
  };
  static std::map<std::string, WorkerCrew> workers;
  static void *WorkerThread(void *);
  
  //Since the class is meant to be used as a static class it's constructor is set to be private
  WorkerManager();
public:
  static PriorityEventQueue *CreateWorkerCrew(int num_workers, std::string worker_name);
  static bool KillWorkerCrew(std::string, bool force_clear_queue);

  static PriorityEventQueue *get_queue(std::string);
};

}

#endif


#ifndef WORKER_CREW_MANAGER_H
#define WORKER_CREW_MANAGER_H

#include <map>
#include <vector>
#include <pthread.h>
#include <chrono>
#include "eventpp/utilities/orderedqueuelist.h"
#include "eventpp/eventqueue.h"


namespace mh {
  
class WorkerCrewManager {

public:
  struct Event {
    int e;
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
  WorkerCrewManager();
public:
  static PriorityEventQueue *createWorkerCrew(int num_workers, std::string worker_name);
  static void KillWorkerCrew(std::string, bool force_clear_queue);

  static PriorityEventQueue *get_queue(std::string);
};

//Exceptions:
class WorkerCrewManager::Exception::WorkerCrewExists : public WorkerCrewManager::Exception {
  virtual const char* what() const noexcept {return "Worker crew already exists";}
};

class WorkerCrewManager::Exception::FailedToSpawnWorker : public WorkerCrewManager::Exception {
  virtual const char* what() const noexcept {return "Failed to Create worker thread";}
};

class WorkerCrewManager::Exception::WorkerCrewNotFound : public WorkerCrewManager::Exception {
  virtual const char* what() const noexcept {return "The requested worker Crew does not exist";}
};

}




#endif


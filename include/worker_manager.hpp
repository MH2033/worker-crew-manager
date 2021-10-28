#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include <map>
#include <vector>
#include <pthread.h>


namespace ir {
class WorkerManager {
public:
  static void CreateWorker(int num_threads, std::string worker_name);
  static void KillWorker(std::string);
};
}

#endif


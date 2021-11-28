#include <exception>
#include "worker_manager.hpp"
#define TERMINATION_EVENT -2

using namespace std;
using namespace ir;

map<string, WorkerManager::WorkerCrew> WorkerManager::workers;

void *WorkerManager::WorkerThread(void *param){
  auto queue = (PriorityEventQueue *)param;

  while(true) {
    queue->wait();
    PriorityEventQueue::QueuedEvent e;
    queue->peekEvent(&e);
    if(e.event == TERMINATION_EVENT)
      break;

    queue->processOne();
  }
  pthread_exit(NULL);
}

WorkerManager::PriorityEventQueue *WorkerManager::CreateWorkerCrew(int num_workers, std::string worker_name){
  if(workers.count(worker_name) > 0)
    throw runtime_error("Worker crew already exists");

  WorkerCrew crew;
  crew.worker_queue = new PriorityEventQueue;
  for(int i = 0; i < num_workers; i++){
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, WorkerThread, crew.worker_queue);
    if(ret != 0){
      delete crew.worker_queue;
      throw runtime_error("Unable to create worker thread");
    }
    crew.thread_ids.push_back(tid);
  }
  workers[worker_name] = crew;
  return crew.worker_queue;
}

bool WorkerManager::KillWorkerCrew(std::string worker_name, bool force_clear_queue) {
  if(workers.count(worker_name) > 0) {
    auto queue = workers[worker_name].worker_queue;
    //Clear remaining events from queue
    if(force_clear_queue)
      queue->clearEvents();

    //Issue the termination event to all threads
    for(auto i:workers[worker_name].thread_ids)
      queue->enqueue(Event{TERMINATION_EVENT, 0}, nullptr);

    //Wait for the threads to join
    for(auto i:workers[worker_name].thread_ids)
      pthread_join(i, NULL);

    delete queue;
    workers.erase(worker_name);
    return true;
  }
  return false;
}


WorkerManager::PriorityEventQueue *WorkerManager::get_queue(std::string worker_name) {
  if(workers.count(worker_name) > 0)
    return workers[worker_name].worker_queue;
  return nullptr;
}


#include <exception>
#include "worker_manager.hpp"
#define TERMINATION_EVENT -2

using namespace std;
using namespace mh;

map<string, WorkerCrewManager::WorkerCrew> WorkerCrewManager::workers;

void *WorkerCrewManager::WorkerThread(void *param){
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

WorkerCrewManager::PriorityEventQueue *WorkerCrewManager::createWorkerCrew(int num_workers, std::string worker_name){
  if(workers.count(worker_name) > 0)
    throw Exception::WorkerCrewExists();

  WorkerCrew crew;
  crew.worker_queue = new PriorityEventQueue;
  for(int i = 0; i < num_workers; i++){
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, WorkerThread, crew.worker_queue);
    if(ret != 0){
      //To-Do: Terminate potentially created threads
      delete crew.worker_queue;
      throw Exception::FailedToSpawnWorker();
    }
    crew.thread_ids.push_back(tid);
  }
  workers[worker_name] = crew;
  return crew.worker_queue;
}

void WorkerCrewManager::KillWorkerCrew(std::string worker_name, bool finish_remained_jobs) {
  if(workers.count(worker_name) > 0) {
    auto queue = workers[worker_name].worker_queue;
    //Clear remaining events from queue if set;
    if(!finish_remained_jobs)
      queue->clearEvents();

    //Issue the termination event to all threads
    for(auto i:workers[worker_name].thread_ids)
      queue->enqueue(Event{TERMINATION_EVENT, 0}, nullptr);

    //Wait for the threads to join
    for(auto i:workers[worker_name].thread_ids)
      pthread_join(i, NULL);

    delete queue;
    workers.erase(worker_name);
  }

  throw Exception::WorkerCrewNotFound();
}


WorkerCrewManager::PriorityEventQueue *WorkerCrewManager::get_queue(std::string worker_name) {
  if(workers.count(worker_name) > 0)
    return workers[worker_name].worker_queue;
  
  throw Exception::WorkerCrewNotFound();
}


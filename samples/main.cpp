#include <iostream>
#include <thread> 
#include "worker_crew_manager.hpp"

using namespace std;
using namespace mh;

int main() {
	auto queue = WorkerCrewManager::createWorkerCrew(2, "Sample");

	//Append listener on event with id 1
	queue->appendListener(1, [](WorkerCrewManager::Event, void *param){
		auto num = (int *)param;
		cout << "Received number: " << *num << endl;
		cout << "Handler called in thread: " << this_thread::get_id() << endl;
		delete num;
	});

	int a = 5;
	for(int i = 0; i < 100; i++){
		queue->enqueue(WorkerCrewManager::Event{1, 1}, new int(i));
	}

	WorkerCrewManager::KillWorkerCrew("Sample", false);
	cout << "Worker Crew terminated" << endl;
	exit(EXIT_SUCCESS);
}
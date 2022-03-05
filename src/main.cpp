#include <iostream>
#include <thread> 
#include "worker_manager.hpp"

using namespace std;
using namespace mh;

int main() {
	auto queue = WorkerCrewManager::createWorkerCrew(2, "test");
	try{
		auto queue1 = WorkerCrewManager::createWorkerCrew(2, "test");
	}

	catch(WorkerCrewManager::Exception &e){
		cout << "Exception occured" << endl;
		exit(EXIT_FAILURE);
	}

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

	// this_thread::sleep_for(std::chrono::seconds(10));


	WorkerCrewManager::KillWorkerCrew("test", true);
	cout << "Worker Crew terminated" << endl;
	// this_thread::sleep_for(std::chrono::seconds(20));
	exit(EXIT_SUCCESS);
}
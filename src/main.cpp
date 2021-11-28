#include <iostream>
#include <thread> 
#include "worker_manager.hpp"

using namespace std;
using namespace ir;

int main() {
	auto queue = WorkerManager::CreateWorkerCrew(2, "test");
	queue->appendListener(1, [](WorkerManager::Event, void *param){
		auto num = (int *)param;
		cout << "Received number: " << *num << endl;
		cout << "Handler called in thread: " << this_thread::get_id() << endl;
		auto var = new int[1000];
		// this_thread::sleep_for(std::chrono::seconds(1));
		delete var;
	});

	int a = 5;
	for(int i = 0; i < 100; i++)
		queue->enqueue(WorkerManager::Event{1, 1}, &a);

	this_thread::sleep_for(std::chrono::seconds(10));


	WorkerManager::KillWorkerCrew("test");

	cout << "Worker Crew terminated" << endl;
	this_thread::sleep_for(std::chrono::seconds(20));


}
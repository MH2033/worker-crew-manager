#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <thread>
#include <mutex>
#include "worker_crew_manager.hpp"
using namespace mh;
using namespace std;
std::mutex myMutex;

TEST_CASE("Creating duplicate instance", "[worker]")
{
  REQUIRE_NOTHROW(WorkerCrewManager::create_worker_crew(2, "test"));
  REQUIRE_THROWS_AS(WorkerCrewManager::create_worker_crew(2, "test"),
                    WorkerCrewManager::Exception::WorkerCrewExists);
}

TEST_CASE("Create worker with zero number of worker", "[worker]")
{
  REQUIRE_THROWS_AS(WorkerCrewManager::create_worker_crew(0, "test2"),
                    WorkerCrewManager::Exception::FailedToSpawnWorker);
}

TEST_CASE("Create worker with negative number of worker", "[worker]")
{
  REQUIRE_THROWS_AS(WorkerCrewManager::create_worker_crew(-1, "test3"),
                    WorkerCrewManager::Exception::FailedToSpawnWorker);
}

TEST_CASE("Create worker with empty name", "[worker]")
{
  REQUIRE_THROWS_AS(WorkerCrewManager::create_worker_crew(1, ""),
                    WorkerCrewManager::Exception::FailedToSpawnWorker);
}

TEST_CASE("Create worker greator than max of cpu number")
{
  int max = std::thread::hardware_concurrency();
  REQUIRE_THROWS_AS(WorkerCrewManager::create_worker_crew(max + 1, "test5"),
                    WorkerCrewManager::Exception::FailedToSpawnWorker);
}

TEST_CASE("Kill wrong worker crew", "[worker]")
{
  REQUIRE_NOTHROW(WorkerCrewManager::create_worker_crew(2, "test6"));
  REQUIRE_THROWS_AS(WorkerCrewManager::kill_worker_crew("_test6", false),
                    WorkerCrewManager::Exception::WorkerCrewNotFound);
}

TEST_CASE("Getting wrong worker_queue", "[worker]")
{
  REQUIRE_NOTHROW(WorkerCrewManager::create_worker_crew(2, "test7"));
  REQUIRE_THROWS_AS(WorkerCrewManager::get_queue("_test7"),
                    WorkerCrewManager::Exception::WorkerCrewNotFound);
}

TEST_CASE("Performance Test Accumulate params", "worker")
{
  static std::atomic<int32_t> accum{0};
  int max = std::thread::hardware_concurrency();
  INFO("");
  auto queue = WorkerCrewManager::create_worker_crew(max, "test8");
  queue->appendListener(1, [=](WorkerCrewManager::Event, void *param)
                        {

   auto num = (int *)param;
    accum += *num;
    INFO("Received number: " << *num);
   INFO("Handler called in thread: " << this_thread::get_id());
    INFO("accum value:" << accum);
   delete num; });

  for (int i = 0; i <= 100; i++)
  {
    queue->enqueue(WorkerCrewManager::Event{1, 1}, new int(i));
  }

  REQUIRE_NOTHROW(WorkerCrewManager::kill_worker_crew("test8", true));
  INFO("Worker Crew terminated");
  CHECK(accum == 5050);
}

TEST_CASE("Performance Test Accumulate vector", "worker")
{

  struct Container
  {
    std::vector<int> *vec;
    int *result;
  };

  int max = std::thread::hardware_concurrency();
  auto queue = WorkerCrewManager::create_worker_crew(max, "test9");
  for (int i = 0; i < 2; i++)
  {
    queue->appendListener(i + 1, [](WorkerCrewManager::Event, void *param1)
                          {
  auto s = (Container *)param1;

  *s->result =  accumulate(s->vec->begin(), s->vec->end(), 0);
  INFO("Handler called in thread: " << this_thread::get_id()); });
  }
  std::vector<int> v1, v2;
  int res1, res2;

  for (int i = 0; i <= 100; i++)
  {
    v1.push_back(i);
    v2.push_back(-i);
  }
  Container container1;
  Container container2;

  container1.vec = &v1;
  container1.result = &res1;
  queue->enqueue(WorkerCrewManager::Event{1, 1}, &container1);

  container2.vec = &v2;
  container2.result = &res2;
  queue->enqueue(WorkerCrewManager::Event{2, 1}, &container2);

  REQUIRE_NOTHROW(WorkerCrewManager::kill_worker_crew("test9", true));
  INFO("Worker Crew terminated");
  CHECK(res1 == 5050);
  CHECK(res2 == -res1);
}

TEST_CASE("Performance Test turn off alarm", "worker")
{
#define SIZE 10000
  typedef enum
  {
    On,
    Off
  } AlarmType;
  static std::map<int, AlarmType> map;

  for (size_t i = 0; i < SIZE; i++)
  {
    map.insert(std::make_pair(i, AlarmType::On));
  }

  int max = std::thread::hardware_concurrency();
  auto queue = WorkerCrewManager::create_worker_crew(max, "test10");
  for (int i = 0; i < SIZE; i++)
  {
    queue->appendListener(i, [=](WorkerCrewManager::Event, void *param)
                          {
  auto num = (int *)param;
  std::lock_guard<std::mutex> guard(myMutex);
  auto it = map.find(i);
  if (it != map.end())
     it->second = AlarmType::Off;

  INFO("Handler called in thread: " << this_thread::get_id()); });
  }

  for (int i = 0; i < SIZE; i++)
  {
    queue->enqueue(WorkerCrewManager::Event{i, 1}, new int(i));
  }

  REQUIRE_NOTHROW(WorkerCrewManager::kill_worker_crew("test10", true));
  INFO("Worker Crew terminated");
  for (auto it = map.begin(); it != map.end(); it++)
    cout << it->first << "-" << it->second << endl;
}

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "worker_crew_manager.hpp"
using namespace mh;

TEST_CASE( "Creating duplicate instance", "[worker]" ) {
  REQUIRE_NOTHROW(WorkerCrewManager::createWorkerCrew(2, "test"));
  REQUIRE_THROWS_AS(WorkerCrewManager::createWorkerCrew(2, "test"), WorkerCrewManager::Exception::WorkerCrewExists);

}

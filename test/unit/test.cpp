#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "worker_crew_manager.hpp"
using namespace mh;

TEST_CASE( "Creating duplicate instance", "[worker]" ) {
  REQUIRE_NOTHROW(WorkerCrewManager::create_worker_crew(2, "test"));
  REQUIRE_THROWS_AS(WorkerCrewManager::create_worker_crew(2, "test"), WorkerCrewManager::Exception::WorkerCrewExists);

}

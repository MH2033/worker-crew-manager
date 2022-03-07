project(unit_tests)

add_executable(${PROJECT_NAME} test.cpp)

target_link_libraries(${PROJECT_NAME} worker_crew_manager)
add_catch_test(${PROJECT_NAME})
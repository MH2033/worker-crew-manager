cmake_minimum_required(VERSION 3.15.0)
project(worker_crew_manager_samples)

#Add all .cpp and .c files in src folder to compile list
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp
                    ${PROJECT_SOURCE_DIR}/src/*.c
)

add_executable(sample main.cpp)
target_link_libraries(sample worker_crew_manager)
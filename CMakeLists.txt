cmake_minimum_required(VERSION 3.15.0)
project(worker_crew_manager VERSION 0.1.0)


#Setting compilation flags 
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

#Build flags
option(BUILD_SAMPLES "Build available samples" ON)
option(BUILD_TESTS "Build available samples" ON)


#Add all .cpp and .c files in src folder to compile list
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp
                    ${PROJECT_SOURCE_DIR}/src/*.c
    )

#Add all header files located include to headers list
include_directories(${PROJECT_SOURCE_DIR}/inc)

#Setting target name
add_library(${CMAKE_PROJECT_NAME} STATIC ${PROJECT_SOURCE_DIR}/src/worker_crew_manager.cpp)

install(TARGETS ${CMAKE_PROJECT_NAME} ARCHIVE)

install(TARGETS ${CMAKE_PROJECT_NAME} PUBLIC_HEADER)

if(BUILD_SAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/samples)
endif()

if(BUILD_TESTS)
  add_subdirectory(test)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}
                      pthread
)




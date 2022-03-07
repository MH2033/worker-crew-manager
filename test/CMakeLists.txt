Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v2.13.8)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)


link_libraries(Catch2::Catch2)

macro(add_catch_test name)
    include(CTest)
    include(Catch)
    catch_discover_tests(${name})
endmacro()

add_subdirectory(unit)

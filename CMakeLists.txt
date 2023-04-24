cmake_minimum_required(VERSION 3.0.0)
project(tinyraytracing VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(tinyraytracing main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_CXX_STANDARD 17)

# find_package(OpenMP)
# if(OpenMP_CXX_FOUND)
#     target_link_libraries(tinyraytracing PUBLIC OpenMP::OpenMP_CXX)
# endif()

# OpemMp
if(NOT TARGET OpenMP::OpenMP_CXX)
    find_package(Threads REQUIRED)
    add_library(OpenMP::OpenMP_CXX IMPORTED INTERFACE)
    set_property(TARGET OpenMP::OpenMP_CXX
                 PROPERTY INTERFACE_COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
    # Only works if the same flag is passed to the linker; use CMake 3.9+ otherwise (Intel, AppleClang)
    set_property(TARGET OpenMP::OpenMP_CXX
                 PROPERTY INTERFACE_LINK_LIBRARIES ${OpenMP_CXX_FLAGS} Threads::Threads)
endif()
target_link_libraries(tinyraytracing PUBLIC OpenMP::OpenMP_CXX)
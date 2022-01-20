cmake_minimum_required(VERSION 3.20)
project(testapp C)

set(CMAKE_C_STANDARD 99)

add_executable(testapp main.c string_array.c list.c re.c string.c error.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(testapp ${PYTHON_LIBRARIES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(testapp Threads::Threads)

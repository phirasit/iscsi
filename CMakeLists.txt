cmake_minimum_required(VERSION 3.10)
project(iscsi-cli)

include_directories(.)

file(GLOB main_source *.[ch] request/*.[ch])
add_executable(iscsi ${main_source})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(iscsi PRIVATE Threads::Threads)


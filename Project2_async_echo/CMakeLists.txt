cmake_minimum_required(VERSION 4.0)
project(Project2_async_echo)

set(CMAKE_CXX_STANDARD 20)

#add_executable(Project2_async_echo src/main.cpp)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Use standard-compliant flags

find_package(Boost 1.65 REQUIRED COMPONENTS system)

add_executable(async_echo_server src/server.cpp
)
target_include_directories(async_echo_server PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(async_echo_server PRIVATE Boost::system)

# Optional: if you want to build the sync client too
add_executable(sync_echo_client src/client.cpp)  # Copy from Project 1
target_link_libraries(sync_echo_client PRIVATE Boost::system)
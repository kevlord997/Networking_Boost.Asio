cmake_minimum_required(VERSION 4.0)
project(Project1_echo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)  # Explicitly require C++17 (Boost.Asio loves modern C++)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Boost (Asio is header-only, but specify components)
find_package(Boost REQUIRED COMPONENTS system)  # 'system' for Asio dependencies

add_compile_options(-Wall -Wextra)

include_directories(${Boost_INCLUDE_DIRS})

# Server executable
add_executable(echo_server src/server.cpp)
target_include_directories(echo_server PRIVATE ${Boost_LIBRARIES})
target_link_libraries(echo_server PRIVATE Boost::system)

# Client executable
add_executable(echo_client src/client.cpp)
target_include_directories(echo_client PRIVATE ${Boost_LIBRARIES})
target_link_libraries(echo_client PRIVATE Boost::system)
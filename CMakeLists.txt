cmake_minimum_required(VERSION 4.0.0)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED true)

project(epoll_server)

add_subdirectory(src)


option(BUILD_TESTING "Build unit tests" ON)

if(BUILD_TESTING)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    add_subdirectory(tests)
endif()

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(
    server_main
    server_main.cpp
)

target_link_libraries(server_main PRIVATE epoll_server Boost::program_options)

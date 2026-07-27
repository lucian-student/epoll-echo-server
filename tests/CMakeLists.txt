add_executable(
    unit_tests
    test_server.cpp
)

target_compile_options(unit_tests PRIVATE -Wno-interference-size)

target_link_libraries(unit_tests PRIVATE 
    GTest::gtest
    GTest::gtest_main  # <-- This automatically provides the int main() for GTest!
    epoll_server         # <-- Links your main library target
)

target_include_directories(
    unit_tests PRIVATE ${CMAKE_CURRENT_LIST_DIR}/
)
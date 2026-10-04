cmake_minimum_required(VERSION 3.16)
project(low_latency_hft_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(order_book src/order_book.cpp)
target_include_directories(order_book PUBLIC include)
target_compile_options(order_book PRIVATE -Wall -Wextra -Wpedantic)

add_executable(hft_engine src/main.cpp)
target_link_libraries(hft_engine PRIVATE order_book)

enable_testing()
add_executable(order_book_tests tests/test_order_book.cpp)
target_link_libraries(order_book_tests PRIVATE order_book)
add_test(NAME order_book_tests COMMAND order_book_tests)

cmake_minimum_required(VERSION 3.16)
project(bems_edge_ai_gateway LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_library(bems_gateway_core
  src/Gateway.cpp
)

target_include_directories(bems_gateway_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(bems_gateway_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE bems_gateway_core)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

add_executable(gateway_tests tests/GatewayTests.cpp)
target_link_libraries(gateway_tests PRIVATE bems_gateway_core)
target_compile_options(gateway_tests PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME gateway_tests COMMAND gateway_tests)

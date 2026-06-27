cmake_minimum_required(VERSION 3.16)
project(sensor_stats VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Treat the codebase strictly — good signal in an interview.
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

# --- Library (so the logic is unit-testable independently of the CLI) ---
add_library(stats src/stats.cpp)
target_include_directories(stats PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# --- Executable ---
add_executable(sensor_stats src/main.cpp)
target_link_libraries(sensor_stats PRIVATE stats)

# --- Tests ---
option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  # For Windows / MSVC compatibility
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  add_executable(unit_tests tests/test_stats.cpp)
  target_link_libraries(unit_tests PRIVATE stats GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(unit_tests)
endif()

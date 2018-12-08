cmake_minimum_required(VERSION 3.10)

project(tests)

set(CMAKE_CXX_STANDARD 11)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(tests
  assignment.cpp
  tests_main.cpp
  constructors.cpp
  observers.cpp
  issues.cpp
  values.cpp
  create_destroy.cpp
  exception.cpp
  overhead.cpp
  )
target_include_directories(tests PRIVATE ..)
target_link_libraries(tests PRIVATE CONAN_PKG::catch2)
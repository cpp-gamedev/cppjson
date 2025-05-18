cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

include(version.cmake)

project(cppjson VERSION ${cppjson_version})

option(CPPJSON_BUILD_TESTS "Build cppjson tests" ${PROJECT_IS_TOP_LEVEL})

add_subdirectory(cppjson)

if(CPPJSON_BUILD_TESTS)
  enable_testing()
  add_subdirectory(Test)
endif()

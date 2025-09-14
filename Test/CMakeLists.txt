add_executable(cppjson-Test)

target_link_libraries(cppjson-Test PRIVATE
  cppjson::cppjson
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)


target_sources(cppjson-Test PRIVATE
  Test.cpp
)

target_link_libraries(cppjson-Test PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(cppjson-Test)

if(MSVC)
    add_compile_options("/Zi")
    add_link_options("/PROFILE")
endif()

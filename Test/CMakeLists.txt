add_executable(cppjson-Test)

target_link_libraries(cppjson-Test PRIVATE
  cppjson::cppjson
)

target_sources(cppjson-Test PRIVATE
  Test.cpp
)

add_test(
  NAME cppjson-Test
  COMMAND cppjson-Test
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

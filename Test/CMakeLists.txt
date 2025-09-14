add_executable(cppjson-Test)

target_link_libraries(cppjson-Test PRIVATE
  cppjson::cppjson
)

target_sources(cppjson-Test PRIVATE
  Test.cpp
)

set(TEST_SOURCES
    TestEmptyObject.cpp
    TestPrimitives.cpp
    TestNestedObjects.cpp
)

if(MSVC)
    add_compile_options("/Zi")
    add_link_options("/PROFILE")
endif()


foreach(TEST_SRC IN LISTS TEST_SOURCES)
  message(Adding ${TEST_SRC})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} tests/${TEST_SRC})
    target_link_libraries(${TEST_NAME} PRIVATE cppjson::cppjson)
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

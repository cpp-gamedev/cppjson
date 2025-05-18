add_library(cppjson)
add_library(cppjson::cppjson ALIAS cppjson)

target_compile_features(cppjson PUBLIC
  cxx_std_23
)

file(GLOB_RECURSE headers LIST_DIRECTORIES false "include/*.hpp")
target_sources(cppjson PUBLIC FILE_SET HEADERS
  BASE_DIRS include
  FILES ${headers}
)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "src/*.[hc]pp")
target_sources(cppjson PRIVATE
  ${sources}
)



set(CMAKE_CXX_STANDARD 17)

include_directories(${COMMON_DIR}/config/)


add_library(libmemtracker SHARED src/memory-tracker.cpp)

target_link_libraries(libmemtracker PRIVATE dl)
target_compile_options(libmemtracker PRIVATE -fPIC)

set_target_properties(libmemtracker PROPERTIES LINKER_LANGUAGE CXX)

set_source_files_properties(src/memory-tracker.cpp PROPERTIES LANGUAGE CXX)

add_subdirectory(tests)
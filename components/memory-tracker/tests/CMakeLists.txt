include_directories(${COMMON_DIR})

add_executable(mem_tracker_test tracker-test.cpp)

set_target_properties(mem_tracker_test PROPERTIES LINKER_LANGUAGE CXX)
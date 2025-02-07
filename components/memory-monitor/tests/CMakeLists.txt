
include_directories(${COMMON_DIR}/testing)
include_directories(../include)

add_executable(mem_monitor_test test.c
        ../src/mem-info.c
        ../src/mem-writer.c
        ../src/mem-threading.c
        ../src/mem-reader.c
        ../src/process-reader.c
)

message(WARNING "Monitor test has debug symbols")
set(CMAKE_BUILD_TYPE Debug)
target_compile_definitions(mem_monitor_test PRIVATE MEM_TEST)
target_link_libraries(mem_monitor_test PRIVATE pthread)

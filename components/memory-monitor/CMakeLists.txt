
include_directories("include/")
include_directories(${COMMON_DIR}/config)

add_executable(mem_monitor
        src/main.c
        src/mem-info.c
        src/mem-writer.c
        src/mem-threading.c
        src/mem-reader.c
        src/process-reader.c
)

target_link_libraries(mem_monitor PRIVATE pthread)


add_subdirectory(tests)
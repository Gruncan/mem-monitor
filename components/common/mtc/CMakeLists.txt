

set(MTC_SOURCES
    src/mtccdecoder.c
    src/tmtcdecoder.c
)

set_source_files_properties(${MTC_SOURCES} PROPERTIES LANGUAGE C)

message(WARNING "Decoder has debug symbols")
set(CMAKE_BUILD_TYPE Debug)


include_directories(include/ ${COMMON_DIR}/config/)

add_library(mtc_decoder STATIC ${MTC_SOURCES})
#target_compile_definitions(mtc_decoder PRIVATE VERSION_1)

add_subdirectory(tests)
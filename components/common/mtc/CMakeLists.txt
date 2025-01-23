

set(MTC_SOURCES
    src/mtccdecoder.c
    src/tmtcdecoder.c
)

set_source_files_properties(${MTC_SOURCES} PROPERTIES LANGUAGE C)

message(WARNING "Decoder has debug symbols")
set(CMAKE_BUILD_TYPE Debug)

include_directories(include/)

if (UNIX)
    add_library(mtc_decoder STATIC ${MTC_SOURCES})
else (WIN32)
    add_library(mtc_decoder STATIC ${MTC_SOURCES})
endif ()
add_subdirectory(tests)
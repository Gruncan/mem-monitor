


set(MTC_SOURCES
    src/mtc-decoder.cpp
    src/memory-decoder.cpp
    src/mtc-object.cpp
#    include/mtc-config.h
)
set_source_files_properties(${MTC_SOURCES} PROPERTIES LANGUAGE CXX)

include_directories(include/)
if (UNIX)
    add_library(mtc_decoder SHARED ${MTC_SOURCES})
else (WIN32)
    add_library(mtc_decoder STATIC ${MTC_SOURCES})
endif ()
add_subdirectory(tests)
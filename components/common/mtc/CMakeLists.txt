


set(MTC_SOURCES
    src/mtc-decoder.cpp
    src/memory-decoder.cpp
    src/mtc-object.cpp
)
set_source_files_properties(${MTC_SOURCES} PROPERTIES LANGUAGE CXX)

include_directories(include/)

add_library(mtc_decoder SHARED ${MTC_SOURCES})
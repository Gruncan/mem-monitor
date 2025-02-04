
include_directories(../include ${COMMON_DIR})

set_source_files_properties(${MTC_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(mtc_tests mtc-tests.cpp)

target_link_libraries(mtc_tests PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtcdecoder.a)

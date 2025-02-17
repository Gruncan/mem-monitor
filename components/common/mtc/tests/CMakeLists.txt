
include_directories(../include ${COMMON_DIR})


add_executable(mtc_tests mtc-tests.cpp)
add_executable(tmtc_stream_tests tmtc-stream-tests.cpp)
add_executable(tmtc_tests tmtc-tests.cpp)

target_link_libraries(mtc_tests PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.a)
target_link_libraries(tmtc_stream_tests PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.a)
target_link_libraries(tmtc_tests PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.a)

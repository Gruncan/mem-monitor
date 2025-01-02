
include_directories(../include)
include_directories(../../)

add_executable(mtc_tests mtc-tests.cpp)


target_link_libraries(mtc_tests PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.so)
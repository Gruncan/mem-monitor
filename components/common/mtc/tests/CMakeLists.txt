
include_directories(../include)

add_executable(mtc_tests mtc-tests.cpp)

if (UNIX)
    target_link_libraries(mtc_tests PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.so)
else (WIN32)
    target_link_libraries(mtc_tests PRIVATE ${CMAKE_BINARY_DIR}\\common\\mtc\\libmtc_decoder.a)
endif ()
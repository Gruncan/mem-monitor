
include_directories(../include)

#set_source_files_properties(${MTC_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(mtc_tests mtc-tests.cpp)

add_executable(tmtc_tests tmtc-tests.cpp)

if (UNIX)
#    target_link_libraries(mtc_tests PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.so)
    target_link_libraries(tmtc_tests PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.so)
else (WIN32)
#    target_link_libraries(mtc_tests PRIVATE ${CMAKE_BINARY_DIR}\\common\\mtc\\libmtc_decoder.a)
    target_link_libraries(tmtc_tests PRIVATE ${CMAKE_BINARY_DIR}\\common\\mtc\\libmtc_decoder.a)
endif ()
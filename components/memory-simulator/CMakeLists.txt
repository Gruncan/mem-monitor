



SET(memorysimulator_sources src/main.cpp)

include_directories(include/
        ../common/mtc/include)


add_executable(memorysimulator ${memorysimulator_sources})

target_link_libraries(MemoryGUI PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.a)
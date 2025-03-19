



SET(memorysimulator_sources src/memory-simulator.cpp)

message(WARNING "Simulation has debug symbols")
set(CMAKE_BUILD_TYPE Debug)

include_directories(include/
        ../common/mtc/include
        ../common/config)


add_executable(memorysimulator ${memorysimulator_sources})

target_link_libraries(memorysimulator PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.a)
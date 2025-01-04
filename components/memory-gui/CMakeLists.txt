
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Make this dynamic
set(CMAKE_PREFIX_PATH /home/duncan/Qt/6.8.1/gcc_64)

find_package(Qt6 COMPONENTS Core Widgets OpenGL REQUIRED)
find_package(Qt6 COMPONENTS PrintSupport REQUIRED)

SET(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        include/mainwindow.h
        include/decoder-worker.h
        src/decoder-worker.cpp
        include/decoder-monitor.h
        src/decoder-monitor.cpp
        src/ui/mainwindow.ui
        src/qcustomplot.cpp
        include/qcustomplot.h
        include/qmtcloader.h
        src/qmtcloader.cpp
        src/qmtcloadersgroup.cpp
        include/qmtcloadersgroup.h
        include/qplotcontrolsidebar.h
        src/qplotcontrolsidebar.cpp
        include/plotdata.h
        src/qmemoryplotter.cpp
        include/qmemoryplotter.h
        src/qplotrenderer.cpp
        include/qplotrender.h
)
include_directories(include/
                    ../common/mtc/include)


qt_add_executable(MemoryGUI MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(MemoryGUI PRIVATE Qt::Core Qt::Widgets Qt::OpenGL)
target_link_libraries(MemoryGUI PRIVATE Qt::PrintSupport)
target_link_libraries(MemoryGUI PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.so)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/MemoryGUI_autogen/include)

target_compile_definitions(MemoryGUI PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY QCUSTOMPLOT_USE_OPENGL)


qt_finalize_executable(MemoryGUI)
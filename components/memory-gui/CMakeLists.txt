
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32)
    add_compile_options(-static-libstdc++ -static-libgcc)
endif ()

# Make this dynamic
if(UNIX)
    set(CMAKE_PREFIX_PATH /home/duncan/Qt/6.8.1/gcc_64)
elseif (WIN32)
    set(CMAKE_PREFIX_PATH C:\\Qt\\6.8.1\\mingw_64)
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()
find_package(Qt6 COMPONENTS Core Widgets OpenGL REQUIRED)
find_package(Qt6 COMPONENTS PrintSupport REQUIRED)

SET(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/decoder-worker.cpp
        src/decoder-monitor.cpp
        src/ui/mainwindow.ui
        src/qcustomplot.cpp
        src/qplotcontrolsidebar.cpp
        src/qmtcloader.cpp
        src/qmtcloadersgroup.cpp
        src/qmemoryplotter.cpp
        src/qplotrenderer.cpp
        src/qtreememorywidgetitem.cpp
        src/qmemoryanimatecontrols.cpp
        include/mainwindow.h
        include/decoder-worker.h
        include/decoder-monitor.h
        include/plotdata.h
        include/qcustomplot.h
        include/qmemoryplotter.h
        include/qmtcloader.h
        include/qmtcloadersgroup.h
        include/qplotcontrolsidebar.h
        include/qplotrender.h
        include/qtreememorywidgetitem.h
        include/qmemoryanimatecontrols.h
        include/animation-data-container.h
)
include_directories(include/
                    ${COMMON_DIR}/config/
                    ${COMMON_DIR}/mtc/include/)


qt_add_executable(MemoryGUI MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(MemoryGUI PRIVATE Qt::Core Qt::Widgets Qt::OpenGL)
target_link_libraries(MemoryGUI PRIVATE Qt::PrintSupport)

if (UNIX)
    target_link_libraries(MemoryGUI PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.a)
elseif(WIN32)
    target_link_libraries(MemoryGUI PRIVATE ${CMAKE_BINARY_DIR}\\common\\mtc\\libmtc_decoder.a)
else ()
    message(FATAL_ERROR "Can't link to decode library")
endif ()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/MemoryGUI_autogen/include)

target_compile_definitions(MemoryGUI PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY QCUSTOMPLOT_USE_OPENGL)


qt_finalize_executable(MemoryGUI)
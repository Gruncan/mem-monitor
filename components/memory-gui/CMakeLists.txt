set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(QT_ROOT "/home/duncan/Qt/6.8.1/build_static")
set(QT_LIB_PATH "${QT_ROOT}/lib")
set(QT_CMAKE_PATH "${QT_LIB_PATH}/cmake")

set(Qt6Core_DIR "${QT_CMAKE_PATH}/Qt6Core")
set(Qt6Gui_DIR "${QT_CMAKE_PATH}/Qt6Gui")
set(Qt6Widgets_DIR "${QT_CMAKE_PATH}/Qt6Widgets")
set(Qt6OpenGL_DIR "${QT_CMAKE_PATH}/Qt6OpenGL")
set(Qt6PrintSupport_DIR "${QT_CMAKE_PATH}/Qt6PrintSupport")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    add_compile_options(-static-libstdc++ -static-libgcc)
endif()

set(PROJECT_SOURCES
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
        ../common/mem-monitor-config.h
        include/animation-data-container.h
        include/qmemorytreemapplot.h
)

include_directories(
        include/
        ../common/mtc/include
        ${QT_ROOT}/include
)

set(CMAKE_PREFIX_PATH "/home/duncan/Qt/6.8.1/gcc_64/lib/cmake")

find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL PrintSupport REQUIRED)
qt_add_executable(MemoryGUI MANUAL_FINALIZATION ${PROJECT_SOURCES})

set_property(TARGET MemoryGUI PROPERTY LINK_SEARCH_START_STATIC ON)
set_property(TARGET MemoryGUI PROPERTY LINK_SEARCH_END_STATIC ON)
set_property(TARGET MemoryGUI PROPERTY LINK_WHAT_YOU_USE ON)

target_link_libraries(MemoryGUI PRIVATE
        ${QT_LIB_PATH}/libQt6Core.a
        ${QT_LIB_PATH}/libQt6Gui.a
        ${QT_LIB_PATH}/libQt6Widgets.a
        ${QT_LIB_PATH}/libQt6OpenGL.a
        ${QT_LIB_PATH}/libQt6PrintSupport.a
        -static
        -lX11
        -lxcb
        -lxkbcommon
        -lxkbcommon-x11
        -lfontconfig
        -lfreetype
        -lz
        -luuid
        -lXrender
        -lXext
        -lX11-xcb
        -lSM
        -lICE
        -ldl
        -lpthread
)

if(UNIX)
    target_link_libraries(MemoryGUI PRIVATE ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.so)
elseif(WIN32)
    target_link_libraries(MemoryGUI PRIVATE ${CMAKE_BINARY_DIR}\\common\\mtc\\libmtc_decoder.a)
else()
    message(FATAL_ERROR "Can't link to decode library")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/MemoryGUI_autogen/include)

target_compile_definitions(MemoryGUI PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY QCUSTOMPLOT_USE_OPENGL)

qt_finalize_executable(MemoryGUI)
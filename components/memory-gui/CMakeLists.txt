
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(-static-libstdc++ -static-libgcc)
endif()

set(CMAKE_PREFIX_PATH "")
if(UNIX)
    set(CMAKE_PREFIX_PATH /home/duncan/Qt/6.8.1/gcc_64/lib/cmake)
    set(Qt6_DIR /home/duncan/Qt/6.8.1/gcc_64/lib/cmake/Qt6)
elseif(WIN32)
    set(CMAKE_PREFIX_PATH C:\\Qt\\6.8.1\\mingw_64\\lib\\cmake)
    set(Qt6_DIR C:\\Qt\\6.8.1\\mingw_64\\lib\\cmake\\Qt6)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

set(QT_USE_STATIC_LIBS ON)
set(QT_USE_STATIC_RUNTIME ON)

find_package(Qt6 COMPONENTS Core Widgets OpenGL PrintSupport REQUIRED)

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
        ../common/mem-monitor-config.h
)

include_directories(include/
        ../common/mtc/include)

qt_add_executable(MemoryGUI MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(MemoryGUI PRIVATE
        Qt6::CorePrivate
        Qt6::WidgetsPrivate
        Qt6::OpenGLPrivate
        Qt6::PrintSupportPrivate
)

if(WIN32)
    target_link_options(MemoryGUI PRIVATE -static)
    set_target_properties(MemoryGUI PROPERTIES WIN32_EXECUTABLE TRUE)

    target_link_libraries(MemoryGUI PRIVATE
            dwmapi
            uxtheme
            version
            winmm
            ws2_32
            ${CMAKE_BINARY_DIR}\\common\\mtc\\libmtc_decoder.a
    )
elseif(UNIX)
    # Linux specific static linking
    target_link_libraries(MemoryGUI PRIVATE
            -static-libgcc
            -static-libstdc++
            ${CMAKE_BINARY_DIR}/common/mtc/libmtc_decoder.so
            -lGL
            -lX11
            -lXrender
            -lfontconfig
            -lfreetype
            -lexpat
    )
endif()

target_compile_definitions(MemoryGUI PRIVATE
        QCUSTOMPLOT_COMPILE_LIBRARY
        QCUSTOMPLOT_USE_OPENGL
        QT_STATICPLUGIN
        QT_STATIC_BUILD
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/MemoryGUI_autogen/include)

qt_finalize_executable(MemoryGUI)

if(WIN32)
    set_target_properties(MemoryGUI PROPERTIES
            LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()
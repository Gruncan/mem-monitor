
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Make this dynamic
set(CMAKE_PREFIX_PATH /home/duncan/Qt/6.8.1/gcc_64)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

SET(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    src/ui/mainwindow.ui
)
include_directories(include/)


qt_add_executable(MemoryGUI MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(MemoryGUI PRIVATE Qt::Widgets)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/MemoryGUI_autogen/include)


qt_finalize_executable(MemoryGUI)
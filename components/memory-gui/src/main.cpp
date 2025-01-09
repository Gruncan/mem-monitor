#include "mainwindow.h"

#include <QApplication>
#include <iostream>

#ifndef _WIN32
#include <execinfo.h>


void printBacktrace() {
    const int maxFrames = 100; // Maximum number of frames to capture
    void* buffer[maxFrames];   // Array to store backtrace frames

    // Capture backtrace
    int numFrames = backtrace(buffer, maxFrames);

    // Resolve symbols to human-readable form
    char** symbols = backtrace_symbols(buffer, numFrames);
    if (symbols == nullptr) {
        std::cerr << "Error: Failed to retrieve backtrace symbols." << std::endl;
        return;
    }

    // Print the backtrace
    std::cout << "Backtrace (most recent call last):" << std::endl;
    for (int i = 0; i < numFrames; ++i) {
        std::cout << symbols[i] << std::endl;
    }

    // Free memory allocated by backtrace_symbols
    free(symbols);
}

#endif
int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);
        MainWindow w;
        w.show();
        return app.exec();
    }catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
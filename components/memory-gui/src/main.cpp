#include "mainwindow.h"

#include <QApplication>
#include <iostream>

#ifndef _WIN32
#include <execinfo.h>


void printBacktrace() {
    const int maxFrames = 100;
    void* buffer[maxFrames];

    int numFrames = backtrace(buffer, maxFrames);

    char** symbols = backtrace_symbols(buffer, numFrames);
    if (symbols == nullptr) {
        std::cerr << "Error: Failed to retrieve backtrace symbols." << std::endl;
        return;
    }

    std::cout << "Backtrace (most recent call last):" << std::endl;
    for (int i = 0; i < numFrames; ++i) {
        std::cout << symbols[i] << std::endl;
    }

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
        printBacktrace();
    }
}
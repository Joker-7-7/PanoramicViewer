#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

#include <QVTKOpenGLNativeWidget.h>

extern "C" {
    _declspec(dllexport) int NvOptimusEnablement = 1;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

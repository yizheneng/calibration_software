#include "mainwindow.h"
#include <QApplication>
#include <QMetaType>
#include "calibraon.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<cv::Mat>("Mat");

    calibraon * calib = new calibraon(&w);

    return a.exec();
}

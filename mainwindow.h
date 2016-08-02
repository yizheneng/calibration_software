#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <cv.h>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_chooseImagesPushButton_clicked();
    void slotOfImageDisplay(cv::Mat image, int index);
    void slotOfDisplayProgress(int);
    void slotOfDisplayData(cv::Mat cameraMatrix,cv::Mat distCoeffs);

    void on_startPushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();

private:
    QImage imgConvert(cv::Mat image);
    QStringList imgList;

    Ui::MainWindow *ui;

signals:
    void signalOfStartCalibraon(QStringList,int,int,float);
    void signalOfRemapImage(QString);

};

#endif // MAINWINDOW_H

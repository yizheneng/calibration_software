#ifndef CALIBRAON_H
#define CALIBRAON_H
#include <QObject>
#include <QStringList>
#include <QMessageBox>
#include <QSettings>
#include <QThread>
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
#include "mainwindow.h"

#define DATA_INI_FILE_NAME "data.ini"

using namespace cv;
using namespace std;

enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

class calibraon : public QThread
{
    Q_OBJECT
public:
    calibraon(MainWindow *w);

    Size boardSize;
    bool calibraonOK;
private:
    void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType = CHESSBOARD);//计算棋盘角点
    double computeReprojectionErrors(    //计算二次投影误差
            const vector<vector<Point3f> >& objectPoints,
            const vector<vector<Point2f> >& imagePoints,
            const vector<Mat>& rvecs, const vector<Mat>& tvecs,
            const Mat& cameraMatrix, const Mat& distCoeffs,
            vector<float>& perViewErrors );
    void run();

    QStringList imagePath;
    int width;
    int height;
    float squareSize;

    MainWindow *displaywindow;
    Mat map1,map2;
    Mat cameraMatrix, distCoeffs;
public slots:
    void startCalibraon(QStringList imagePath,int w,int h, float squareSize = 1.f);
    void slotForRemapImage(QString path);

signals:
    void signalOfDisplayImage(cv::Mat image, int index);
    void signalOfDisplayProgress(int);
    void signalOfDisplayData(cv::Mat, cv::Mat);
};

#endif // CALIBRAON_H

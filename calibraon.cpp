#include "calibraon.h"
#include <QDebug>


calibraon::calibraon(MainWindow *w)
{
    displaywindow = w;
    calibraonOK = false;

    connect(this,SIGNAL(signalOfDisplayImage(cv::Mat,int)),w,SLOT(slotOfImageDisplay(cv::Mat,int)));
    connect(w,SIGNAL(signalOfStartCalibraon(QStringList,int,int,float)),this,SLOT(startCalibraon(QStringList,int,int,float)));
    connect(this,SIGNAL(signalOfDisplayProgress(int)),w,SLOT(slotOfDisplayProgress(int)));
    connect(w,SIGNAL(signalOfRemapImage(QString)),this,SLOT(slotForRemapImage(QString)));
    connect(this,SIGNAL(signalOfDisplayData(cv::Mat,cv::Mat)),w,SLOT(slotOfDisplayData(cv::Mat,cv::Mat)));
}

void calibraon::run()
{
    Size imageSize;
    bool found;
    vector<vector<Point2f> > imagePoints;
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    vector<float> reprojErrs;

    //QMessageBox::information(NULL, "run...", "now runing....", QMessageBox::NoButton);

    boardSize.width = width;
    boardSize.height = height;

    for(int i = 0;i < imagePath.size();i ++)
    {
        Mat view, viewGray;

        view = imread(imagePath.at(i).toStdString());

        if(view.empty())
            break;

        imageSize = view.size();

        cvtColor(view, viewGray, COLOR_BGR2GRAY); //转换为灰度图//////

        vector<Point2f> pointbuf;//////

        found = findChessboardCorners( view, boardSize, pointbuf,   //寻找角点
            CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

        if(found)
        {
            cornerSubPix( viewGray, pointbuf, Size(11,11),   //如果找到了棋盘角点  //检测亚像素角点/////////
                        Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));

            imagePoints.push_back(pointbuf);

            drawChessboardCorners( view, boardSize, Mat(pointbuf), found );   //绘制检测到的棋盘角点

            emit signalOfDisplayImage(view,1);
        }
        emit signalOfDisplayProgress(((i + 1) * 100) / imagePath.size());
    }
    if(imagePoints.size() > 0)
    {
        cameraMatrix = Mat::eye(3, 3, CV_64F);
        distCoeffs = Mat::zeros(5, 1, CV_64F);

        vector<vector<Point3f> > objectPoints(1);
        calcChessboardCorners(boardSize, squareSize, objectPoints[0], CHESSBOARD);

        objectPoints.resize(imagePoints.size(),objectPoints[0]);
        double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                                     distCoeffs, rvecs, tvecs, CALIB_FIX_K4|CALIB_FIX_K5);

        emit signalOfDisplayData(cameraMatrix, distCoeffs);

        qDebug()<<"cameraMatrix:";
        for(int i = 0; i < 3; i++)
        {
            qDebug() << *((double*)(cameraMatrix.row(i).col(0).data))
                     << "    "
                     << *((double*)(cameraMatrix.row(i).col(1).data))
                     << "    "
                     << *((double*)(cameraMatrix.row(i).col(2).data));
        }

        qDebug()<<"distCoeffs:";
        for(int i = 0; i < 5; i++)
        {
            qDebug() << *((double*)(distCoeffs.data + i * 8));
        }

        if(calibraonOK = checkRange(cameraMatrix) && checkRange(distCoeffs))//检查标定参数是否正确
        {
            double totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                                                           rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

            initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
                                    getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
                                    imageSize, CV_16SC2, map1, map2); /////////

            QSettings setting(DATA_INI_FILE_NAME, QSettings::IniFormat);

            //输出标定信息到配置文件
            for(int i = 0; i < 3; i++)
            {
                setting.setValue(QString("cameraMatrix/row%1_col0").arg(i), *((double*)(cameraMatrix.row(i).col(0).data)));
                setting.setValue(QString("cameraMatrix/row%1_col1").arg(i), *((double*)(cameraMatrix.row(i).col(1).data)));
                setting.setValue(QString("cameraMatrix/row%1_col2").arg(i), *((double*)(cameraMatrix.row(i).col(2).data)));
            }

            for(int i = 0; i < 5; i++)
            {
                setting.setValue(QString("distCoeffs/data%1").arg(i), *((double*)(distCoeffs.data + i * 8)));
            }

            setting.setValue(QString("image_size/width"), imageSize.width);
            setting.setValue(QString("image_size/height"), imageSize.height);

            qDebug() << "map1:  " << "type:" << map1.type() << "rows:" << map1.rows << "cols:" << map1.cols;
            qDebug() << "map2:  " << "type:" << map2.type() << "rows:" << map2.rows << "cols:" << map2.cols;
        }
    }
    else
    {
        qDebug()<<"error!!";
    }
}

void calibraon::startCalibraon(QStringList imagePath, int w, int h, float squareSize)
{
    this->imagePath = imagePath;
    this->width = w;
    this->height = h;
    this->squareSize = squareSize;

    this->start();
}

void calibraon::slotForRemapImage(QString path)
{
    Mat img1;
    Mat img = imread(path.toStdString());
    remap(img, img1, map1, map2, INTER_LINEAR);
    emit signalOfDisplayImage(img1,3);
}


void calibraon::calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType)//计算棋盘角点
{
    corners.resize(0);

    switch(patternType)
    {
      case CHESSBOARD:
      case CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(Point3f(float(j*squareSize),
                                          float(i*squareSize), 0));
        break;

      case ASYMMETRIC_CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(Point3f(float((2*j + i % 2)*squareSize),
                                          float(i*squareSize), 0));
        break;

//      default:
//        CV_Error(Error::cv::StsBadArg, "Unknown pattern type\n");
    }
}

double calibraon::computeReprojectionErrors(    //计算二次投影误差
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
        const Mat& cameraMatrix, const Mat& distCoeffs,
        vector<float>& perViewErrors )
{
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); i++ )
    {
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
                      cameraMatrix, distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), NORM_L2);
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)std::sqrt(err*err/n);
        totalErr += err*err;
        totalPoints += n;
    }

    return std::sqrt(totalErr/totalPoints);
}

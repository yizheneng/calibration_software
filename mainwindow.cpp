#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDebug"
#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_chooseImagesPushButton_clicked()
{
    imgList = QFileDialog::getOpenFileNames(this,QString::fromUtf8("选择图像文件"),".",tr("Image Files(*.jpg *.png *.bmp *.tif *.gif)"));
    ui->imageListWidget->clear();
    ui->imageListWidget->addItems(imgList);
}

QImage MainWindow::imgConvert(cv::Mat image)
{
    QImage img_temp;
    if(image.channels() == 3)
    {
        img_temp = QImage(image.cols,image.rows, \
                           QImage::Format_RGB888);
        cv::Mat_<cv::Vec3b>::iterator it = image.begin<cv::Vec3b>();
        cv::Mat_<cv::Vec3b>::iterator itend = image.end<cv::Vec3b>();
        int x = 0;
        for(;it != itend;it ++)
        {
            QColor cc;

            cc.setRed((*it)[2]);
            cc.setGreen((*it)[1]);
            cc.setBlue((*it)[0]);

            img_temp.setPixel(x%image.cols,x/image.cols,cc.rgb());
            x++;
        }
    }else if(image.channels() == 1)
    {
        img_temp = QImage(image.cols,image.rows, \
                           QImage::Format_RGB888);
        int temp = 0;
        for(int y=0;y<image.rows;y++)
            for(int x=0;x<image.cols;x++)
            {
                temp = image.at<unsigned char>(y,x);
                QColor cc;
                cc.setBlue(temp);
                cc.setRed(temp);
                cc.setGreen(temp);
                img_temp.setPixel(x,y,cc.rgb());
            }
    }

    return img_temp;
}

void MainWindow::slotOfImageDisplay(cv::Mat image, int index)
{
        QImage img = imgConvert(image);
        switch (index) {
        case 1:
            img = img.scaled(QSize(ui->label->size().width() - 2, ui->label->size().height() - 2));
            ui->label->setPixmap(QPixmap::fromImage(img));
            break;
        case 2:
//            img = img.scaled(ui->label_2->size());
            img = img.scaled(QSize(ui->label_2->size().width() - 1, ui->label_2->size().height() - 1));
            ui->label_2->setPixmap(QPixmap::fromImage(img));
            break;
        case 3:
//            img = img.scaled(ui->label_3->size());
            img = img.scaled(QSize(ui->label_3->size().width() - 1, ui->label_3->size().height() - 1));
            ui->label_3->setPixmap(QPixmap::fromImage(img));
            break;
        default:
            break;
        }
        qDebug() << ui->label->size();
}

void MainWindow::on_startPushButton_clicked()
{
    if(imgList.size() > 3)
    {
        if((ui->XSpinBox->value() > 0) && (ui->YSpinBox->value() > 0))
            emit signalOfStartCalibraon(imgList, ui->XSpinBox->value(), ui->YSpinBox->value(), ui->doubleSpinBox_3->value());
        else
        {
            QMessageBox::critical ( this,
                                    QString::fromUtf8("参数错误！！"),
                                    QString::fromUtf8("请输入正确的棋盘纵轴和横轴的交点个数！！"));
        }
    }
    else
    {
        QMessageBox::critical ( this,
                                QString::fromUtf8("图片数量错误！！"),
                                QString::fromUtf8("请选择4张以上的图片用来标定！！"));
    }
}

void MainWindow::slotOfDisplayProgress(int i)
{
    ui->progressBar->setValue(i);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString imgPath = QFileDialog::getOpenFileName(this,QString::fromUtf8("选择图像文件"),".",tr("Image Files(*.jpg *.png *.bmp)"));

    QImage img;
    if(img.load(imgPath))
    {
        img = img.scaled(ui->label_2->size());
        ui->label_2->setPixmap(QPixmap::fromImage(img));

        emit signalOfRemapImage(imgPath);
    }
    else
    {
        QMessageBox::critical ( this,
                                QString::fromUtf8("图片格式错误！！"),
                                QString::fromUtf8("请选择重新图片！！！！"));
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->label_3->clear();
}

void MainWindow::slotOfDisplayData(cv::Mat cameraMatrix,cv::Mat distCoeffs)
{
    ui->lineEditCameraMatrix11->setText(QString::number(*((double*)(cameraMatrix.row(0).col(0).data))));
    ui->lineEditCameraMatrix12->setText(QString::number(*((double*)(cameraMatrix.row(0).col(1).data))));
    ui->lineEditCameraMatrix13->setText(QString::number(*((double*)(cameraMatrix.row(0).col(2).data))));
    ui->lineEditCameraMatrix21->setText(QString::number(*((double*)(cameraMatrix.row(1).col(0).data))));
    ui->lineEditCameraMatrix22->setText(QString::number(*((double*)(cameraMatrix.row(1).col(1).data))));
    ui->lineEditCameraMatrix23->setText(QString::number(*((double*)(cameraMatrix.row(1).col(2).data))));
    ui->lineEditCameraMatrix31->setText(QString::number(*((double*)(cameraMatrix.row(2).col(0).data))));
    ui->lineEditCameraMatrix32->setText(QString::number(*((double*)(cameraMatrix.row(2).col(1).data))));
    ui->lineEditCameraMatrix33->setText(QString::number(*((double*)(cameraMatrix.row(2).col(2).data))));

    ui->lineEditDistCoeffs0->setText(QString::number(*((double*)(distCoeffs.data + 0 * 8))));
    ui->lineEditDistCoeffs1->setText(QString::number(*((double*)(distCoeffs.data + 1 * 8))));
    ui->lineEditDistCoeffs2->setText(QString::number(*((double*)(distCoeffs.data + 2 * 8))));
    ui->lineEditDistCoeffs3->setText(QString::number(*((double*)(distCoeffs.data + 3 * 8))));
    ui->lineEditDistCoeffs4->setText(QString::number(*((double*)(distCoeffs.data + 4 * 8))));
}

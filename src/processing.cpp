#include "processing.h"

#include <QDebug>
#include <QFileInfo>
#include <QApplication>

Processing::Processing(){
    Processing(1, 1);
}

Processing::Processing(float x, float y){
    m_scaleX = x;
    m_scaleY = y;
}

void Processing::process(const QString img, vector<Mat> &data, vector<Rect> &rects,
                         vector<Scalar> &colors, QProgressBar *bar, float total){
    QFileInfo file(img);
    vector<vector<Point> > contoursPoly;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);


    Mat orig = imread(img.toLocal8Bit().constData(), 1);
    Mat src = Mat::zeros(orig.size(), CV_8UC3);
    int w = orig.cols * m_scaleX;
    int h = orig.rows * m_scaleY;
    int save =  bar->value();


    try{
        float in = (float) save;
        float counter = ((bar->maximum()*total)/100.0f)/6.0f;
        bar->setFormat(QObject::tr("Recherche des contours de '%1'. %0%").arg(bar->value()).arg(file.fileName()));

        cvtColor(orig, src, CV_BGR2GRAY);
        threshold(src, src, 128, 255, CV_THRESH_BINARY_INV);
        erode(src, src, cv::Mat());
        dilate(src, src, cv::Mat());
        Canny(src, src, 100, 200, 3);
        GaussianBlur(src, src, Size(9, 9), 0, 0);
        // I may need to use RETR_EXTERNAL instead of RETR_TREE
        findContours(src, contours, hierarchy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
        contoursPoly.resize(contours.size());

        in += counter*3;
        bar->setValue(in);
        bar->setFormat(QObject::tr("Approximation des contours de '%1'. %0%").arg(bar->value()).arg(file.fileName()));
        QApplication::processEvents();

        for( size_t k = 0; k < contours.size(); k++ )
            approxPolyDP(Mat(contours[k]), contoursPoly[k], 3, true);

        in += counter;
        bar->setValue(in);
        bar->setFormat(QObject::tr("Creation des donnees de '%1'. %0%").arg(bar->value()).arg(file.fileName()));
        QApplication::processEvents();

        for(unsigned int i=0; i<contoursPoly.size(); i++){
            CvRect rect;
            CvPoint pt1, pt2;

            rect = boundingRect(contoursPoly.at(i));
            pt1.x = rect.x;
            pt2.x = (rect.x+rect.width);
            pt1.y = rect.y;
            pt2.y = (rect.y+rect.height);
            if(rect.width >= w && rect.height >= h){
                Scalar color = Scalar(rng.uniform(128,255), rng.uniform(128,255), rng.uniform(128,255));
                //Mat dst = Mat::zeros(src.size(), CV_8UC3);
                //drawContours(dst, contoursPoly, i, color, CV_FILLED, 8, hierarchy, 0, Point());
                Mat dst = orig(rect).clone();
                data.push_back(dst);
                rects.push_back(rect);
                colors.push_back(color);

                in += (counter*2)/(float)contoursPoly.size();
                bar->setValue(in);
                bar->setFormat(QObject::tr("Creation des donnees de '%1' en cours ... %0%").arg(bar->value()).arg(file.fileName()));
                QApplication::processEvents();
            }
        }
    }catch(Exception e){
        qDebug() << "Exception : " << e.what();
    }
    bar->setValue(save+total);
}

Mat Processing::createImg(const QString img, QStringList list, vector<Rect> rects,
                              vector<Scalar> colors, QProgressBar *bar, float total){
    Mat src;
    QFileInfo file(img);
    int save =  bar->value();
    float in = (float)save;
    float counter = ((bar->maximum()*total)/100.0f)/(float)rects.size();

    bar->setFormat(QObject::tr("Creation d'une image temporaire à partir de '%1'. %0%").arg(bar->value()).arg(file.fileName()));
    try{
        src = imread(img.toLocal8Bit().constData(), 1);
        for(unsigned int i=0; i<rects.size(); i++){
            Rect rect = rects.at(i);
            Point pt1, pt2;

            pt1.x = rect.x;
            pt2.x = (rect.x+rect.width);
            pt1.y = rect.y;
            pt2.y = (rect.y+rect.height);
            rectangle(src, pt1, pt2, colors.at(i), 2, 8, 0);
            putText(src, list.at(i).toLocal8Bit().constData(), pt1, FONT_HERSHEY_DUPLEX, 0.8, colors.at(i));

            in += counter;
            bar->setValue(in);
            bar->setFormat(QObject::tr("Creation des donnees de '%1' en cours ... %0%").arg(bar->value()).arg(file.fileName()));
            QApplication::processEvents();
        }
    }catch(Exception e){
        qDebug() << "Exception : " << e.what();
        return Mat();
    }
    bar->setValue(save+total);
    return src;
}

Mat Processing::createImg(const QString img, QString object, vector<Rect> rects,
                              vector<Scalar> colors, QProgressBar *bar, float total){
    Mat src;
    QFileInfo file(img);
    int save =  bar->value();
    float in = (float)save;
    float counter = ((bar->maximum()*total)/100.0f)/(float)rects.size();
    bar->setFormat(QObject::tr("Creation d'une image temporaire à partir de '%1'. %0%").arg(bar->value()).arg(file.fileName()));
    try{
        src = imread(img.toLocal8Bit().constData(), 1);
        for(unsigned int i=0; i<rects.size(); i++){
            Rect rect = rects.at(i);
            Point pt1, pt2;

            pt1.x = rect.x;
            pt2.x = (rect.x+rect.width);
            pt1.y = rect.y;
            pt2.y = (rect.y+rect.height);
            rectangle(src, pt1, pt2, colors.at(i), 2, 8, 0);
            putText(src, object.toLocal8Bit().constData(), pt1, FONT_HERSHEY_DUPLEX, 0.8, colors.at(i));

            in += counter;
            bar->setValue(in);
            bar->setFormat(QObject::tr("Creation des donnees de '%1' en cours ... %0%").arg(bar->value()).arg(file.fileName()));
            QApplication::processEvents();
        }
    }catch(Exception e){
        qDebug() << "Exception : " << e.what();
        return Mat();
    }
    bar->setValue(save+total);
    return src;
}

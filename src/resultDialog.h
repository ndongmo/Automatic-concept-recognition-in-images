#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <vector>

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QStringList>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

class ResultDialog : public QDialog
{
    Q_OBJECT

public:
    ResultDialog(){}
    ResultDialog(QWidget *parent, Qt::WindowFlags, vector<Mat>);

public slots:
    void changeImage(int);
    void saveImage();

private:
    void setImage();
    unsigned int index;
    vector<Mat> list;
    QLabel *central;
    QPushButton *save;
    QPushButton *bt1;
    QPushButton *bt2;
    QPushButton *bt3;
    QPushButton *bt4;
};
#endif // RESULTDIALOG_H

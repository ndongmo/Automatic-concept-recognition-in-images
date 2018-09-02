#include "resultDialog.h"

#include <QSignalMapper>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

ResultDialog::ResultDialog(QWidget *parent, Qt::WindowFlags flags, vector<Mat> l)
    :QDialog(parent, flags){
    //setFixedSize(600, 400);

    list = l;
    QSignalMapper *mapper = new QSignalMapper(this);
    central = new QLabel();
    save = new QPushButton("Enregistrer", this);

    bt1 = new QPushButton("<<", this);
    bt2 = new QPushButton("<", this);
    bt3 = new QPushButton(">", this);
    bt4 = new QPushButton(">>", this);

    QHBoxLayout *lh = new QHBoxLayout;
    lh->addWidget(bt1);
    lh->addWidget(bt2);
    lh->addWidget(bt3);
    lh->addWidget(bt4);

    QVBoxLayout *lv = new QVBoxLayout;
    lv->addLayout(lh);
    lv->addWidget(central);
    lv->addWidget(save);

    setLayout(lv);

    connect(bt1, SIGNAL(clicked()), mapper,SLOT(map()));
    connect(bt2, SIGNAL(clicked()), mapper,SLOT(map()));
    connect(bt3, SIGNAL(clicked()), mapper,SLOT(map()));
    connect(bt4, SIGNAL(clicked()), mapper,SLOT(map()));

    mapper->setMapping(bt1, 0);
    mapper->setMapping(bt2, 1);
    mapper->setMapping(bt3, 2);
    mapper->setMapping(bt4, 3);

    connect(mapper, SIGNAL(mapped(int)), this,SLOT(changeImage(int)));
    connect(save, SIGNAL(clicked()), this,SLOT(saveImage()));

    index = 0;
    setImage();
    if(list.size() == 1){
        bt1->setEnabled(false);
        bt2->setEnabled(false);
        bt3->setEnabled(false);
        bt4->setEnabled(false);
    }
}

void ResultDialog::setImage(){
    Mat workImage = list.at(index);
    QImage img= QImage((const unsigned char*)(workImage.data),
    workImage.cols,workImage.rows,QImage::Format_RGB888);
    central->setPixmap(QPixmap::fromImage(img));
    central->resize(central->pixmap()->size());
    if(index == 0){
        bt1->setEnabled(false);
        bt2->setEnabled(false);
        bt3->setEnabled(true);
        bt4->setEnabled(true);
    }
    else if(index == list.size()-1){
        bt1->setEnabled(true);
        bt2->setEnabled(true);
        bt3->setEnabled(false);
        bt4->setEnabled(false);
    }
    else{
        bt1->setEnabled(true);
        bt2->setEnabled(true);
        bt3->setEnabled(true);
        bt4->setEnabled(true);
    }
}

void ResultDialog::changeImage(int type){
    if(type == 0)
        index = 0;
    else if(type == 1)
        index = (index > 0)?index-1:0;
    else if(type == 2)
        index = (index < list.size()-1)?index+1:list.size()-1;
    else if(type == 3)
        index = list.size()-1;
    setImage();
}

void ResultDialog::saveImage(){
    QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), QString(),
                                tr("Images (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP"));
    if(path.isEmpty())
        return;
    imwrite(path.toLocal8Bit().constData(), list.at(index));
}

#include "classifier.h"

#include <iostream>

#include <QObject>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDebug>
#include <QApplication>

Classifier::Classifier(){
    Classifier(new BagOfWords());
}

Classifier::Classifier(BagOfWords *bow){
    CvTermCriteria criteria;
    criteria.max_iter = 100;
    criteria.epsilon = 0.00001;
    criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
    m_params.train_method = CvANN_MLP_TrainParams::BACKPROP;
    m_params.bp_dw_scale = 0.1;
    m_params.bp_moment_scale = 0.1;
    m_params.term_crit = criteria;
    m_bow = bow;
    m_pro = Processing(0.2f, 0.1f);
    istraining = false;
    rnaPath = QDir::currentPath().append("/data/rna.xml");
    rnaPath = QDir::toNativeSeparators(rnaPath);
    classPath = QDir::currentPath().append("/data/class.txt");
    classPath = QDir::toNativeSeparators(classPath);
}

int Classifier::getNbClass(){
    return m_class.count();
}

void Classifier::setClass(QStringList &cl){
    m_class = cl;
}

bool Classifier::save(){

    m_mlp.save(rnaPath.toLocal8Bit().constData());
    QFile fOut(classPath.toLocal8Bit().constData());
    if(fOut.open(QFile::WriteOnly | QFile::Text)){
        QTextStream s(&fOut);
        for(int i = 0; i < m_class.count(); ++i){
            if(i != m_class.count()-1)
                s << m_class.at(i) << '\n';
            else
                s << m_class.at(i);
        }
    }
    else{
        qDebug() << "Erreur d'ouverture du fichier class.txt\n";
        return false;
    }
    fOut.close();
    return true;
}

bool Classifier::load(){
    m_mlp.load(rnaPath.toLocal8Bit().constData());
    m_class.clear();
    QFile fIn(classPath.toLocal8Bit().constData());
    if(fIn.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream sIn(&fIn);
        while(!sIn.atEnd())
            m_class += sIn.readLine();
    } else {
        qDebug() << "Erreur d'ouverture du fichier class\n";
        return false;
    }
    istraining = true;
    return true;
}

void Classifier::loadClass(QString directory, QProgressBar *bar){
    QString class1, class2;

    QDir dir(directory);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QFileInfoList info = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    float in = (float) bar->value();
    float counter = ((bar->maximum()*10)/100)/(float)info.size();
    bar->setFormat(QObject::tr("Chargement des classes du RNA. %0%").arg(bar->value()));
    m_class.clear();

    for(int i=0; i<info.size(); i++){
        class2 = BagOfWords::getClass(info.at(i).fileName());
        if(class1 != class2){
            class1 = class2;
            m_class << class1;
            in += counter;
            bar->setValue(in);
            bar->setFormat(QObject::tr("Chargement de la classe '%1'. %0%").arg(bar->value()).arg(class1));
            QApplication::processEvents();
        }
    }
}


void Classifier::addLayers(int nb){
    tempLayers.push_back(nb);
}

void Classifier::deleteLayers(int nb){
    nb--;
    if(tempLayers.size() > nb)
        tempLayers.erase(tempLayers.begin()+nb);
}

void Classifier::createNN(){
    createNN(m_bow->getK());
}

void Classifier::createNN(int nbCluster){
    Mat layers = Mat(2+tempLayers.size(), 1, CV_32SC1);
    layers.row(0) = Scalar(nbCluster);
    for(int i=0; i<tempLayers.size(); i++)
        layers.row(i+1) = Scalar(tempLayers.at(i));
    layers.row(1+tempLayers.size()) = Scalar(m_class.count());

    m_mlp.create(layers);
}

bool Classifier::train(QString directory,  QProgressBar *bar){
    Mat trainingData;
    Mat trainingClasses;

    try{
        m_bow->loadTrainingSet(directory,m_class,trainingData, trainingClasses, bar);
        assert(trainingData.rows == trainingClasses.rows);

        int counter = (bar->maximum()*30)/100;
        bar->setFormat(QObject::tr("Apprentissage du RNA en cours ... %0%").arg(bar->value()));
        QApplication::processEvents();
        m_mlp.train(trainingData,trainingClasses,Mat(),Mat(),m_params);
        bar->setValue(bar->value()+counter);
        bar->setFormat(QObject::tr("Enregistrement du RNA (rna.xml). %0%").arg(bar->value()));
        save();
        bar->setValue(bar->value()+5);
        QApplication::processEvents();
        istraining = true;
    }catch(Exception& e){
        qDebug() << "exception dans train(): " << e.what();
        return false;
    }

    return true;
}

Mat Classifier::predict(Mat &data, QProgressBar *bar){
    Mat predicted;
    float in = (float) bar->value();
    float counter = ((bar->maximum()*20)/100)/(float)data.rows;
    bar->setFormat(QObject::tr("Prediction des donnees de test. %0%").arg(bar->value()));

    for(int i = 0; i < data.rows ; i ++){
        Mat response (1, m_class.count(), CV_32FC1);
        Mat sample = data.row(i);
        m_mlp.predict(sample, response);
        predicted.push_back(response);
        bar->setFormat(QObject::tr("Prediction des donnees de test(%1). %0%").arg(bar->value()).arg(i));
        in += counter;
        bar->setValue(in);
        QApplication::processEvents();
    }
    return predicted;
}

map<QString, float> Classifier::evaluate(Mat &predicted, Mat &actual, QProgressBar *bar){
    assert( predicted.rows == actual.rows);
    map<QString, float> result;
    float tp = 0.0f;
    float fp = 0.0f;
    float tn = 0.0f;
    float fn = 0.0f;
    float p = 0.0f;
    float n = 0.0f;
    float in = (float) bar->value();
    float counter = ((bar->maximum()*10)/100)/(float)actual.rows;
    bar->setFormat(QObject::tr("Evaluation des donnees de test. %0%").arg(bar->value()));

    for(int i = 0; i < actual.rows ; i ++) {
        for(int j = 0; j < actual.cols; j++){
            float p = predicted.at<float>(i,j);
            float a = actual.at<float>(i,j);
            if(a > 0.0f && p >= 0.0f){
                tp++;
                bar->setFormat(QObject::tr("Prediction : donnees(%2) -> classe(%1). %0%")
                               .arg(bar->value()).arg(j).arg(i));
                in += counter;
                bar->setValue(in);
                QApplication::processEvents();
            }
            else if(a > 0.0f && p <= 0.0f)
                fn++;
            else if(a < 0.0f && p <= 0.0f)
                tn++;
            else if(a < 0.0f && p >= 0.0f)
                fp++;
        }
    }

    p = tp +fn;
    n = fp + tn;
    float mytpr = (p != 0.0f)?(tp/p):0.0f;
    float myfpr = (n != 0.0f)?(fp/n):0.0f;
    float pre = ((tp+fp) != 0.0f)?(tp/(tp+fp)):0.0f;
    float acc = ((p+n) != 0.0f)?((tp+tn)/(p+n)):0.0f;
    float mesure = (((2*tp)+fp+fn) != 0.0f)?((2*tp)/((2*tp)+fp+fn)):0.0f;

    pair<QString, float> tpr("TPR", mytpr);
    pair<QString, float> fpr("FPR", myfpr);
    pair<QString, float> recall("RECALL", mytpr);
    pair<QString, float> precision("PRECISION", pre);
    pair<QString, float> accuracy("ACCURACY", acc);
    pair<QString, float> fmesure("F-MESURE", mesure);
    result.insert(tpr);
    result.insert(fpr);
    result.insert(recall);
    result.insert(precision);
    result.insert(accuracy);
    result.insert(fmesure);
    return result;
}

map<QString, float> Classifier::test(QString directory, QProgressBar *bar){
    Mat testData;
    Mat testClasses;
    Mat predicted;
    map<QString, float> accuracy;

    try{
        m_bow->loadTrainingSet(directory,m_class,testData,testClasses, bar);
        predicted = predict(testData, bar);
        accuracy = evaluate(predicted, testClasses, bar);
    }catch(Exception& e){
        qDebug() << "Exception dans test(): " << e.what();
    }
    return accuracy;
}

QString Classifier::getClass(const QString image){
    Mat bowDescriptor;
    Mat response(1, m_class.count(), CV_32FC1);

    try{
        m_bow->loadData(image, bowDescriptor);
        m_mlp.predict(bowDescriptor, response);
        for(int i=0; i<m_class.count(); i++){
            if(response.at<float>(0,i) >= 0.0)
                return m_class.at(i);
        }
    }catch(Exception& e){
        qDebug() << "Exception dans getClass(): " << e.what();
    }
    return QString("classe inconnue !");
}

QStringList Classifier::getObjects(const QString img, Mat & dst, QProgressBar *bar){
    Mat response (1, m_class.count(), CV_32FC1);
    QFileInfo file(img);
    vector<Mat> data;
    vector<Rect> rects;
    vector<Scalar> colors;
    vector<Rect> tempRects;
    vector<Scalar> tempColors;
    QStringList objects;

    try{
        bar->setFormat(QObject::tr("Traitement de l'image '%1'. %0%")
                       .arg(bar->value()).arg(file.fileName()));

        m_pro.process(img, data, rects, colors, bar, 40);

        bar->setFormat(QObject::tr("Detection des objets de l'image '%1'. %0%")
                       .arg(bar->value()).arg(file.fileName()));

        m_bow->loadData(data, bar, 30);
        for(unsigned int j=0; j<data.size(); j++){
            m_mlp.predict(data.at(j), response);
            for(int i=0;i<response.cols; i++){
                if(response.at<float>(0,i) >= 0.0){
                    objects << m_class.at(i);
                    tempColors.push_back(colors.at(j));
                    tempRects.push_back(rects.at(j));
                    break;
                }
            }
        }
    }catch(Exception& e){
        qDebug() << "Exception dans getObjects(): " << e.what();
    }

    bar->setValue(bar->value()+10);
    bar->setFormat(QObject::tr("Création d'une image temporaire. %0%")
                   .arg(bar->value()));
    if(tempRects.size() > 0)
        dst = m_pro.createImg(img, objects, tempRects, tempColors, bar, 20);
    return objects;
}

Mat Classifier::getMat(const QString img,QString object, QProgressBar *bar, int total){
    Mat response (1, m_class.count(), CV_32FC1);
    QFileInfo file(img);
    vector<Mat> data;
    vector<Rect> rects;
    vector<Scalar> colors;
    vector<Rect> tempRects;
    vector<Scalar> tempColors;
    Mat src;
    float counter = (bar->maximum()*total)/100.0f;

    try{
        bar->setFormat(QObject::tr("Traitement de l'image '%1'. %0%")
                       .arg(bar->value()).arg(file.fileName()));
        QApplication::processEvents();
        m_pro.process(img, data, rects, colors, bar, 40/counter);

        bar->setFormat(QObject::tr("Detection des objets de l'image '%1'. %0%")
                       .arg(bar->value()).arg(file.fileName()));
        QApplication::processEvents();
        m_bow->loadData(data, bar, 30/counter);
        for(unsigned int j=0; j<data.size(); j++){
            if(data.at(j).rows == 0)
                continue;
            m_mlp.predict(data.at(j), response);

            for(int i=0;i<response.cols; i++){
                if(response.at<float>(0,i) >= 0.0){
                    if(m_class.at(i) == object){
                        tempColors.push_back(colors.at(j));
                        tempRects.push_back(rects.at(j));
                    }
                    break;
                }
            }
        }
    }catch(Exception& e){
        qDebug() << "Exception dans getMat(): " << e.what();
    }

    bar->setValue(bar->value()+(10/counter));
    bar->setFormat(QObject::tr("Création d'une image temporaire. %0%")
                   .arg(bar->value()));
    if(tempRects.size() > 0)
        src = m_pro.createImg(img, object, tempRects, tempColors, bar, 20/counter);
    return src;
}

vector<Mat> Classifier::compareSrcClass(const QString directory, const QString cl,
                                        QProgressBar *bar){

    QDir dir(directory);
    vector<Mat> list;
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QFileInfoList info = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);

    for(int i=0; i<info.size(); i++){
        Mat src = getMat(info.at(i).absoluteFilePath(), cl, bar, info.size());
        if(src.rows > 0)
            list.push_back(src);
    }
    return list;
}

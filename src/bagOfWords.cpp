#include "bagOfWords.h"

#include <iostream>

#include <QObject>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDebug>
#include <QApplication>

BagOfWords::BagOfWords(){
    m_retries = 1;
    m_k = 0;
    m_degree = 10;
    m_with = false;
    autoK = true;
    m_flags = KMEANS_PP_CENTERS;
    m_tc = TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,100,0.00001);
    QString str = QDir::currentPath().append(DICTIONNARY);
    m_dictionaryPath = QDir::toNativeSeparators(str);
}

void BagOfWords::setK(int k){m_k = k; autoK = false;}
void BagOfWords::setK(){
    Mat dictionary;

    FileStorage fs(m_dictionaryPath.toLocal8Bit().constData(), FileStorage::READ);
    fs["vocabulary"] >> dictionary;
    fs.release();

    m_k = dictionary.rows;
}

QString BagOfWords::getClass(const QString img){
    return img.split("_").at(0);
}

bool BagOfWords::isTrainingImg(const QString img){
    if(m_degree == 1)
        return true;
    try{
        int val = img.split("__").at(1).split(".").at(0).toInt();
       return (((val%m_degree) == 0) ^ m_with);
    }catch(Exception ex){
         qDebug() << "Image mal formatée : " << img;
    }
    return false;
}

void BagOfWords::setCriteria(int type, int maxCount, double epsilon){
    m_tc = TermCriteria(type, maxCount, epsilon);
}

bool BagOfWords::createBow(QString directory, QProgressBar *bar){
    Mat data;
    Mat input;
    Mat dictionary;
    vector<KeyPoint> keypoints;
    Mat descriptor;

    Ptr<FeatureDetector> detector(new SiftFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor());

    try{
        QDir dir(directory);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
        QFileInfoList info = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);

        float counter = (85/(float)info.size());
        float total = 0;
        bar->setFormat(QObject::tr("Chargement des descripteurs. %0%").arg(bar->value()));

        for(int i=0; i<info.size(); i++){
            total+=counter;
            bar->setValue(total);
            if(!isTrainingImg(info.at(i).fileName()))
                continue;
            bar->setFormat(QObject::tr("Chargement du descripteurs(%1). %0%").arg(bar->value())
                           .arg(info.at(i).fileName()));
            QApplication::processEvents();
            input = imread(info.at(i).absoluteFilePath().toLocal8Bit().constData(),
                           CV_LOAD_IMAGE_GRAYSCALE);
            detector->detect(input, keypoints);
            extractor->compute(input, keypoints,descriptor);
            data.push_back(descriptor);
        }
        if(autoK)
            m_k = (int)sqrt((double)(data.rows/2));

        bar->setFormat(QObject::tr("Creation du sac de mot visuel(nombre de cluster = %1). %0%")
                       .arg(bar->value()).arg(m_k));
        QApplication::processEvents();

        BOWKMeansTrainer trainer(m_k,m_tc,m_retries,m_flags);
        dictionary = trainer.cluster(data);

        bar->setValue(bar->value()+10);

        bar->setFormat(QObject::tr("Enregistrement du dictionnaire(dictionary.xml). %0%").arg(bar->value()));

        FileStorage fs(m_dictionaryPath.toLocal8Bit().constData(), FileStorage::WRITE);
        fs << "vocabulary" << dictionary;
        fs.release();

        return true;
    }catch(Exception e){
        qDebug() << "Exception dans createBow(): " << e.what();
    }
    return false;
}

void BagOfWords::loadData(const QString image, Mat &bowDescriptor){
    Mat input;
    vector<KeyPoint> keypoints;
    Mat dictionary;

    FileStorage fs(m_dictionaryPath.toLocal8Bit().constData(), FileStorage::READ);
    fs["vocabulary"] >> dictionary;
    fs.release();

    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
    Ptr<FeatureDetector> detector(new SiftFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor());

    BOWImgDescriptorExtractor bowDE(extractor,matcher);
    bowDE.setVocabulary(dictionary);

    QFileInfo file(image);
    input = imread(file.absoluteFilePath().toLocal8Bit().constData(), CV_LOAD_IMAGE_GRAYSCALE);
    detector->detect(input, keypoints);
    bowDE.compute(input, keypoints, bowDescriptor);
}

void BagOfWords::loadData(vector<Mat> &input, QProgressBar *bar, float total){
    vector<KeyPoint> keypoints;
    Mat bowDescriptor;
    Mat dictionary;

    float in = (float) bar->value();
    float counter = ((bar->maximum()*total)/100.0f)/(float)input.size();
    bar->setFormat(QObject::tr("Chargement du dictionnaire. %0%").arg(bar->value()));

    FileStorage fs(m_dictionaryPath.toLocal8Bit().constData(), FileStorage::READ);
    fs["vocabulary"] >> dictionary;
    fs.release();

    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
    Ptr<FeatureDetector> detector(new SiftFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor());

    BOWImgDescriptorExtractor bowDE(extractor,matcher);
    bowDE.setVocabulary(dictionary);

    in += counter*2;
    bar->setValue(in);
    bar->setFormat(QObject::tr("Creation des descripteurs. %0%").arg(bar->value()));

    for(unsigned int i=0; i<input.size(); i++){
        detector->detect(input.at(i), keypoints);
        bowDE.compute(input.at(i), keypoints, bowDescriptor);
        if(bowDescriptor.rows > 0)
            input.at(i) = bowDescriptor;
        else
            input.at(i) = Mat();

        in += counter;
        bar->setValue(in);
        bar->setFormat(QObject::tr("Creation des descripteurs en cours ... %0%").arg(bar->value()));
        QApplication::processEvents();
    }
}

void BagOfWords::loadDataSet(QString directory, Mat &data){
    Mat input;
    Mat bowDescriptor;
    vector<KeyPoint> keypoints;
    Mat dictionary;

    FileStorage fs(m_dictionaryPath.toLocal8Bit().constData(), FileStorage::READ);
    fs["vocabulary"] >> dictionary;
    fs.release();

    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
    Ptr<FeatureDetector> detector(new SiftFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor());

    BOWImgDescriptorExtractor bowDE(extractor,matcher);
    bowDE.setVocabulary(dictionary);

    QDir dir(directory);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QFileInfoList info = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);

    for(int i=0; i<info.size(); i++){
        input = imread(info.at(i).absoluteFilePath().toLocal8Bit().constData(),
                       CV_LOAD_IMAGE_GRAYSCALE);
        detector->detect(input, keypoints);
        bowDE.compute(input, keypoints, bowDescriptor);
        data.push_back(bowDescriptor);
    }
}

void BagOfWords::loadTrainingSet(QString directory,
                                        QStringList &nnClass, Mat &data, Mat &label, QProgressBar *bar){
    int pos = 0;
    QString class1;
    Mat input;
    Mat bowDescriptor;
    vector<KeyPoint> keypoints;
    Mat dictionary;

    FileStorage fs(m_dictionaryPath.toLocal8Bit().constData(), FileStorage::READ);
    fs["vocabulary"] >> dictionary;
    fs.release();

    Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);
    Ptr<FeatureDetector> detector(new SiftFeatureDetector());
    Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor());

    BOWImgDescriptorExtractor bowDE(extractor,matcher);
    bowDE.setVocabulary(dictionary);

    QDir dir(directory);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QFileInfoList info = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    float in = (float) bar->value();
    float counter = ((bar->maximum()*70)/100.0f)/(float)info.size();

    bar->setFormat(QObject::tr("Creation de l'ensemble d'apprentissage. %0%").arg(bar->value()));

    for(int i=0; i<info.size(); i++){
        in += counter;
        bar->setValue(in);
        if(!isTrainingImg(info.at(i).fileName()))
            continue;
        bar->setFormat(QObject::tr("Chargement des caracteristiques(%1). %0%").arg(bar->value())
                        .arg(info.at(i).fileName()));
        QApplication::processEvents();
        bool test = false;
        class1 = BagOfWords::getClass(info.at(i).fileName());
        for(int j=0;j<nnClass.count();j++){
            if(class1 == nnClass.at(j)){
                pos = j;
                test = true;
                break;
            }
        }

        input = imread(info.at(i).absoluteFilePath().toLocal8Bit().constData(),
                       CV_LOAD_IMAGE_GRAYSCALE);
        detector->detect(input, keypoints);
        bowDE.compute(input, keypoints, bowDescriptor);
        if(bowDescriptor.rows == 0){
            qDebug() << "ATTENTION : '" << info.at(i).fileName() << "' pas de descripteur trouvé !";
            continue;
        }
        data.push_back(bowDescriptor);

        Mat row = Mat (1, nnClass.count(), CV_32FC1);
        for(int j=0; j<row.cols; j++)
            row.at<float>(0,j) = (j == pos && test)?1.0f:-1.0f;
        label.push_back(row);
        if(!test) qDebug() << "L'objet '" << class1 << "' n'a pas de classe !";
    }
}

#include "interfaceui.h"
#include "ui_interfaceui.h"
#include "resultDialog.h"

#include <QObject>
#include <QMessageBox>
#include <QSignalMapper>
#include <QTextStream>
#include <QFileDialog>
#include <QFileInfoList>
#include <QApplication>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>

InterfaceUI::InterfaceUI(QWidget *parent):QMainWindow(parent),ui(new Ui::InterfaceUI){
    ui->setupUi(this);

    setWindowTitle("Algo de Reconnaissance d'images");
    QPixmap bkgnd(":/img/fond");
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    QSignalMapper* mapper1 = new QSignalMapper(this);
    QSignalMapper* mapper2 = new QSignalMapper(this);
    QSignalMapper* mapper3 = new QSignalMapper(this);
    QSignalMapper* mapper4 = new QSignalMapper(this);

    //actions
    connect(ui->actionQuitter, SIGNAL(triggered()), this,SLOT(close()));
    connect(ui->actionSac_de_Mots_visuels, SIGNAL(triggered()), mapper1,SLOT(map()));
    connect(ui->actionR_seau_de_neurones, SIGNAL(triggered()), mapper1,SLOT(map()));
    connect(ui->actionTest, SIGNAL(triggered()), mapper1,SLOT(map()));

    //connect Tab 1
    connect(ui->radioButton, SIGNAL(clicked()), this,SLOT(selecDico()));
    connect(ui->radioButton_2, SIGNAL(clicked()), this,SLOT(selecDico()));
    connect(ui->pushButton, SIGNAL(clicked()), mapper2,SLOT(map()));
    connect(ui->lineEdit, SIGNAL(editingFinished()), mapper2,SLOT(map()));
    connect(ui->pushButton_2, SIGNAL(clicked()), mapper2,SLOT(map()));
    connect(ui->pushButton_4, SIGNAL(clicked()), mapper2,SLOT(map()));
    connect(ui->checkBox_2, SIGNAL(clicked()), mapper2,SLOT(map()));
    connect(ui->lineEdit_2, SIGNAL(editingFinished()), mapper2,SLOT(map()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this,SLOT(createBow()));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), mapper2,SLOT(map()));

    //connect Tab 2
    connect(ui->radioButton_5, SIGNAL(clicked()), mapper3,SLOT(map()));
    connect(ui->radioButton_6, SIGNAL(clicked()), mapper3,SLOT(map()));
    connect(ui->pushButton_7, SIGNAL(clicked()), mapper3,SLOT(map()));
    connect(ui->lineEdit_5, SIGNAL(editingFinished()), mapper3,SLOT(map()));
    connect(ui->pushButton_16, SIGNAL(clicked()), mapper3,SLOT(map()));
    connect(ui->pushButton_14, SIGNAL(clicked()), mapper3,SLOT(map()));
    connect(ui->pushButton_15, SIGNAL(clicked()), mapper3,SLOT(map()));
    connect(ui->lineEdit_11, SIGNAL(editingFinished()), mapper3,SLOT(map()));
    connect(ui->pushButton_8, SIGNAL(clicked()),this,SLOT(beginTraining()));

    //connect Tab 3
    connect(ui->pushButton_5, SIGNAL(clicked()), mapper4,SLOT(map()));
    connect(ui->pushButton_9, SIGNAL(clicked()), mapper4,SLOT(map()));
    connect(ui->pushButton_10, SIGNAL(clicked()), mapper4,SLOT(map()));
    connect(ui->pushButton_12, SIGNAL(clicked()), mapper4,SLOT(map()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this,SLOT(execTest()));
    connect(ui->pushButton_11, SIGNAL(clicked()),this,SLOT(execApli1()));
    connect(ui->pushButton_13, SIGNAL(clicked()),this,SLOT(execApli2()));

    //mapper actions
    mapper1->setMapping(ui->actionSac_de_Mots_visuels, 0);
    mapper1->setMapping(ui->actionR_seau_de_neurones, 1);
    mapper1->setMapping(ui->actionTest, 2);

    //mapper Tab 1
    mapper2->setMapping(ui->pushButton, 0);
    mapper2->setMapping(ui->lineEdit, 1);
    mapper2->setMapping(ui->pushButton_4, 5);
    mapper2->setMapping(ui->pushButton_2, 2);
    mapper2->setMapping(ui->lineEdit_2, 3);
    mapper2->setMapping(ui->comboBox_2, 4);
    mapper2->setMapping(ui->checkBox_2, 6);

    //mapper Tab 2
    mapper3->setMapping(ui->pushButton_7, 5);
    mapper3->setMapping(ui->lineEdit_5, 6);
    mapper3->setMapping(ui->radioButton_5, 8);
    mapper3->setMapping(ui->radioButton_6, 8);
    mapper3->setMapping(ui->pushButton_16, 9);
    mapper3->setMapping(ui->pushButton_14, 11);
    mapper3->setMapping(ui->pushButton_15, 12);
    mapper3->setMapping(ui->lineEdit_11, 10);

    //mapper Tab 3
    mapper4->setMapping(ui->pushButton_5, 0);
    mapper4->setMapping(ui->pushButton_9, 1);
    mapper4->setMapping(ui->pushButton_10, 2);
    mapper4->setMapping(ui->pushButton_12, 3);

    //connect mapper
    connect(mapper1, SIGNAL(mapped(int)), this,SLOT(changeTab(int)));
    connect(mapper2, SIGNAL(mapped(int)), this,SLOT(manageBowTab(int)));
    connect(mapper3, SIGNAL(mapped(int)), this,SLOT(manageRnnTab(int)));
    connect(mapper4, SIGNAL(mapped(int)), this,SLOT(manageTestTab(int)));

    init();
}

InterfaceUI::~InterfaceUI(){
    delete ui;
}

void InterfaceUI::init(){
    bow = new BagOfWords();
    cl= new Classifier(bow);

    QFileInfo file(bow->getDictionaryPath());
    QFileInfo file2(cl->getRnaPath());

    if(!file.exists()){
        refresh(1);
    }
    else{
        ui->radioButton->setChecked(true);
        ui->radioButton_2->setChecked(false);
        ui->groupBox->setEnabled(true);
        ui->groupBox_3->setEnabled(false);
        ui->lineEdit->setText(file.absoluteFilePath());
        ui->label_2->setText(file.fileName());
        ui->label_2->setStyleSheet("QLabel {color : green;}");
    }
    if(file2.exists()){
        ui->radioButton_5->setChecked(true);
        refresh(4);
    }
    ui->doubleSpinBox_5->setValue(cl->getDwScale());
    ui->doubleSpinBox_7->setValue(cl->getMomentScale());
    ui->comboBox_5->setCurrentIndex(cl->getType()-1);
    ui->spinBox_9->setValue(cl->getMaxIter());
    ui->doubleSpinBox_2->setValue(cl->getEpsilon());
    ui->tabWidget->setCurrentIndex(0);
    loadComboClass();
}

void InterfaceUI::loadComboClass(){
    QFile fIn(cl->getClassPath());
    if(fIn.open(QFile::ReadOnly | QFile::Text)){
        QTextStream sIn(&fIn);
        while(!sIn.atEnd())
            ui->comboBox_4->addItem(sIn.readLine());
    }
    else
        qDebug("Erreur d'ouverture du fichier class.txt\n");
}

void InterfaceUI::refresh(int type){
    QFileInfo file;

    if(type == 0){
        file.setFile(bow->getDictionaryPath());
        if(!file.exists())
            bow->setDictionaryPath(QString(""));
        else{
            ui->lineEdit->setText(file.absoluteFilePath());
            ui->label_2->setText(file.fileName());
            ui->label_2->setStyleSheet("QLabel {color : green;}");
        }
        ui->lineEdit_2->setText("");
        ui->label_14->setText("...");
        ui->label_14->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 1){
        ui->lineEdit->setText("");
        ui->label_2->setText("...");
        ui->label_2->setStyleSheet("QLabel {color : green;}");
        ui->spinBox->setValue(bow->getRetries());
        ui->comboBox->setCurrentIndex(bow->getFlags()-1);
        ui->comboBox_3->setCurrentIndex(bow->getType()-1);
        ui->spinBox_3->setValue(bow->getCount());
        ui->doubleSpinBox->setValue(bow->getEpsilon());
    }
    else if(type == 2){
        ui->lineEdit_5->setText("");
        ui->label_20->setText("...");
        ui->label_20->setStyleSheet("QLabel {color : green;}");
        ui->lineEdit_11->setText("");
        ui->label_22->setText("...");
        ui->label_22->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 4){
        file.setFile(cl->getRnaPath());
        if(file.exists()){
            ui->lineEdit_11->setText(file.absoluteFilePath());
            ui->label_22->setText(file.fileName());
            ui->label_22->setStyleSheet("QLabel {color : green;}");
        }
        ui->lineEdit_5->setText("");
        ui->label_20->setText("...");
        ui->label_20->setStyleSheet("QLabel {color : green;}");
        ui->groupBox_14->setEnabled(true);
        ui->groupBox_10->setEnabled(false);
    }
}

void InterfaceUI::changeTab(int i){
    ui->tabWidget->setCurrentIndex(i);
}

void InterfaceUI::selecDico(){
    if(!ui->radioButton->isChecked()){
        ui->groupBox->setEnabled(false);
        ui->groupBox_3->setEnabled(true);
        refresh(1);
    }
    else{
        ui->groupBox->setEnabled(true);
        ui->groupBox_3->setEnabled(false);
        refresh(0);
    }
}

void InterfaceUI::createBow(){
    QString filepath = ui->lineEdit_2->text();
    QFileInfo file(filepath);

    if(!file.exists()){
        QMessageBox::warning(this, tr("Creation du dictionnaire"), tr("Repertoire d'image incorrect !"));
        return;
    }
    else if(getNbImages(file.absoluteFilePath()) < 1){
        QMessageBox::warning(this, tr("Creation du dictionnaire"), tr("Repertoire d'image vide !"));
        return;
    }
    ui->tabWidget->setEnabled(false);
    setDirectory(filepath);
    if(bow->createBow(filepath, ui->progressBar)){
        ui->progressBar->setValue(ui->progressBar->maximum());
        ui->progressBar->setFormat(QString("Apprentissage OK. %0%").arg(ui->progressBar->maximum()));
        ui->radioButton->setChecked(true);
        selecDico();
        QMessageBox::information(this, tr("Creation du dictionnaire"),
                                 tr("Creation du dictionnaire <strong>OK</strong> (nombre de cluster = %1)").arg(bow->getK()));
    }
    else
        QMessageBox::critical(this, tr("Creation du dictionnaire"),
                              tr("Erreur lors de la creation du dictionnaire !</br> Verifiez le nombre de cluster ou le nombre d'images"));
    ui->tabWidget->setEnabled(true);
    ui->progressBar->setValue(ui->progressBar->minimum());
    ui->progressBar->setFormat(QString("%0%").arg(ui->progressBar->value()));
}

void InterfaceUI::beginTraining(){
    QFileInfo file(bow->getDictionaryPath());

    if(!file.exists()){
        QMessageBox::warning(this, tr("Apprentissage du RNA"),
                             tr("Vous devez prealablement creer le dictionnaire de mots visuels."));
        return;
    }

    ui->progressBar->setFormat(QObject::tr("Initialisation en cours ... %0%").arg(ui->progressBar->value()));

    cl->setDwScale(ui->doubleSpinBox_5->value());
    cl->setMomentScale(ui->doubleSpinBox_7->value());
    cl->setEpsilon(ui->doubleSpinBox_2->value());
    cl->setMaxIter(ui->spinBox_9->value());
    cl->setType(ui->comboBox_5->currentIndex()-1);

    QString path = ui->lineEdit_5->text();
    file.setFile(path);
    if(!file.exists()){
        QMessageBox::warning(this, tr("Apprentissage du RNA"),
                             tr("Chemin vers le repertoire d'image incorrect !"));
        return;
    }
    else if(getNbImages(file.absoluteFilePath()) < 1){
        QMessageBox::warning(this, tr("Apprentissage du RNA"), tr("Repertoire d'image vide !"));
        return;
    }
    ui->tabWidget->setEnabled(false);
    setDirectory(path);
    bow->setK();
    cl->loadClass(path, ui->progressBar);
    cl->createNN();

    if(cl->train(path, ui->progressBar)){
        ui->progressBar->setValue(ui->progressBar->maximum());
        ui->progressBar->setFormat(QString("Apprentissage OK. %0%").arg(ui->progressBar->maximum()));
        QString str = tr("Apprentissage du RNA <strong>OK</strong>.<ul>"
                         "<li>Nombre de neurones de la couche d'entree : %0 </li>"
                         "<li>Nombre de neurones de la couche de sortie : %1</li>"
                         "<li>Nombre de Couche cachee : %2</li></ul>").arg(bow->getK())
                .arg(cl->getNbClass()).arg(cl->getLayers());
        QMessageBox::information(this, tr("Apprentissage du RNA"),str);
    }
    else
        QMessageBox::critical(this, tr("Apprentissage du RNA"),
                              tr("Erreur lors de l'Apprentissage du RNA !"));

    ui->tabWidget->setEnabled(true);
    ui->progressBar->setValue(ui->progressBar->minimum());
    ui->progressBar->setFormat(QString("%0%").arg(ui->progressBar->value()));

    file.setFile(cl->getRnaPath());
    if(file.exists()){
        ui->radioButton_5->setChecked(true);
        ui->comboBox_4->clear();
        ui->comboBox_4->insertItems(0,cl->getClass());
        refresh(4);
    }
    else
        ui->comboBox_4->clear();
}

void InterfaceUI::manageBowTab(int type){
    QString filepath;
    QFileInfo file;

    if(type == 0){
        filepath = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), QString(),
                                                tr("Dictionnaire (*.xml)"));
        file.setFile(filepath);
        if(!file.exists() || file.isDir()){
            ui->label_2->setText(tr("Fichier '%1' innexistant !").arg(file.fileName()));
            ui->label_2->setStyleSheet("QLabel {color : red;}");
            bow->setDictionaryPath(QString(""));
            return;
        }
        ui->lineEdit->setText(file.absoluteFilePath());
        ui->label_2->setText(file.fileName());
        ui->label_2->setStyleSheet("QLabel {color : green;}");
        bow->setDictionaryPath(file.filePath());
    }
    else if(type == 1){
        filepath = ui->lineEdit->text();
        file.setFile(filepath);
        if(!file.exists() || file.isDir()){
            ui->label_2->setText(tr("Fichier '%1' innexistant !").arg(file.fileName()));
            ui->label_2->setStyleSheet("QLabel {color : red;}");
            return;
        }
        ui->label_2->setText(file.fileName());
        ui->label_2->setStyleSheet("QLabel {color : green;}");
        bow->setDictionaryPath(file.filePath());
    }
    else if(type == 2){
        filepath = QFileDialog::getExistingDirectory(this);
        file.setFile(filepath);
        if(!file.exists()){
            ui->lineEdit_2->setText(file.absoluteFilePath());
            ui->label_14->setText(tr("Chemin vers le repertoire '%0' incorrect !").arg(file.fileName()));
            ui->label_14->setStyleSheet("QLabel {color : red;}");
            return;
        }
        ui->lineEdit_2->setText(file.absoluteFilePath());
        ui->label_14->setText(QString("%1").arg(getSetImages(file.absoluteFilePath())));
        ui->label_14->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 3){
        filepath = ui->lineEdit_2->text();
        file.setFile(filepath);
        if(!file.exists()){
            ui->label_14->setText(tr("Chemin vers le repertoire '%0' incorrect !").arg(file.fileName()));
            ui->label_14->setStyleSheet("QLabel {color : red;}");
            return;
        }
        ui->label_14->setText(QString("%1").arg(getSetImages(file.absoluteFilePath())));
        ui->label_14->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 4){
        if(ui->spinBox_2->isEnabled())
            ui->spinBox_2->setEnabled(false);
        else
            ui->spinBox_2->setEnabled(true);
    }
    else if(type == 5){
        filepath = ui->lineEdit_2->text();
        file.setFile(filepath);
        if(!file.exists()){
            ui->label_14->setText(tr("Chemin vers le repertoire '%0' incorrect !").arg(file.fileName()));
            ui->label_14->setStyleSheet("QLabel {color : red;}");
            return;
        }
        bow->setDegree(ui->spinBox_4->value());
        ui->label_14->setText(QString("%1").arg(getSetImages(file.absoluteFilePath())));
        ui->label_14->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 6){
        if(ui->checkBox_2->isChecked())
            bow->setWith(true);
        else
            bow->setWith(false);
    }
}

void InterfaceUI::setDirectory(QString &d){
    if(d.endsWith('/') || d.endsWith('\\'))
        return;
    d += "/";
}

int InterfaceUI::getNbImages(const QString directory){
    QDir dir(directory);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QFileInfoList info = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    return info.size();
}

QString InterfaceUI::getSetImages(const QString directory){
    QDir dir(directory);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    QFileInfoList info = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    int train = 0, test = 0;
    for(int i=0; i<info.size(); i++){
        if(!bow->isTrainingImg(info.at(i).fileName()))
            test++;
        else
            train++;
    }
    return QString("train_set: %0. test_set: %1").arg(train).arg(test);
}

void InterfaceUI::manageRnnTab(int type){
    QString filepath;
    QFileInfo file;

    if(type == 5){
        filepath = QFileDialog::getExistingDirectory(this);
        file.setFile(filepath);
        if(!file.exists()){
            ui->lineEdit_5->setText(file.absoluteFilePath());
            ui->label_20->setText(tr("Chemin vers le repertoire '%0' incorrect !").arg(file.fileName()));
            ui->label_20->setStyleSheet("QLabel {color : red;}");
            return;
        }
        ui->lineEdit_5->setText(file.absoluteFilePath());
        ui->label_20->setText(QString("%0").arg(getSetImages(file.absoluteFilePath())));
        ui->label_20->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 6){
        filepath = ui->lineEdit_5->text();
        file.setFile(filepath);
        if(!file.exists()){
            ui->label_20->setText(tr("Chemin vers le repertoire '%0' incorrect !").arg(file.fileName()));
            ui->label_20->setStyleSheet("QLabel {color : red;}");
            return;
        }
        ui->label_20->setText(QString("%0").arg(getSetImages(file.absoluteFilePath())));
        ui->label_20->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 8){
        if(!ui->radioButton_5->isChecked()){
            ui->groupBox_14->setEnabled(false);
            ui->groupBox_10->setEnabled(true);
            refresh(2);
        }
        else{
            refresh(4);
        }
    }
    else if(type == 9){
        filepath = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), QString(),
                                                tr("RNA (*.xml)"));
        file.setFile(filepath);
        if(!file.exists() || file.isDir()){
            ui->label_22->setText(tr("Fichier '%1' innexistant !").arg(file.fileName()));
            ui->label_22->setStyleSheet("QLabel {color : red;}");
            return;
        }
        ui->lineEdit_11->setText(file.absoluteFilePath());
        ui->label_22->setText(file.fileName());
        ui->label_22->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 10){
        filepath = ui->lineEdit_11->text();
        file.setFile(filepath);
        if(!file.exists() || file.isDir()){
            ui->label_22->setText(tr("Fichier '%1' innexistant !").arg(file.fileName()));
            ui->label_22->setStyleSheet("QLabel {color : red;}");
            return;
        }
        ui->label_22->setText(file.fileName());
        ui->label_22->setStyleSheet("QLabel {color : green;}");
    }
    else if(type == 11){
        int nb = ui->spinBox_6->value();
        cl->addLayers(nb);
        ui->label_28->setText(QString("%0").arg(cl->getLayers()));
        ui->spinBox_5->setMaximum(cl->getLayers());
        ui->spinBox_5->setEnabled(true);
        ui->pushButton_15->setEnabled(true);
    }
    else if(type == 12){
        int nb = ui->spinBox_5->value();
        cl->deleteLayers(nb);
        ui->label_28->setText(QString("%0").arg(cl->getLayers()));
        if(cl->getLayers() < 1){
            ui->spinBox_5->setEnabled(false);
            ui->pushButton_15->setEnabled(false);
        }
        else
            ui->spinBox_5->setMaximum(cl->getLayers());
    }
}

void InterfaceUI::manageTestTab(int type){
    QString filepath;
    QFileInfo file;

    if(type != 1)
        filepath = QFileDialog::getExistingDirectory(this);
    else
        filepath = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"), QString(),
                                                tr("Images (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP"));
    file.setFile(filepath);

    if(type == 0)
        ui->lineEdit_4->setText(file.absoluteFilePath());
    else if(type == 1){
        ui->lineEdit_6->setText(file.absoluteFilePath());
        printObjects(file.absoluteFilePath());
    }
    else if(type == 2)
        ui->lineEdit_7->setText(file.absoluteFilePath());
    else if(type == 3)
        ui->lineEdit_8->setText(file.absoluteFilePath());
}

void InterfaceUI::execTest(){
    QString path = ui->lineEdit_4->text();
    QFileInfo file(path);
    if(!file.exists() || file.isFile()){
        QMessageBox::warning(this, tr("Evaluation de la classification"),
                             tr("Chemin vers le repertoire d'evaluation incorrect !"));
        return;
    }
    file.setFile(cl->getRnaPath());
    if(!file.exists()){
        QMessageBox::warning(this, tr("Evaluation de la classification"),
                             tr("Vous devez effectuer l'apprentissage du RNA au prealable !"));
        return;
    }
    ui->progressBar->setFormat(QObject::tr("Chargement du RNA. %0%").arg(ui->progressBar->value()));
    if(!cl->isTrainning()) cl->load();
    ui->progressBar->setValue(ui->progressBar->value()+5);
    ui->tabWidget->setEnabled(false);
    setDirectory(path);
    bow->setWith(!bow->getWith());
    map<QString,float> result = cl->test(path, ui->progressBar);
    bow->setWith(!bow->getWith());
    QString str ="<ul>";
    map<QString, float>::iterator up;
    for(up = result.begin();up != result.end(); ++up)
        str.append(QString("<li>%0 : %1</il>").arg(up->first).arg(up->second));
    str.append("</ul>");
    ui->tabWidget->setEnabled(true);
    ui->label_16->setText(str);
    ui->progressBar->setValue(ui->progressBar->maximum());
    ui->progressBar->setFormat(QString("Evaluation OK. %0%").arg(ui->progressBar->maximum()));
    QString msg = tr("Evaluation des donnees <strong>OK</strong> les resultats suivant : %0").arg(str);
    QMessageBox::information(this, tr("Evaluation de la classification"), msg);
    ui->progressBar->setValue(ui->progressBar->minimum());
    ui->progressBar->setFormat(QString("%0%").arg(ui->progressBar->value()));
}

void InterfaceUI::printObjects(const QString path){
    if(ui->checkBox->isChecked()){
        QFileInfo file(path);
        if(!file.exists() || file.isDir()){
            QMessageBox::warning(this, tr("Application 1"),
                                 tr("Fichier introuvable !"));
            return;
        }
        if(!QFileInfo(cl->getRnaPath()).exists()){
            QMessageBox::warning(this, tr("Application 1"),
                                 tr("Vous devez effectuer l'apprentissage du RNA au prealable !"));
            return;
        }
        ui->progressBar->setFormat(QObject::tr("Initialisations en cours... %0%")
                                   .arg(ui->progressBar->value()));
        if(!cl->isTrainning()) cl->load();
        Mat dst;
        QStringList list = cl->getObjects(path, dst, ui->progressBar);
        ui->progressBar->setValue(ui->progressBar->maximum());
        ui->progressBar->setFormat(QString("Recherche terminee. %0%").arg(ui->progressBar->maximum()));
        if(list.isEmpty()){
            QMessageBox::information(this, tr("Application 1"),
                                     tr("Aucun objet trouve dans '%1'").arg(file.fileName()));
        }
        else{
            vector<Mat> img;
            img.push_back(dst);
            list.removeDuplicates();
            ui->comboBox_6->addItems(list);
            ResultDialog result(this, Qt::Dialog, img);
            result.exec();
        }
        ui->progressBar->setValue(ui->progressBar->minimum());
        ui->progressBar->setFormat(QString("%0%").arg(ui->progressBar->value()));
    }
}

void InterfaceUI::execApli1(){
    QString path = ui->lineEdit_7->text();
    QString classs = ui->comboBox_6->currentText();
    QFileInfo file(path);

    if(classs.isEmpty()){
        QMessageBox::warning(this, tr("Application 1"),
                             tr("Aucun objet n'a ete selectionne !"));
        return;
    }
    if(!file.exists() || file.isFile()){
        QMessageBox::warning(this, tr("Application 1"),
                             tr("Chemin vers le repertoire de recherche incorrect !"));
        return;
    }
    if(!QFileInfo(cl->getRnaPath()).exists()){
        QMessageBox::warning(this, tr("Application 1"),
                             tr("Vous devez effectuer l'apprentissage du RNA au prealable !"));
        return;
    }
    ui->progressBar->setFormat(QObject::tr("Initialisations en cours... %0%")
                               .arg(ui->progressBar->value()));
    ui->tabWidget->setEnabled(false);
    if(!cl->isTrainning()) cl->load();
    setDirectory(path);
    vector<Mat> list = cl->compareSrcClass(path, classs, ui->progressBar);
    ui->tabWidget->setEnabled(true);
    ui->progressBar->setValue(ui->progressBar->maximum());
    ui->progressBar->setFormat(QString("Recherche terminee. %0%").arg(ui->progressBar->maximum()));

    if(list.size() == 0){
        QMessageBox::information(this, tr("Application 1"),
                                 tr("Aucun objet dans '%0' ne correspond a la classe '%1'")
                                 .arg(file.fileName()).arg(classs));
    }
    else{
        ResultDialog result(this, Qt::Dialog, list);
        result.exec();
    }
    ui->progressBar->setValue(ui->progressBar->minimum());
    ui->progressBar->setFormat(QString("%0%").arg(ui->progressBar->value()));
}

void InterfaceUI::execApli2(){
    QString path = ui->lineEdit_8->text();
    QString classs = ui->comboBox_4->currentText();
    QFileInfo file(path);
    if(!file.exists() || file.isFile()){
        QMessageBox::warning(this, tr("Application 2"),
                             tr("Chemin vers le repertoire de recherche incorrect !"));
        return;
    }
    if(!QFileInfo(cl->getRnaPath()).exists()){
        QMessageBox::warning(this, tr("Application 2"),
                             tr("Vous devez effectuer l'apprentissage du RNA au prealable !"));
        return;
    }
    ui->progressBar->setFormat(QObject::tr("Initialisations en cours... %0%")
                               .arg(ui->progressBar->value()));
    ui->tabWidget->setEnabled(false);
    if(!cl->isTrainning()) cl->load();
    setDirectory(path);
    vector<Mat> list = cl->compareSrcClass(path, classs, ui->progressBar);
    ui->tabWidget->setEnabled(true);
    ui->progressBar->setValue(ui->progressBar->maximum());
    ui->progressBar->setFormat(QString("Recherche terminee. %0%").arg(ui->progressBar->maximum()));

    if(list.size() == 0){
        QMessageBox::information(this, tr("Application 2"),
                                 tr("Aucun objet dans '%0' ne correspond a la classe '%1'")
                                 .arg(file.fileName()).arg(classs));
    }
    else{
        ResultDialog result(this, Qt::Dialog, list);
        result.exec();
    }
    ui->progressBar->setValue(ui->progressBar->minimum());
    ui->progressBar->setFormat(QString("%0%").arg(ui->progressBar->value()));
}

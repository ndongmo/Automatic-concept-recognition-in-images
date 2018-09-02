#ifndef INTERFACEUI_H
#define INTERFACEUI_H

#include <QMainWindow>

#include "bagOfWords.h"
#include "classifier.h"

namespace Ui {
class InterfaceUI;
}

class InterfaceUI : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit InterfaceUI(QWidget *parent = 0);
    ~InterfaceUI();
    void loadComboClass();
    void setDirectory(QString &);
    int getNbImages(const QString directory);
    QString getSetImages(const QString directory);
    void printObjects(const QString path);

public slots:
    void changeTab(int);
    void selecDico();
    void manageBowTab(int);
    void manageRnnTab(int);
    void manageTestTab(int);
    void refresh(int);
    void createBow();
    void beginTraining();
    void execTest();
    void execApli1();
    void execApli2();

private:
    void init();

    Ui::InterfaceUI *ui;
    BagOfWords *bow;
    Classifier *cl;
};

#endif // INTERFACEUI_H

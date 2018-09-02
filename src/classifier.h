#ifndef CLASSIFIER
#define CLASSIFIER

/*!
* \file classifier.h
* \brief Classifieur
* \author Ndongmo Silatsa Fabrice
* \version 1.1
*/

#include <map>
#include <vector>

#include <QString>
#include <QProgressBar>
#include <QStringList>

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp>  

#include "bagOfWords.h"
#include "processing.h"

using namespace std;
using namespace cv;

/*! \class Classifier
* \brief Classe chargée de la création du Réseau de neurones,
*  de son apprentissage et de la classification des images
*  Type : Perceptron Multi-couches
*  Méthode d'apprentissage : Rétropropagation du gradient
*/
class Classifier{
public:
    /*!
        *  \brief Constructeur par défaut de la classe Classifier
        */
    Classifier();
    /*!
        *  \brief Constructeur de la classe Classifier
        *
        *  \param bow : Pointeur vers le Sac de mots visuels
        */
    Classifier(BagOfWords *);
    /*!
        *  \brief Méthode de création du Réseau de neurones
        */
    void createNN();
    /*!
        *  \brief Méthode de création du Réseau de neurones
        *
        *  \param k : nombre de clusters
        */
    void createNN(int k);
    /*!
        *  \brief Méthode d'entrainement du Réseau de neurones
        *
        *  \param directory : chemin vers le repertoire d'images
        */
    bool train(QString directory, QProgressBar *);
    /*!
        *  \brief Méthode de test du Réseau de neurones
        *
        *  \param directory : chemin vers le repertoire d'images
        *  \return précision du Réseau de neurones
        */
    map<QString, float> test(QString directory, QProgressBar *);
    /*!
        *  \brief Méthode qui prédit la classe d'une image
        *
        *  \param image : chemin vers le fichier image
        *  \return classe prédite par le Réseau de neurones
        */
    QString getClass(const QString image);
    QStringList getObjects(const QString, Mat &, QProgressBar *);
    Mat getMat(const QString, const QString,  QProgressBar *, int);
    /*!
        *  \brief Méthode qui prédit les sorties des données
        *
        *  \param data : entrées du Réseau de neurones
        *  \return données en sortie du Réseau de neurones
        */
    Mat predict(Mat &data, QProgressBar *);
    /*!
        *  \brief Méthode d'évaluation du Réseau de neurones
        *
        *  \param predicted : prédiction du Réseau de neurones
        *  \param actual : vraie nature des données
        *  \return précision obtenue en comparant predicted et actual
        */
    map<QString, float> evaluate(Mat &predicted, Mat &actual, QProgressBar *);
    vector<Mat> compareSrcClass(const QString, const QString, QProgressBar *);

    /*!
        *  \brief Méthode qui charge le Réseau de neurones à partir d'une sauvegarde existante
        *
        *  \return true si le chargement s'est bien déroulé, false sinon
        */
    bool load();
    /*!
        *  \brief Méthode qui sauvegarde le Réseau de neurones dans un fichier xml
        *
        *  \return true si la sauvegarde s'est bien déroulé, false sinon
        */
    bool save();
    /*!
        *  \brief Méthode qui charge les classes d'objets du
        *  Réseau de neurones à partir d'un repertoire d'images
        *
        *  \param directory : chemin vers le repertoire d'images
        */
    void loadClass(QString directory, QProgressBar *);
    void setClass(QStringList &);
    void addLayers(int nb);
    void deleteLayers(int nb);

    /*!
        *  \brief Méthode qui retourne le nombre de classe du Réseau de neurones
        *
        *  \return nombre de classe
        */
    int getNbClass();
    inline QString getRnaPath(){return rnaPath;}
    inline QString getClassPath(){return classPath;}
    inline QStringList getClass(){return m_class;}
    inline bool isTrainning(){return istraining;}
    inline int getLayers(){return tempLayers.size();}
    inline int getMaxIter(){return m_params.term_crit.max_iter;}
    inline int getType(){return m_params.term_crit.type;}
    inline double getEpsilon(){return m_params.term_crit.epsilon;}
    inline double getDwScale(){return m_params.bp_dw_scale;}
    inline double getMomentScale(){return m_params.bp_moment_scale;}

    inline void setMaxIter(int i){m_params.term_crit.max_iter = i;}
    inline void setType(int t){m_params.term_crit.type = t;}
    inline void setEpsilon(double e){m_params.term_crit.epsilon = e;}
    inline void setDwScale(double d){m_params.bp_dw_scale = d;}
    inline void setMomentScale(double m){m_params.bp_moment_scale = m;}
    inline void EmptyClass(){m_class.clear();}

private:
    CvANN_MLP m_mlp; /*!< Réseau de neurones */
    CvANN_MLP_TrainParams m_params; /*!< Paramètres du Réseau de neurones (voir doc opencv) */
    QStringList m_class; /*!< Ensemble des classes d'objets */
    BagOfWords *m_bow; /*!< Sac de mots visuels */
    Processing m_pro;
    bool istraining;
    QString rnaPath;
    QString classPath;
    vector<int> tempLayers;
};
#endif

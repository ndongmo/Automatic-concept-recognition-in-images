#ifndef BAG_OF_WORDS
#define BAG_OF_WORDS

#define DICTIONNARY "/data/dictionary.xml"

/*!
* \file bagOfWords.h
* \brief Sac de mots visuel
* \author Ndongmo Silatsa Fabrice
* \version 1.1
*/

#include <vector>

#include <QString>
#include <QProgressBar>
#include <QStringList>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;

/*! \class BagOfWords
* \brief Classe chargée de la création du Sac de mots visuels,
*  elle crée le sac de mots visuels et génère pour une image un ensemble
*  de données issu de la clusterisation de ses descripteurs en comparaison
*  aux descripturs du sac de mots visuels
*  Descripteurs utilisés : SIFT
*  Méthode de clustering : K-Means
*/
class BagOfWords{
public:
    /*!
    *  \brief Constructeur de la classe BagOfWords
    */
    BagOfWords();

    /*!
    *  \brief Teste si une image fait partie de l'ensemble d'apprentissage ou de test
    *
    *  Méthode qui effectue le tri d'images de test et d'apprentissage
    *  \param img : chemin de l'image
    *  \return true si img fait partie de l'ensemble d'apprentissage, false sinon
    */
    bool isTrainingImg(const QString img);
    /*!
    *  \brief Méthode qui crée le Sac de mots visuels (dictionnaire)
    *
    *  \param directory : chemin vers le repertoire d'images
    *  \return true si la création s'est bien déroulée, false sinon
    */
    bool createBow(QString directory, QProgressBar *);
    /*!
    *  \brief Méthode qui récup?re les descripteurs d'une image
    *  et gén?re des données en comparaison au Sac de mots
    *
    *  \param img : chemin de l'image
    *  \param data : données ? charger
    */
    void loadData(const QString img, Mat &data);
    void loadData(vector<Mat> &input, QProgressBar *, float total);
    /*!
    *  \brief Méthode qui récup?re les descripteurs de plusieurs images
    *  et gén?re des données en comparaison au Sac de mots
    *
    *  \param directory : chemin vers le repertoire d'images
    *  \param data : données ? charger
    */
    void loadDataSet(QString directory, Mat &data);
    /*!
    *  \brief Methode qui récup?re les données d'apprentissage et l'ensemble
    *  des labels correspondant ? partir d'un repertoire d'images,
    *  charge aussi les classes d'objets
    *
    *  \param directory : chemin vers le repertoire d'images
    *  \param nnClass : classes d'objets ? charger
    *  \param data : données d'apprentissage ? charger
    *  \param label : ensemble des labels d'apprentissage ? charger
    */
    void loadTrainingSet(QString directory, QStringList &nnClass, Mat &data, Mat &label, QProgressBar *);

    /*!
    *  \brief Méthode qui modifie les critères de calcul de K-Means
    *
    *  \param type : Type met fin à l'algorithme K-Means (voir doc opencv)
    *  \param maxCount : Nombre maximal de bouclage de K-Means (voir doc opencv)
    *  \param epsilon : facteur de précision de K-Means (voir doc opencv)
    */
    void setCriteria(int type, int maxCount, double epsilon);
    /*!
    *  \brief Méthode qui calcule le nombre de cluster
    *  à partir du Sac de mots préalablement créé (fichier dictionnaire)
    */
    void setK();
    void setK(int k);

    inline void setRetries(int retries){m_retries = retries;}
    inline void setFlags(int flags){m_flags = flags;}
    inline void setWith(bool with){m_with = with;}
    inline void setDegree(int degree){m_degree = degree;}
    inline void setDictionaryPath(QString path){m_dictionaryPath = path;}

    inline TermCriteria getCriteria(){return m_tc;}
    inline int getRetries(){return m_retries;}
    inline int getFlags(){return m_flags;}
    inline bool getWith(){return m_with;}
    inline int getK(){return m_k;}
    inline int getType(){return m_tc.type;}
    inline int getCount(){return m_tc.maxCount;}
    inline double getEpsilon(){return m_tc.epsilon;}
    inline QString getDictionaryPath(){return m_dictionaryPath;}

    /*!
    *  \brief Méthode qui détermine la classe d'objets à laquelle appartient une image
    *
    *  \param img : chemin vers l'image
    *  \return nom de la classe d'objets
    */
    static QString getClass(const QString img);

private:
    QString m_dictionaryPath; /*!< Chemin vers le fichier dictionnaire */
    TermCriteria m_tc; /*!< Crit?re de calul des moyennes de K-Means */
    int m_retries; /*!< Indique le nombre de fois l'algo K-Means est exécuté en utilisant
                   différents initialisations, retient la sortie la plus compacte */
    int m_flags; /*!< Précise la méthode d'initialisation des centres de K-Means */
    int m_k; /*!< nombre de cluster */
    int m_degree; /*!< Intervalle d'images de test ou d'apprentissage */
    bool m_with; /*!< Combinée ? m_degree, permet de sélectionner les images de test ou d'apprentissage */
    bool autoK; /*!< clacul de k automatique */
};
#endif

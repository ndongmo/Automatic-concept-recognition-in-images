#ifndef PROCESSING
#define PROCESSING

/*!
* \file bagOfWords.h
* \brief Sac de mots visuel
* \author Ndongmo Silatsa Fabrice
* \version 1.1
*/

#include <QString>
#include <QStringLIst>
#include <QProgressBar>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;

/*! \class Processing
* \brief Classe chargée du pré-traitement d'images
*/
class Processing{
public:
    /*!
    *  \brief Constructeur de la classe Processing
    */
    Processing();
    Processing(float x, float y);

    /*!
    *  \brief Méthode
    *
    *  \param img : chemin vers l'image
    */
    void process(const QString , vector<Mat> &, vector<Rect> &, vector<Scalar> &, QProgressBar *, float total);
    Mat createImg(const QString, QStringList, vector<Rect>, vector<Scalar>, QProgressBar *, float total);
    Mat createImg(const QString, QString, vector<Rect>, vector<Scalar>, QProgressBar *, float total);


private:
    float m_scaleX;
    float m_scaleY; /*!< Echelle d'objets à détecter par rapport à la taille de l'image */
};
#endif

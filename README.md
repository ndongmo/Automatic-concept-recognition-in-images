# Automatic concept recognition in images

This project aims to implement a strong C++ algorithm for concept recognition in images based on Neural Networks. Moreover, it implements a friendly user interface for evaluation and testing using Qt library. We also use the well-known OpenCv library for images processing and classification. Our method is a two phases approach. First, we detect and localize a concept (shape) in an image. Then, we recognize and classify the concept with our Neral Networks. However, before starting detection and classification, we need to do some work :

- create a bag of visual words and extract image signatures from the training dataset of images
- train the Neural Networks with the training dataset

## Building of the bag of visual words and image signatures extraction

### Image dataset and features extraction

First of all, we need to choose a right training dataset for feature extraction. Here are some features we have to take into account when choosing a training images dataset :
- the size of the dataset
- the level of detail
- the variety of image viewpoints

Once the training dataset has been selected, we can start features extraction. A good feature should be invariant to geometry (rotation, scaling, affine transformation) and photometry. There are many methods of extraction implemented by OpenCv, we choose the SIFT method because it has a better performance for some features.

<table width="100">
  <tr>
    <th>Method</th> <th>Time</th> <th>Scale</th> <th>Rotation</th> <th>Blur</th> <th>Illumination</th> <th>Affine</th> 
  </tr>
  <tr>
    <td>SIFT</td> <td>Normal</td> <td>Best</td> <td>Best</td> <td>Best</td> <td>Normal</td> <td>Good</td>
  </tr>
  <tr>
    <td>PCA-SIFT</td> <td>Good</td> <td>Normal</td> <td>Good</td> <td>Normal</td> <td>Good</td> <td>Good</td>
  </tr>
  <tr>
    <td>SURF</td> <td>Best</td> <td>Good</td> <td>Normal</td> <td>Good</td> <td>Best</td> <td>Good</td>
  </tr>
</table>

### Bag of visual words
This is done by clustering the extracted features with __kMeans algorithm__ provided by OpenCv.

### Image signatures
Once we built the bag of visual words, for each image we can now extract its signature. This signature is a vector V where |V| = the size of the clusters set and V[i] is the number of descriptors contained in the ith cluster.

<img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/signature.png" width="60%" />

## Classification

The classification aims to associate automatically keywords to images or concepts in our context. In order to achieve this goal, we are going to use Artificial Neural Networks. But before using them for classification, we must train them with the training dataset. We use a supervised learning method which consists of a mapping between an input and an output. So, we map a concept signature to a class name or label. Also, our learning method is based on multilayer perceptron using a __backpropagation algorithm__ to calculate the gradient. Once this step is done, we can present a concept to our Artificial Neural Network and it will return its class name.

<img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/perceptron.png" width="50%" />

## Implementation

Our algorithm is implemented through 3 major classes :
- __BagOfWords__ : create and load image signatures
- __Classifier__ : classify concepts by the mean of Artificial Neural Networks
- __Processing__ : process images and return a set of concepts (shape)

<img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/class_diagram.PNG" width="70%" />

## Evaluation and testing

## Evaluation
In order to evaluate our algorithm, we use Columbia Object Image Library (COIL) which is a dataset available in several different versions : COIL-100, COIL-20. We use __cross validation__ as our evaluation method where we use the half dataset for training and the other for evaluation and testing. Here are the results we got according to data classification indices for COIL-100 dataset :

- True Positive Rate (TPR) : 69%
- False Positive Rate (TPR) : 0.4%
- Recall : 69%
- Precision : 61%
- Accuracy : 99%
- F-mesure : 65%

<img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/coil_100.png" width="30%" />

## Testing
Our software present 3 different menus :

 - Bag of visual words : here we can create a new dictionary (image signatures) with precise configurations and load a saved dictionary.
 - Neural Networks : here we can configure, train or load a trained Neural Networks.
 - Tests and applications : here we can evaluate our classification, find different concepts in a given set of images (directory) and for every found concept, we can use it for searching in a given directory.
 
 <table style="width:100%">
  <tr>
    <td>
      <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app1.PNG" />
    </td>
    <td>
      <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app2.PNG" />
    </td>
    <td>
     <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app3.PNG" />
    </td>
  </tr>
  <tr>
    <td>
      <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app4.PNG" />
    </td>
    <td>
      <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app5.PNG" />
    </td>
    <td>
     <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app6.PNG" />
    </td>
  </tr>
 </table>
 
 <table style="width:100%">
  <tr>
    <td>
      <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app7.PNG" />
    </td>
    <td>
      <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app8.PNG" />
    </td>
    <td>
     <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app9.PNG" />
    </td>
    <td>
     <img src="https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/images/app10.PNG" />
    </td>
  </tr>
 </table>

## Authors

* F. Ndongmo Silatsa

## Licence

This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/ndongmo/Automatic-concept-recognition-in-images/blob/master/LICENSE.md) file for details

## Acknowledgments

* D. Frolova and D. Simakov. Matching with invariant features. page 35, mars 2004.
* L. Juan and O. Gwun. A comparison of sift, pca-sift and surf. International Journal of Image Processing
(IJIP), pages 143-152, 2009.
* P. Borne, M. Benrejeb, and J. Haggege. Les réseaux de neurones : Présentation et applications. Editions
TECHNIP, 2007.
* B. Tomasik, P. Thiha, and D. Turnbull. Tagging products using image classification.
* S. A. Nene, S. K. Nayar, and H. Murase. Columbia object image library (coil-100). Technical Report No.
CUCS-006-96.

# Automatic concept recognition in images

This project aims to implement a strong C++ algorithm for concept recognition in images based on Neural Networks. Moreover, it implements a friendly user interface for testing using Qt library. We also use the well-known OpenCv library for images processing and classification. Our method is a two phases approach. First, we detect and localize a concept (shape) in an image. Then, we recognize and classify the concept with our Neral Networks. However, before starting detection and classification, we need to do some work :

- create a bac of visual words and extract image signatures from the training dataset of images
- train the Neural Networks with the training dataset

## Building of the bac of visual words and image signatures extraction

### Training image dataset and features extraction

First of all, we need to choose a right training dataset for features extraction. Here are some features we have to take into account when choosing a training images dataset :
- the size of the dataset
- the level of detail
- the variety of image viewpoints

Once the training dataset has been selected, we can start features extraction. A good feature should be invariant to geometry (rotation, scalling, affine transformation) and to photometry. There are many methods of extraction implemented by OpenCv, we choose the SIFT method because it has a better performance for some features.

<table width="100">
  <th>
    <td>Method</td> <td>Time</td> <td>Scale</td> <td>Rotation</td> <td>Blur</td> <td>Illumination</td> <td>Affine</td> 
  </th>
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

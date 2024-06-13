___Project idea inspired from [Samson Zhang](https://www.youtube.com/watch?v=w8yWXqWQYmU). Design and implementation are quite different from the one in his [Kaggle Notebook](https://www.kaggle.com/code/wwsalmon/simple-mnist-nn-from-scratch-numpy-no-tf-keras/notebook) that___

- This implementation uses `Numba` to speed up the computations
- Uses OOP to modularize code.
- Uses the real `MNIST` Idx data sets, instead of the Kaggle provided `.csv` files.
- Includes separate classes for handling `Idx1`, `Idx3` IO.
- `NNetworkMinimal` class can save a trained model's state to disk and construct a new model from the serialized model file, this prevents the need to retrain the model, given that the training dataset hasn't been altered since the last training.

___MNIST___

![MNIST](./readme/MnistExamplesModified.png)

___Outline of the `NNetworkMinimal` class training process___

- The NNetworkMinimal class represents a very simple three layer fully connected neural network. The input layer has 784 neurons, each designated to handle one pixel from the training image.    
- The middle hidden layer has 10 neurons, fully connected to the input and output layers.
- The output layer has 10 neurons, each registering the probability of a given image representing the n th digit (the digits can be 0 through 9, hence the 10 neurons).
- The training images are arrays of 28 x 28 black and white pixels (each one byte in size, with 0 mapping to white and 255 mapping to black).

$I_{784 \times N} \Longrightarrow H_{10 \times N} \Longrightarrow O_{10 \times N}$            
$\underbrace{I}_{784 \times N} = |pixels~in~image| \times |images|$

___Phase 1) Forward propagation___


```math
\underbrace{H}_{10 \times N} = \underbrace{W}_{784 \times 10} \cdot \underbrace{I}_{784 \times N} + \underbrace{B}_{10 \times 1} \\
```
```math
\underbrace{\hat{H}}_{10 \times N} = \underbrace{f_{activation}(H)}_{10 \times N} \\
```
```math
\underbrace{\hat{H}}_{10 \times N} = \underbrace{ReLU(H)}_{10 \times N} \\
```
```math
ReLU(x): ~ x ~ if ~ (x > 0) ~ else ~ 0 \\
```
```math
\underbrace{O}_{10 \times N} = \underbrace{w}_{10 \times 10} \cdot \underbrace{\hat{H}}_{10 \times N} + \underbrace{b}_{10 \times 1} \\
```
```math
\underbrace{\hat{O}}_{10 \times N} = \underbrace{f_{softmax}(O)}_{10 \times N} \\
```
```math
softmax = \frac{e^O}{\sum_{j = 1}^{K} {e_{j}}^O} \\
```
```math
softmax(\underbrace{\begin{bmatrix}
0.9 \\
1.7 \\
8.4 \\
\vdots \\
2.1 \\
8.5 \\
9.1 \\
\end{bmatrix}}_{10 \times 1}) =
\underbrace{\begin{bmatrix}
0.10 \\
0.23 \\
0.00 \\
\vdots \\
0.74 \\
0.01 \\
0.01 \\
\end{bmatrix}}_{10 \times 1} \\
```

___Phase 2) Back propagation___

```math
prediction = \underbrace{\begin{bmatrix}
0.01 \\
0.05 \\
0.69 \\
\vdots \\
0.00 \\
0.10 \\
0.07 \\
\end{bmatrix}}_{10 \times 1} \\
```
```math
\text{one hot encoded label} = \underbrace{\begin{bmatrix}
0 \\
0 \\
1 \\
\vdots \\
0 \\
0 \\
0 \\
\end{bmatrix}}_{10 \times 1} \\
```
```math
\underbrace{\mathrm{d}O}_{10 \times N} = \underbrace{\hat{O}}_{10 \times N} - \underbrace{L_{True}}_{10 \times N} \\
```
```math
\mathrm{d}O_i = \begin{bmatrix}
0.01 \\
0.05 \\
0.69 \\
\vdots \\
0.00 \\
0.10 \\
0.07 \\
\end{bmatrix} - \begin{bmatrix}
0 \\
0 \\
1 \\
\vdots \\
0 \\
0 \\
0 \\
\end{bmatrix} \\
```
```math
\underbrace{\mathrm{d}{w}}_{10 \times 10} = \frac{  \overbrace{\mathrm{d}{O}}^{10 \times N} \cdot   \overbrace{\hat{H}^T}^{N \times 10}}{N} \\
```
```math
\underbrace{\mathrm{d}{b}}_{10 \times 1} =  \frac{\sum_{i~=~1}^{N}{\overbrace{\mathrm{d}{O_i}}^{10 \times 1}}}{N} \\
```
```math
\underbrace{\mathrm{d}{H}}_{10 \times N} = \underbrace{w^T}_{10 \times 10} \cdot \underbrace{\mathrm{d}{O}}_{10 \times N} \times \underbrace{f^{\prime}(H)}_{10 \times N} \\
```
```math
\underbrace{\mathrm{d}{W}}_{10 \times 784} = \frac{\overbrace{\mathrm{d}{H}}^{10 \times N} \cdot \overbrace{I^T}^{N \times 784}}{N} \\
```
```math
\underbrace{\mathrm{d}{B}}_{10 \times 1} = \frac{\sum_{i~=~1}^{N}{\overbrace{\mathrm{d}{H_i}}^{10 \times 1}}}{N} \\
```


___Phase 3) Simultaneous paramater updates___

```math
W = W - \alpha \cdot \mathrm{d}{W}
```
```math
B = B - \alpha \cdot \mathrm{d}{B}
```
```math
w = w - \alpha \cdot \mathrm{d}{w}
```
```math
b = b - \alpha \cdot \mathrm{d}{b}
```
$\alpha$ is the learning rate

After 5,000 iterations, the accuracy scores for `MNIST` datasets were:
- Training dataset - 0.935367 (93.54%)
- Test dataset - 0.928800 (92.88%)

For a thorough, step by step walkthrough, refer the source code. It's comprehensively annotated!

___Fashion MNIST___

![Fashion-MNIST](./readme/fashion-mnist-sprite.png)

Using the same `NNetworkMinimal` class used for MNIST datasets with 5000 iterations, the accuracy scores were:
- Training dataset - 0.262367 (26.24%)
- Test dataset - 0.263800 (26.38%)
### ___Structure of the `NNetworkMinimal` class implementation___
---------------------
## $I_{784 \times N} \Longrightarrow H_{10 \times N} \Longrightarrow O_{10 \times N}$
## $\underbrace{I}_{784 \times N} = |pixels~in~image| \times |images|$

<br>
                
### ___Forward propagation___
---------------------
## $\underbrace{H}_{10 \times N} = \underbrace{W}_{784 \times 10} \cdot \underbrace{I}_{784 \times N} + \underbrace{B}_{10 \times 1}$

## $\underbrace{\hat{H}}_{10 \times N} = \underbrace{f_{activation}(H)}_{10 \times N}$

## $\underbrace{\hat{H}}_{10 \times N} = \underbrace{ReLU(H)}_{10 \times N}$

## $ReLU(x): ~ x ~ if ~ (x > 0) ~ else ~ 0$

## $\underbrace{O}_{10 \times N} = \underbrace{w}_{10 \times 10} \cdot \underbrace{\hat{H}}_{10 \times N} + \underbrace{b}_{10 \times 1}$

## $\underbrace{\hat{O}}_{10 \times N} = \underbrace{f_{softmax}(O)}_{10 \times N}$

## $softmax = \frac{e^O}{\sum_{j = 1}^{K} {e_{j}}^O}$

## $softmax(\underbrace{\begin{bmatrix}
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
\end{bmatrix}}_{10 \times 1}$

<br>

### ___Backward propagation___
---------------

## $prediction = \underbrace{\begin{bmatrix}
0.01 \\
0.05 \\
0.69 \\
\vdots \\
0.00 \\
0.10 \\
0.07 \\
\end{bmatrix}}_{10 \times 1}
$

## $\text{one hot encoded label} = \underbrace{\begin{bmatrix}
0 \\
0 \\
1 \\
\vdots \\
0 \\
0 \\
0 \\
\end{bmatrix}}_{10 \times 1}$

# $\underbrace{\mathrm{d}O}_{10 \times N} = \underbrace{\hat{O}}_{10 \times N} - \underbrace{L_{True}}_{10 \times N}$

## $\mathrm{d}O_i = \begin{bmatrix}
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
\end{bmatrix}$

## $\underbrace{\mathrm{d}{w}}_{10 \times 10} = \frac{  \overbrace{\mathrm{d}{O}}^{10 \times N} \cdot   \overbrace{ H^T}^{N \times 10}}{N}$

## $\underbrace{\mathrm{d}{b}}_{10 \times 1} =  \frac{\sum_{i~=~1}^{N}{\overbrace{\mathrm{d}{O_i}}^{10 \times 1}}}{N}$

## $\underbrace{\mathrm{d}{H}}_{10 \times N} = \underbrace{w^T}_{10 \times 10} \cdot \underbrace{\mathrm{d}{O}}_{10 \times N} \times \underbrace{f^{\prime}(H)}_{10 \times N}$

## $\underbrace{\mathrm{d}{W}}_{10 \times 784} = \frac{\overbrace{\mathrm{d}{H}}^{10 \times N} \cdot \overbrace{I^T}^{N \times 784}}{N}$

## $\underbrace{\mathrm{d}{B}}_{10 \times 1} = \frac{\sum_{i~=~1}^{N}{\overbrace{\mathrm{d}{H_i}}^{10 \times 1}}}{N}$

<br>

### ___Paramater updates___
---------------------
## $W = W - \alpha \cdot \mathrm{d}{W}$
## $B = B - \alpha \cdot \mathrm{d}{B}$
## $w = w - \alpha \cdot \mathrm{d}{w}$
## $b = b - \alpha \cdot \mathrm{d}{b}$
### _Project idea inspired from `Samson Zhang`@[YouTube](https://www.youtube.com/watch?v=w8yWXqWQYmU)_

-------------
### _GitHub sucks big time when it comes to rendering Jupyter Notebooks correctly. If interested in the Python implementation, please download the notebook and experiment locally on your machine, because GitHub renders Markdown mathmode and some parts of vanilla Markdown horrendously._
--------------

Implementation is quite different from the one in [his Kaggle Notebook](https://www.kaggle.com/code/wwsalmon/simple-mnist-nn-from-scratch-numpy-no-tf-keras/notebook) that:
- This implementation uses `Numba` to speed up the computations (I doubt this actually boosted the performance)
- Uses OOP to modularize code.
- Uses the real `MNIST` Idx data sets, instead of Kaggle provided `csv`s.
- Includes classes for handling `Idx1`, `Idx3` IO.
-------------

___After 2,500 iterations, the accuracy scores for `MNIST` datasets were:___
- Training dataset - 0.8937 (89.37%)
- Test dataset - 0.8938 (89.38%)

-------------
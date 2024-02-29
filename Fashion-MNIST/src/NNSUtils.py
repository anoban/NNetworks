import numpy as np
from numpy.typing import NDArray

np.seterr(all="raise")
from numba import jit


@jit(nopython=True, fastmath=False, parallel=False)
def ReLU(data: NDArray[np.float64]) -> NDArray[np.float64]:
    """
    Rectified Linear Unit: x if x >= 0 else 0

    Parameters:
    data: NDArray[np.float64] - a matrix of image pixels (normalized, values in the of 0 and 1, in the first iteration)
    that are tobe updated in subsequent iterations. (could breach the above specified range!)

    Returns:
    NDArray[np.float64] - ReLUed pixel values.

    Notes:
    np.maximum returns the elementwise maxima of two arrays.
    If one input is a scalar, then the scalar gets broadcasted to the shape of the array, followed by the determination of elementwise maxima.
    So, this implementation scans through the array, and if the element is greater than or equal to 0, it takes that value else uses 0 to fillers.
    Implemented as a simple wrapper around np.maximum with a default value.
    """

    return np.maximum(data, 0.000)


@jit(nopython=True, fastmath=True, parallel=True)
def LeakyReLU(data: NDArray[np.float64]) -> NDArray[np.float64]:
    """
    Leaky ReLU: x if x >= 0 else 0.01x

    Parameters:
    data: NDArray[np.float64] - a matrix of image pixels (normalized, values in the of 0 and 1, in the first iteration)
    that are tobe updated in subsequent iterations. (could breach the above specified range!)

    Returns:
    NDArray[np.float64] - ReLUed pixel values.

    Notes:
    Addresses the primary drawback of vanilla ReLU, the dying neuron problem, where a neuron with a negative bias will
    never be activated. i.e when the bias is added to the input's curent state, with a negative bias, input values of that neuron's row
    will likely end up with negative values (granted that the bias is larger than the matrix element). This will lead to zeros filling the
    input matrix that is being iteratively processed in forward propagation.

    Instead of zeroing negative matrix elements Leaky ReLU reduces their magnitude, by scaling them down by a factor of 100, thereby
    reducing their chances of
    """

    return np.maximum(data, 0.000)


@jit(nopython=True, fastmath=False, parallel=False)
def softmax(data: NDArray[np.float64]) -> NDArray[np.float64]:
    """
    Softmax(X) =:
        e is exponentiated to the elements of column vector (X), followed by an element-wise
        division by the sum of exponentiated values.

    Parameters:
    data: NDArray[np.float64] - image pixel matrix

    Returns:
    NDArray[np.float64] - appropriateness probabilities for each label, for each image in the data matrix

    Notes:
    Video by Samson Zhang uses .sum() to divide by the total sum, but division by column sums is more appropriate here,
    as we do not need to normalize globally, we just need normalize the exponentiation results for each column (image), so we get a
    valid set of probabilities for each label for each image!
    """

    exp: NDArray[np.float64] = np.exp(data)
    return exp / exp.sum(axis=0)


def onehot(labels: NDArray[np.float64]) -> NDArray[np.float64]:
    """
    Parameters:
    labels: NDArray[np.float64] - true labels of the idx3 image matrices

    Returns:
    NDArray[np.float64] - one hot encoded labels (composed of binary flags)
    e.g. a class label 4, where the possible labels are 0 to 5 will produce an array => [0, 0, 0, 0, 1, 0],
    where only the fifth flag (corresponding to the value 4) is `turned on`.

    Notes:
    Returned array will be of the shape (labels.max() - labels.min() + 1) x labels.size, where each label will be one-hot
    encoded as separate columns. Labels are assumed to comprise only of non-negative 64 bit floats, with minimum always
    at 0.0 (the range is assumed to be 0 to labels.max()).
    """

    labels_range: np.int64 = np.int64(
        labels.max() - labels.min() + 1
    )  # defines the number of elements that should be in a column (number of rows)
    zeromat: NDArray[np.float64] = np.zeros(shape=(labels_range, labels.size), dtype=np.float64)
    zeromat[labels.astype(np.uint64), np.arange(start=0, stop=labels.size, dtype=np.uint64)] = (
        1.0000  # array subscript with two arrays doesn't work with Numba
    )
    return zeromat


@jit(nopython=True, parallel=False, fastmath=True)
def undoReLU(activated_layer: NDArray[np.float64]) -> NDArray[np.float64]:
    """
    Returns the derivative of ReLU activation results.
    Possible results are 0 and 1

    Parameters:
    activated_layer: NDArray[np.float64] - post-activation hidden layer (in the process of back propagation)

    Returns:
    NDArray[np.float64] - activation reversed hidden layer (ReLU undone)

    Notes:
    Considering the ReLU's derivative function, slope is 0 (m = 0) as long as x <= 0
    Slope becomes 1 (m = 1) where x > 1
    If the ReLU result is 0, the original input must have been a negative value, which will give us a slope of 0.
    If the ReLU result is greater than 0, then the original input must have been greater than 0, which will give us a slope of 1.
    Using fastmath as the only possible results are 0s and 1s, accuracy doesn't matter much here.
    """

    return (activated_layer > 1).astype(np.float64)

import numpy as np
from numpy.typing import NDArray
np.seterr(all = "raise")

def ReLU(data: NDArray[np.float64]) -> NDArray[np.float64]:
    """
    Rectified Linear Unit: x if x > 0 else 0
    np.maximum returns the elementwise maxima of two arrays.
    If one input is a scalar, then the scalar gets broadcasted to the shape of the array, followed by the determination of elementwise maxima.
    So, this implementation scans through the array, and if the element is greater than or equal to 0, it takes that value else uses 0 to fillers.
    Implemented as a simple wrapper around np.maximum with a default value.
    """
    return np.maximum(data, 0.000)  

def softmax(data: NDArray[np.float64]) -> NDArray[np.float64]:
    """ 
    Softmax(X) =:
        e is exponentiated to the elements of column vector (X), followed by an element-wise 
        division by the sum of exponentiated values.
    """
    exp: NDArray[np.float64] = np.exp(data) 
    return exp / exp.sum(axis = 0)

def onehot(labels: NDArray[np.float64]) -> NDArray[np.float64]:
    """
    issubclass(labels.dtype.type, np.floating) must evaluate to True, else execution will result in RuntimeError.
    Returned array will be of the shape (labels.max() - labels.min() + 1) x labels.size, where each label will be one-hot encoded as separate columns.
    Labels are assumed to comprise only of non-negative 64 bit floats, with minimum always at 0.0 (the range is assumed to be 0 to labels.max()).
    """
    
    assert issubclass(labels.dtype.type, np.floating)
    
    labels_range: np.int64 = np.int64(labels.max() - labels.min() + 1)  # defines the number of elements that should be in a column (number of rows)
    
    tmp: NDArray[np.float64] = np.zeros(shape = (labels_range, labels.size), dtype = np.float64)  
    tmp[labels.astype(np.uint64), np.arange(start = 0, stop = labels.size, dtype = np.uint64)] = 1.0000  # array subscript with two arrays won't work if Numba is used
    return tmp


def undoReLU(activated_layer: NDArray[np.float64]) -> NDArray[np.float64]:
    """
    Returns the derivative of ReLU activation results.
    Considering the ReLU's derivative function, slope is 0 (m = 0) as long as x <= 0
    Slope becomes 1 (m = 1) where x > 1
    Possible results are 0 and 1
    If the ReLU result is 0, the original input must have been a negative value, which will give us a slope of 0.
    If the ReLU result is greater than 0, then the original input must have been greater than 0, which will give us a slope of 1.
    """
    return (activated_layer > 1).astype(np.float64)
import numpy as np
import warnings
from numpy.typing import NDArray
from sklearn.metrics import accuracy_score
np.seterr(all = "raise")
from NNSUtils import onehot, softmax, ReLU, undoReLU

class NNetworkExtended:
    """
    A class representing an extended variant of NNetworkMinimal, with one input layer, one output layer and customizeable number of hidden layers
    """

    def __init__(self, nodes_in: int = 784, nlayers_hidden: int = 5, nodes_hid: list[int] = [784, 588, 392, 286, 196], nodes_out: int = 10, alpha: float = 0.1, maxiterations: int = 2500) -> None:
        """
        Parameters:
        nodes_in: np.uint64 - number of nodes in the input layer
        nlayers_hidden: int - number of hidden layers to use in the network
        nodes_hid: list[int] - number of nodes to include in each hidden layer
        nodes_out: np.uint64 - number of nodes in the output layer
        alpha: float - learning rate, default value is 0.001
        maxiterations: int - the maximum number of iteration gradient descent is allowed to make
        
        Returns:
        None
        """
        
        assert(nlayers_hidden == len(nodes_hid))   # make sure each hidden layer has a node count specified.
        if max(nodes_hid) >= 1000:
            warnings.warn("Using large number of nodes in hidden layers can severely handicap performance", RuntimeWarning)
        
        self.__is_trained: bool = False
        self.__learning_rate: float = alpha
        self.__maxiter: int = maxiterations
        # this stores the number of nodes in all layers of the NNetworkExtended object, including the input and output layers.
        self.__nnodes: tuple[int, ...] = tuple([nodes_in] + nodes_hid + [nodes_out]) 
        
        # if there are N layers in total (counting the input and output layers), there'll be N - 1 group of connections.
        # we needs weights for these N - 1 group of connections and biases for N layers of neurons.
        
        # store the shapes of weight matrices, deliberately using tuples to make the shapes immutable.
        self.__wshapes: tuple[tuple[int, int], ...] = tuple(zip(self.__nnodes[1:], self.__nnodes[:len(self.__nnodes)-1]))
        assert(len(self.__wshapes) == len(self.__nnodes) - 1)
        
        # store the weights of connections in a list (including the connections that directly involve the input and output layers)
        self.__weights: list[NDArray[np.float64]] = [np.random.rand(r, c) - 0.5 for (r, c) in self.__wshapes]
        
        # store the biases of nodes in all layers of the NNetworkExtended object. (excluding the input layer)
        # baises are only considered for layers following the input layer.
        # biases need to be column vectors
        self.__biases: list[NDArray[np.float64]] = [np.random.rand(n, 1) - 0.5 for n in self.__nnodes[1:]]
        assert(len(self.__biases) == len(self.__nnodes) - 1)    # leaving the input layer out.
        

    def __repr__(self) -> str:
        # return f"Untrained NNetworkMinimal model object <I>: {self.__nodes_in}, <H>: {self.__nodes_hid}, <O>: {self.__nodes_out}" if not self.__is_trained else f"Trained NNetworkMinimal model object <I>: {self.__nodes_in}, <H>: {self.__nodes_hid}, <O>: {self.__nodes_hid}"
        pass
    
    def gradient_descent(self, data: NDArray[np.float64], labels: NDArray[np.float64]) -> None:
        """
        A rather complex routine that does the forward propagation and back propagation iteratively.
        Doesn't return the coefficients, but realizes the learning effects by altering the internal state of the `NNetworkMinimal` class instance.
        
        Parameters:
        data: np.NDArray[np.float64] - training dataset
        labels: np.NDArray[np.float64] - training labels
        
        Returns:
        None
        
        Notes:
        Comments below inside are MNIST tailored
        """
        if self.__is_trained:   # if the model has already been trained, raise an error.
            raise NotImplementedError("class <NNetworkMinimal> doesn't allow retraining!")
        
        # a simple data /= 255.00 could be used here.
        # this will modify the array in-place (not a local copy, but the referenced original array), if we ask the model to predict the labels for
        # training data, predict() method will again scale this data down (assuming the same array gets passed in again) and we'll ultimately have
        # garbage as predictions! i.e. we'll end up with a pixels array where each pixel have been scaled down by (255 * 255)
        # this normalization is needed to avoid FloatingPointError s in np.exp() (inside softmax())
        data_normed: NDArray[np.float64] = data / 255.00  
         
        ##########  Storing the onehot encoded labels, hidden and output layers, their post-activation state, the differences computed in
        # CAVEAT #  back propagation as instance attributes makes the model object gratuitously bloated!. If the model objects is to stay
        ##########  in memory for long, consider storing these as temporaries scoped to the gradient_descent() method.
        
        # one-hot encode the true labels
        self.__onehot_true_labels: NDArray[np.float64] = onehot(labels = labels)
        
        # Warning:: Following annotations assume that the inputs have the same structure as MNIST datasets.
        for i in range(self.__maxiter):
            if not (i % 200):
                print(f"Iteration: {i:4d}")
            
            #######################
            # FORWARD PROPAGATION #
            #######################
                
            # computing H 
            #    10 x N                         10 x 784  784 x N     10 x 1
            self.__H: NDArray[np.float64] = self.__winhid.dot(data_normed) + self.__bhid
                
            # activating H layer -> H_hat
            #    10 x N                                  10 x N
            self.__H_hat: NDArray[np.float64] = ReLU(self.__H)
                
            # then, we compute the output layer
            #    10 x N                          10 x 10       10 x N         10 x 1
            self.__O: NDArray[np.float64] = self.__whidout.dot(self.__H_hat) + self.__bout
            
            # activation of O layer
            #    10 x N                                      10 x N
            # print(self.__O.mean())
            self.__O_hat: NDArray[np.float64] = softmax(self.__O)      # WARNING :: NAN HOSTSPOT (if the pixel data is not normalized)
            
            ####################
            # BACK PROPAGATION #
            ####################
            
            # compute the difference between the outputs and the one-hot encoded true labels
            #    10 x N                          10 x N         10 x N
            self.__dO: NDArray[np.float64] = self.__O_hat - self.__onehot_true_labels
                
            # see how much the weights of connections between the output and hidden layer contributed to this difference (dO)
            #    10 x 10                          10 x N        N x 10
            self.__dw: NDArray[np.float64] = self.__dO.dot(self.__H_hat.T) / labels.size
                
            # how much the biases of nodes in the output layer contributed to this difference (dO)
            #    10 x 1                           10 x N
            self.__db: NDArray[np.float64] = (self.__dO.sum(axis = 1) / labels.size).reshape(10, 1) 
            # array.sum(axis = 1) gives row sums as a 1 x 10 row vector
            # we need a 10 x 1 column vector as the result, hence the reshaping
                
            # now we move on to transformation from the hidden layer to the input layer
            # first the ReLU activation needs to be undone
            #    10 x N                           10 x 10        10 x N
            self.__dH: NDArray[np.float64] = self.__whidout.T.dot(self.__dO) * undoReLU(self.__H)

            # then compute how much the weights of the connexions between the input and hidden layers contributed to this.
            #    10 x 784                         10 x N    N x 784
            self.__dW: NDArray[np.float64] = self.__dH.dot(data_normed.T) / labels.size

            # compute how much the biases of nodes in the hidden layer contributed to this.
            #    10 x 1                           10 x 1       
            self.__dB: NDArray[np.float64] = self.__dH.sum(axis = 1).reshape(10, 1) / labels.size
            
            #####################    
            # PARAMETER UPDATES #
            #####################
            
            self.__winhid -= (self.__dW * self.__learning_rate)
            self.__bhid -= (self.__dB * self.__learning_rate)
            self.__whidout -= (self.__dw * self.__learning_rate)
            self.__bout -= (self.__db * self.__learning_rate)
            
        # register that the model instance has been trained.
        self.__is_trained = True

        # perfect, none of the matrices are out of shape :)
        # print(f"W: {self.__W.shape}, B: {self.__B.shape}, w: {self.__w.shape}, b: {self.__b.shape}")
        # print(f"H: {self.__H.shape}, H_hat: {self.__H_hat.shape}, O: {self.__O.shape}, O_hat: {self.__O_hat.shape}")
        # print(f"dO: {self.__dO.shape}, dw: {self.__dw.shape}, db: {self.__db.shape}, dH: {self.__dH.shape}, dW: {self.__dW.shape}, dB: {self.__dB.shape}")
    
    def predict(self, data: NDArray[np.float64]) -> NDArray[np.int64]:
        """
        Does a forward propagation with the input data, using learned weights and biases and returns the predictions.
        Calling this method on an untrained model will raise NotImplementedError.
                
        Parameters:
        data: NDArray[np.float64] - a matrix of image pixels to make predictions on
        
        Returns:
        NDArray[np.int64] - predictions
        
        Notes:
        The input data will remain untouched, as the method works only with a local normalized copy of it.
        """
        if not self.__is_trained:
            raise NotImplementedError("Untrained <NNetworkMinimal> models cannot make predictions!")
        
        data_normed: NDArray[np.float64] = data / 255.00      # get a normalized copy of the incoming data
        # then we repeat the steps in forward propagation with learned weights and biases, to finally make the prediction
        H: NDArray[np.float64] = self.__winhid.dot(data_normed) + self.__bhid
        H_hat: NDArray[np.float64] = ReLU(H)
        O: NDArray[np.float64] = self.__whidout.dot(H_hat) + self.__bout
        O_hat: NDArray[np.float64] = softmax(O)
        return np.argmax(O_hat, axis = 0)   # O_hat is 10 x N shaped. the offset of the max value in each column will be the model's prediction
    
    def accuracy_score(self, data: NDArray[np.float64] , true_labels: NDArray[np.float64]) -> float:
        """
        Does a simple forward propagation with the passed data, using the current state of the model, then the predictions are 
        compared with the true labels.
        
        Parameters:
        data: NDArray[np.float64] - 
        true_labels: NDArray[np.float64] - 
        
        Returns:
        float - fraction of predictions that were correct. (in the range of 0 to 1)
        
        Notes:
        Accuracy is computed based on raw idenitity checks against true labels and predictions. using sklearn.metrics.accuracy_score()
        This mechanism disregards how close the predictions are to the true labels!
        """
        if not self.__is_trained:
            raise NotImplementedError("Untrained <NNetworkMinimal> models cannot make predictions!")
        
        data_normed: NDArray[np.float64] = data / 255.00      # get a normalized copy of the incoming data
        # then we repeat the steps in forward propagation with learned weights and biases, to finally make the prediction
        H: NDArray[np.float64] = self.__winhid.dot(data_normed) + self.__bhid
        H_hat: NDArray[np.float64] = ReLU(H)
        O: NDArray[np.float64] = self.__whidout.dot(H_hat) + self.__bout
        O_hat: NDArray[np.float64] = softmax(O)
        predictions: NDArray[np.float64] = np.argmax(O_hat, axis = 0)   # O_hat is 10 x N shaped. the offset of the max value in each column will be the model's prediction
        return accuracy_score(true_labels, predictions, normalize = True)

        
    def coefficients(self) -> dict[str, NDArray[np.float64]]:
        """
        Returns the weights and biases as a dictionary.
        
        Parameters:
        None
        
        Returns:
        
        """
        pass
    
    def save(self, filepath: str) -> None:
        """
        Serializes the model to disk, preserving the current state of the model (weights and biases).
        
        Parameters:
        filepath: str - the path to serialize the <NNetworkMinimal> model object. 
        
        Returns:
        None
        
        Notes:
        DO NOT SPECIFY AN EXTENSION with the filepath.
        .save() internally uses `.nnm` extension to serialize <NNetworkMinimal> model objects. This extension is leveraged to validate inputs
        to .load() method. Internally a `.nnm` file is a Numpy `.npy` file.
        """
        # self.__W, self.__B, self.__w, self.__b
        # first 64 bytes (8 64 bit floats) are the dimensions of the weights and biases, in the above mentioned order.
        # dimensions are stored row numbers first.
        coeffs: NDArray[np.float64] = np.concatenate([self.__winhid.shape, self.__bhid.shape, self.__whidout.shape, self.__bout.shape,
                                                       self.__winhid.flatten(), self.__bhid.flatten(), self.__whidout.flatten(), self.__bout.flatten()], dtype = np.float64)
        with open(file = f"{filepath}.nnm", mode =  "wb") as fp:
            np.save(fp, coeffs, allow_pickle = False)
        
        
    def load(self, filepath: str) -> None:
        """
        Loads in a serialized model from disk and creates a NNetworkMinimal object, 
        restoring the state of the saved model. This method doesn't return anything but realizes the effects by altering the weights
        and biases using the data stored in the .nnm file. Convenient in reusing models for new, similarly structured test data.
        The stored model is expected to have a `.nnm` extension. Else, a exception will be raised.
        
        Parameters:
        filepath: str - path to the serialized model object, including the extension (expected `.nnm`).
        
        Returns:
        None
        """
        if not filepath.endswith(".nnm"):
            TypeError("Only models serialized with .save() method with an .nnm extension are supported!")
        
        with open(file = filepath, mode = "rb") as fp:
            coeffs: NDArray[np.float64] = np.load(fp)   # np.load() is used to load in binary files serialized with np.save() method.
        # np.fromfile() assumes that the binary file contains only raw bytes. But np.save() encodes metadata in the .npy file format.
        # .npy format is the file format used by np.save() to serialize Numpy array objects.
        caret: int = 0  # use this as a moving caret that points to the offset where the next subscriting should begin.
        # extract the dimensions of the weights and biases.
        
        # prefixes W, B are for the hidden layer & prefixes w, b are for the output layer.
        Wrows, Wcols, Brows, Bcols, wrows, wcols, brows, bcols = coeffs[:8].astype(np.uint64)
        print(coeffs[:8].astype(np.uint64))
        
        caret += 8  
        self.__winhid = coeffs[caret:(caret + Wrows * Wcols)].reshape(Wrows, Wcols)
        caret += (Wrows * Wcols)
        self.__bhid = coeffs[caret: caret + Brows * Bcols].reshape(Brows, Bcols)
        caret += (Brows * Bcols)
        self.__whidout = coeffs[caret: caret + wrows * wcols].reshape(wrows, wcols)
        caret += (wrows * wcols)
        self.__bout = coeffs[caret: caret + brows * bcols].reshape(brows, bcols)
        
        # mark the model as trained.
        self.__is_trained = True
        del coeffs
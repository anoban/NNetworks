import numpy as np
from numpy.typing import NDArray
np.seterr(all = "raise")
from NNSUtils import onehot, softmax, ReLU, undoReLU

class NNetworkMinimal:
    """
    A class representing a bare minimum neural network with one input layer, one hidden layer and an output layer
    """

    
    def __init__(self, nodes_in: int, nodes_hid: int, nodes_out: int, alpha: float = 0.1, maxiterations: int = 500) -> None:
        """
        The constructor that takes in arguments for:
        nodes_in: np.uint64 - number of nodes in the input layer
        nodes_hid: np.uint64 - number of nodes in the hidden layer
        nodes_out: np.uint64 - number of nodes in the output layer
        alpha: float - learning rate, default value is 0.001
        """
        
        self.__is_trained: bool = False
        self.__learning_rate: float = alpha
        self.__maxiter: int = maxiterations
        self.__nodes_in: int = nodes_in
        self.__nodes_hid: int = nodes_hid
        self.__nodes_out: int = nodes_out
        
        self.__W: NDArray[np.float64] = np.random.rand(nodes_hid, nodes_in) - 0.5    # 10 x 784 matrix for MNIST
        self.__B: NDArray[np.float64] = np.random.rand(nodes_hid, 1) - 0.5           # 10 x 1 column vector for MNIST
        self.__w: NDArray[np.float64] = np.random.rand(nodes_hid, nodes_out) - 0.5   # 10 x 10 matrix for MNIST
        self.__b: NDArray[np.float64] = np.random.rand(nodes_out, 1) - 0.5           # 10 x 1 column vector for MNIST

    
    def __repr__(self) -> str:
        return f"Untrained NNetworkMinimal model object <I>: {self.__nodes_in}, <H>: {self.__nodes_hid}, <O>: {self.__nodes_hid}" if not self.__is_trained else f"Trained NNetworkMinimal model object <I>: {self.__nodes_in}, <H>: {self.__nodes_hid}, <O>: {self.__nodes_hid}"

    
    def gradient_descent(self, data: NDArray[np.float64], labels: NDArray[np.float64]) -> None:
        """
        A rather complex routine that does the forward propagation and back propagation iteratively.
        Input arguments are:
        data: np.NDArray[np.float64] - training dataset
        labels: np.NDArray[np.float64] - training labels

        This function returns None but realizes the inferences by altering the internal state of the `NNetworkMinimal` class instance.
        i.e updates the weights and biases internally

        TODO: consider creating jitted class independent implementations (free functions) that can be wrapped by class methods.
        Would boost the perofrmance exponentially.

        (comments inside are MNIST tailored)
        """
        if self.__is_trained:
            print(self)
            raise PermissionError("class <NNetworkMinimal> doesn't allow retraining!")
            
        # one-hot encode the true labels
        self.__onehot_true_labels: NDArray[np.float64] = onehot(labels = labels)
        
        # Following annotations assume that the inputs have the same structure as MNIST datasets.

        for i in range(self.__maxiter):
            if not (i % 50):
                print(f"Iteration: {i:4d}")
            
            # FORWARD PROPAGATION
            # computing H 
            #    10 x N                         10 x 784  784 x N     10 x 1
            self.__H: NDArray[np.float64] = self.__W.dot(data) + self.__B    # implicit type promotion from uint8 to float64
                
            # activating H layer -> H_hat
            #    10 x N                                  10 x N
            self.__H_hat: NDArray[np.float64] = ReLU(self.__H)
                
            # then, we compute the output layer
            #    10 x N                          10 x 10       10 x N         10 x 1
            self.__O: NDArray[np.float64] = self.__w.dot(self.__H_hat) + self.__b
            
            # activation of O layer
            #    10 x N                                      10 x N
            # print(self.__O.mean())
            self.__O_hat: NDArray[np.float64] = softmax(self.__O)      # WARNING :: NAN HOSTSPOT
    
            # BACK PROPAGATION
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
            self.__dH: NDArray[np.float64] = self.__w.T.dot(self.__dO) * undoReLU(self.__H)

            # then compute how much the weights of the connexions between the input and hidden layers contributed to this.
            #    10 x 784                         10 x N    N x 784
            self.__dW: NDArray[np.float64] = self.__dH.dot(data.T) / labels.size

            # compute how much the biases of nodes in the hidden layer contributed to this.
            #    10 x 1                           10 x 1       
            self.__dB: NDArray[np.float64] = self.__dH.sum(axis = 1).reshape(10, 1) / labels.size
                
            # PARAMETER UPDATES
            self.__W -= (self.__dW * self.__learning_rate)
            self.__B -= (self.__dB * self.__learning_rate)
            self.__w -= (self.__dw * self.__learning_rate)
            self.__b -= (self.__db * self.__learning_rate)
            
        # register that the model instance has been trained.
        self.__is_trained = True

        # perfect, none of the matrices are out of shape :)
        # print(f"W: {self.__W.shape}, B: {self.__B.shape}, w: {self.__w.shape}, b: {self.__b.shape}")
        # print(f"H: {self.__H.shape}, H_hat: {self.__H_hat.shape}, O: {self.__O.shape}, O_hat: {self.__O_hat.shape}")
        # print(f"dO: {self.__dO.shape}, dw: {self.__dw.shape}, db: {self.__db.shape}, dH: {self.__dH.shape}, dW: {self.__dW.shape}, dB: {self.__dB.shape}")
    
    def predict(self, data: NDArray[np.uint8]) -> NDArray[np.int64]:
        """
        
        """
        H: NDArray[np.float64] = self.__W.dot(data) + self.__B
        H_hat: NDArray[np.float64] = ReLU(H)
        O: NDArray[np.float64] = self.__w.dot(H_hat) + self.__b
        O_hat: NDArray[np.float64] = softmax(O)
        return np.argmax(O_hat, axis = 0)
    
    def trainset_accuracy() -> np.float64:
        """
        """
        pass
    
    def coefs() -> None:
        """
        
        """
        pass
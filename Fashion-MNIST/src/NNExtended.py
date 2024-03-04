import numpy as np
from numpy.typing import NDArray
from sklearn.metrics import accuracy_score

np.seterr(all="raise")
from NNSUtils import onehot, softmax, ReLU, undoReLU


class NNetworkExtended:
    """
    A class representing an extended variant of <<NNetworkMinimal>>, with one input layer, one output layer and customizeable number of hidden layers
    """

    def __init__(
        self,
        nodes_in: int = 784,
        nlayers_hidden: int = 5,
        nodes_hid: list[int] = [784, 588, 392, 286, 196],
        nodes_out: int = 10,
        alpha: float = 0.1,
        maxiterations: int = 5000,
    ) -> None:
        """
        `Parameters`:
        nodes_in: np.uint64 - number of nodes in the input layer
        nlayers_hidden: int - number of hidden layers to use in the network
        nodes_hid: list[int] - number of nodes to include in each hidden layer
        nodes_out: np.uint64 - number of nodes in the output layer
        alpha: float - learning rate, default value is 0.001
        maxiterations: int - the maximum number of iteration gradient descent is allowed to make

        `Returns`:
        None
        """

        assert nlayers_hidden == len(
            nodes_hid
        ), f"Each hidden layer needs a node count specified. Number of hidden layers:<{nlayers_hidden}>, Number of node counts:<{len(nodes_hid)}>"

        if max(nodes_hid) >= 1000:
            # don't fancy monkey patching warnings.warn() here (would have to)
            print(
                "RuntimeWarning: Using large number of nodes in hidden layers in <<NNetworkExtended>> models can severely handicap the performance!",
            )

        self.__is_trained: bool = False
        self.__learning_rate: float = alpha
        self.__maxiter: int = maxiterations
        # this stores the number of nodes in all layers of the NNetworkExtended object, including the input and output layers.
        self.__nnodes: tuple[int, ...] = tuple([nodes_in] + nodes_hid + [nodes_out])
        self.__nlayers: int = len(nodes_hid) + 2

        # if there are N layers in total (counting the input and output layers), there'll be N - 1 group of connections.
        # we needs weights for these N - 1 group of connections and biases for N - 1 layers of neurons (discounting the input layer).

        # store the shapes of weight matrices, deliberately using tuples to make them immutable.
        self.__wshapes: tuple[tuple[int, int], ...] = tuple(zip(self.__nnodes[1:], self.__nnodes[:-1]))
        assert len(self.__wshapes) == len(self.__nnodes) - 1

        # store the weights of connections in a list (including the connections that directly involve the input and output layers)
        self.__weights: list[NDArray[np.float64]] = [np.random.rand(r, c) - 0.5 for (r, c) in self.__wshapes]

        # store the biases of nodes in all layers of the NNetworkExtended object. (excluding the input layer)
        # baises are only considered for layers following the input layer (they need to be column vectors).
        self.__biases: list[NDArray[np.float64]] = [np.random.rand(n, 1) - 0.5 for n in self.__nnodes[1:]]
        assert len(self.__biases) == self.__nlayers - 1  # leaving the input layer out.

    def __repr__(self) -> str:
        return (
            f"Untrained <<NNetworkExtended>> model object <I>: {self.__nnodes[0]}, <H: {self.__nlayers:2d}>: {self.__nnodes[1:-1]}, <O>: {self.__nnodes[-1]}"
            if not self.__is_trained
            else f"Trained <<NNetworkExtended>> model object <I>: {self.__nnodes[0]}, <H: {self.__nlayers:2d}>: {self.__nnodes[1:-1]}, <O>: {self.__nnodes[-1]}"
        )

    def gradient_descent(self, train_data: NDArray[np.float64], train_labels: NDArray[np.float64]) -> None:
        """
        A rather complex routine that does the forward propagation and back propagation iteratively.
        Doesn't return the coefficients, but realizes the learning effects by altering the internal state of the `<<NNetworkExtended>>` class instance.

        `Parameters`:
        data: np.NDArray[np.float64] - training dataset
        labels: np.NDArray[np.float64] - training (true) labels

        `Returns`:
        None

        `Notes`:
        Comments below inside are MNIST tailored
        """

        if self.__is_trained:  # if the model has already been trained, raise an error.
            raise NotImplementedError("class <<NNetworkExtended>> doesn't allow retraining!")

        # a simple data /= 255.00 could be used here.
        # this will modify the array in-place (not a local copy, but the referenced original array), if we ask the model to predict the labels for
        # training data, predict() method will again scale this data down (assuming the same array gets passed in again) and we'll ultimately have
        # garbage as predictions! i.e. we'll end up with a pixels array where each pixel have been scaled down by (255 * 255)
        # this normalization is needed to avoid FloatingPointError s in np.exp() (inside softmax())
        data_normed: NDArray[np.float64] = train_data / 255.00

        # one-hot encode the true labels
        onehot_true_labels: NDArray[np.float64] = onehot(labels=train_labels)

        # a list that holds the states of input data, before activation (in every iteration of gradient descent)
        # except the input layer
        pre_activation_layers: list[NDArray[np.float64]] = []

        # a list to store the states of the input data after being processed by each neural layer. (in every iteration of gradient descent)
        # except the input layer
        post_activation_layers: list[NDArray[np.float64]] = []

        # Warning:: Following annotations assume that the inputs have the same structure as MNIST datasets.
        for i in range(self.__maxiter):
            print(f"Iteration: {i:5d}", end="")

            #######################
            # FORWARD PROPAGATION #
            #######################

            for j in range(self.__nlayers - 1):  # loop over |layers| - 1 times
                # self.__weights[0] will be the weights between the input and the first hidden layer
                # self.__biases[0] will be the biases of the nodes in the first hidden layer.

                if not j:  # for the raw pixel data (j == 0)
                    pre_activation_layers.append(self.__weights[0].dot(data_normed) + self.__biases[0])
                    post_activation_layers.append(ReLU(pre_activation_layers[0]))
                    continue

                # otherwise, create the next unactivated matrix
                pre_activation_layers.append(self.__weights[j].dot(post_activation_layers[-1]) + self.__biases[j])

                # for the activation of the newly created matrix
                if j == (self.__nlayers - 2):  # for the last layer of connections, use the softmax activation.
                    post_activation_layers.append(
                        softmax(pre_activation_layers[-1])
                    )  # this will be our final class probabilities for all images in the train data.
                else:  # for other layers of connections, use ReLU (or perhaps Leaky ReLU??)
                    post_activation_layers.append(ReLU(pre_activation_layers[-1]))

            ########################################
            # BACK PROPAGATION & PARAMETER UPDATES #
            ########################################

            # compute the difference between the outputs and the one-hot encoded true labels
            delta_preds: NDArray[np.float64] = post_activation_layers[-1] - onehot_true_labels

            # total number of images whose pixels and labels are provided in the training data
            NIMAGES: int = train_labels.size

            for r in range(self.__nlayers, 0, -1):  # r for reverse
                delta_w: NDArray[np.float64] = delta_preds.dot(preds.T) / NIMAGES
                delta_b: NDArray[np.float64] = delta_preds.sum(axis=0) / NIMAGES

            # see how much the weights of connections between the output and hidden layer contributed to this difference (dO)
            #    10 x 10                          10 x N        N x 10
            self.__dw: NDArray[np.float64] = self.__dO.dot(self.__H_hat.T) / train_labels.size

            # how much the biases of nodes in the output layer contributed to this difference (dO)
            #    10 x 1                           10 x N
            self.__db: NDArray[np.float64] = (self.__dO.sum(axis=1) / train_labels.size).reshape(10, 1)
            # array.sum(axis = 1) gives row sums as a 1 x 10 row vector
            # we need a 10 x 1 column vector as the result, hence the reshaping

            # now we move on to transformation from the hidden layer to the input layer
            # first the ReLU activation needs to be undone
            #    10 x N                           10 x 10        10 x N
            self.__dH: NDArray[np.float64] = self.__whidout.T.dot(self.__dO) * undoReLU(self.__H)

            # then compute how much the weights of the connexions between the input and hidden layers contributed to this.
            #    10 x 784                         10 x N    N x 784
            self.__dW: NDArray[np.float64] = self.__dH.dot(data_normed.T) / train_labels.size

            # compute how much the biases of nodes in the hidden layer contributed to this.
            #    10 x 1                           10 x 1
            self.__dB: NDArray[np.float64] = self.__dH.sum(axis=1).reshape(10, 1) / train_labels.size

            print("\r\r\r\r\r", end="")

        # register that the model instance has been trained.
        self.__is_trained = True
        print("")

    def predict(self, data: NDArray[np.float64]) -> NDArray[np.int64]:
        """
        Does a forward propagation with the input data, using learned weights and biases and returns the predictions.
        Calling this method on an untrained model will raise NotImplementedError.

        `Parameters`:
        data: NDArray[np.float64] - a matrix of image pixels to make predictions on

        `Returns`:
        NDArray[np.int64] - predictions

        `Notes`:
        The input data will remain untouched, as the method works only with a local normalized copy of it.
        """

        if not self.__is_trained:
            raise NotImplementedError("Untrained <<NNetworkExtended>> models cannot make predictions!")

        data_normed: NDArray[np.float64] = data / 255.00  # get a normalized copy of the incoming data
        # then we repeat the steps in forward propagation with learned weights and biases, to finally make the prediction

        return np.argmax(
            O_hat, axis=0
        )  # O_hat is 10 x N shaped. the offset of the max value in each column will be the model's prediction

    def accuracy_score(self, data: NDArray[np.float64], true_labels: NDArray[np.float64]) -> float:
        """
        Does a simple forward propagation with the passed data, using the current state of the model, then the predictions are
        compared with the true labels.

        `Parameters`:
        data: NDArray[np.float64] -
        true_labels: NDArray[np.float64] -

        `Returns`:
        float - fraction of predictions that were correct. (in the range of 0 to 1)

        `Notes`:
        Accuracy is computed based on raw idenitity checks against true labels and predictions. using sklearn.metrics.accuracy_score()
        This mechanism disregards how close the predictions are to the true labels!
        """

        if not self.__is_trained:
            raise NotImplementedError("Untrained <<NNetworkExtended>> models cannot make predictions!")

        data_normed: NDArray[np.float64] = data / 255.00  # get a normalized copy of the incoming data

        # O_hat is 10 x N shaped. the offset of the max value in each column will be the model's prediction
        return accuracy_score(true_labels, predictions, normalize=True)

    def coefficients(self) -> dict[str, NDArray[np.float64]]:
        """
        Returns the weights and biases as a dictionary.

        `Parameters`:
        None

        `Returns`:

        """

        pass

    def save(self, filepath: str) -> None:
        """
        Serializes the model to disk, preserving the current state of the model (weights and biases).

        `Parameters`:
        filepath: str - the path to serialize the <<NNetworkExtended>> model object.

        `Returns`:
        None

        `Notes`:
        DO NOT SPECIFY AN EXTENSION with the filepath.
        .save() internally uses `.nnmext` extension to serialize <<NNetworkExtended>> model objects. This extension is leveraged to validate inputs
        to .load() method. Internally a `.nnm` file is a Numpy `.npy` file.
        """

        # first 16N bytes (2N 64 bit floats) store the dimensions of the weights and biases, in the above mentioned order. (weights and biases)
        # where N denotes the number of connection interfaces with weights and the number of node layers with biases.
        # dimensions are stored row numbers first. (R, C)
        coeffs: NDArray[np.float64] = np.concatenate(
            [],
            dtype=np.float64,
        )
        with open(file=f"{filepath}.nnmext", mode="wb") as fp:
            np.save(fp, coeffs, allow_pickle=False)

    def load(self, filepath: str) -> None:
        """
        Loads in a serialized <<NNetworkExtended>> model from disk and creates a NNetworkExtended object,
        restoring the state of the saved model. This method doesn't return anything but realizes the effects by altering the weights
        and biases using the data stored in the .nnm file. Convenient in reusing models for new, similarly structured test data.
        The stored model is expected to have a `.nnmext` extension. Else, a exception will be raised.

        `Parameters`:
        filepath: str - path to the serialized model object, including the extension (expected `.nnmext`).

        `Returns`:
        None
        """

        if not filepath.endswith(".nnmext"):
            raise TypeError(
                "Only <<NNetworkExtended>> models serialized with .save() method with an .nnmext extension are supported!"
            )

        with open(file=filepath, mode="rb") as fp:
            coeffs: NDArray[np.float64] = np.load(
                fp
            )  # np.load() is used to load in binary files serialized with np.save() method.
        # np.fromfile() assumes that the binary file contains only raw bytes. But np.save() encodes metadata in the .npy file format.
        # .npy format is the file format used by np.save() to serialize Numpy array objects.
        caret: int = 0  # use this as a moving caret that points to the offset where the next subscriting should begin.
        # extract the dimensions of the weights and biases.

        # prefixes W, B are for the hidden layer & prefixes w, b are for the output layer.
        Wrows, Wcols, Brows, Bcols, wrows, wcols, brows, bcols = coeffs[:8].astype(np.uint64)
        print(coeffs[:8].astype(np.uint64))

        # mark the model as trained.
        self.__is_trained = True
        del coeffs

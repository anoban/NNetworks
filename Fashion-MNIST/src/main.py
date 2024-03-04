from NNExtended import NNetworkExtended
from IdxUtils import Idx1, Idx3


def main() -> None:

    # Fashion-MNIST training data
    train_images = Idx3(r"../idx/train-images-idx3-ubyte")
    train_labels = Idx1(r"../idx/train-labels-idx1-ubyte")

    # Fashion-MNIST test data
    test_images = Idx3(r"../idx/t10k-images-idx3-ubyte")
    test_labels = Idx1(r"../idx/t10k-labels-idx1-ubyte")

    model = NNetworkExtended(
        nodes_in=784, nodes_hid=[784, 588, 392, 286, 196], nodes_out=10, maxiterations=2500, alpha=0.125
    )
    model.gradient_descent(train_images.data, train_labels.data)
    # model.accuracy_score(test_images.data, test_labels.data)


if __name__ == "__main__":
    main()

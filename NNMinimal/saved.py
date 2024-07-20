import sys
from typing import NamedTuple
from NNetwork import NNetworkMinimal
from IdxUtils import Idx1, Idx3

class AccuracyScores(NamedTuple):
    train: float
    test: float


def evalualte_nnminimal_for_mnists(model_path: str) -> AccuracyScores:
    """
    Evaluate the accuracy of the NNetworkMinimal model, reconstructed from a serialized .nnm file.

    `Parameters`:
    filepath: str - path to a serialized NNetworkMinimal model object.

    `Returns`:
    None
    """

    model = NNetworkMinimal(nodes_in=1, nodes_hid=1, nodes_out=1, alpha=0.15, maxiterations=1)
    model.load(filepath=model_path)

    mnist_train_images = Idx3(r"../MNIST/train-images.idx3-ubyte")
    mnist_test_images = Idx3(r"../MNIST/t10k-images.idx3-ubyte")
    mnist_train_labels = Idx1(r"../MNIST/train-labels.idx1-ubyte")
    mnist_test_labels = Idx1(r"../MNIST/t10k-labels.idx1-ubyte")

    return AccuracyScores(model.accuracy_score(mnist_train_images.data, mnist_train_labels.data),model.accuracy_score(mnist_test_images.data, mnist_test_labels.data))


def evalualte_nnminimal_for_fashion_mnists(model_path: str) -> AccuracyScores:
    """
    Evaluate the accuracy of the NNetworkMinimal model, reconstructed from a serialized .nnm file.

    `Parameters`:
    filepath: str - path to a serialized NNetworkMinimal model object.

    `Returns`:
    None
    """

    model = NNetworkMinimal(nodes_in=1, nodes_hid=1, nodes_out=1)
    model.load(filepath=model_path)

    fashion_mnist_train_images = Idx3(r"../Fashion-MNIST/train-images-idx3-ubyte")
    fashion_mnist_test_images = Idx3(r"../Fashion-MNIST/t10k-images-idx3-ubyte")
    fashion_mnist_train_labels = Idx1(r"../Fashion-MNIST/train-labels-idx1-ubyte")
    fashion_mnist_test_labels = Idx1(r"../Fashion-MNIST/t10k-labels-idx1-ubyte")

    return AccuracyScores(model.accuracy_score(fashion_mnist_train_images.data, fashion_mnist_train_labels.data),model.accuracy_score(fashion_mnist_test_images.data, fashion_mnist_test_labels.data))


def main() -> None:

    print(
        f"[[MNIST]] :: testset accuracy score = {:.5f}"
    )
    print(
        f"[[MNIST]] :: testset accuracy score = {:.5f}"
    )

    print(
        f"[[Fashion-MNIST]] :: trainset accuracy score = {:.5f}"
    )
    print(
        f"[[Fashion-MNIST]] :: testset accuracy score = {:.5f}"
    )

    sys.exit(0)


if __name__ == "__main__":
    main()

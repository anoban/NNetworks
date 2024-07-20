from NNetwork import NNetworkMinimal
from IdxUtils import Idx1, Idx3
import sys


def main() -> None:

    mnist_train_images = Idx3(r"../MNIST/train-images.idx3-ubyte")
    mnist_test_images = Idx3(r"../MNIST/t10k-images.idx3-ubyte")
    mnist_train_labels = Idx1(r"../MNIST/train-labels.idx1-ubyte")
    mnist_test_labels = Idx1(r"../MNIST/t10k-labels.idx1-ubyte")

    model_0 = NNetworkMinimal(nodes_in=784, nodes_hid=10, nodes_out=10, alpha=0.125, maxiterations=10_000)
    model_0.fit(mnist_train_images.data, mnist_train_labels.data)

    print(
        f"[[MNIST]] :: testset accuracy score ={model_0.accuracy_score(mnist_train_images.data, mnist_train_labels.data):.5f}"
    )
    print(
        f"[[MNIST]] :: testset accuracy score = {model_0.accuracy_score(mnist_test_images.data, mnist_test_labels.data):.5f}"
    )

    fashion_mnist_train_images = Idx3(r"../Fashion-MNIST/train-images-idx3-ubyte")
    fashion_mnist_test_images = Idx3(r"../Fashion-MNIST/t10k-images-idx3-ubyte")
    fashion_mnist_train_labels = Idx1(r"../Fashion-MNIST/train-labels-idx1-ubyte")
    fashion_mnist_test_labels = Idx1(r"../Fashion-MNIST/t10k-labels-idx1-ubyte")

    mode_1 = NNetworkMinimal(nodes_in=784, nodes_hid=10, nodes_out=10, alpha=0.1, maxiterations=10_000)
    mode_1.fit(fashion_mnist_train_images.data, fashion_mnist_train_labels.data)

    print(
        f"[[Fashion-MNIST]] :: trainset accuracy score = {mode_1.accuracy_score(fashion_mnist_train_images.data, fashion_mnist_train_labels.data):.5f}"
    )
    print(
        f"[[Fashion-MNIST]] :: testset accuracy score = {mode_1.accuracy_score(fashion_mnist_test_images.data, fashion_mnist_test_labels.data):.5f}"
    )

    sys.exit(0)


def evaluate(filepath: str) -> None:
    """
    Evaluate the accuracy of the NNetworkMinimal model, reconstructed from a serialized .nnm file using the MNIST resources.
    """

    model = NNetworkMinimal(nodes_in=1, nodes_hid=1, nodes_out=1, alpha=0.15, maxiterations=1)
    model.load(filepath=filepath)

    train_images = Idx3(r"../train-images.idx3-ubyte")
    train_labels = Idx1(r"../train-labels.idx1-ubyte")

    test_images = Idx3(r"../t10k-images.idx3-ubyte")
    test_labels = Idx1(r"../t10k-labels.idx1-ubyte")

    print(f"Train-set accuracy: {model.accuracy_score(train_images.data, train_labels.data):.6f}")
    print(f"Test-set accuracy:  {model.accuracy_score(test_images.data, test_labels.data):.6f}")


if __name__ == "__main__":
    main()

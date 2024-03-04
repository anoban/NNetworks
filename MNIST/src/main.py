from NNetwork import NNetworkMinimal
from IdxUtils import Idx1, Idx3


def main() -> None:

    # MNIST training data
    train_images = Idx3(r"../idx/train-images.idx3-ubyte")
    train_labels = Idx1(r"../idx/train-labels.idx1-ubyte")
    # MNIST test data
    test_images = Idx3(r"../idx/t10k-images.idx3-ubyte")
    test_labels = Idx1(r"../idx/t10k-labels.idx1-ubyte")

    model = NNetworkMinimal(nodes_in=784, nodes_hid=10, nodes_out=10, alpha=0.125, maxiterations=5000)
    model.gradient_descent(train_images.data, train_labels.data)

    print(f"Train-set accuracy: {model.accuracy_score(train_images.data, train_labels.data):.6f}")
    print(f"Test-set accuracy: {model.accuracy_score(test_images.data, test_labels.data):.6f}")


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

from NNMinimal import NNetworkMinimal
from IdxUtils import Idx1, Idx3


def main() -> None:

    # Fashion-MNIST training data
    train_images = Idx3(r"../idx/train-images-idx3-ubyte")
    train_labels = Idx1(r"../idx/train-labels-idx1-ubyte")

    # Fashion-MNIST test data
    test_images = Idx3(r"../idx/t10k-images-idx3-ubyte")
    test_labels = Idx1(r"../idx/t10k-labels-idx1-ubyte")

    model = NNetworkMinimal(nodes_in=784, nodes_hid=10, nodes_out=10, maxiterations=5000, alpha=0.125)
    model.gradient_descent(train_images.data, train_labels.data)

    train_ascore: float = model.accuracy_score(train_images.data, train_labels.data)
    test_ascore: float = model.accuracy_score(test_images.data, test_labels.data)

    print(f"Accuracy score for training dataset: {train_ascore:5f}")
    print(f"Accuracy score for test dataset:     {test_ascore:5f}")

    model.save("./fmnist")


if __name__ == "__main__":
    main()

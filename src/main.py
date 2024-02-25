from NNetwork import Idx1, Idx3, NNetworkMinimal

def main() -> None:
    train_images = Idx3(r"../MNIST/train-images.idx3-ubyte")
    train_labels = Idx1(r"../MNIST/train-labels.idx1-ubyte")
    
    test_images = Idx3(r"../MNIST/t10k-images.idx3-ubyte")
    test_labels = Idx1(r"../MNIST/t10k-labels.idx1-ubyte")
    
    model = NNetworkMinimal(nodes_in = 784, nodes_hid = 10, nodes_out = 10, alpha = 0.1, maxiterations = 1000)
    model.gradient_descent(train_images.data, train_labels.data)
    
if __name__ == "__main__":
    main()
from NNetwork import NNetworkMinimal
from IdxUtils import Idx1, Idx3
from sklearn.metrics import accuracy_score

def main() -> None:
    train_images = Idx3(r"../train-images.idx3-ubyte")
    train_labels = Idx1(r"../train-labels.idx1-ubyte")
    
    test_images = Idx3(r"../t10k-images.idx3-ubyte")
    test_labels = Idx1(r"../t10k-labels.idx1-ubyte")
    
    # 2,500 iterations is a little extreme, but who cares
    model = NNetworkMinimal(nodes_in = 784, nodes_hid = 10, nodes_out = 10, alpha = 0.15, maxiterations = 200)
    model.gradient_descent(train_images.data, train_labels.data)
    
    predictions_train = model.predict(train_images.data)
    predictions_test = model.predict(test_images.data)
    
    print(f"Train-set accuracy: {accuracy_score(train_labels.data, predictions_train):.4f}")
    print(f"Test-set accuracy: {accuracy_score(test_labels.data, predictions_test):.4f}")
    model.save("model")
    
if __name__ == "__main__":
    main()
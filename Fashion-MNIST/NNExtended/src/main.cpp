#include <iostream>

#include <idxio.hpp>

auto main(void) -> int {
    const auto train_images { idxio::idx3 { L"../idx/train-images-idx3-ubyte" } };
    const auto train_labels { idxio::idx1 { L"../idx/train-labels-idx1-ubyte" } };

    const auto test_images { idxio::idx3 { L"../idx/t10k-images-idx3-ubyte" } };
    const auto test_labels { idxio::idx1 { L"../idx/t10k-labels-idx1-ubyte" } };

    std::wcout << L"There are " << train_labels.size() << L" images in the train-images-idx3-ubyte file\n";
    std::wcout << L"There are " << train_labels.size() << L" labels in the train-labels-idx1-ubyte file\n";
    std::wcout << L"There are " << test_images.size() << L" images in the t10k-images-idx3-ubyte file\n";
    std::wcout << L"There are " << test_labels.size() << L" labels in the t10k-labels-idx1-ubyte file\n";

    return 0;
}

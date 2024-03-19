#ifndef __TEST_NNExtended__
    #include <stdio.h>
    #include <idxio.h>
    #include <utilities.h>

int main(void) {
    const auto train_images { idxio::idx3 { L"../idx/train-images-idx3-ubyte" } };
    const auto train_labels { idxio::idx1 { L"../idx/train-labels-idx1-ubyte" } };

    const auto test_images { idxio::idx3 { L"../idx/t10k-images-idx3-ubyte" } };
    const auto test_labels { idxio::idx1 { L"../idx/t10k-labels-idx1-ubyte" } };

    const auto train_shape = train_images.shape(), test_shape = test_images.shape();

    std::wcout << L"There are " << train_labels.size() << L" images in the train-images-idx3-ubyte file\n";
    std::wcout << L"There are " << train_labels.size() << L" labels in the train-labels-idx1-ubyte file\n";
    std::wcout << L"There are " << test_images.size() << L" images in the t10k-images-idx3-ubyte file\n";
    std::wcout << L"There are " << test_labels.size() << L" labels in the t10k-labels-idx1-ubyte file\n";

    std::wcout << L"Height: " << train_shape.first << L" Width: " << train_shape.second << L'\n';
    std::wcout << L"Height: " << test_shape.first << L" Width: " << test_shape.second << L'\n';

    const utils::random<float> rgen { -0.5, 0.5 };

    std::array<float, 10000>   randoms {};
    std::generate(randoms.begin(), randoms.end(), rgen);

    const auto minmax { std::minmax_element(randoms.cbegin(), randoms.cend()) };
    std::wcout << *minmax.first << L' ' << *minmax.second << L' ' << std::endl;

    return 0;
}

#endif // !__TEST_NNExtended__

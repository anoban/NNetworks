#include <iostream>
#include <idxio.hpp>

auto main(void) -> int {

    const auto train_labels{ idxio::idx1 { L"../idx/train-labels-idx1-ubyte" } };
    std::wcout << L"Did it succeed? " << std::boolalpha << train_labels.is_usable() << std::endl;
    std::wcout << L"There are " << train_labels.size() << L" labels in the idx1 file\n";

    const auto nlabels { std::distance(train_labels.cbegin(), train_labels.cend()) };
    std::wcout << L"There are " << nlabels << L" labels in the idx1 file\n";

    return 0;
}

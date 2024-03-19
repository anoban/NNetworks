#include <idxio.h>
#include <stdio.h>
#include <utilities.h>

int main(void) {
    const idx1_t train_labels = OpenIdx1(L"../idx/train-labels-idx1-ubyte");
    const idx3_t train_images = OpenIdx3(L"../idx/train-images-idx3-ubyte");
    const idx1_t test_labels  = OpenIdx1(L"../idx/t10k-labels-idx1-ubyte");
    const idx3_t test_images  = OpenIdx3(L"../idx/t10k-images-idx3-ubyte");

    PrintIdx1(&train_labels);
    PrintIdx3(&train_images);
    PrintIdx1(&test_labels);
    PrintIdx3(&test_images);

    FreeIdx1(&train_labels);
    FreeIdx3(&train_images);
    FreeIdx1(&test_labels);
    FreeIdx3(&test_images);

    PrintIdx1(&train_labels);
    PrintIdx3(&train_images);
    PrintIdx1(&test_labels);
    PrintIdx3(&test_images);

    return 0;
}

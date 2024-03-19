#ifdef _DEBUG

    #include <assert.h>
    #include <idxio.h>
    #include <stdio.h>
    #include <utilities.h>

int main(void) {
    const idx1_t train_labels = OpenIdx1(L"../idx/train-labels-idx1-ubyte");
    const idx3_t train_images = OpenIdx3(L"../idx/train-images-idx3-ubyte");

    const idx1_t test_labels  = OpenIdx1(L"../idx/t10k-labels-idx1-ubyte");
    const idx3_t test_images  = OpenIdx3(L"../idx/t10k-images-idx3-ubyte");

    assert(train_images.nimages == 60000);
    assert(train_images.buffer);
    assert(train_images.idxmagic == 2051);
    assert(train_images.ncols_perimage == 28);
    assert(train_images.nrows_perimage == 28);

    assert(train_labels.nlabels == 60000);
    assert(train_labels.buffer);
    assert(train_labels.idxmagic == 2049);

    assert(test_images.nimages == 10000);
    assert(test_images.buffer);
    assert(test_images.idxmagic == 2051);
    assert(test_images.ncols_perimage == 28);
    assert(test_images.nrows_perimage == 28);

    assert(test_labels.nlabels == 10000);
    assert(test_labels.buffer);
    assert(test_labels.idxmagic == 2049);

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

#endif // _DEBUG

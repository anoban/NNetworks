#include <idxio.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const idx3_t images = openidx3(L"D:/NNetworks/Fashion-MNIST/train-images-idx3-ubyte");
    const idx1_t labels = openidx1(L"D:/NNetworks/Fashion-MNIST/train-labels-idx1-ubyte");

    wprintf_s(
        L"idx3_t images: magic: %08X, nimages: %u, nrows: %u, ncols: %u\n", images.magic, images.nimages, images.nrows_img, images.ncols_img
    );
    wprintf_s(L"idx1_t labels: magic: %08X, nlabels: %u\n", labels.magic, labels.nlabels);

    free(images.data);
    free(labels.data);

    return 0;
}
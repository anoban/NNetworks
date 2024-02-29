from typing import Iterator
import numpy as np
from numpy.typing import NDArray

np.seterr(all="raise")
import matplotlib.pyplot as plt


class Idx1:
    """
    A minimal class to handle IO operations using idx1 files

    IDX1 file format:
    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000801(2049) magic number (MSB first)
    0004     32 bit integer  10000            number of items
    0008     unsigned byte   ??               label
    0009     unsigned byte   ??               label
    ........
    xxxx     unsigned byte   ??               label
    """

    def __init__(self, filepath: str) -> None:
        """
        filepath: str is a string pointing to the unzipped idx1 resource
        uses numpy under the hood
        """

        with open(file=filepath, mode="rb") as fp:  # let open() handle IO errors.
            self.__data: NDArray[np.uint8] = np.fromfile(fp, dtype=np.uint8)  # private

        self.type: str = "idx1"
        self.__magic: int = int.from_bytes(
            self.__data[:4], byteorder="big"
        )  # idx magic number
        self.count: int = int.from_bytes(
            self.__data[4:8], byteorder="big"
        )  # count of the data elements (labels)

        assert (
            self.count == self.__data.size - 8
        ), "There seems to be a parsing error or the binary file is corrupted!"
        # the actual data
        # type casting the data from np.uint8 to np.float64 since np.exp() raises FloatingPointError with np.uint8 arrays
        self.data: NDArray[np.float64] = self.__data[8:].astype(np.float64)
        # get rid of the original data
        del self.__data

    def __repr__(self) -> str:
        return f"idx1 object(magic: {self.__magic}, count: {self.count:,})"

    def __iter__(self) -> Iterator[np.float64]:
        yield from self.data

    def __getitem__(
        self, index: int
    ) -> np.float64:  # let numpy handle index out of bound errors
        """
        returns the index th element in the data array
        """
        return self.data[index]


class Idx3:
    """
    A minimal class to handle IO operations using idx3 files

    IDX3 file format:
    [offset] [type]          [value]          [description]
    0000     32 bit integer  0x00000803(2051) magic number
    0004     32 bit integer  10000            number of images
    0008     32 bit integer  28               number of rows
    0012     32 bit integer  28               number of columns
    0016     unsigned byte   ??               pixel
    0017     unsigned byte   ??               pixel
    ........
    xxxx     unsigned byte   ??               pixel
    """

    def __init__(self, filepath: str) -> None:
        """
        filepath: str is a string pointing to the unzipped idx resource
        uses numpy under the hood
        """

        with open(file=filepath, mode="rb") as fp:
            self.__data: NDArray[np.uint8] = np.fromfile(fp, dtype=np.uint8)  # private

        self.__type: str = "idx3"
        self.__magic: int = int.from_bytes(
            self.__data[:4], byteorder="big"
        )  # idx magic number
        self.count: int = int.from_bytes(
            self.__data[4:8], byteorder="big"
        )  # count of the data elements (images)
        # shape is shape of each element NOT the shape of the overall data
        self.shape: tuple[int, int] = (
            int.from_bytes(self.__data[8:12], byteorder="big"),
            int.from_bytes(self.__data[12:16], byteorder="big"),
        )
        # supposed to be 28 x 28 for MNIST inspired datasets.
        self.__ppimage: int = (
            self.shape[0] * self.shape[1]
        )  # pixels per image (28 x 28)
        assert (self.count * self.__ppimage) == (
            self.__data.size - 16
        ), "There seems to be a parsing error or the binary file is corrupted!"
        # the actual data
        # idx3 file stores data as bytes but we'll load in each byte as a 64 bit double
        # because np.exp() raises a FloatingPointError with np.uint8 type arrays
        self.data: NDArray[np.float64] = (
            self.__data[16:].reshape(self.count, self.__ppimage).T.astype(np.float64)
        )
        del self.__data  # don't need this anymore.

    def __repr__(self) -> str:
        return f"idx3 object(magic: {self.__magic}, shape: {self.shape}, count: {self.count:,})"

    def __iter__(self) -> Iterator[np.float64]:
        yield from self.data

    def __getitem__(self, index: int) -> NDArray[np.float64]:
        """
        this routine returns all the pixels of index th image, i.e returns the index th column of the transposed matrix
        """
        return self.data[:, index]


def peek_idx(
    idx3: str,
    idx1: str,
    idx3elem_dim: tuple[int, int] = (28, 28),
    colormap: str = "binary",
) -> None:
    """
    samples 15 randomly choosen pairs if images and labels from the provided idx resources and plots them.
    uses numpy, matplotlib under the hood.
    idx3: str                                            the path to the idx3 resource
    idx1: str                                            path to corresponding idx1 resource
    idx3elem_dim: typing.Tuple[int, int] = (28, 28)      shape of the image matrices in the idx3 file
    colormap: str = "binary"                             matplotlib cmap specification to use, when plotting the images
    """

    images = Idx3(idx3)
    labels = Idx1(idx1)
    assert (
        images.count == labels.count
    ), f"Mismatch in the number of elements stored in idx1 and idx3 files:: idx1: {labels.count}, idx3: {images.count}"

    nrows, ncols = idx3elem_dim
    fig, axes = plt.subplots(nrows=1, ncols=15)
    fig.set_size_inches(20, 4)
    random_indices: NDArray[np.int64] = np.random.choice(
        np.arange(0, images.count), size=15, replace=False
    )
    for ax, idx in zip(axes, random_indices):
        ax.imshow(images[idx].reshape(nrows, ncols), cmap=colormap)
        ax.set_title(f"{labels[idx]:1.0f}")  # remember the labels are now np.float64 s
        ax.set_xticks([])  # remove the x axis ticks and labels
        ax.set_yticks([])  # remove the y axis ticks and labels
    plt.show()
    return

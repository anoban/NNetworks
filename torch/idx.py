from typing import Any, override

import numpy as np

import torch
from torch.utils.data import DataLoader, Dataset

__all__ = (r"IdxDataset", r"IdxLoader")


class IdxDataset(Dataset[torch.Tensor]):
    """
    constructs a `PyTorch` `Dataset` compatible class instance that can also be used with the `PyTorch` `DataLoader` class
    converts the Idx1 and Idx3 objects from `uchar` to `float` and stores them as separate `torch.Tensor` objects
    """

    def __init__(self, idx1_filepath: str, idx3_filepath: str) -> None:
        """
        provide the path to an Idx1 file (raw binary) and a path to its corresponding Idx3 file (raw binary)
        the passed pair of Idx1 and Idx3 objects must have the same number of elements (images and labels)
        this class is not compatible with other formats of Idx data (.csv or .npy or .npz or any form of compressed archives like .zip, .tar, .tar.gz)
        """

        super(Dataset, self).__init__()

        try:
            with open(file=idx1_filepath, mode="rb") as fp:
                temp = np.fromfile(file=fp, dtype=np.uint8)
        except FileNotFoundError as error:
            raise RuntimeError("") from error

    def __len__(self) -> int:
        return 5

    @override
    def __getitem__(self, index: int) -> Any:
        pass


class IdxLoader(DataLoader[torch.Tensor]):
    def __init__(self) -> None:
        pass

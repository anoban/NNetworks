from typing import Any, override

import numpy as np
from numpy.typing import NDArray
from torch.utils.data import Dataset


class Idx1(Dataset[NDArray[np.uint8]]):
    """
    #
    """

    def __init__(self, filepath: str) -> None:
        super().__init__()
        try:
            with open(file=filepath, mode="rb") as fp:
                temp = np.fromfile(file=fp, dtype=np.uint8)
        except FileNotFoundError as error:
            raise RuntimeError("") from error

    def __len__(self) -> int:
        return 5

    @override
    def __getitem__(self, index: int) -> Any:
        pass

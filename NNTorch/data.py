from typing import Any, override

from torch.utils.data import Dataset


class Idx1(Dataset):
    """
    #
    """

    def __init__(self) -> None:
        super().__init__()

    def __len__(self) -> int:
        return 5

    @override
    def __getitem__(self, index: int) -> Any:
        pass

from abc import ABC, abstractmethod

import numpy as np


class Model(ABC):
    def __init__(self):
        """Initialize the model."""
        super().__init__()

    @abstractmethod
    def _create_model(self):
        """Create the model based on the parameters."""
        pass

    @abstractmethod
    def step(self, x: np.ndarray, u: np.ndarray, dt: float = None):
        """Perform a single step of the model."""
        pass

from src.modeling.models.model import Model
import control
import numpy as np


class LinearModel(Model):
    n: int
    m: int
    p: int

    ss: control.StateSpace | None
    ss_discrete: control.StateSpace | None

    is_discrete: bool = False

    def __init__(self, params: dict):
        """Initialize the linear model with parameters."""
        super().__init__()
        self.params = params

        assert (
            "A" in params and "B" in params and "C" in params
        ), "Parameters A, B, and C must be provided."
        self.n = params.get("A").shape[0]
        self.m = params.get("B").shape[1]
        self.p = params.get("C").shape[0]

        self._create_model()

    def _create_model(self):
        """Create the linear model based on the parameters."""

        self.ss = control.ss(
            self.params.get("A"),
            self.params.get("B"),
            self.params.get("C"),
            self.params.get("D", None),
        )

    def step(
        self, x: np.ndarray, u: np.ndarray, dt: float = None
    ) -> tuple[np.ndarray, np.ndarray]:
        """
        Perform a single step of the model.
        :param x: initial state vector
        :param u: control input vector
        :param dt: step time
        :return:
            - x_next: next state vector
            - y: output vector
        """
        response = control.forced_response(
            self.ss_discrete, T=[0, dt], U=np.repeat(u, repeats=2, axis=0), X0=x
        )
        return response.states[:, -1], response.outputs[:, -1]

    def discretize(self, ts: float = 1):
        """Discretizes the linear model."""
        self.is_discrete = True
        self.ss_discrete = control.c2d(self.ss, ts, method="tustin")

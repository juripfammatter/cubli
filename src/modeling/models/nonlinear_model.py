from src.modeling.models.linear_model import LinearModel
from src.modeling.models.model import Model
import sympy as sp
import numpy as np


class NonlinearModel(Model):
    def __init__(self, params: dict):
        """Initialize the nonlinear model with parameters."""
        super().__init__()
        self.params = params
        self._create_model()

    def _create_model(self):
        """Create the nonlinear model based on the parameters."""
        x_1, x_2, x_3, x_4 = sp.symbols(
            "x_1 x_2 x_3 x_4"
        )  # theta_b, theta_b_dot, theta_w, theta_w_dot
        u = sp.symbols("u")

        I_c = self.params["I_body_pivot"] + self.params["l"] ** 2 * (
            self.params["m_total"] - self.params["m_body"]
        )
        I_w = self.params["I_wheel_center"]
        m_total = self.params["m_total"]
        K = self.params["K_tau"]

        self.model = sp.Matrix(
            [
                [x_2],
                [1 / I_c * (K * u + m_total * self.params["l"] * 9.81 * sp.sin(x_1))],
                [
                    1
                    / I_c
                    * (
                        (1 + I_c / I_w) * K * u
                        - m_total * self.params["l"] * 9.81 * sp.sin(x_1)
                    )
                ],
            ]
        )

    def step(self, x: np.ndarray, u: np.ndarray, dt: float = None):
        raise NotImplementedError

    def linearize(self, x_s: np.ndarray, u_s: np.ndarray) -> LinearModel:
        """Linearize the nonlinear model."""
        A = self.model.jacobian(sp.symbols("x_1 x_2 x_3"))
        B = self.model.jacobian([sp.symbols("u")])
        C = sp.Matrix([[0, 1, 0], [0, 0, 1]])
        D = sp.zeros(2, 1)

        params = self.params.copy()
        params.setdefault(
            "A",
            np.array(
                A.evalf(subs={"x_1": x_s[0], "x_2": x_s[1], "x_3": x_s[2]}),
                dtype=float,
            ),
        )
        params.setdefault("B", np.array(B.evalf(subs={"u": u_s[0]}), dtype=float))
        params.setdefault("C", np.array(C, dtype=float))
        params.setdefault("D", np.array(D, dtype=float))
        return LinearModel(params)

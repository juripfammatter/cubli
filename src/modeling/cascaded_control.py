import os

import control
import numpy as np
from matplotlib import pyplot as plt
import seaborn as sns
import json

from src.modeling.models.nonlinear_model_cascaded import NonlinearModel


def main():
    params = {
        "m_total": 0.325,  # kg
        "m_wheel": 0.0832,
        "m_body": 0.0117 + 0.0636,
        "m_controller": 0.0783,
        "m_cable": 0.0024,
        "m_screws": 0.0044,
        "l": 60.781,  # mm
        "I_body_pivot": 731.12,  # kg*mm^2
        "I_wheel_pivot": 372.11,  # kg*mm^2
        "I_wheel_center": 64.171,  # kg*mm^2
        "K_tau": 0.065 / 0.927,  # Unit? Nm/A : Motor constant TODO
        "omega_0": 10,  # Natural frequency of PD controller
        "xi": 0.7,  # Damping ratio
    }
    json_path = os.path.join(os.getcwd(), "params.json")
    with open(json_path, "w") as f:
        json.dump(params, f, indent=4)

    ts = 0.1  # Sampling time for discretization
    T_sim = 10  # Simulation time in seconds

    nonlinear_model = NonlinearModel(params)
    linear_model = nonlinear_model.linearize(
        x_s=np.array([0, 0, 0, 0]), u_s=np.array([0])
    )

    linear_model.discretize(ts=ts)

    print(f"Eigenvalues of A: {np.linalg.eigvals(linear_model.ss.A)}")
    print(
        f"Discretized eigenvalues of A: {np.linalg.eigvals(linear_model.ss_discrete.A)}"
    )
    print(
        f"Rank of controllability matrix: {np.linalg.matrix_rank(control.ctrb(linear_model.ss_discrete.A, linear_model.ss_discrete.B))}"
    )
    print(
        f"Rank of observability matrix: {np.linalg.matrix_rank(control.obsv(linear_model.ss_discrete.A, linear_model.ss_discrete.C))}"
    )

    step_response = control.step_response(
        linear_model.ss_discrete, T=np.arange(0, T_sim, ts)
    )

    Q = np.diag([0.1, 1, 10, 1000])  # State cost matrix
    R = np.array([[1.0]])  # Input cost matrix
    K, S, E = control.dlqr(linear_model.ss_discrete, Q, R)

    print(f"Closed loop eigenvalues:\n{E}")
    print(f"Feedback gain K:\n{K}")
    print(f"Absolute values of closed loop eigenvalues:\n{np.abs(E)}")

    closed_loop_ss = control.ss(
        linear_model.ss_discrete.A - linear_model.ss_discrete.B @ K,
        linear_model.ss_discrete.B,
        linear_model.ss_discrete.C,
        linear_model.ss_discrete.D,
    )
    closed_loop_step_response = control.step_response(
        closed_loop_ss, T=np.arange(0, T_sim, ts)
    )

    x = np.zeros((linear_model.n, int(T_sim / ts)))  # Initial state
    for t in range(1, int(T_sim / ts)):
        u = -K @ x[:, t - 1] + 1
        x[:, t], _ = linear_model.step(x[:, t - 1], u, dt=ts)

    sns.set_theme(style="darkgrid")
    fig, axs = plt.subplots(4, 2, figsize=(10, 8))
    [
        axs[i, 0].plot(
            step_response.time, step_response.states[i, 0], label=f"$x_{ {i + 1} }$"
        )
        for i in range(4)
    ]
    [
        axs[i, 1].plot(
            closed_loop_step_response.time,
            x[i],
            label=f"$x_{ {i + 1} }$",
        )
        for i in range(4)
    ]

    [ax.legend(facecolor="white") for ax in axs.flatten()]
    axs[0, 0].set_title("Step Response of Linearized Model")
    axs[0, 1].set_title("Step Response of Closed Loop System")
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()

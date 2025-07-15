import argparse
import json
import os

import control
import numpy as np
import seaborn as sns
import pandas as pd
from matplotlib import pyplot as plt

from src.modeling.models.nonlinear_model import NonlinearModel
from src.visualization.utils.convert_to_json import convert_to_json
from src.visualization.utils.plot_utils import plot_measurements, plot_states


def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Visualize sensor measurements.")
    parser.add_argument(
        "-f",
        "--file",
        type=str,
        required=True,
        help="Path to the sensor measurements file.",
    )

    args = parser.parse_args()
    file_path = os.path.join(os.getcwd(), args.file)
    params_path = os.path.join(os.getcwd(), "src/modeling/params.json")

    if not os.path.exists(file_path):
        raise FileNotFoundError(f"File not found: {file_path}")

    # load measurements
    measurements = convert_to_json(file_path)

    with open(measurements, "r") as file:
        measurements_df = pd.read_json(file)
        measurements_df["imu"] = measurements_df["imu"].apply(lambda x: np.array(x))
        measurements_df["motor_vel"] = np.zeros_like(measurements_df["imu"])
    # load parameters
    if not os.path.exists(params_path):
        raise FileNotFoundError(f"Parameters file not found: {params_path}")
    with open(params_path, "r") as file:
        params = json.load(file)

    # Plot the measurements
    plot_measurements(measurements_df)

    # setup Kalman filter
    divider = 1
    ts = 0.01 * divider  # Sampling time for discretization

    nonlinear_model = NonlinearModel(params)
    linear_model = nonlinear_model.linearize(x_s=np.array([0, 0, 0]), u_s=np.array([0]))
    linear_model.discretize(ts=ts)

    Q_lqe = np.diag([1, 1, 1])  # TODO replace with actual covariance matrix
    R_lqe = 7.14025e-7 * np.diag([1, 1])

    A, H = linear_model.ss_discrete.A, linear_model.ss_discrete.C
    P, _, _ = control.dare(A.T, H.T, Q_lqe, R_lqe)
    K_lqe = P @ H.T @ np.linalg.inv(H @ P @ H.T + R_lqe)

    with np.printoptions(precision=8, suppress=True):
        print(f"Kalman gain: {K_lqe.flatten()}")
        print(f"Discrete A matrix:\n{linear_model.ss_discrete.A.flatten()}")
        print(f"Discrete C matrix:\n{linear_model.ss_discrete.C.flatten()}")

    n_sim = measurements_df.shape[0]
    n_est = int(n_sim / divider)
    x_hat = np.zeros((linear_model.n, n_est + 1))  # Initial state
    time = [measurements_df["time"][0]]

    for i in range(1, n_est):
        time.append(measurements_df["time"][i * divider])
        x_hat[:, i] = linear_model.ss_discrete.A @ x_hat[:, i - 1] + K_lqe @ (
            np.hstack(
                [
                    measurements_df["imu"][i * divider][5],
                    measurements_df["imu"][i * divider][6],
                ]
            )
            - linear_model.ss_discrete.C @ x_hat[:, i - 1]
        )

    print("")
    estimated_states = pd.DataFrame(
        data={
            "time": time,
            "x_hat_1": x_hat[0, :-1],
            "x_hat_2": x_hat[1, :-1],
            "x_hat_3": x_hat[2, :-1],
        }
    )

    plot_states(estimated_states, measurements_df)


if __name__ == "__main__":
    main()

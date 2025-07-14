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

    # Approximation of the PT2 system
    k = 10.2

    # Stadler's method to estimate the parameters
    # log_decr = np.log(
    #     [
    #         (k - 8.5) / (14.50 - k),
    #         (11 - k) / (k - 8.5),
    #         (k - 9.8) / (11 - k),
    #     ]
    # )
    # dt = sum([0.95 - 0.857, 1.04 - 0.95, 1.14 - 1.04]) / 3
    #
    # xi = 1 / np.sqrt(1 + (np.pi**2) / ((sum(log_decr) / len(log_decr)) ** 2)) * 1.1
    # omega_0 = np.pi / (dt * np.sqrt(1 - xi**2)) * 1.03
    #
    # scale = 10.2 / 10.44

    # hand tuned values PT1 + PT2 system
    # xi = 0.45
    # omega_0 = 30
    # scale = 0.67
    #
    # pt1 = control.tf([scale * 0.2 * omega_0], [1, 0.4 * omega_0])
    #
    # # Add a 0.01s time delay using a first-order Pade approximation
    # num_delay, den_delay = control.pade(0.05, 1)
    # delay_tf = control.tf(num_delay, den_delay)
    # sys = (
    #     control.tf([scale * omega_0**2], [1, 2 * xi * omega_0, omega_0**2])
    #     * delay_tf
    # )
    #
    # sys = pt1 + sys

    xi = 0.7
    omega_0 = 10
    scale = 1.0

    num_delay, den_delay = control.pade(0.01, 1)
    delay_tf = control.tf(num_delay, den_delay)
    sys = (
        control.tf([scale * omega_0**2], [1, 2 * xi * omega_0, omega_0**2])
        # * delay_tf
    )

    ss = control.ss(sys)
    T = np.array([[0, 1], [1, 0]])
    ss = control.similarity_transform(ss, T)

    print(f"Approximated PT2 closed-loop System:\n{ss}")

    # input response
    u = measurements_df["input"].apply(lambda x: x[0])
    time = measurements_df["time"][0] + np.arange(
        0, len(measurements_df) * 0.005, 0.005
    )
    state_evolution = control.forced_response(ss, T=time, U=u, X0=[0, 0])

    sns.set_theme(style="darkgrid")
    fig, axs = plt.subplots(1, 1, figsize=(8, 10), sharex=True, dpi=300)

    sns.lineplot(
        data=measurements_df,
        x="time",
        y=measurements_df["imu"].apply(lambda x: x[6]),
        label="Motor Velocity Measurements",
        color="red",
        linestyle="-",
        linewidth=1,
        ax=axs,
        alpha=0.5,
    )
    sns.lineplot(
        data=measurements_df,
        x="time",
        y=measurements_df["input"].apply(lambda x: x[0]),
        label="Input",
        color="blue",
        linestyle="-",
        linewidth=1,
        ax=axs,
        alpha=0.5,
    )

    axs.plot(
        time,
        state_evolution.outputs,
        label="Fitted PT2 System Response",
        color="green",
        linestyle="--",
        linewidth=1,
    )

    # axs.set(xlim=(0.75, 1.5), ylim=(-1, 15))
    # axs.set_xticks([0.76, 0.857, 0.95, 1.04, 1.14, 1.24])
    # axs.set_yticks([8.5, 9.8, k, 10.44, 11, 14.5])
    axs.legend(loc="upper right", facecolor="white")
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()

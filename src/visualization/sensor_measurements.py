import argparse
import os

import numpy as np
import seaborn as sns
import pandas as pd
from matplotlib import pyplot as plt

from src.visualization.utils.convert_to_json import convert_to_json


def main():
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

    if not os.path.exists(file_path):
        raise FileNotFoundError(f"File not found: {file_path}")

    json_file = convert_to_json(file_path)

    with open(json_file, "r") as file:
        df = pd.read_json(file)
        df["imu"] = df["imu"].apply(lambda x: np.array(x))

    sns.set_theme(style="darkgrid")
    fig, axs = plt.subplots(6, 1, figsize=(8, 12), sharex=True)
    labels = ["$a_x$", "$a_y$", "$a_z$", "$\\omega_x$", "$\\omega_y$", "$\\omega_z$"]
    units = [
        "$\\frac{m}{s^2}$",
        "$\\frac{m}{s^2}$",
        "$\\frac{m}{s^2}$",
        "$\\frac{rad}{s}$",
        "$\\frac{rad}{s}$",
        "$\\frac{rad}{s}$",
    ]
    [
        sns.lineplot(
            data=df[:][5:],
            x="time",
            y=df["imu"].apply(lambda x: x[i]),
            label=labels[i],
            color="blue",
            linewidth=0.5,
            ax=axs[i],
            alpha=0.5,
        )
        for i in range(6)
    ]

    [
        ax.set(xlabel="time [s]", ylabel=labels[i] + f" [{units[i]}]")
        for i, ax in enumerate(axs)
    ]

    [ax.legend(facecolor="white", loc="upper right") for ax in axs.flatten()]
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()

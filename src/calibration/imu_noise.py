import argparse
import os

import numpy as np
import seaborn as sns
import pandas as pd
from matplotlib import pyplot as plt

from src.visualization.utils.convert_to_json import convert_to_json
from src.visualization.utils.plot_utils import plot_measurements


def std_dev(df: pd.DataFrame, sensor: int) -> float:
    """
    Calculate the standard deviation of a specific sensor measurement.
    :param df: dataframe containing sensor measurements
    :param sensor: index of the sensor
    :return: standard deviation of the sensor measurements
    """
    # remove first 5 measurements to avoid zero values
    data = df["imu"].apply(lambda x: x[sensor])[5:]

    return data.std()


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

    std_dev_imu = np.array([std_dev(df, i) for i in range(6)])

    plot_measurements(df)
    # with np.printoptions(precision=12, suppress=True):
    print("Standard Deviation of IMU Measurements:")
    print(f"{std_dev_imu.round(6)}")
    print("Variance of IMU Measurements:")
    print(f"{std_dev_imu.round(6)**2}")


if __name__ == "__main__":
    main()

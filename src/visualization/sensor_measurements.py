import argparse
import os

import numpy as np
import seaborn as sns
import pandas as pd
from matplotlib import pyplot as plt

from src.visualization.utils.convert_to_json import convert_to_json
from src.visualization.utils.plot_utils import plot_measurements


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

    plot_measurements(df)


if __name__ == "__main__":
    main()

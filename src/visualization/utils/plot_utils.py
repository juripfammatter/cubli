import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

rad2deg = lambda x: x * 180 / 3.141592653589793


def plot_states(
    states: pd.DataFrame, measurements: pd.DataFrame = None, u_hat: np.ndarray = None
):
    sns.set_theme(style="darkgrid")
    fig, axs = plt.subplots(5, 1, figsize=(8, 12), sharex=True)

    sns.lineplot(
        data=states,
        x="time",
        y=states["x_hat_1"].apply(rad2deg),
        label="$\\widehat{x}_1 = \\theta_c$",
        linewidth=2.0,
        ax=axs[0],
    )
    sns.lineplot(
        data=states,
        x="time",
        y="x_hat_2",
        label="$\\widehat{x}_2 = \\dot{\\theta}_c$",
        linewidth=2.0,
        ax=axs[1],
    )
    sns.lineplot(
        data=states,
        x="time",
        y="x_hat_3",
        label="$\\widehat{x}_3 = \\theta_w$",
        linewidth=2.0,
        ax=axs[2],
    )
    sns.lineplot(
        data=states,
        x="time",
        y="x_hat_4",
        label="$\\widehat{x}_4 = \\dot{\\theta}_w$",
        linewidth=2.0,
        ax=axs[3],
    )

    if measurements is not None:
        sns.lineplot(
            data=measurements,
            x="time",
            y=measurements["imu"].apply(lambda x: x[5]),
            label="IMU Measurements",
            color="red",
            marker="o",
            linestyle="--",
            linewidth=1,
            ax=axs[1],
            alpha=0.5,
        )
        sns.lineplot(
            data=measurements,
            x="time",
            y=measurements["imu"].apply(lambda x: x[6]),
            label="Motor Velocity Measurements",
            color="red",
            marker="o",
            linestyle="--",
            linewidth=1,
            ax=axs[3],
            alpha=0.5,
        )

        sns.lineplot(
            data=measurements,
            x="time",
            y=measurements["input"].apply(lambda x: x[0]),
            label="Input",
            color="red",
            marker="o",
            linestyle="--",
            linewidth=1,
            ax=axs[4],
            alpha=0.5,
        )

        if measurements["state"] is not None:
            sns.lineplot(
                data=measurements,
                x="time",
                y=measurements["state"].apply(lambda x: rad2deg(x[0])),
                label="$\\theta_c^{edge}$",
                color="green",
                marker="o",
                linestyle="--",
                linewidth=1,
                ax=axs[0],
                alpha=0.5,
            )
            sns.lineplot(
                data=measurements,
                x="time",
                y=measurements["state"].apply(lambda x: x[1]),
                label="$\\dot{\\theta}_c^{edge}$",
                color="green",
                marker="o",
                linestyle="--",
                linewidth=1,
                ax=axs[1],
                alpha=0.5,
            )
            sns.lineplot(
                data=measurements,
                x="time",
                y=measurements["state"].apply(lambda x: x[2]),
                label="$\\theta_w^{edge}$",
                color="green",
                marker="o",
                linestyle="--",
                linewidth=1,
                ax=axs[2],
                alpha=0.5,
            )
            sns.lineplot(
                data=measurements,
                x="time",
                y=measurements["state"].apply(lambda x: x[3]),
                label="$\\dot{\\theta}_w^{edge}$",
                color="green",
                marker="o",
                linestyle="--",
                linewidth=1,
                ax=axs[3],
                alpha=0.5,
            )

    if u_hat is not None:
        sns.lineplot(
            data=states,
            x="time",
            y=u_hat[0, :],
            label="$u_{pred}$",
            color="orange",
            marker="o",
            linestyle="--",
            linewidth=1,
            ax=axs[4],
            alpha=0.5,
        )

    [ax.legend(facecolor="white", loc="upper right") for ax in axs.flatten()]
    axs[0].set_title("Estimated States")

    labels = [
        "$\\theta_c$",
        "$\\dot{\\theta}_c$",
        "$\\theta_w$",
        "$\\dot{\\theta}_w$",
        "$u$",
    ]
    units = [
        "$\\left[deg\\right]$",
        "$\\left[\\frac{rad}{s}\\right]$",
        "$\\left[deg\\right]$",
        "$\\left[\\frac{rad}{s}\\right]$",
        "$\\left[\\frac{rad}{s}\\right]$",
    ]
    [
        ax.set(xlabel="time [s]", ylabel=labels[i] + f"{units[i]}")
        for i, ax in enumerate(axs)
    ]
    plt.tight_layout()
    plt.show()


def plot_measurements(df: pd.DataFrame):
    sns.set_theme(style="darkgrid")

    fig, axs = plt.subplots(7, 1, figsize=(8, 12), sharex=True)
    labels = [
        "$a_x$",
        "$a_y$",
        "$a_z$",
        "$\\omega_x$",
        "$\\omega_y$",
        "$\\omega_z$",
        "$\\omega_m$",
    ]
    units = [
        "$\\frac{m}{s^2}$",
        "$\\frac{m}{s^2}$",
        "$\\frac{m}{s^2}$",
        "$\\frac{rad}{s}$",
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
        for i in range(7)
    ]

    [
        ax.set(xlabel="time [s]", ylabel=labels[i] + f" [{units[i]}]")
        for i, ax in enumerate(axs)
    ]

    [ax.legend(facecolor="white", loc="upper right") for ax in axs.flatten()]
    axs[0].set_title("Sensor Measurements")

    plt.tight_layout()
    plt.show()

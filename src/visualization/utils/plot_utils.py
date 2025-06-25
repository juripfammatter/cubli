import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


def plot_states(states: pd.DataFrame, measurements: pd.DataFrame = None):
    sns.set_theme(style="darkgrid")
    fig, axs = plt.subplots(3, 1, figsize=(8, 12), sharex=True)

    sns.lineplot(
        data=states,
        x="time",
        y="x_hat_1",
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
        label="$\\widehat{x}_3 = \\dot{\\theta}_w$",
        linewidth=2.0,
        ax=axs[2],
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
            y=measurements["motor_vel"],
            label="Motor Velocity Measurements",
            color="red",
            marker="o",
            linestyle="--",
            linewidth=1,
            ax=axs[2],
            alpha=0.5,
        )

    [ax.legend(facecolor="white", loc="upper right") for ax in axs.flatten()]
    axs[0].set_title("Estimated States")

    labels = ["$\\theta_c$", "$\\dot{\\theta}_c$", "$\\dot{\\theta}_w$"]
    units = [
        "$\\left[rad\\right]$",
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
    axs[0].set_title("Sensor Measurements")

    plt.tight_layout()
    plt.show()

import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


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

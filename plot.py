import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

def ensure_plot_folder():
    if not os.path.exists("plots"):
        os.makedirs("plots")

def plot_boxplots(df, n):
    # Feasible ratio
    plt.figure()
    sns.boxplot(y=df["feasible_ratio"])
    plt.title(f"Feasible Ratio (n={n})")
    plt.savefig(f"plots/feasible_ratio_boxplot_n{n}.png", dpi=300)
    plt.close()

    # Avg total violations
    plt.figure()
    sns.boxplot(y=df["avg_total"])
    plt.title(f"Avg Total Violations (n={n})")
    plt.savefig(f"plots/avg_total_boxplot_n{n}.png", dpi=300)
    plt.close()

def plot_scatter(df, n):
    plt.figure()
    plt.scatter(df["feasible_ratio"], df["avg_total"], alpha=0.5)
    plt.xlabel("Feasible Ratio")
    plt.ylabel("Avg Total Violations")
    plt.title(f"Feasibility vs Violations (n={n})")

    plt.savefig(f"plots/scatter_n{n}.png", dpi=300)
    plt.close()

def plot_all():
    ensure_plot_folder()

    df = pd.read_csv("Results/results_two_steps_aggregate_8.csv")

    n = 8  # or infer from filename if you want

    plot_boxplots(df, n)
    plot_scatter(df, n)

if __name__ == "__main__":
    plot_all()
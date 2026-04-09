import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import sys
import os

# ── Configuration ────────────────────────────────────────────────────────────
FILE = "Results/random_walk/random_walk_global_summary.csv"   # ← change to your CSV path

OUTPUT_FEASIBLE   = "team_size_avg_feasible_ratio.png"
OUTPUT_VIOLATIONS = "team_size_avg_total_violations.png"

COLOR_MAIN_1 = "#1B4F8A"   # deep navy
COLOR_MAIN_2 = "#1CAD55"   # green
EDGE_COLOR   = "#0D2B4E"
# ─────────────────────────────────────────────────────────────────────────────


def load(path):
    if not os.path.exists(path):
        sys.exit(f"[ERROR] File not found: {path}")

    df = pd.read_csv(path)
    required = {"team_size", "avg_feasible_ratio", "avg_total_violations"}
    if not required.issubset(df.columns):
        sys.exit(f"[ERROR] {path} must contain columns: {required}")

    return df.sort_values("team_size").reset_index(drop=True)


def make_plot(x_vals, y_vals, color, ylabel, title, output, legend_label, ylim=None):
    n = len(x_vals)
    x = np.arange(n)
    x_labels = x_vals.astype(str).tolist()

    fig, ax = plt.subplots(figsize=(max(8, n * 0.9), 6))
    fig.patch.set_facecolor("#F7F9FC")
    ax.set_facecolor("#F7F9FC")

    # Line behind points
    ax.plot(x, y_vals,
            color=color,
            linewidth=1.8,
            alpha=0.9,
            zorder=1)

    # Points on top
    ax.scatter(x, y_vals,
               color=color,
               edgecolors=EDGE_COLOR,
               linewidths=0.6,
               s=45,
               alpha=0.95,
               zorder=2)

    # Value labels
    y_range = max(y_vals) - min(y_vals) if len(y_vals) > 1 else max(y_vals)
    offset = 0.015 if ylabel == "Feasible Ratio" else max(0.02 * max(y_vals), 0.02)

    for i, y in enumerate(y_vals):
        if ylabel == "Feasible Ratio":
            label = f"{y:.3f}"
        else:
            label = f"{y:.2f}"
        ax.text(i, y + offset, label,
                ha="center", va="bottom",
                fontsize=9, color=color, fontweight="bold")

    # ── Grid & spines ─────────────────────────────────────────────────────────
    ax.yaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
    ax.xaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
    ax.set_axisbelow(True)

    for spine in ["top", "right"]:
        ax.spines[spine].set_visible(False)
    for spine in ["left", "bottom"]:
        ax.spines[spine].set_color("#C8D6E5")
        ax.spines[spine].set_linewidth(0.8)

    # ── Axes labels & ticks ───────────────────────────────────────────────────
    ax.set_xticks(x)
    ax.set_xticklabels(x_labels, fontsize=11, fontfamily="DejaVu Sans",
                       color="#2C3E50")
    ax.set_xlabel("Team Size", fontsize=16, fontweight="bold",
                  color="#1A252F", labelpad=10)
    ax.set_ylabel(ylabel, fontsize=16, fontweight="bold",
                  color="#1A252F", labelpad=15)
    ax.tick_params(axis="y", labelsize=9, colors="#2C3E50")
    ax.tick_params(axis="x", length=0)

    if ylim is not None:
        ax.set_ylim(*ylim)
        
    else:
        ymin = 0
        ymax = max(y_vals) * 1.18 if max(y_vals) > 0 else 1
        ax.set_ylim(ymin, ymax)

    ax.margins(x=0.03)

    # ── Title ─────────────────────────────────────────────────────────────────
    ax.set_title(title,
                 fontsize=20, fontweight="bold", color="#1A252F",
                 pad=16, fontfamily="DejaVu Sans")

    # ── Legend ────────────────────────────────────────────────────────────────
    patch = mpatches.Patch(facecolor=color, edgecolor=EDGE_COLOR,
                           linewidth=0.6, label=legend_label)
    legend = ax.legend(
        handles=[patch],
        title="Metric",
        title_fontsize=8.5,
        fontsize=9,
        loc="upper right",
        framealpha=0.9,
        edgecolor="#C8D6E5",
        fancybox=False,
    )
    legend.get_title().set_color("#2C3E50")

    plt.tight_layout()
    plt.subplots_adjust(left=0.12)
    plt.savefig(output, dpi=180, bbox_inches="tight", facecolor=fig.get_facecolor())
    print(f"[✓] Plot saved → {output}")
    plt.show()


df = load(FILE)

# Plot 1: Average feasible ratio
make_plot(
    x_vals=df["team_size"],
    y_vals=df["avg_feasible_ratio"],
    color=COLOR_MAIN_1,
    ylabel="Feasible Ratio",
    title="Average Feasible Ratio by Team Size",
    output=OUTPUT_FEASIBLE,
    legend_label="Average feasible ratio",
    ylim=(0, min(1.0, df["avg_feasible_ratio"].max() * 1.18))
)

# Plot 2: Average total violations
make_plot(
    x_vals=df["team_size"],
    y_vals=df["avg_total_violations"],
    color=COLOR_MAIN_2,
    ylabel="Average Total Violations",
    title="Average Total Violations by Team Size",
    output=OUTPUT_VIOLATIONS,
    legend_label="Average total violations"
)
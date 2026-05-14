import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import sys
import os

# ── Configuration ────────────────────────────────────────────────────────────
FILE = "../Results/random_walk/random_walk_global_summary.csv"

OUTPUT_FEASIBLE   = "graphs/team_size_avg_feasible_ratio.png"
OUTPUT_VIOLATIONS = "graphs/team_size_avg_total_violations.png"

COLOR_MAIN_1 = "#1B4F8A"   # deep navy
COLOR_MAIN_2 = "#1CAD55"   # green
EDGE_COLOR   = "#0D2B4E"
FIT_COLOR    = "#2C3E50"
# ─────────────────────────────────────────────────────────────────────────────


def load(path):
    if not os.path.exists(path):
        sys.exit(f"[ERROR] File not found: {path}")

    df = pd.read_csv(path)
    required = {"team_size", "avg_feasible_ratio", "avg_total_violations"}
    if not required.issubset(df.columns):
        sys.exit(f"[ERROR] {path} must contain columns: {required}")

    return df.sort_values("team_size").reset_index(drop=True)


def make_plot(
    x_vals,
    y_vals,
    color,
    ylabel,
    title,
    output,
    legend_label,
    ylim=None,
    quadratic_fit=False
):
    x_real = x_vals.to_numpy(dtype=float)
    y = y_vals.to_numpy(dtype=float)

    n = len(x_real)
    x_plot = np.arange(n)
    x_labels = x_vals.astype(str).tolist()

    fig, ax = plt.subplots(figsize=(max(8, n * 0.9), 6))
    fig.patch.set_facecolor("#F7F9FC")
    ax.set_facecolor("#F7F9FC")

    # Points
    ax.scatter(
        x_plot, y,
        color=color,
        edgecolors=EDGE_COLOR,
        linewidths=0.3,
        s=45,
        alpha=0.95,
        zorder=2
    )

    # Quadratic fit
    fit_label = None

    if quadratic_fit and len(x_real) >= 3:
        a, b, c = np.polyfit(x_real, y, 2)

        x_margin = 0.8

        x_smooth_real = np.linspace(
            x_real.min() - x_margin,
            x_real.max() + x_margin,
            400
        )

        y_smooth = a * x_smooth_real**2 + b * x_smooth_real + c

        # linear mapping from real x → plot coordinates
        scale = (x_plot[-1] - x_plot[0]) / (x_real[-1] - x_real[0])

        x_smooth_plot = ((x_smooth_real - x_real[0]) * scale) + x_plot[0]

        ax.plot(
            x_smooth_plot,
            y_smooth,
            color=FIT_COLOR,
            linestyle="--",
            linewidth=2,
            alpha=0.9,
            zorder=0,
            label="Quadratic fit"
        )

        fit_label = rf"$y = {a:.3f}x^2 {b:.3f}x {c:+.3f}$"

        ax.text(
            0.03, 0.94,
            fit_label,
            transform=ax.transAxes,
            fontsize=14,
            color="#1A252F",
            va="top",
            bbox=dict(
                facecolor="white",
                alpha=0.85,
                edgecolor="#C8D6E5",
                boxstyle="round,pad=0.3"
            )
        )

        print(f"[fit] {title}: y = {a:.6f}x² + {b:.6f}x + {c:.6f}")

    # Value labels
    offset = 0.06 if ylabel == "Feasible Ratio" else max(0.06 * max(y), 0.1)

    for i, val in enumerate(y):
        label = f"{val:.3f}" if ylabel == "Feasible Ratio" else f"{val:.2f}"
        ax.text(
            x_plot[i], val + offset, label,
            ha="center",
            va="bottom",
            fontsize=11,
            color=color,
            fontweight="bold"
        )

    # Grid & spines
    ax.yaxis.grid(True, color="#D0D8E4", linewidth=1.5, linestyle="--", zorder=0)
    ax.xaxis.grid(True, color="#D0D8E4", linewidth=1.5, linestyle="--", zorder=0)
    ax.set_axisbelow(True)

    for spine in ["top", "right"]:
        ax.spines[spine].set_visible(False)

    for spine in ["left", "bottom"]:
        ax.spines[spine].set_color("#C8D6E5")
        ax.spines[spine].set_linewidth(0.8)

    # Axes labels & ticks
    ax.set_xticks(x_plot)
    ax.set_xticklabels(
        x_labels,
        fontsize=11,
        fontfamily="DejaVu Sans",
        color="#2C3E50"
    )

    ax.set_xlabel(
        "Team Size",
        fontsize=16,
        fontweight="bold",
        color="#1A252F",
        labelpad=10
    )

    ax.set_ylabel(
        ylabel,
        fontsize=16,
        fontweight="bold",
        color="#1A252F",
        labelpad=15
    )

    ax.tick_params(axis="y", labelsize=11, colors="#2C3E50")
    ax.tick_params(axis="x", length=0)

    if ylim is not None:
        ax.set_ylim(*ylim)
    else:
        ymin = 0
        ymax = max(y) * 1.18 if max(y) > 0 else 1
        ax.set_ylim(ymin, ymax)

    ax.margins(x=0.03)

    # Title
    ax.set_title(
        title,
        fontsize=20,
        fontweight="bold",
        color="#1A252F",
        pad=16,
        fontfamily="DejaVu Sans"
    )

    # Legend
    handles = [
        mpatches.Patch(
            facecolor=color,
            edgecolor=EDGE_COLOR,
            linewidth=0.3,
            label=legend_label
        )
    ]

    if quadratic_fit:
        handles.append(
            plt.Line2D(
                [0], [0],
                color=FIT_COLOR,
                linestyle="--",
                linewidth=2,
                label="Quadratic fit"
            )
        )



  
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
    ylim=(0, min(1.0, df["avg_feasible_ratio"].max() * 1.18)),
    quadratic_fit=False
)

# Plot 2: Average total violations with quadratic fit
make_plot(
    x_vals=df["team_size"],
    y_vals=df["avg_total_violations"],
    color=COLOR_MAIN_2,
    ylabel="Average Total Violations",
    title="Average Total Violations by Team Size",
    output=OUTPUT_VIOLATIONS,
    legend_label="Average total violations",
    quadratic_fit=True
)
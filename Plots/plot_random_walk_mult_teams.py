import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

# ── Configuration ────────────────────────────────────────────────────────────
FILES = {
    8:  "../Results/random_walk/single_op/round_swap/random_walk_single_round_swap_8.csv",
    12: "../Results/random_walk/single_op/round_swap/random_walk_single_round_swap_12.csv",
    16: "../Results/random_walk/single_op/round_swap/random_walk_single_round_swap_16.csv",
}

OUTPUT = "graphs/random_walk_round_swap_multi_team_range.png"

MAX_STEP = 5000
XTICK_STEP = 1000

COLORS = {
    8:  "#2ECC71",
    12: "#3498DB",
    16: "#E74C3C",
}
# ─────────────────────────────────────────────────────────────────────────────

def load(path):
    if not os.path.exists(path):
        sys.exit(f"[ERROR] File not found: {path}")

    df = pd.read_csv(path)

    required = {
        "walk_id",
        "schedule_id",
        "team_size",
        "step",
        "running_avg_total"
    }

    missing = required - set(df.columns)
    if missing:
        sys.exit(f"[ERROR] Missing columns in {path}: {missing}")

    return df.sort_values(["walk_id", "step"]).reset_index(drop=True)


def summarize_team_file(path, team_size):
    df = load(path)

    df = df[df["team_size"] == team_size].copy()
    df = df[df["step"] <= MAX_STEP]

    if df.empty:
        sys.exit(f"[ERROR] No data found for team size {team_size} in {path}")

    summary = (
        df.groupby("step")
          .agg(
              min_running_avg=("running_avg_total", "min"),
              max_running_avg=("running_avg_total", "max"),
              mean_running_avg=("running_avg_total", "mean")
          )
          .reset_index()
    )

    n_walks = df["walk_id"].nunique()

    return summary, n_walks


# ── Plot ─────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(14, 6))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

legend_labels = []

for team_size, path in FILES.items():
    summary, n_walks = summarize_team_file(path, team_size)

    color = COLORS[team_size]

    # Range
    ax.fill_between(
        summary["step"],
        summary["min_running_avg"],
        summary["max_running_avg"],
        color=color,
        alpha=0.16,
        zorder=1
    )

    # Mean curve
    ax.plot(
        summary["step"],
        summary["mean_running_avg"],
        color=color,
        linewidth=2.0,
        alpha=0.95,
        zorder=2,
        label=f"{team_size} teams ({n_walks} walks)"
    )

# ── Grid & spines ────────────────────────────────────────────────────────────
ax.yaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
ax.xaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
ax.set_axisbelow(True)

for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)

for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

# ── Axes ─────────────────────────────────────────────────────────────────────
xticks = np.arange(0, MAX_STEP + XTICK_STEP, XTICK_STEP)

ax.set_xticks(xticks)
ax.set_xticklabels([f"{x:,}" for x in xticks])

ax.set_xlabel(
    "Step",
    fontsize=14,
    fontweight="bold",
    color="#1A252F",
    labelpad=10
)

ax.set_ylabel(
    "Average Total Violations",
    fontsize=14,
    fontweight="bold",
    color="#1A252F",
    labelpad=15
)

ax.tick_params(axis="x", labelsize=9, colors="#2C3E50")
ax.tick_params(axis="y", labelsize=9, colors="#2C3E50")

# ── Title ────────────────────────────────────────────────────────────────────
ax.set_title(
    "Random Walk Round Swap Average Violations",
    fontsize=18,
    fontweight="bold",
    color="#1A252F",
    pad=16,
    fontfamily="DejaVu Sans"
)

ax.legend(
    fontsize=11,
    loc="upper right",
    bbox_to_anchor=(1.0, 1.1),  
    framealpha=0.9,
    edgecolor="#C8D6E5",
    fancybox=False
)

plt.tight_layout()
plt.subplots_adjust(left=0.08)

plt.savefig(
    OUTPUT,
    dpi=180,
    bbox_inches="tight",
    facecolor=fig.get_facecolor()
)

print(f"[✓] Plot saved → {OUTPUT}")
plt.show()
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

# ── Configuration ────────────────────────────────────────────────────────────
TEAM_SIZE = 16

FILES = {
    "Home/Away Swap":
        "../Results/random_walk/single_op/home_away/random_walk_single_home_away_swap_16.csv",

    "Round Swap":
        "../Results/random_walk/single_op/round_swap/random_walk_single_round_swap_16.csv",

    "Match Swap":
        "../Results/random_walk/single_op/match_swap/random_walk_single_match_swap_16.csv",

    "Match Round Swap":
        "../Results/random_walk/single_op/match_round_swap/random_walk_single_match_round_swap_16.csv",
}

OUTPUT = "graphs/random_walk_16_all_operators_range.png"

MAX_STEP = 5000
XTICK_STEP = 1000

COLORS = {
    "Home/Away Swap": "#2ECC71",
    "Round Swap": "#3498DB",
    "Match Swap": "#E74C3C",
    "Match Round Swap": "#9B59B6",
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


def summarize_operator_file(path, operator_name):
    df = load(path)

    df = df[df["team_size"] == TEAM_SIZE].copy()
    df = df[df["step"] <= MAX_STEP]

    if df.empty:
        sys.exit(f"[ERROR] No data found for {operator_name} in {path}")

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

for operator_name, path in FILES.items():
    summary, n_walks = summarize_operator_file(path, operator_name)

    color = COLORS[operator_name]

    # Range
    ax.fill_between(
        summary["step"],
        summary["min_running_avg"],
        summary["max_running_avg"],
        color=color,
        alpha=0.14,
        zorder=1
    )

    # Mean curve
    ax.plot(
        summary["step"],
        summary["mean_running_avg"],
        color=color,
        linewidth=2,
        alpha=0.95,
        zorder=2,
        label=operator_name
    )

# ── Grid & spines ────────────────────────────────────────────────────────────
ax.yaxis.grid(True, color="#D0D8E4", linewidth=1.5, linestyle="--", zorder=0)
ax.xaxis.grid(True, color="#D0D8E4", linewidth=1.5, linestyle="--", zorder=0)
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
    fontsize=16,
    fontweight="bold",
    color="#1A252F",
    labelpad=10
)

ax.set_ylabel(
    "Average Total Violations",
    fontsize=16,
    fontweight="bold",
    color="#1A252F",
    labelpad=15
)

ax.tick_params(axis="x", labelsize=11, colors="#2C3E50")
ax.tick_params(axis="y", labelsize=11, colors="#2C3E50")

# ── Title ────────────────────────────────────────────────────────────────────
ax.set_title(
    f"Comparing Mutation Operators in Random Walks\n"
    f"({TEAM_SIZE} Teams)",
    fontsize=20,
    fontweight="bold",
    color="#1A252F",
    pad=16,
    fontfamily="DejaVu Sans"
)

# Reverse legend order
handles, labels = ax.get_legend_handles_labels()

ax.legend(
    handles[::-1],
    labels[::-1],
    fontsize=14,
    loc="lower right",
    #bbox_to_anchor=(1.0, 1.0),
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
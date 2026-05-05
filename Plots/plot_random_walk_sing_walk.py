import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

# ── Configuration ────────────────────────────────────────────────────────────
FILE   = "../Results/random_walk/single_instance/random_walk_16.csv"   # ← change to your CSV path
OUTPUT = "graphs/random_walk_16_running_avg.png"

COLOR_LINE      = "#2C3E50"
COLOR_FEASIBLE  = "#2ECC71"
EDGE_FEASIBLE   = "#1A7A43"

LINE_WIDTH      = 1.4
POINT_SIZE      = 18
POINT_ALPHA     = 0.85

XTICK_STEP      = 2_000
# ─────────────────────────────────────────────────────────────────────────────

def load(path):
    if not os.path.exists(path):
        sys.exit(f"[ERROR] File not found: {path}")

    df = pd.read_csv(path)

    required = {
        "schedule_id",
        "team_size",
        "step",
        "running_avg_total",
        "feasible"
    }

    missing = required - set(df.columns)
    if missing:
        sys.exit(f"[ERROR] Missing columns: {missing}")

    return df.sort_values("step").reset_index(drop=True)


MAX_STEP = 20_000  

df = load(FILE)
df = df[df["step"] <= MAX_STEP]

# Keep one instance only, in case the file contains multiple schedules
schedule_id = df["schedule_id"].iloc[0]
df = df[df["schedule_id"] == schedule_id].copy()

feasible = df[df["feasible"] == 1]

n_total = len(df)
n_feasible = len(feasible)
team_size = df["team_size"].iloc[0]

# ── Plot ─────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(14, 6))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

# Main running average curve
ax.plot(
    df["step"],
    df["running_avg_total"],
    color=COLOR_LINE,
    linewidth=LINE_WIDTH,
    alpha=0.85,
    zorder=2,
    label="Running average total violations"
)

# Feasible schedules marked on the curve
ax.scatter(
    feasible["step"],
    feasible["running_avg_total"],
    color=COLOR_FEASIBLE,
    edgecolors=EDGE_FEASIBLE,
    linewidths=0.5,
    s=POINT_SIZE,
    alpha=POINT_ALPHA,
    zorder=3,
    label=f"Feasible schedules  (n={n_feasible:,})"
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

# ── Axes labels & ticks ──────────────────────────────────────────────────────
max_step = int(df["step"].max())
xticks = np.arange(0, max_step + XTICK_STEP, XTICK_STEP)

ax.set_xticks(xticks)
ax.set_xticklabels([f"{x:,}" for x in xticks])

ax.set_xlabel(
    "Step",
    fontsize=11,
    fontweight="bold",
    color="#1A252F",
    labelpad=10
)

ax.set_ylabel(
    "Running Average Total Violations",
    fontsize=11,
    fontweight="bold",
    color="#1A252F",
    labelpad=15
)

ax.tick_params(axis="x", labelsize=9, colors="#2C3E50")
ax.tick_params(axis="y", labelsize=9, colors="#2C3E50")

# ── Title ────────────────────────────────────────────────────────────────────
ax.set_title(
    f"Random Walk Running Average Violations "
    f"(Team Size {team_size}, {n_total:,} steps)",
    fontsize=13,
    fontweight="bold",
    color="#1A252F",
    pad=16,
    fontfamily="DejaVu Sans"
)

# ── Legend ───────────────────────────────────────────────────────────────────
ax.legend(
    fontsize=9,
    loc="upper right",
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
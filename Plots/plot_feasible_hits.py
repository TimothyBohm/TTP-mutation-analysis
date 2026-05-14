import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

# ── Configuration ─────────────────────────────────────────
FILE = "../Results/random_walk/feasible_hits/feasible_hits_8.csv"
TOP_N = 160
OUTPUT = "graphs/feasible_schedule_frequency_8.png"
# ─────────────────────────────────────────────────────────

df = pd.read_csv(FILE)

# Aggregate counts
counts = (
    df.groupby("schedule_key")
      .size()
      .reset_index(name="times_found")
      .sort_values("times_found", ascending=False)
)

top = counts.head(TOP_N).reset_index(drop=True)

# Stats
total_hits = len(df)
unique_schedules = len(counts)

# X positions
x = range(1, len(top) + 1)

# ── Plot ─────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(12, 6))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

ax.bar(
    x,
    top["times_found"],
    color="#2C3E50",
    alpha=0.9,
    linewidth=0.8
)

# Labels
ax.set_xlabel(
    "Schedule Rank (Most Frequent -> Less Frequent)",
    fontsize=16,
    fontweight="bold",
    color="#1A252F",
    labelpad=10
)

ax.set_ylabel(
    "Frequency Count",
    fontsize=16,
    fontweight="bold",
    color="#1A252F",
    labelpad=15
)

# Title
ax.set_title(
    f"Most Frequently Visited Feasible Schedules\n"
    f"(8 Teams, 50 Walks, 1 000 000 Steps)",
    fontsize=20,
    fontweight="bold",
    color="#1A252F",
    pad=16,
    fontfamily="DejaVu Sans"
)

ax.yaxis.set_major_locator(MaxNLocator(integer=True))

# Ticks
ax.set_xticks(x[::5])
ax.tick_params(axis="x", labelsize=11, colors="#2C3E50")
ax.tick_params(axis="y", labelsize=11, colors="#2C3E50")

# Grid & spines
ax.yaxis.grid(True, color="#D0D8E4", linewidth=1.5, linestyle="--", zorder=0)
ax.set_axisbelow(True)

for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)

for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

# Text box
ax.text(
    0.98, 0.95,
    f"Total hits: {total_hits:,}\nUnique schedules: {unique_schedules:,}",
    transform=ax.transAxes,
    ha="right",
    va="top",
    fontsize=14,
    color="#1A252F",
    bbox=dict(
        facecolor="white",
        alpha=0.85,
        edgecolor="#C8D6E5",
        boxstyle="round,pad=0.3"
    )
)

plt.tight_layout()

plt.savefig(OUTPUT, dpi=180, bbox_inches="tight", facecolor=fig.get_facecolor())
print(f"[✓] Plot saved → {OUTPUT}")

plt.show()
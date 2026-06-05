import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

# ── Configuration ─────────────────────────────────────────
FILE = "../schedule_generation/combined_freq/dfs_4_combined.csv"
TOP_N = 160
OUTPUT = "graphs/generation_frequency_4_dfs_frequency.png"
# ─────────────────────────────────────────────────────────

# Read frequency file: count;schedule
rows = []

with open(FILE, "r") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue

        count_str, schedule_key = line.split(";", 1)
        rows.append({
            "schedule_key": " ".join(schedule_key.split()),
            "times_found": int(count_str)
        })

counts = (
    pd.DataFrame(rows)
      .sort_values("times_found", ascending=False)
      .reset_index(drop=True)
)

top = counts.head(TOP_N).reset_index(drop=True)

total_hits = counts["times_found"].sum()
unique_schedules = len(counts)

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

ax.set_title(
    "Most Frequently Generated Schedules\n"
    "(4 Teams, Frequency File)",
    fontsize=20,
    fontweight="bold",
    color="#1A252F",
    pad=16,
    fontfamily="DejaVu Sans"
)

ax.yaxis.set_major_locator(MaxNLocator(integer=True))
ax.set_xticks(list(x)[::5])

ax.tick_params(axis="x", labelsize=11, colors="#2C3E50")
ax.tick_params(axis="y", labelsize=11, colors="#2C3E50")

ax.yaxis.grid(True, color="#D0D8E4", linewidth=1.5, linestyle="--", zorder=0)
ax.set_axisbelow(True)

for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)

for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

ax.text(
    0.98, 0.95,
    f"Total generated: {total_hits:,}\nUnique schedules: {unique_schedules:,}",
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
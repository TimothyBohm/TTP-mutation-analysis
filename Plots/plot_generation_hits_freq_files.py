from collections import defaultdict
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

# ── Configuration ─────────────────────────────────────────
FILES = [
    "../schedule_generation/dfs/schedules_4_frequency_1.csv",
    "../schedule_generation/dfs/schedules_4_frequency_2.csv",
    "../schedule_generation/dfs/schedules_4_frequency_3.csv",
    "../schedule_generation/dfs/schedules_4_frequency_4.csv",
]

TOP_N = 160
OUTPUT_CSV = "../schedule_generation/dfs/frequency_4_combined.csv"
OUTPUT_GRAPH = "graphs/generation_frequency_4_dfs_combined.png"
TITLE_METHOD = "DFS"
# ─────────────────────────────────────────────────────────

# ── Combine frequency files ───────────────────────────────
frequencies = defaultdict(int)

for file_path in FILES:
    with open(file_path, "r") as file:
        for line in file:
            line = line.strip()

            if not line:
                continue

            count_str, schedule_key = line.split(";", 1)
            schedule_key = " ".join(schedule_key.split())

            frequencies[schedule_key] += int(count_str)

# Save combined frequency file
with open(OUTPUT_CSV, "w") as file:
    for schedule_key, count in sorted(frequencies.items()):
        file.write(f"{count};{schedule_key}\n")

# ── Prepare dataframe ─────────────────────────────────────
counts = (
    pd.DataFrame(
        [
            {"schedule_key": schedule_key, "times_found": count}
            for schedule_key, count in frequencies.items()
        ]
    )
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
    f"Most Frequently Generated Schedules\n"
    f"(4 Teams, {TITLE_METHOD})",
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
    f"Total generated: {total_hits:,}\n"
    f"Unique schedules: {unique_schedules:,}",
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
plt.savefig(
    OUTPUT_GRAPH,
    dpi=180,
    bbox_inches="tight",
    facecolor=fig.get_facecolor()
)

print(f"[✓] Combined frequency file saved → {OUTPUT_CSV}")
print(f"[✓] Plot saved → {OUTPUT_GRAPH}")
print(f"[✓] Total generated: {total_hits:,}")
print(f"[✓] Unique schedules: {unique_schedules:,}")

if unique_schedules > 160:
    print("[WARNING] More than 160 unique schedules found. Check normalization.")

plt.show()
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import sys
import os

# ── Configuration ────────────────────────────────────────────────────────────
FILE_1 = "Results/one_step/results_one_step_home_away_16.csv"          # ← change to your first CSV path
FILE_2 = "Results/one_step/results_one_step_round_swap_16.csv"          # ← change to your second CSV path
LABEL_1 = "Home/Away"        # ← legend label for file 1
LABEL_2 = "Round Swap"        # ← legend label for file 2
OUTPUT  = "feasible_ratio_comparison.png"

COLOR_DARK  = "#1B4F8A"   # deep navy  – taller bar (always behind)
COLOR_LIGHT = "#1CAD55"   # sky blue   – shorter bar (always in front)
EDGE_COLOR  = "#0D2B4E"
# ─────────────────────────────────────────────────────────────────────────────

def load(path):
    if not os.path.exists(path):
        sys.exit(f"[ERROR] File not found: {path}")
    df = pd.read_csv(path)
    required = {"schedule_id", "feasible_ratio"}
    if not required.issubset(df.columns):
        sys.exit(f"[ERROR] {path} must contain columns: {required}")
    return df[["schedule_id", "feasible_ratio"]].copy()

df1 = load(FILE_1).iloc[30:130]
df2 = load(FILE_2)

# Merge on schedule_id so both files align
merged = pd.merge(df1, df2, on="schedule_id", suffixes=("_1", "_2"))
merged = merged.sort_values("schedule_id").reset_index(drop=True)



ids     = merged["schedule_id"].astype(str).tolist()
ratio1  = merged["feasible_ratio_1"].values
ratio2  = merged["feasible_ratio_2"].values
n       = len(ids)
x       = np.arange(n)

# ── Assign colours: larger value → dark (behind), smaller → light (front) ──
bar_bottom = np.minimum(ratio1, ratio2)
bar_top    = np.maximum(ratio1, ratio2)

# Which file is taller for each schedule?
file1_is_taller = ratio1 >= ratio2


# ── Plot ─────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(max(12, n * 0.1), 6))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

BAR_W = 0.6

# Draw the TALLER bar first (behind), then the SHORTER bar on top
ax.bar(x, bar_top,    width=BAR_W, color=COLOR_DARK,  edgecolor=EDGE_COLOR,
       linewidth=0.6, label="_nolegend_", zorder=2)
ax.bar(x, bar_bottom, width=BAR_W, color=COLOR_LIGHT, edgecolor=EDGE_COLOR,
       linewidth=0.6, label="_nolegend_", zorder=3)

# ── Subtle grid ───────────────────────────────────────────────────────────────
ax.yaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
ax.set_axisbelow(True)

# ── Axes labels & ticks ───────────────────────────────────────────────────────
ax.set_xticks([])
ax.set_xlabel("Schedule ID", fontsize=16, fontweight="bold",
              color="#1A252F", labelpad=5)
ax.set_ylabel("Feasible Ratio", fontsize=16, fontweight="bold",
              color="#1A252F", labelpad=5)
ax.set_ylim(0, min(1.0, bar_top.max() * 1.15) if bar_top.max() > 0 else 1.0)
ax.tick_params(axis="y", labelsize=9, colors="#2C3E50")
ax.tick_params(axis="x", length=0)
ax.margins(x=0)
# ── Title ─────────────────────────────────────────────────────────────────────
ax.set_title("Home/Away VS Round Swap (N = 16)",
             fontsize=20, fontweight="bold", color="#1A252F",
             pad=16, fontfamily="DejaVu Sans")

# ── Legend: map colours back to file labels ───────────────────────────────────
patch_dark  = mpatches.Patch(facecolor=COLOR_DARK,  edgecolor=EDGE_COLOR,
                              linewidth=0.6, label=f"{LABEL_1} (larger value)")
patch_light = mpatches.Patch(facecolor=COLOR_LIGHT, edgecolor=EDGE_COLOR,
                              linewidth=0.6, label=f"{LABEL_2} (smaller value)")

# Build a smarter legend that shows which colour = which file
# (colour assignment flips per bar, so we explain the overlap mechanic)
note_dark  = mpatches.Patch(facecolor=COLOR_DARK,  edgecolor=EDGE_COLOR,
                             linewidth=0.6, label=LABEL_1)
note_light = mpatches.Patch(facecolor=COLOR_LIGHT, edgecolor=EDGE_COLOR,
                             linewidth=0.6, label=LABEL_2)

legend = ax.legend(
    handles=[note_dark, note_light],
    title="Data source",
    title_fontsize=12,
    fontsize=11,
    loc="upper right",
    framealpha=0.9,
    edgecolor="#C8D6E5",
    fancybox=False,
)
legend.get_title().set_color("#2C3E50")

# ── Spine styling ─────────────────────────────────────────────────────────────
for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)
for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

plt.tight_layout()
plt.savefig(OUTPUT, dpi=180, bbox_inches="tight", facecolor=fig.get_facecolor())
print(f"[✓] Plot saved → {OUTPUT}")
plt.show()
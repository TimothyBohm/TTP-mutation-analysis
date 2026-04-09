import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import sys
import os

# ── Configuration ────────────────────────────────────────────────────────────
FILE_1  = "Results/two_step/global_summary_two_step_home_away.csv"         # ← change to your first CSV path
FILE_2  = "Results/two_step/global_summary_two_step_round_swap.csv"         # ← change to your second CSV path
LABEL_1 = "Home/Away operator"        # ← legend label for file 1 (always larger value)
LABEL_2 = "Round Swap operator"        # ← legend label for file 2 (always smaller value)
OUTPUT  = "team_size_avg_total.png"

COLOR_DARK  = "#1B4F8A"   # deep navy  – taller bar (behind)
COLOR_LIGHT = "#1CAD55"   # sky blue   – shorter bar (in front)
EDGE_COLOR  = "#0D2B4E"
# ─────────────────────────────────────────────────────────────────────────────

def load(path):
    if not os.path.exists(path):
        sys.exit(f"[ERROR] File not found: {path}")
    df = pd.read_csv(path)
    required = {"team_size", "avg_total"}
    if not required.issubset(df.columns):
        sys.exit(f"[ERROR] {path} must contain columns: {required}")
    # Average avg_total per team_size in case there are multiple rows
    return df.groupby("team_size", as_index=False)["avg_total"].mean()

df1 = load(FILE_1)
df2 = load(FILE_2)

# Merge on team_size
merged = pd.merge(df1, df2, on="team_size", suffixes=("_1", "_2"))
merged = merged.sort_values("team_size").reset_index(drop=True)

team_sizes = merged["team_size"].astype(str).tolist()
ratio1     = merged["avg_total_1"].values   # always larger (behind)
ratio2     = merged["avg_total_2"].values   # always smaller (in front)
n          = len(team_sizes)
x          = np.arange(n)

# ── Plot ──────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(max(8, n * 0.9), 6))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

BAR_W = 0.5

# Draw file 2 first (taller, behind), then file 1 on top (shorter, in front)
ax.bar(x, ratio2, width=BAR_W, color=COLOR_LIGHT, edgecolor=EDGE_COLOR,
       linewidth=0.6, zorder=2)
ax.bar(x, ratio1, width=BAR_W, color=COLOR_DARK,  edgecolor=EDGE_COLOR,
       linewidth=0.6, zorder=3)

# ── Value labels ──────────────────────────────────────────────────────────────
for i, (r1, r2) in enumerate(zip(ratio1, ratio2)):
    # File 2 (taller) — label above the bar
    ax.text(i, r2 + (r2 * 0.01), f"{r2:.3f}", ha="center", va="bottom",
            fontsize=9, color=COLOR_DARK, fontweight="bold")
    # File 1 (shorter) — label inside the bar near the top
    ax.text(i, r1 - (r1 * 0.02), f"{r1:.3f}", ha="center", va="top",
            fontsize=9, color="white", fontweight="bold")

# ── Grid & spines ─────────────────────────────────────────────────────────────
ax.yaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
ax.set_axisbelow(True)
for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)
for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

# ── Axes labels & ticks ───────────────────────────────────────────────────────
ax.set_xticks(x)
ax.set_xticklabels(team_sizes, fontsize=11, fontfamily="DejaVu Sans",
                   color="#2C3E50")
ax.set_xlabel("Team Size", fontsize=14, fontweight="bold",
              color="#1A252F", labelpad=10)
ax.set_ylabel("Avg Total Violation Count", fontsize=14, fontweight="bold",
              color="#1A252F", labelpad=15)
ax.set_ylim(0, max(ratio1.max(), ratio2.max()) * 1.18)
ax.tick_params(axis="y", labelsize=9, colors="#2C3E50")
ax.tick_params(axis="x", length=0)

# ── Title ─────────────────────────────────────────────────────────────────────
ax.set_title("Avg Total Violation Count by Team Size (2 step)",
             fontsize=20, fontweight="bold", color="#1A252F",
             pad=16, fontfamily="DejaVu Sans")

# ── Legend ────────────────────────────────────────────────────────────────────
patch_1 = mpatches.Patch(facecolor=COLOR_DARK,  edgecolor=EDGE_COLOR,
                          linewidth=0.6, label=LABEL_1)
patch_2 = mpatches.Patch(facecolor=COLOR_LIGHT, edgecolor=EDGE_COLOR,
                          linewidth=0.6, label=LABEL_2)
legend = ax.legend(
    handles=[patch_1, patch_2],
    title="Data source",
    title_fontsize=8.5,
    fontsize=9,
    loc="upper right",
    framealpha=0.9,
    edgecolor="#C8D6E5",
    fancybox=False,bbox_to_anchor=(1, 1.05),
)
legend.get_title().set_color("#2C3E50")

plt.tight_layout()
plt.subplots_adjust(left=0.12)
plt.savefig(OUTPUT, dpi=180, bbox_inches="tight", facecolor=fig.get_facecolor())
print(f"[✓] Plot saved → {OUTPUT}")
plt.show()
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import sys
import os

# ── Configuration ────────────────────────────────────────────────────────────
FILE   = "Results/random_walk/random_walk_8.csv"              # ← change to your CSV path
OUTPUT = "walk_violations.png"

COLOR_FEASIBLE     = "#2ECC71"   # green  – feasible steps
COLOR_INFEASIBLE   = "#DF2C18"   # red    – infeasible steps
EDGE_FEASIBLE      = "#1A7A43"
EDGE_INFEASIBLE    = "#922B21"
POINT_SIZE         = 8           # reduce if too crowded, increase if too sparse
POINT_ALPHA        = 0.55
# ─────────────────────────────────────────────────────────────────────────────

def load(path):
    if not os.path.exists(path):
        sys.exit(f"[ERROR] File not found: {path}")
    df = pd.read_csv(path)
    required = {"step", "total", "feasible"}
    if not required.issubset(df.columns):
        sys.exit(f"[ERROR] {path} must contain columns: {required}")
    return df.sort_values("step").reset_index(drop=True)

df = load(FILE).iloc[50:100]  

feasible   = df[df["feasible"] == 1]
infeasible = df[df["feasible"] == 0]

n_total      = len(df)
n_feasible   = len(feasible)
n_infeasible = len(infeasible)

# ── Plot ──────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(14, 6))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

# Draw connecting line (behind points)
ax.plot(df["step"], df["total"],
        linewidth=1.2,
        alpha=0.6,
        zorder=1)

# Infeasible first (behind), feasible on top
ax.scatter(infeasible["step"], infeasible["total"],
           color=COLOR_INFEASIBLE, edgecolors=EDGE_INFEASIBLE,
           linewidths=0.3, s=POINT_SIZE, alpha=POINT_ALPHA,
           zorder=2, label=f"Infeasible  (n={n_infeasible:,})")

ax.scatter(feasible["step"], feasible["total"],
           color=COLOR_FEASIBLE, edgecolors=EDGE_FEASIBLE,
           linewidths=0.3, s=POINT_SIZE, alpha=POINT_ALPHA,
           zorder=3, label=f"Feasible  (n={n_feasible:,})")

# ── Grid & spines ─────────────────────────────────────────────────────────────
ax.yaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
ax.xaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
ax.set_axisbelow(True)
for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)
for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

# ── Axes labels & ticks ───────────────────────────────────────────────────────
ax.set_xticks([])   # step numbers hidden as requested
ax.set_xlabel("Steps", fontsize=11, fontweight="bold",
              color="#1A252F", labelpad=10)
ax.set_ylabel("Total Violations", fontsize=11, fontweight="bold",
              color="#1A252F", labelpad=15)
ax.tick_params(axis="y", labelsize=9, colors="#2C3E50")

# ── Title ─────────────────────────────────────────────────────────────────────
schedule_id = df["schedule_id"].iloc[0]
team_size   = df["team_size"].iloc[0] if "team_size" in df.columns else "?"
ax.set_title(f"Total Violations Along Walk  (Team Size {team_size},  {n_total:,} steps)",
             fontsize=13, fontweight="bold", color="#1A252F",
             pad=16, fontfamily="DejaVu Sans")

# ── Legend ────────────────────────────────────────────────────────────────────
legend = ax.legend(fontsize=9, loc="upper right", framealpha=0.9,
                   edgecolor="#C8D6E5", fancybox=False)

plt.tight_layout()
plt.subplots_adjust(left=0.08)
plt.savefig(OUTPUT, dpi=180, bbox_inches="tight", facecolor=fig.get_facecolor())
print(f"[✓] Plot saved → {OUTPUT}")
plt.show()
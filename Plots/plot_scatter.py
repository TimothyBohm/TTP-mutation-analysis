import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import sys
import os
from scipy import stats

# ── Configuration ────────────────────────────────────────────────────────────
FILE    = "Results/one_step/results_one_step_home_away_16.csv"                        # ← change to your CSV path
OUTPUT  = "feasible_ratio_vs_avg_total.png"

SCATTER_COLOR  = "#2E86C1"   # medium blue
SCATTER_EDGE   = "#1A5276"
SCATTER_ALPHA  = 0.65
REG_COLOR      = "#E74C3C"   # red regression line
CI_COLOR       = "#FADBD8"   # light red confidence interval band
# ─────────────────────────────────────────────────────────────────────────────

def load(path):
    if not os.path.exists(path):
        sys.exit(f"[ERROR] File not found: {path}")
    df = pd.read_csv(path)
    required = {"feasible_ratio", "avg_total"}
    if not required.issubset(df.columns):
        sys.exit(f"[ERROR] {path} must contain columns: {required}")
    return df[["schedule_id", "feasible_ratio", "avg_total", "team_size"]].copy()

df = load(FILE)
df = df.dropna(subset=["feasible_ratio", "avg_total"])

x = df["feasible_ratio"].values
y = df["avg_total"].values

# ── Regression ────────────────────────────────────────────────────────────────
slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
x_line = np.linspace(x.min(), x.max(), 300)
y_line = slope * x_line + intercept

# 95% confidence interval around the regression line
n        = len(x)
x_mean   = x.mean()
se_line  = std_err * np.sqrt(1/n + (x_line - x_mean)**2 / np.sum((x - x_mean)**2))
t_crit   = stats.t.ppf(0.975, df=n - 2)
ci_upper = y_line + t_crit * se_line
ci_lower = y_line - t_crit * se_line

# ── Plot ──────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(10, 6))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

# Confidence interval band
ax.fill_between(x_line, ci_lower, ci_upper, color=CI_COLOR,
                alpha=0.6, zorder=1, label="95% Confidence Interval")

# Regression line
ax.plot(x_line, y_line, color=REG_COLOR, linewidth=2.0, zorder=3,
        label=f"Regression  (y = {slope:+.3f}x {'+' if intercept >= 0 else ''}{intercept:.3f})")

# Scatter points
ax.scatter(x, y, color=SCATTER_COLOR, edgecolors=SCATTER_EDGE,
           linewidths=0.5, s=55, alpha=SCATTER_ALPHA, zorder=4,
           label="Schedule")

# ── Annotations ───────────────────────────────────────────────────────────────
stats_text = (
    f"$R^2$ = {r_value**2:.4f}\n"
    f"$r$  = {r_value:.4f}\n"
    f"$p$  = {p_value:.2e}\n"
    f"$n$  = {n}"
)
ax.text(0.98, 0.97, stats_text, transform=ax.transAxes,
        fontsize=9, verticalalignment="top", horizontalalignment="right",
        bbox=dict(boxstyle="round,pad=0.45", facecolor="white",
                  edgecolor="#C8D6E5", alpha=0.9),
        fontfamily="DejaVu Sans", color="#1A252F")

# ── Grid & spines ─────────────────────────────────────────────────────────────
ax.yaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
ax.xaxis.grid(True, color="#D0D8E4", linewidth=0.7, linestyle="--", zorder=0)
ax.set_axisbelow(True)
for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)
for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

# ── Labels & title ────────────────────────────────────────────────────────────
ax.set_xlabel("Feasible Ratio", fontsize=11, fontweight="bold",
              color="#1A252F", labelpad=10)
ax.set_ylabel("Avg Total Violation Count", fontsize=11, fontweight="bold",
              color="#1A252F", labelpad=10)
ax.set_title("Correlation: Feasible Ratio vs Avg Total Violation Count",
             fontsize=13, fontweight="bold", color="#1A252F",
             pad=16, fontfamily="DejaVu Sans")
ax.tick_params(labelsize=9, colors="#2C3E50")

ax.legend(fontsize=9, loc="upper left", framealpha=0.9,
          edgecolor="#C8D6E5", fancybox=False)

plt.tight_layout()
plt.savefig(OUTPUT, dpi=180, bbox_inches="tight", facecolor=fig.get_facecolor())
print(f"[✓] Plot saved → {OUTPUT}")
plt.show()
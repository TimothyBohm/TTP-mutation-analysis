import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import MaxNLocator

# ── Configuration ─────────────────────────────────────────
FILE = "../Results/random_walk/transition_matrix/transition_matrix_8.csv"
OUTPUT = "graphs/transition_matrix_2_8.png"
# ─────────────────────────────────────────────────────────

df = pd.read_csv(FILE)

required = {"previous_total", "current_total", "count"}
missing = required - set(df.columns)

if missing:
    raise ValueError(f"Missing columns: {missing}")

# Only transitions where the next/current schedule is feasible
to_feasible = (
    df[df["current_total"] == 0]
      .sort_values("count", ascending=False)
      .reset_index(drop=True)
)

print("Transitions into feasible schedules:")
print(to_feasible)

total_feasible_transitions = to_feasible["count"].sum()

print(f"\nTotal transitions into feasible schedules: {total_feasible_transitions:,}")

# ── Plot ─────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(10, 6))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

ax.bar(
    to_feasible["previous_total"],
    to_feasible["count"],
    width=0.92,
    color="#2C3E50",
    alpha=0.9,
    linewidth=0,
    label="Transitions to feasible"
)

# ── Fit curve: y = a / x + b ──────────────────────────────
x = to_feasible["previous_total"].to_numpy(dtype=float)
y = to_feasible["count"].to_numpy(dtype=float)

mask = x > 0
x_fit = x[mask]
y_fit = y[mask]

if len(x_fit) >= 2:
    inv_x = 1 / x_fit
    a, b = np.polyfit(inv_x, y_fit, 1)

    # Highest bar height
    y_max = y.max()

    # Start x where curve reaches highest bar
    x_start = max(0.01, a / (y_max - b))

    x_smooth = np.linspace(x_start, x.max(), 300)
    y_smooth = a / x_smooth + b

    ax.plot(
        x_smooth,
        y_smooth,
        color="#DF2C18",
        linestyle="--",
        linewidth=2,
        alpha=0.95,
        label=rf"$y = {a:.2f}/x {b:+.2f}$"
    )

    print(f"[fit] y = {a:.6f}/x + {b:.6f}")

# ── Labels ────────────────────────────────────────────────
ax.set_xlabel(
    "Previous Total Violations",
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

ax.xaxis.set_major_locator(MaxNLocator(integer=True))

ax.tick_params(axis="x", labelsize=11, colors="#2C3E50")
ax.tick_params(axis="y", labelsize=11, colors="#2C3E50")

# ── Title ─────────────────────────────────────────────────
ax.set_title(
    "Violation Counts that Lead to Feasible Schedules\n"
    "(8 Teams, 10M Steps)",
    fontsize=20,
    fontweight="bold",
    color="#1A252F",
    pad=16,
    fontfamily="DejaVu Sans"
)

# ── Grid & spines ─────────────────────────────────────────
ax.yaxis.grid(True, color="#D0D8E4", linewidth=1.5, linestyle="--", zorder=0)
ax.set_axisbelow(True)

for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)

for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

# ── Legend ────────────────────────────────────────────────
ax.legend(
    fontsize=14,
    loc="upper right",
    framealpha=0.9,
    edgecolor="#C8D6E5",
    fancybox=False
)

ax.text(
    0.98, 0.70,
    f"Total feasible transitions: {total_feasible_transitions:,}",
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
    OUTPUT,
    dpi=180,
    bbox_inches="tight",
    facecolor=fig.get_facecolor()
)

print(f"[✓] Plot saved → {OUTPUT}")

plt.show()
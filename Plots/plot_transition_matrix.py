import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# ── Configuration ─────────────────────────────────────────
FILE = "../Results/random_walk/transition_matrix/transition_matrix_8.csv"
OUTPUT = "graphs/transition_matrix_8.png"
# ─────────────────────────────────────────────────────────

# Load data
df = pd.read_csv(FILE)

# Create matrix
matrix = df.pivot(
    index="current_total",
    columns="previous_total",
    values="count"
).fillna(0)

# ── Plot ─────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(10, 8))
fig.patch.set_facecolor("#F7F9FC")
ax.set_facecolor("#F7F9FC")

im = ax.imshow(
    matrix.values,
    origin="lower",
    aspect="auto"
)

# ── Axis ticks every 2 ──────────────────────────────────

# X-axis
x_positions = np.arange(0, len(matrix.columns), 2)

ax.set_xticks(x_positions)
ax.set_xticklabels(
    [matrix.columns[i] for i in x_positions],
    rotation=90,
    fontsize=11,
    color="#2C3E50"
)

# Y-axis
y_positions = np.arange(0, len(matrix.index), 2)

ax.set_yticks(y_positions)
ax.set_yticklabels(
    [matrix.index[i] for i in y_positions],
    fontsize=11,
    color="#2C3E50"
)

# ── Grid & spines ────────────────────────────────────────
for spine in ["top", "right"]:
    ax.spines[spine].set_visible(False)

for spine in ["left", "bottom"]:
    ax.spines[spine].set_color("#C8D6E5")
    ax.spines[spine].set_linewidth(0.8)

# ── Labels ───────────────────────────────────────────────
ax.set_xlabel(
    "Previous Total Violations",
    fontsize=16,
    fontweight="bold",
    color="#1A252F",
    labelpad=10
)

ax.set_ylabel(
    "Current Total Violations",
    fontsize=16,
    fontweight="bold",
    color="#1A252F",
    labelpad=15
)

# ── Title ────────────────────────────────────────────────
ax.set_title(
    "Violation Count Transition Matrix (8 Teams, 10M Steps)",
    fontsize=20,
    fontweight="bold",
    color="#1A252F",
    pad=16,
    fontfamily="DejaVu Sans"
)

# ── Colorbar ─────────────────────────────────────────────
cbar = plt.colorbar(im, ax=ax)

cbar.set_label(
    "Transition Frequency",
    fontsize=16,
    fontweight="bold",
    color="#1A252F",
    labelpad=12
)

cbar.ax.tick_params(labelsize=11, colors="#2C3E50")

# Make colorbar outline match thesis style
cbar.outline.set_linewidth(0.8)
cbar.outline.set_edgecolor("#C8D6E5")

# ── Layout ───────────────────────────────────────────────
plt.tight_layout()

# Save
plt.savefig(
    OUTPUT,
    dpi=180,
    bbox_inches="tight",
    facecolor=fig.get_facecolor()
)

print(f"[✓] Plot saved → {OUTPUT}")

plt.show()
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

FILE = "../Results/random_walk/transition_matrix/transition_matrix_16.csv"
OUTPUT = "graphs/transition_matrix_3_16.png"

df = pd.read_csv(FILE)

# Total outgoing transitions from each previous violation count
total_from_previous = (
    df.groupby("previous_total")["count"]
      .sum()
      .reset_index(name="total_transitions")
)

# Transitions that go to feasible schedules
to_feasible = (
    df[df["current_total"] == 0]
      .groupby("previous_total")["count"]
      .sum()
      .reset_index(name="feasible_transitions")
)

# Combine and compute probability
prob = total_from_previous.merge(
    to_feasible,
    on="previous_total",
    how="left"
)

prob["feasible_transitions"] = prob["feasible_transitions"].fillna(0)
prob["probability"] = prob["feasible_transitions"] / prob["total_transitions"]

print(prob.sort_values("probability", ascending=False))

# ── Plot ─────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(10, 6))

ax.bar(
    prob["previous_total"],
    prob["probability"],
    label="P(next schedule is feasible)"
)

ax.set_xlabel("Previous Total Violations", fontsize=16)
ax.set_ylabel("Probability of Feasible Next Step", fontsize=16)

ax.set_title(
    "Probability of Reaching a Feasible Schedule (16 teams, 10M steps)",
    fontsize=18,
    fontweight="bold"
)

ax.xaxis.set_major_locator(MaxNLocator(integer=True))


ax.legend(fontsize=11)
ax.grid(axis="y", linestyle="--", alpha=0.6)

plt.tight_layout()
plt.savefig(OUTPUT, dpi=180)
print(f"[✓] Plot saved → {OUTPUT}")

plt.show()
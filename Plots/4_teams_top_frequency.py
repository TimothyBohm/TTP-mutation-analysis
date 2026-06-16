# ── Configuration ─────────────────────────────────────────
FILE = "../schedule_generation/4_teams_10k/stacked_dist/100k.csv"
TOP_N = 3
# ─────────────────────────────────────────────────────────

schedules = []

with open(FILE, "r") as file:
    for line in file:
        line = line.strip()

        if not line:
            continue

        count_str, schedule = line.split(";", 1)

        schedules.append(
            {
                "frequency": int(count_str),
                "schedule": " ".join(schedule.split())
            }
        )

schedules.sort(key=lambda x: x["frequency"], reverse=True)

print(f"Top {TOP_N} most frequent schedules:\n")

for rank, item in enumerate(schedules[:TOP_N], start=1):
    #print(f"Rank {rank}")
    print(f"Frequency: {item['frequency']:,} ||| Schedule: {item['schedule']}")
    print()
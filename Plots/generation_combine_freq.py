from collections import defaultdict


INPUT_FILES = [
    "../schedule_generation/dfs/schedules_4_frequency_1.csv",
    "../schedule_generation/dfs/schedules_4_frequency_2.csv",
    "../schedule_generation/dfs/schedules_4_frequency_3.csv",
    "../schedule_generation/dfs/schedules_4_frequency_4.csv",
]

OUTPUT_FILE = "../schedule_generation/combined_freq/dfs_4_combined.csv"

# COMBINE

frequencies = defaultdict(int)

for filename in INPUT_FILES:
    print(f"Reading {filename}")

    with open(filename, "r") as file:
        for line in file:
            line = line.strip()

            if not line:
                continue

            count_str, schedule = line.split(";", 1)

            frequencies[schedule] += int(count_str)

# SAVE

with open(OUTPUT_FILE, "w") as file:
    for schedule, count in sorted(frequencies.items()):
        file.write(f"{count};{schedule}\n")

print(f"\nCombined schedules: {len(frequencies)}")
print(f"Saved to: {OUTPUT_FILE}")

if len(frequencies) > 160:
    print("[WARNING] More than 160 unique schedules found.")
# TTP-Mutation-Analysis

This repository contains experimental tools and mutation-operator analysis for the **Traveling Tournament Problem (TTP)**. The project was developed as part of a Bachelor thesis at **Vrije Universiteit Amsterdam** under the supervision of **Daan van den Berg**.

The repository builds upon the original **TTP-DFS** implementation developed by **Bas Loyen et al.**, specifically the following files have been used from his repository {TTP.py, calc.py, helper.py, and run.py}:

https://github.com/OdyMeister/TTP-DFS

The original DFS-based schedule generator is used to generate feasible Traveling Tournament Problem schedules, which are then used for large-scale random walk experiments and mutation-operator analysis.

/mutation_analysis contains all of the implementation, while /plots is used for all the neccesarry plotting.

---

# Project Overview

Implemented mutation operators include:

- TeamSwap
- RoundSwap
- HomeAwaySwap
- MatchSwap
- MatchRoundSwap

---

### Python Schedule Generation

The repository includes the original DFS-based TTP schedule generator from the TTP-DFS project.

Generates all normalized (-N) schedules for `n=4` and saves them to the folder `Schedules_All`:

```sh
python3 run.py -N -s=All 4
```

---

### Python Schedule Generation

The repository also contains a C++ implementation of randomized schedule-generation algorithms.

Compile: g++ -std=c++17 main.cpp ../mutation_analysis/schedule.cpp -o run -O3 -Wall -Wextra

Command line arguments:
    ./run [teams] [num_schedules] [output_file] [method] [seed] [--append]

Arguments:
    teams           Number of teams (must be even and >= 4)
    num_schedules   Number of schedules to generate
    output_file     CSV file to save schedules
    method          methods: dfs or randdfs
    seed            Random seed (optional, default = 42)
    --append        Append schedules to existing file instead of overwriting

Examples:
    Generate 1000 schedules for 6 teams using DFS:
        ./run 6 1000 dfs/schedules_6.csv dfs

    Append 1,000 additional schedules to an existing file:
        ./run 8 1000 rand_dfs/schedules_8.csv randdfs 42 --append

---

### Run Random Walk Experiments

Random walk experiments are executed through functions defined in `main.cpp`.

To run an experiment:

1. Open `main.cpp`
2. Uncomment or add the desired experiment function call
3. Compile the project

```sh
g++ -std=c++17 *.cpp -o run -Wall -Wextra
```

4. Run the executable

```sh
./run
```

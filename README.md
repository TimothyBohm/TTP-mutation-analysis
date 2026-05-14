# TTP-Mutation-Analysis

This repository contains experimental tools and mutation-operator analysis for the **Traveling Tournament Problem (TTP)**. The project was developed as part of a Bachelor thesis at **Vrije Universiteit Amsterdam** under the supervision of **Daan van den Berg**.

The repository builds upon the original **TTP-DFS** implementation developed by **Bas Loyen et al.**, this includes the following files {TTP.py, calc.py, helper.py, and run.py}:

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

### Generate Schedules

The repository includes the original DFS-based TTP schedule generator from the TTP-DFS project.

Generates all normalized (-N) schedules for `n=4` and saves them to the folder `Schedules_All`:

```sh
python3 run.py -N -s=All 4
```


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

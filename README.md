## TTP-DFS

This repository builds upon the TTP-DFS implementation originally developed by Bas Loyen et al.:

https://github.com/OdyMeister/TTP-DFS

The original code is used as a foundation for experiments conducted in a Bachelor thesis at Vrije Universiteit Amsterdam under the supervision of Daan van den Berg.

Additional work in this repository focuses on testing and analyzing mutation operators on Traveling Tournament Problem schedules.
DFS pruning algorithm for finding all valid TTP schedules.

## Usage

### Generate Schedules

Generates all normalized (-N) schedules for `n=4` and saves them to the folder `Schedules_All`:

```sh
python3 run.py -N -s=All 4
```

### Calculate Differences

Calculates the differences between the normalized schedules and saves them to the folder `Differences`:

```sh
python3 calc.py ./Schedules/Schedules_All/All-4.csv 4
```

### Generate Plot

Generates the plot from the paper and saves it to the folder `Plots`:

```sh
python3 plot.py
```
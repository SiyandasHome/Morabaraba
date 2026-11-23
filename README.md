# Morabaraba: Minimax vs Heuristic AI (C++)

A C++ simulator of **Morabaraba**, a traditional Southern African strategy board game, in which a **depth-limited Minimax** player competes against a **rule-based heuristic** player. Built to compare the two decision-making approaches across different starting conditions.

> Team project by **Siyanda Ngwenya** and **Relebohile Matoetoe**, Wits School of Electrical and Information Engineering.

## Highlights

- Full game engine: placement phase, movement phase, mill detection, piece removal and terminal states. The flying phase is excluded by the project brief.
- **Minimax search** (depth 4) with a points-based evaluation function.
- **Heuristic player** that forms mill pairs, blocks the opponent and scores one-move look-ahead states.
- **Batch simulation**: reads game setups from a file and logs every move in a strict output format.
- Modular object-oriented design across three modules: gameboard, player and Minimax.

## Results

Each starting condition was simulated 15 times (60 games in total).

| Pieces per player | Minimax wins | Heuristic wins | Draws | Minimax win rate |
|---|---|---|---|---|
| 4 | 10 | 3 | 2 | 66.67% |
| 6 | 13 | 2 | 0 | 86.67% |
| 9 | 13 | 2 | 0 | 86.67% |
| 12 | 13 | 1 | 1 | 86.67% |
| **Total** | **49** | **8** | **3** | **81.67%** |

**Takeaway:** with few pieces both players rely on similar tactics, so look-ahead adds little. As the piece count grows, Minimax gets a clear edge by planning placements and blocking mills, at the cost of more computation.

## How it works

### Board
The board is a static 49-element array that mirrors the 7×7 grid layout. Three concentric squares give 24 playable intersections, and the other slots are marked invalid. The board stores the 20 possible mills (horizontal, vertical and diagonal) and exposes functions for mill detection, move generation, adjacency and scoring.

### Heuristic player
- **Placement:** random first piece, then complete a mill if possible, otherwise build pairs that can become mills, otherwise place randomly.
- **Movement:** prefer a move that creates a potential mill, otherwise simulate each legal move one step ahead and pick the best score.

### Minimax player
- Builds a game tree of simulated boards for placement, movement and removal decisions, using **depth 4**.
- Maximises its own evaluation minus the opponent's and back-propagates the best score to the current state.
- Evaluation rewards forming mills, piece count and well-connected intersections, and penalises the opponent's potential mills.

### Game end conditions
A player loses when reduced to fewer than three pieces or when left with no legal moves. The game is a draw when neither side can move, both fall below three pieces, or **250 moves** pass without a result.

## Project structure

```
morabaraba-ai/
├── src/
│   ├── main.cpp         # game loop, terminal-state checks, file I/O
│   ├── gameboard.h/.cpp # board, mills, move generation, evaluation
│   ├── Player.h/.cpp    # player state + heuristic algorithm
│   └── Min_max.h/.cpp   # Minimax search
├── docs/
│   ├── Final_Report.pdf
│   └── flowchart.png
├── input.txt            # example input
└── README.md
```

## Build and run

Requires a C++11 (or newer) compiler such as `g++`.

```bash
g++ -std=c++17 -O2 src/main.cpp src/gameboard.cpp src/Player.cpp src/Min_max.cpp -o morabaraba
./morabaraba
```

### Input
`input.txt` (in the working directory) holds one starting piece count per line. Each number is one game:

```
4
6
9
12
```

### Output
Moves are written to `Morabaraba_Results.txt`, followed by the winner. `Alg_1` is the heuristic player and `Alg_2` is the Minimax player. Real output for a 4-piece game:

```
4
Alg_2 00-S0,i4
Alg_1 00-S2,i1
Alg_2 00-S2,i2
Alg_1 00-S0,i5
Alg_2 00-S2,i3
Alg_1 00-S1,i7
Alg_2 00-S2,i4 (mill = S2i4,S2i3,S2i2) Alg_1 looses cow (S1,i7) | 2 left
...
Alg_2 wins
```

Placement lines look like `Alg_x 00-<slot>`, movement lines like `Alg_x <from>-<to>`, and a newly formed mill is logged in brackets together with the piece it removes.

## Design notes and limitations

- The Minimax evaluation is blended with extra rule-based conditions to keep run time acceptable, so it is not a pure textbook Minimax.
- Search depth is capped, and there is no alpha-beta pruning yet.
- The heuristic player is deliberately simple.

### Future work
- Alpha-beta pruning to search deeper at the same cost.
- A cleaner, standalone evaluation function, separated from the rule-based shortcuts.
- A stronger heuristic opponent and more trials for statistical confidence.
- Shrinking the board array to the 24 playable slots.

## Acknowledgements

Board layout inspired by [Liberty Kapungu's Morabaraba implementation](https://github.com/LibertyKapungu/Morabaraba-Spider-Insect-Algorithm), adapted for this project.

## Documentation

Full report with design, results and discussion: [`docs/Final_Report.pdf`](docs/Final_Report.pdf)

## Authors

- **Siyanda Ngwenya**
- **Relebohile Matoetoe**

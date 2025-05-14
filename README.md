# Chess Engine Project

## Overview

This repository contains a C++17-based chess engine framework featuring multiple search algorithms and optimized data structures for fast move generation and evaluation. You can compile five distinct engine variants and pit them against each other in Arena.

## Key Features

* **64-bit Bitboard Representation**: Ultra-fast move generation and static evaluation using low-level bitwise operations.
* **Zobrist Hashing & Transposition Table**: Caches search results to avoid redundant computations and accelerate alpha-beta and MCTS searches.
* **Hybrid Search Strategies**: Implements Alpha‑Beta Minimax and four UCT‑based MCTS variants (pure MCTS, MCTS-IR, MCTS-IC, MCTS-IP).
* **Informed Rollouts, Cutoffs & Priors**: Enhancements in MCTS to improve tactical accuracy and mitigate shallow‑trap vulnerabilities.
* **UCI Compliance**: Standardized protocol support with FEN parsing and "position"/"go" command handling.
* **Arena Tournament Integration**: Easy setup for rigorous benchmarking via Arena’s GUI.

## Prerequisites

* C++17 compiler (e.g., g++ 9.0+)
* GNU Make (optional)
* [Arena GUI](http://www.playwitharena.de/) installed on Windows or Wine on Linux/macOS

## Compilation Commands

Open a terminal in the project root and run the following for each engine variant:

```bash
# 1) Alpha-Beta Minimax Engine
g++ -O2 -DUSE_MINIMAX *.cpp -o minimax-engine

# 2) Pure MCTS Engine
g++ -O2 -DUSE_MCTS *.cpp -o mcts-engine

# 3) MCTS with Informed Rollouts (MCTS-IR)
g++ -O2 -DUSE_MCTS_IR *.cpp -o mcts-ir-engine

# 4) MCTS with Informed Cutoffs (MCTS-IC)
g++ -O2 -DUSE_MCTS_IC *.cpp -o mcts-ic-engine

# 5) MCTS with Informed Priors (MCTS-IP)
g++ -O2 -DUSE_MCTS_IP *.cpp -o mcts-ip-engine
```

*All binaries will be generated in the current directory.*

## UCI Command-Line Usage

You can test any engine directly via UCI commands in the terminal:

```bash
echo -e "uci
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
go movetime 1000" | ./minimax-engine
```

Adjust `movetime` (in ms) or other UCI options as needed.

## Setting Up a Tournament in Arena

1. **Launch Arena** and navigate to **Engines > Manage...**.
2. Click **New** and create entries for each engine:

   * **Executable**: Browse to the compiled binary (e.g., `minimax-engine`).
   * **Protocol**: Select **UCI**.
   * **Name**: Assign a descriptive name (e.g., "Minimax (O2)").
3. After adding all five engines, go to **Engines > Tournament...**.
4. In the **Tournament** dialog:

   * **Participants**: Add your five engines.
   * **Time control**: Set to **1 min/move** (or customize).
   * **Number of games**: **100** (per pairing).
   * **Starting positions**: Use **Default** or **Load PGN** for specific test suites.
   * **Pairing system**: **Round Robin** recommended.
5. Click **Start** to run the tournament. Monitor live results and statistics.
6. To **export** results, go to **File > Save > Save as CSV** or **Export PGN**.

## Results & Analysis

Results from Arena will include win/draw/loss counts, Elo estimates, and cross‑scores. Use these to compare engine variants quantitatively.

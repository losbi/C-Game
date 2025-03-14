# 2D Platformer Game - Competition Version

![run-tests](../../workflows/run-tests/badge.svg)

## Overview

This project is a 2D platformer game developed as part of the **Data Structures and Algorithms Competition** organized by **Kostas Chatzikokolakis**. The competition challenged participants to design and implement efficient algorithms and data structures to optimize game performance and mechanics.

The `competition` version of the game focuses on high performance and advanced data structure usage, showcasing the transition from vector-based to set-based object management for improved efficiency.

---

## Features

- **Dynamic Gameplay**: Includes jumping, running, and falling animations for the player character.
- **Randomized Platforms**: Procedurally generated platforms with unique objects and coins.
- **Game States**: Pause, resume, and frame updates implemented with efficient data structures.
- **High Performance**: Optimized using `set` data structures for better FPS compared to `vector` implementations.
- **High Score Tracking**: Displays the player's best score.
- **Custom Animations**: Includes animations for coins, background, and player actions.
- **Sound Effects and Music**: Background music and sound effects for actions like collecting coins and losing the game.
- **Start Screen**: A start screen with a button to begin the game.

---

## Technical Details

### Data Structures

- **Set Implementation**: The `competition` version replaces vectors with sets for object management, resulting in significant performance improvements.
- **Custom Comparison Functions**: Used for efficient set operations, ensuring fast lookups and updates.

### Key Modules

1. **Game Logic**: 
   - Implemented in [`state_alt.c`](modules/state_alt.c).
   - Handles game state updates, object management, and collision detection.

2. **Interface**:
   - Implemented in [`interface.c`](programs/game/interface.c) and [`interface.h`](programs/game/interface.h).
   - Manages the game's user interface and interactions.

3. **Testing**:
   - Unit tests for the `competition` version are located in [`state_alt_test.c`](alt_tests/state_alt_test.c).

---

## How to Run

### Prerequisites

- A C compiler (e.g., GCC).
- Make utility.
- OpenGL development libraries (e.g., `libgl1-mesa-dev`).

### Build Instructions

1. Clone the repository:
   ```sh
   git clone <repository-url>
   cd C-Game

### Navigate to the programs/competition folder:
cd programs/competition
### Build the competition version:
make
### Run the game:
make run 

or simply press the .exe file

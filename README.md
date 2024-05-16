# LemIPC

LemIPC is a game where players, gathered in teams, battle on a 2D board. To win, a team must be the last one remaining on the board.

## Game Rules

- Players battle in teams on a 2D board.
- To win, a team must be the last one remaining on the board.
- When a player dies, they disappear from the board.
- To kill a player, they must be surrounded by at least 2 players from the same team, positioned on adjacent tiles (including diagonals).
- The killed player must leave the board and terminate their execution.
- Only one player can occupy a board tile at a time.

## Technical Constraints

- Each client is a process, and there must be only one executable.
- The first player creates shared resources (shm, msgq, semaphores).
- When a player quits, they must ensure they are the last player on the board. If so, they must clean up all IPCs created by the first player so they don't remain in memory.
- The board must be stored in a shared memory segment (SHM).
- Each player can check the board's content, but must respect the constraints tied to shared resources and competitive access (semaphores).
- A player can only communicate with other players through MSGQ.
- On the map, a player can see if a tile is empty or occupied by another player. If occupied, the player's team number will be displayed. Players from the same team cannot be differentiated.

## Display

- The game must display the board's state.
  - **Text Mode**: The first player displays the board content, or each player does, or a specific process handles it.
  - **Graphical Mode**: The first player creates the display, or another dedicated executable can be used for display.

## Features

- Team battles on a 2D board.
- Management of shared resources (SHM, MSGQ, semaphores).
- Display in text mode or graphical mode.

## Technical Choices

- Project implemented in the C programming language.
- Shared memory (SHM) used to store the board.
- Message queues (MSGQ) used for communication between players.
- Semaphores used for competitive access to shared resources.

## Demonstration

A video demonstration of the project can be found here:

### Graphical Mode (100 players in 4 teams )
[![Youtube](https://img.youtube.com/vi/W-q01PR-wS4/0.jpg)](https://www.youtube.com/watch?v=W-q01PR-wS4)

### Graphical Mode (400 players in 20 teams )
[![Youtube](https://img.youtube.com/vi/L7fzWmrBHdU/0.jpg)](https://www.youtube.com/watch?v=L7fzWmrBHdU)

## Usage

To run the program, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/lemipc.git
   make run
   ```
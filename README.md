# Sokoban
Single player game - Sokoban of Theory of Computer Games: Fall 2018

[Platform]

	Linux

[AI Game] Implement a solver of Sokoban.

	Main Search Algorithm: Bi-directional A* Search
	
	Prevent Recursive: use unorder map to hash unique board
	
	Prevent DeadLock: Simple deadlocks + Freeze deadlocks + Corner deadlocks + M deadlocks + 2*2 deadlocks
	
	Heuristic function: Manhattan distance for each box to nearest goal plus the manhattan distance from the player to the nearest box

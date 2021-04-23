#pragma once
#include <iostream>
using namespace std;
#include <vector>

struct WilsonCell {
	Pos pos;
	bool topWall, downWall, leftWall, rightWall;
	WilsonCell* next;
	bool inMaze;
};

class Wilson {
public:
	int m = 0, n = 0;

	WilsonCell*** maze;
	vector<WilsonCell*> mazeCells;
	vector<WilsonCell*> unusedCells;

	void init(int _n, int _m) {
		n = _n;
		m = _m;
		maze = new WilsonCell * *[n];
		for (int i = 0; i < n; i++) {
			maze[i] = new WilsonCell * [m];
			for (int j = 0; j < m; j++) {
				maze[i][j] = createCell();
				maze[i][j]->pos = Pos{ i,j };
				unusedCells.push_back(maze[i][j]);

			}
		}
	}

	~Wilson() {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				if (maze[i][j] != nullptr)
					delete maze[i][j];
			}
			if (maze[i] != nullptr)
				delete[] maze[i];
		}
		if (maze != nullptr)
			delete[] maze;
	}

	WilsonCell* createCell() {
		WilsonCell* c = new WilsonCell;
		c->topWall = true;
		c->downWall = true;
		c->leftWall = true;
		c->rightWall = true;
		c->next = nullptr;
		c->inMaze = false;
		return c;
	}

	WilsonCell* randomCells(vector<WilsonCell*> cells) {
		return cells[rand() % (int)cells.size()];
	}

	Pos randomDir() {
		Pos wallDir[4] = { Pos{-1,0}, Pos{1,0},Pos{0,-1}, Pos{0,1} };
		return wallDir[rand() % 4];
	}

	void RemoveCell(std::vector<WilsonCell*>& cells, Pos pos) {
		cells.erase(
			std::remove_if(cells.begin(), cells.end(), [&](WilsonCell* const& cell) {
				return cell->pos.x == pos.x && cell->pos.y == pos.y;
				}),
			cells.end());
	}

	bool Safe(Pos p) {
		return p.y >= 0 && p.y < n&& p.x >= 0 && p.x < m;
	}

	void putInMaze(WilsonCell* c) {
		mazeCells.push_back(c);
		RemoveCell(unusedCells, c->pos);
		c->inMaze = true;
	}

	void Generate() {
		WilsonCell* mazeStartCell = randomCells(unusedCells);
		putInMaze(mazeStartCell);

		while (!unusedCells.empty()) {
			WilsonCell* src = randomCells(unusedCells);
			WilsonCell* currentCell = src;

			while (!currentCell->inMaze) {

				Pos neighborDir = randomDir();
				while (!Safe(Pos{ currentCell->pos.y + neighborDir.y,currentCell->pos.x + neighborDir.x }))
					neighborDir = randomDir();
				WilsonCell* neighbor = maze[currentCell->pos.y + neighborDir.y][currentCell->pos.x + neighborDir.x];
				currentCell->next = neighbor;
				currentCell = neighbor;
			}

			currentCell = src;

			while (!currentCell->inMaze) {
				if (Safe(Pos{ currentCell->pos.y - 1, currentCell->pos.x }) &&
					currentCell->next == maze[currentCell->pos.y - 1][currentCell->pos.x]) {
					currentCell->topWall = false;
					currentCell->next->downWall = false;
				}
				else if (Safe(Pos{ currentCell->pos.y + 1, currentCell->pos.x }) &&
					currentCell->next == maze[currentCell->pos.y + 1][currentCell->pos.x]) {
					currentCell->downWall = false;
					currentCell->next->topWall = false;
				}
				else if (Safe(Pos{ currentCell->pos.y , currentCell->pos.x - 1 }) &&
					currentCell->next == maze[currentCell->pos.y][currentCell->pos.x - 1]) {
					currentCell->leftWall = false;
					currentCell->next->rightWall = false;
				}
				else if (Safe(Pos{ currentCell->pos.y , currentCell->pos.x + 1 }) &&
					currentCell->next == maze[currentCell->pos.y][currentCell->pos.x + 1]) {
					currentCell->rightWall = false;
					currentCell->next->leftWall = false;
				}

				putInMaze(currentCell);

				currentCell = currentCell->next;

				//DrawMaze(maze, n, m);
			}
			//DrawMaze(maze, n, m);
		}
	}

	WilsonCell*** generateMaze(int n, int m) {
		init(n, m);
		Generate();
		return maze;
	}
};
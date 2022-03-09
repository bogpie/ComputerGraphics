#include "Maze.h"

using namespace std;

Maze::Maze(int noLines, int noCols, int MAX_NO_ENEMIES, int MAX_NO_EXITS)
{
	this->noLines = noLines;
	this->noCols = noCols;
	this->MAX_NO_ENEMIES = MAX_NO_ENEMIES;
	this->MAX_NO_EXITS = MAX_NO_EXITS;
	initialize();
}

Maze::Maze()
{
	this->noLines = DEFAULT_NO_LINES + rand() % 4;
	this->noCols = this->noLines;
	this->MAX_NO_ENEMIES = DEFAULT_MAX_NO_ENEMIES;
	this->MAX_NO_EXITS = DEFAULT_MAX_NO_EXITS;
	initialize();
}

void Maze::initialize()
{
	matrix = vector<vector<int>>(noLines);
	for (int idLine = 0; idLine < noLines; ++idLine)
	{
		matrix[idLine] = vector<int>(noCols);
		for (int idCol = 0; idCol < noCols; ++idCol)
		{
			matrix[idLine][idCol] = 1;
		}
	}

	boundingBox = Box(glm::vec3(0.5, -100, 0.5), glm::vec3(matrix.size(), +100, matrix[0].size()));
}

void Maze::printMatrix()
{
	for (auto& line : matrix)
	{
		for (int cell : line)
		{
			if (cell == 0)
			{
				cout << " ";
			}
			else
			{
				cout << cell;
			}
		}
		cout << "\n";
	}
	cout << "\n";
}

bool Maze::isInside(int idLine, int idCol)
{
	if (idLine >= 1 && idCol >= 1 && idLine <= noLines - 2 && idCol <= noCols - 2)
	{
		return true;
	}
	return false;
}

int Maze::getNoRoads(int idLine, int idCol)
{
	int noRoads = 0;
	for (int lineDeviation = -1; lineDeviation <= 1; ++lineDeviation)
	{
		for (int colDeviation = -1; colDeviation <= 1; ++colDeviation)
		{
			if (abs(lineDeviation - colDeviation) != 1) continue;
			int newIdLine = idLine + lineDeviation;
			int newIdCol = idCol + colDeviation;

			if (!isInside(newIdLine, newIdCol)) continue;
			if (matrix[newIdLine][newIdCol] == 0) ++noRoads;
		}
	}
	return noRoads;
}

vector<pair<int, int>> Maze::getWallNeighbours(int idLine, int idCol, int& noWallNeighboursWithBorder)
{
	noWallNeighboursWithBorder = 0;
	vector<pair<int, int>> neighbours;
	for (int lineDeviation = -1; lineDeviation <= 1; ++lineDeviation)
	{
		for (int colDeviation = -1; colDeviation <= 1; ++colDeviation)
		{
			if (abs(lineDeviation - colDeviation) != 1) continue;
			int newIdLine = idLine + lineDeviation;
			int newIdCol = idCol + colDeviation;

			if (matrix[newIdLine][newIdCol] == 0) continue; // already visited
			noWallNeighboursWithBorder++;

			if (!isInside(newIdLine, newIdCol)) continue;
			neighbours.emplace_back(newIdLine, newIdCol);
		}
	}
	return neighbours;
}

void Maze::DFS(int idLine, int idCol)
{
	matrix[idLine][idCol] = 0;

	vector<pair<int, int>> neighbours;
	int noWallNeighboursWithBorder = 0;

	// Randomly choose a wall at the current cell and open a passage through it to any random, unvisited, adjacent cell. This is now the current cell.
	neighbours = getWallNeighbours(idLine, idCol, noWallNeighboursWithBorder);

	if (noWallNeighboursWithBorder <= 2)
	{
		// Actually a cycle
		matrix[idLine][idCol] = 1;
		return;
	}

	shuffle(neighbours.begin(), neighbours.end(), mt19937(random_device()()));

	for (auto& neighbour : neighbours)
	{
		idCol = neighbour.second;
		idLine = neighbour.first;

		// Take into account new modifications
		if (!isInside(idLine, idCol)) continue;
		if (matrix[idLine][idCol] == 0) continue; // already visited
		DFS(idLine, idCol);
	}

	// If all adjacent cells have been visited, back up to the previous and repeat
	// Stop when the algorithm has backed all the way up to the starting cell.
}

Sphere Player::getSphere()
{
	return Sphere(this->position, 0.20);
	// For comparison, a cube has a side of 1, and, thus, a sphere almost as big as a cube would have a 0.5 radius
}

Player::Player()
{
	this->position = glm::vec3(0);
	this->life = 100;
}

Player::Player(glm::vec3 position)
{
	this->position = position;
	this->life = 100;
}

float Player::getHealthPercentage()
{
	return this->life / 100.0;
}

struct ExitComparator
{
	ExitComparator(Player player) { this->player = player; }
	bool operator () (const pair<int, int>& exit1, const pair<int, int>& exit2)
	{
		float distance1 = sqrt(pow((exit1.first - player.position.x), 2) + pow((exit1.second - player.position.z), 2));
		float distance2 = sqrt(pow((exit2.first - player.position.x), 2) + pow((exit2.second - player.position.z), 2));
		return distance1 > distance2;
	}
	Player player;
};

void Maze::generate()
{
	// Randomly choose a starting cell
	int startingLine = rand() % (noLines - 2) + 1;
	int startingCol = rand() % (noCols - 2) + 1;
	DFS(startingLine, startingCol);

	vector<pair<int, int>> possibleExits;
	vector<pair<int, int>> roads;

	// Choose a player position, some enemies, and a few exits
	for (int idLine = 0; idLine < noLines; ++idLine)
	{
		for (int idCol = 0; idCol < noCols; ++idCol)
		{
			if (isInside(idLine, idCol))
			{
				if (!matrix[idLine][idCol])
				{
					roads.emplace_back(make_pair(idLine, idCol));
				}
			}
			else
			{
				int noRoads = getNoRoads(idLine, idCol);
				if (noRoads > 0)
					possibleExits.emplace_back(make_pair(idLine, idCol));
			}
		}
	}
	shuffle(roads.begin(), roads.end(), mt19937(random_device()()));
	int idRoad = 0;

	// Enemies
	int noEnemies = rand() % MAX_NO_ENEMIES + 5;
	for (int idEnemy = 0; idEnemy < noEnemies; ++idEnemy)
	{
		int idLine = roads[idRoad].first;
		int idCol = roads[idRoad++].second;
		matrix[idLine][idCol] = 2;
		Enemy enemy = Enemy(glm::vec3(0.5 + idLine, 0.5, 0.5 + idCol));
		enemies.emplace_back(enemy);
	}

	// Player
	int idLine = roads[idRoad].first;
	int idCol = roads[idRoad++].second;
	matrix[idLine][idCol] = 3;
	double x = 0.5 + idLine;
	double y = 0.5;
	double z = 0.5 + idCol;
	this->player = Player(glm::vec3(x, y, z));
	cout << idLine << " " << idCol << "\n";
	cout << player.position.x << " " << player.position.z << "\n";

	// Exits
	//shuffle(possibleExits.begin(), possibleExits.end(), mt19937(random_device()()));
	// Sort the exits by euclidian distance in order to make the maze possibly harder
	std::sort(possibleExits.begin(), possibleExits.end(), ExitComparator(player));

	int noExits = rand() % MAX_NO_EXITS + 1;
	for (int idExit = 0; idExit < noExits; ++idExit)
	{
		matrix[possibleExits[idExit].first][possibleExits[idExit].second] = 0;
	}

	// Populate walls array after digging exits
	for (int idLine = 0; idLine < noLines; ++idLine)
	{
		for (int idCol = 0; idCol < noCols; ++idCol)
		{
			if (matrix[idLine][idCol] == Wall)
			{
				Box wall = Box(glm::vec3(0.5 + idLine, 0.5, 0.5 + idCol), 1);
				walls.emplace_back(wall);
			}
		}
	}
}


void Enemy::generateDirection()
{
	this->direction = glm::vec3(rand() % 10 / 1000.0 + 0.001, rand() % 10 / 1000.0 + 0.001, rand() % 10 / 1000.0 + 0.001);
	timeDirectionChange = std::chrono::high_resolution_clock::now();
}


Enemy::Enemy(glm::vec3 position)
{
	this->position = position;
	Enemy::generateDirection();
	timeDirectionChange = std::chrono::high_resolution_clock::now();
	intervalDirectionChange = 1.5f; // seconds
	speed = 1.75;
	radius = 0.15;
	isShot = false;
	boundingBox = Box(position, 1.0f);
	color = glm::vec3(1, 0, 1);
}

Sphere Enemy::getSphere()
{

	return Sphere(position, radius);
}

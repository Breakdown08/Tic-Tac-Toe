#include <iostream>
#include <random>
#include <stdlib.h>
#include <chrono>

using namespace std;

//===================================================================================

enum TCell: char
{
	CROSS = 'X',
	ZERO = '0',
	EMPTY = '_'
};

struct TCoord
{
	size_t y{ 0U };
	size_t x{ 0U };
};

enum TProgress
{
	IN_PROGRESS,
	WON_HUMAN,
	WON_AI,
	DRAW
};

#pragma pack(push, 1)
struct TGame
{
	TCell** ppField{ nullptr };
	size_t SIZE;
	TProgress progress{ IN_PROGRESS };
	TCell human, ai;
	size_t turn{ 0U }; //четные человек, нечетные аи
};
#pragma pack(pop)
//===================================================================================

int32_t getRandomNum(const int32_t min, const int32_t max)
{
	const static auto seed = chrono::system_clock::now().time_since_epoch().count();
	static mt19937_64 generator(seed);
	uniform_int_distribution<int32_t> dis(min, max);
	return dis(generator);
}

void PrintArr(TCoord*& arr, size_t size)
{
	if (arr != nullptr)
	{
		for (size_t i = 0; i < size; i++)
		{
			cout << arr[i].x << ":" << arr[i].y << endl;
		}
	}
}

void ClearScreen()
{
#ifdef __linux__ 
	system("clear");
#elif _WIN32
	system("cls");
#else
	cout << "ERROR: OS is not supported for clear the screen!" << endl;
#endif
}

void ClearPtrArr(TCoord*& arr)
{
	delete[] arr;
	arr = nullptr;
}

void ExtendArr(TCoord*& arr, size_t size, TCoord element)
{
	TCoord* buf;
	buf = new TCoord[size];
	for (size_t i = 0; i < size; i++)
	{
		buf[i] = arr[i];
	}
	delete[] arr;
	arr = new TCoord[size + 1];
	for (size_t i = 0; i < size; i++)
	{
		arr[i] = buf[i];
	}
	arr[size] = element;
	size++;
	ClearPtrArr(buf);
}

void initGame(TGame& g) 
{
	cout << "Enter game field size:";
	cin >> g.SIZE;
	g.ppField = new TCell * [g.SIZE];
	for (size_t i = 0; i < g.SIZE; i++)
	{
		g.ppField[i] = new TCell[g.SIZE];
	}

	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			g.ppField[y][x] = EMPTY;
		}
	}

	if (getRandomNum(0, 1000) > 500)
	{
		g.human = CROSS;
		g.ai = ZERO;
		g.turn = 0;
	}
	else
	{
		g.human = ZERO;
		g.ai = CROSS;
		g.turn = 1;
	}
}

void deinitGame(TGame& g)
{
	for (size_t i = 0U; i < g.SIZE; i++)
	{
		delete[] g.ppField[i];
	}
	delete[] g.ppField;
	g.ppField = nullptr;
}

void printGame(TGame& g)
{
	cout << "     ";
	for (size_t x = 0; x < g.SIZE; x++)
	{
		cout << x + 1 << "   ";
	}
	cout << endl;

	for (size_t y = 0; y < g.SIZE; y++)
	{
		cout << " " << y + 1 << " | ";
		for (size_t x = 0; x < g.SIZE; x++)
		{
			cout << g.ppField[y][x] << " | ";
		}
		cout << endl;
	}

	cout << endl << " Human: " << g.human << endl << " Computer: " << g.ai << endl << endl;
}

void congrats(const TGame& g)
{
	if (g.progress == WON_HUMAN)
		cout << " Human won! =)" << endl;
	else if (g.progress == WON_AI)
		cout << " Computer won! =(" << endl;
	else if (g.progress == DRAW)
		cout << " DRAW =/" << endl;
}

TProgress getWon(const TGame& g)
{
	//1 победа в строках
	for (size_t y = 0; y < g.SIZE; y++)
	{
		int equalCount = 1;
		for (size_t x = 1; x < g.SIZE; x++)
		{
			if (g.ppField[y][x - 1] == g.ppField[y][x] && g.ppField[y][x] != EMPTY)
			{
				equalCount++;
			}
			else 
			{
				equalCount = 1;
			}
		}
		if (equalCount == g.SIZE && g.ppField[y][0] == g.human)
		{
			return WON_HUMAN;
		}
		if (equalCount == g.SIZE && g.ppField[y][0] == g.ai)
		{
			return WON_AI;
		}
	}
	//2 победа в столбцах
	for (size_t x = 0; x < g.SIZE; x++)
	{
		int equalCount = 1;
		for (size_t y = 1; y < g.SIZE; y++)
		{
			if (g.ppField[y - 1][x] == g.ppField[y][x] && g.ppField[y][x] != EMPTY)
			{
				equalCount++;
			}
			else
			{
				equalCount = 1;
			}
		}
		if (equalCount == g.SIZE && g.ppField[0][x] == g.human)
		{
			return WON_HUMAN;
		}
		if (equalCount == g.SIZE && g.ppField[0][x] == g.ai)
		{
			return WON_AI;
		}
	}
	//3 победа по диагонали
	int diagonalCounter = 1;
	for (size_t y = 1; y < g.SIZE; y++)
	{
		if (g.ppField[y][y] == g.ppField[y - 1][y - 1]) 
		{
			diagonalCounter++;
		}
	}
	if (diagonalCounter == g.SIZE)
	{
		if (g.ppField[0][0] == g.human)
		{
			return WON_HUMAN;
		}
		if (g.ppField[0][0] == g.ai)
		{
			return WON_AI;
		}
	}
	int secondDiagonalCounter = 1;
	for (size_t y = 1; y < g.SIZE; y++)
	{
		if (g.ppField[y][g.SIZE - 1 - y] == g.ppField[y - 1][g.SIZE - 1 - y + 1])
		{
			secondDiagonalCounter++;
		}
	}
	if (secondDiagonalCounter == g.SIZE)
	{
		if (g.ppField[g.SIZE - 1][0] == g.human)
		{
			return WON_HUMAN;
		}
		if (g.ppField[g.SIZE - 1][0] == g.ai)
		{
			return WON_AI;
		}
	}
	//4 ничья
	bool draw{ true };
	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[y][x] == EMPTY)
			{
				draw = false;
				break;
			}
		}
		if (!draw)
			break;
	}
	if (draw)
		return DRAW;

	return IN_PROGRESS;
}

TCoord getHumanCoord(const TGame& g)
{
	TCoord c;
	do 
	{
		cout << " Enter X: ";
		cin >> c.x;
		cout << " Enter Y: ";
		cin >> c.y;

		c.x--;
		c.y--;
	} while (c.x > g.SIZE - 1 || c.y > g.SIZE - 1 || g.ppField[c.y][c.x] != EMPTY);

	return c;
}

TCoord getAICoord(TGame& g)
{
	//Pre win situation
	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[y][x] == EMPTY)
			{
				g.ppField[y][x] = g.ai;
				if (getWon(g) == WON_AI)
				{
					g.ppField[y][x] = EMPTY;
					return { y, x };
				}
				g.ppField[y][x] = EMPTY;
			}
		}
	}
	//Pre fail situation
	for (size_t y = 0; y < g.SIZE; y++)
	{
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if (g.ppField[y][x] == EMPTY)
			{
				g.ppField[y][x] = g.human;
				if (getWon(g) == WON_HUMAN)
				{
					g.ppField[y][x] = EMPTY;
					return { y, x };
				}
				g.ppField[y][x] = EMPTY;
			}
		}
	}
	//Ход по приоритетам + rand
	if (g.ppField[1][1] == EMPTY)
	{
		return { 1, 1 };
	}
	
	//углы
	TCoord angleBuffer[4];
	size_t num{ 0U };
	cout << "check angles" << endl;
	if (g.ppField[0][0] == EMPTY)
	{
		angleBuffer[num] = { 0, 0 };
		num++;
	}
	if (g.ppField[g.SIZE - 1][0] == EMPTY)
	{
		angleBuffer[num] = { g.SIZE - 1, 0 };
		num++;
	}
	if (g.ppField[0][g.SIZE - 1] == EMPTY)
	{
		angleBuffer[num] = { 0, g.SIZE - 1 };
		num++;
	}
	if (g.ppField[g.SIZE - 1][g.SIZE - 1] == EMPTY)
	{
		angleBuffer[num] = { g.SIZE - 1, g.SIZE - 1 };
		num++;
	}
	cout << num << endl;
	if (num > 0)
	{
		const size_t index = getRandomNum(0, 1000) % num;
		return angleBuffer[index];
	}
	
	

	//не углы
	TCoord* notAngleBuffer{ nullptr };
	size_t bufferSize = { 0U };

	for (size_t y = 0; y < g.SIZE; y++) //обход на поиск и подсчет неугловых клеток
	{
		cout << "check NOT angles" << endl;
		for (size_t x = 0; x < g.SIZE; x++)
		{
			if ((y != 0 || x != 0) && (y != g.SIZE - 1 || x != g.SIZE - 1) && (y != 0 || x != g.SIZE - 1) && (y != g.SIZE - 1 || x != 0)) //если не угол
			{
				if (g.ppField[y][x] == EMPTY)
				{
					TCoord coord = { y, x };
					ExtendArr(notAngleBuffer, bufferSize, coord); //расширяем динамический массив
					bufferSize++;
				}
			}
		}
	}
	cout << bufferSize << endl;
	if (bufferSize > 0)
	{
		const size_t index = getRandomNum(0, 1000) % bufferSize;
		return notAngleBuffer[index];
	}
	return { 0, 0 };
}

//===================================================================================

int main() 
{
	TGame g;
	initGame(g);
	ClearScreen();
	printGame(g);
	do 
	{
		if (g.turn % 2 == 0)
		{
			//ход человека
			//cout << " Human turn:" << endl;
			const TCoord c = getHumanCoord(g);
			g.ppField[c.y][c.x] = g.human;
		}
		else 
		{
			//ход ПК
			//cout << " Player2 turn:" << endl;
			const TCoord c = getAICoord(g);
			//const TCoord c = getHumanCoord(g);
			g.ppField[c.y][c.x] = g.ai;
		}
		ClearScreen();
		printGame(g);
		g.turn++;
		g.progress = getWon(g);
	} while (g.progress == IN_PROGRESS);
	congrats(g);
	deinitGame(g);
}

//===================================================================================
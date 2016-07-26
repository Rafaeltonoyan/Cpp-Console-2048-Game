/*
	Source code for the Game "2048"
	Supporting Platform(s) : Windows with NET Framework 4.0 or later
	Need to disable Compiler Warning (level 3) C4996 with: _SCL_SECURE_NO_WARNINGS
	(C) IT STEP academy Georgia (07/2016)
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <limits>
#include <stdexcept>

using namespace std;

// -------------------- Control Parameters ---------------
#define DIM 4			// Dimension must not be between 2 and 10
#define Ratio2and4 6	// Frequencies ratio of 2 and 4
// -------------------------------------------------------

#define _UP_ 72
#define _DOWN_ 80
#define _LEFT_ 75
#define _RIGHT_ 77
#define _ESCAPE_ 27

//----------------------------------------------------------------------------
struct console
{
	console(unsigned width, unsigned height, unsigned X_position, unsigned Y_position)
	{
		SMALL_RECT r;
		COORD      c;
		hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (!GetConsoleScreenBufferInfo(hConOut, &csbi))
			throw runtime_error("You must be attached to a human.");

		r.Left =
			r.Top = 0;
		r.Right = width - 1;
		r.Bottom = height - 1;
		SetConsoleWindowInfo(hConOut, TRUE, &r);

		c.X = width;
		c.Y = height;
		SetConsoleScreenBufferSize(hConOut, c);

		MaximizeOutputWindow(X_position, Y_position);
	}

	~console()
	{
		SetConsoleTextAttribute(hConOut, csbi.wAttributes);
		SetConsoleScreenBufferSize(hConOut, csbi.dwSize);
		SetConsoleWindowInfo(hConOut, TRUE, &csbi.srWindow);
	}

	void color(WORD color = 0x07)
	{
		SetConsoleTextAttribute(hConOut, color);
	}

	void MaximizeOutputWindow(unsigned x, unsigned y)
	{
		HWND consoleWindow = GetConsoleWindow(); // This gets the value Windows uses to identify your output window 
		ShowWindow(consoleWindow, SW_MAXIMIZE); // this mimics clicking on its' maximize button

		SetWindowPos(consoleWindow, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	}

	HANDLE                     hConOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
};
console con(10 * DIM + 5, 6 * DIM + 5, 500, 50);
//----------------------------------------------------------------------------

struct Board
{
	int Cells[DIM][DIM]{0};
	int CellsB[DIM][DIM];
	int MaxScores[9];
	int Score = 0;
	int ScoreB = 0;
	int CMap[DIM*DIM + 2];	// collor map for each ship
	Board()
	{
		srand(unsigned int(time(NULL)));
		RandTwo();
		//FillCells();
		BackUp();
		SetColors();
		GetMax();
	}
	void SetColors(void)
	{
		if (DIM >= 2)
		{
			CMap[0] = 0;
			CMap[1] = 10;
			CMap[2] = 11;
			CMap[3] = 9;
			CMap[4] = 14;
			CMap[5] = 13;
		}
		if (DIM >= 3)
		{
			CMap[6] = 12;
			CMap[7] = 15;
			CMap[8] = 6;
			CMap[9] = 5;
			CMap[10] = 4;
		}
		if (DIM >= 4)
		{
			CMap[11] = 3;
			CMap[12] = 2;
			CMap[13] = 1;
			CMap[14] = 7;
			CMap[15] = 10;
		}
		if (DIM >= 4)
			for (int i = 16; i < DIM*DIM + 2; i++) 	CMap[i] = 10;
	}
	void GetMax(void)
	{
		if (ifstream("Info2048"))
		{
			ifstream fread("Info2048");

			for (int i = 0; i < 9; i++)
			{
				fread >> MaxScores[i];
			}
		}
		else
		{
			for (int i = 0; i < 9; i++) MaxScores[i] = 0;
		}
	}
	void FillCells(void)
	{
		int count = 1;
		for (int i = 0; i < DIM; i++)
			for (int j = 0; j < DIM; j++)
			{
				Cells[j][i] = int(pow(2, count));
				Cells[0][0] = 4;
				//Cells[j][i] = 2;
				count++;
			}
	}
	void RandTwo(void)
	{
		Vec V1, V2;
		V1 = RandCell();
		int SV = (rand() % Ratio2and4 == 0) ? 4 : 2;

		Cells[V1.X][V1.Y] = SV;
		Score += SV;

		do
		{
			V2 = RandCell();
		} while (V1.X == V2.X && V1.Y == V2.Y);

		SV = (rand() % Ratio2and4 == 0) ? 4 : 2;

		Cells[V2.X][V2.Y] = SV;
		Score += SV;
		if (Score > MaxScores[DIM - 2]) MaxScores[DIM - 2] = Score;
	}
	void BackUp(void)
	{
		copy(&Cells[0][0], &Cells[0][0] + DIM * DIM, &CellsB[0][0]);
		ScoreB = Score;

		if (Score % 50 == 0) SaveRecords();
	}
	void SaveRecords(void)
	{
		ofstream fwrite("Info2048");

		for (int i = 0; i < 9; i++)
		{
			fwrite << MaxScores[i] << endl;
		}
	}
	void Undo(void)
	{
		copy(&CellsB[0][0], &CellsB[0][0] + DIM * DIM, &Cells[0][0]);
		Score = ScoreB;
	}
	void Restart(void)
	{
		for (int i = 0; i < DIM; i++)
			for (int j = 0; j < DIM; j++)
			{
				Cells[j][i] = 0;
			}

		Score = 0;

		RandTwo();
		BackUp();
		SetColors();
		GetMax();
	}
	struct Vec
	{
		int X;
		int Y;
	};
	Vec RandCell(void)
	{
		Vec V;
		V.X = rand() % DIM;
		V.Y = rand() % DIM;
		return V;
	}
}B;

void Show(void);
void Show_1(char, char);
void Show_2(char, Board, int);
void CopyArray(int [], int, int, bool);
bool Logic(int);
bool OrderRow(int []);
bool AddRand(void);
bool CheckForGameEnd(bool);
template <typename T> void PrintColl(int, int, T);

int main()
{
	if (DIM < 2 || DIM > 10)
	{
		cout << " Dimension must be between 2 and 10 " << endl;
		cin.get();
		return -1;
	}

	Show();

	int Inp = 0;
	bool EndOfGame = false;

	while (Inp != _ESCAPE_)
	{
		if (_kbhit())
		{
			Inp = _getch();
			system("cls");

			switch (Inp)
			{
			case int('d') :
			case int('D') :
			case _RIGHT_ : 
			{
				//cout << "RIGHT" << endl;
				if (!EndOfGame)
				if (Logic(_RIGHT_)) EndOfGame = CheckForGameEnd(AddRand());
			}break;
			case int('s') :
			case int('S') :
			case _DOWN_:
			{
				//cout << "DOWN" << endl;
				if (!EndOfGame)
				if (Logic(_DOWN_)) EndOfGame = CheckForGameEnd(AddRand());
			}break;
			case int('a') :
			case int('A') :
			case _LEFT_:
			{
				//cout << "LEFT" << endl;
				if (!EndOfGame)
				if (Logic(_LEFT_)) EndOfGame = CheckForGameEnd(AddRand());
			}break;
			case int('w') :
			case int('W') :
			case _UP_:
			{
				//cout << "UP" << endl;
				if (!EndOfGame)
				if (Logic(_UP_)) EndOfGame = CheckForGameEnd(AddRand());
			}break;
			case int('u') :
			case int('U') :
			{
				B.Undo();
				EndOfGame = false;
			}break;
			}

			Show();
			if (EndOfGame)
			{
				B.SaveRecords();
				PrintColl(12, 7, "\n      GAME OVER \n");
			}

			if (Inp == 'R' || Inp == 'r')
			{
				int Q;
				if (EndOfGame)
				{
					EndOfGame = false;
					Q = 'Y';
				}
				else
				{
					cout << "\n  Are you sure you want to restart? \n";
					cout << "  < Y > - Yes;  < N > - No : _\b";
					Q = _getch();
				}

				if (Q == 'Y' || Q == 'y')
				{
					B.SaveRecords();
					B.Restart();
					system("cls");
					Show();
				}
				else Inp++;
			}

			if (Inp == _ESCAPE_)
			{
				cout << "\n  Are you sure you want to quit? \n";
				cout << "  < Y > - Yes;  < N > - No : _\b";
				int Q = _getch();
				if (Q == 'Y' || Q == 'y')
				{
					system("cls");
					B.SaveRecords();
				}
				else Inp++;
			}
		}
	}

	return 0;
}

void Show(void)
{
	cout << endl;
	PrintColl(15, 7, "     Score: ");
	PrintColl(10, 7, B.Score);
	PrintColl(15, 7, " | Best: ");
	PrintColl(11, 7, B.MaxScores[DIM - 2]);
	cout << endl << endl;

	for (int i = 0; i < DIM; i++)
	{
		Show_1('|', '-');
		//Show_1('|', ' ');
		Show_1('|', ' ');
		Show_2('|', B, i);
		Show_1('|', ' ');
		//Show_1('|', ' ');
	}
	Show_1('|', '-');

	PrintColl(1, 7, "\n    <U> - Undo | <R> | <Esc> - Quit\n");
}

void Show_1(char A, char B)
{
	cout << "     ";
	for (int i = 0; i <= 7*DIM; i++)
	{
		if (i % 7 == 0) cout << A;
		else cout << B;
	}
	cout << endl;
}

void Show_2(char A, Board B, int J)
{
	cout << "     ";
	for (int i = 0; i < DIM; i++)
	{

		int V = B.Cells[i][J];
		cout << A;
		if (V == 0) cout << "      ";
		else if (V >= 2 && V < 10) 
		{ 
			cout << "   "; 
			PrintColl(B.CMap[int(log2(V))], 7, V); 
			cout << "  "; 
		}
		else if (V >= 10 && V < 100) 
		{ 
			cout << "  "; 			
			PrintColl(B.CMap[int(log2(V))], 7, V);  
			cout << "  "; 
		}
		else if (V >= 100 && V < 1000) 
		{ 
			cout << "  "; 
			PrintColl(B.CMap[int(log2(V))], 7, V);
			cout << " "; 
		}
		else if (V >= 1000 && V < 10000) 
		{ 
			cout << " "; 
			PrintColl(B.CMap[int(log2(V))], 7, V);
			cout << " "; 
		}
		else if (V >= 10000 && V < 100000) 
		{ 
			cout << " ";
			PrintColl(B.CMap[int(log2(V))], 7, V);
		}
		else if (V >= 100000)
		{
			PrintColl(B.CMap[int(log2(V))], 7, V);
		}
	}
	cout <<A << endl;
}

void CopyArray(int S[], int k, int Dir, bool D)
{
	if (D)
	{
		if (Dir == _UP_) for (int i = 0; i < DIM; i++) S[i] = B.Cells[k][i];
		else if (Dir == _RIGHT_) for (int i = 0; i < DIM; i++) S[i] = B.Cells[DIM - i - 1][k];
		else if (Dir == _LEFT_) for (int i = 0; i < DIM; i++) S[i] = B.Cells[i][k];
		else if (Dir == _DOWN_)  for (int i = 0; i < DIM; i++) S[i] = B.Cells[k][DIM - i - 1];
	}
	else
	{
		if (Dir == _UP_) for (int i = 0; i < DIM; i++) B.Cells[k][i] = S[i];
		else if (Dir == _RIGHT_) for (int i = 0; i < DIM; i++) B.Cells[DIM - i - 1][k] = S[i];
		else if (Dir == _LEFT_) for (int i = 0; i < DIM; i++) B.Cells[i][k] = S[i];
		else if (Dir == _DOWN_)  for (int i = 0; i < DIM; i++) B.Cells[k][DIM - i - 1] = S[i];
	}
}

bool Logic(int Dir)
{
	B.BackUp();
	bool Rez = false;
	int S[DIM];

	for (int k = 0; k < DIM; k++)
	{
		CopyArray(S, k, Dir, 1);
		if (OrderRow(S)) Rez = true;
		CopyArray(S, k, Dir, 0);
	}
	return Rez;
}

bool OrderRow(int S[])
{
	bool Rez = false;
	bool DBL[DIM];

	for (int l = 0; l < DIM; l++) DBL[l] = false;

	for (int i = 1; i < DIM; i++)
	{
		if (S[i] == 0) continue;
		else 
			for (int j = i - 1; j >= 0; j--)
			{
				if (S[j] == 0)
				{
					S[j] = S[i];
					S[i] = 0;
					if (i > 1) i--;
					Rez = true;
				}
				else if (S[j] == S[i] && !DBL[j] && !DBL[i])
				{
					S[j] = S[j] << 1;
					S[i] = 0;
					DBL[j] = true;
					if (i > 1) i--;
					Rez = true;
				}
				else break;
			}
	}
	return Rez;
}

bool AddRand(void)
{
	int S[DIM*DIM];
	int Count = 0;

	for (int i = 0; i < DIM; i++)
	{
		for (int j = 0; j < DIM; j++)
		{
			if (B.Cells[i][j] == 0)
			{
				S[Count] = i * 10 + j;
				Count++;
			}
		}
	}

	int r = rand() % Count;
	int SV = (rand() % Ratio2and4 == 0) ? 4 : 2;

	int x = S[r] / 10;
	int y = S[r] % 10;

	B.Cells[x][y] = SV;
	B.Score += SV;
	if (B.Score > B.MaxScores[DIM - 2]) B.MaxScores[DIM - 2] = B.Score;

	return (Count == 1) ? true : false;
}

bool CheckForGameEnd(bool Count)
{
	Board BB = B;
	if (!Count) return false;
	else
	{
		if (Logic(_RIGHT_))
		{
			B = BB;
			return false;
		}
		else if (Logic(_DOWN_))
		{
			B = BB;
			return false;
		}
		else if (Logic(_LEFT_))
		{
			B = BB;
			return false;
		}
		else if (Logic(_UP_))
		{
			B = BB;
			return false;
		}
	}

	B = BB;
	return true;
}

template <typename T> void PrintColl(int IC, int FC, T t)
{
	con.color(IC);
	cout << t;
	con.color(FC);
}
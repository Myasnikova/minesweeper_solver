#include <iostream>
#include <vector>
#include <set>
#include <string>
#include "minesweeper.h"

using namespace std;

int main()
{
	/*vector<vector<int>> matrix{ { 0, 0, 1,-1,-1, 1},
								{ 1, 2, 2,-1,-1,-1},
								{-1,-1,-1,-1,-1,-1},
								{-1,-1,-1,-1,-1,-1}};*/
	/*int max_bomb = 0;
	vector<vector<int>> matrix{ { 0, 0, 0, 1,-1,-1,-1,-1,-1},
								{ 0, 0, 0, 1,-1,-1,-1,-1,-1},
								{ 0, 1, 1, 2, 1,-1,-1,-1,-1},
								{ 0, 1,-1, 2,-1,-1,-1, 3, 1},
								{ 0, 1, 1, 2,-1,-1,-1, 3, 0},
								{ 0, 0, 0, 1, 1, 3,-1, 2, 0},
								{ 1, 1, 1, 0, 0, 1, 1, 1, 0},
								{-1,-1, 2, 1, 0, 0, 0, 0, 0},
 								{-1,-1,-1, 1, 0, 0, 0, 0, 0}};*/
	int h, w;
	cin >> w >> h;
	int max_bomb = 0;
	cin >> max_bomb;
	vector<vector<int>> matrix;

	for (int i = 0; i < h; i++)
	{
		vector<int> line;
		string s;
		cin >> s;
		for (int j = 0; j < w; j++)
		{
			char c = s[j];
			line.push_back(c == 'X' ? -1 : int(c - '0'));
		}
		matrix.push_back(line);
	}
	
		MineSweeper ms(matrix, max_bomb);
		pair<int, int> res;
		ms.getSolution(res);
}


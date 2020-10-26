#ifndef MINESWEEPER_H_
#define MINESWEEPER_H_

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <algorithm>
#include <cmath>
//#define __DEBUG__ true
using namespace std;
enum ECellState
{
	OPENED,
	NOT_OPENED
};
class Group;
class Cell
{
public:
	Cell() : coords(make_pair(-1, -1)) {}
	Cell(pair<int, int> c, Group*g = nullptr) :coords(c) { setGroup(g);}
	pair<int, int> getCoords() { return coords; }
	void setGroup(Group*g);
	void eraseJustGroup(Group*g){group.erase(g);}
	void removeGroup(Group*g);
	set<Group *> getGroups() { return group; }
	//double getProbability(){return probability;}
	//void reculcProbability();
private:
	//double probability;
	set<Group *> group;
	pair<int, int> coords;
};
class Group
{
public:
	Group():bomb(0) {}
	Group(pair<int, int> parent_cell,int bomb) : bomb(bomb) { parent_cells.insert(parent_cell);}

	void addParent(set<pair<int, int>> parent_c);
	set<Cell*> getCells(){return cells;}
	int getBomb(){return bomb;}
	set<Cell*> getIntersection(const Group & other);
	int size() const;
	void addCell(Cell*new_cell);
	void addParent(pair<int, int> parent_cell);
	void addCells(set<Cell*>new_cells);
	void removeCells(const set<Cell*> & del_cells);
	bool is_include(const Group & little);
	bool is_intersect(const Group & other, set<Cell*> & intersection);
	bool subtraction(Group * other);
	bool intersect(Group * other, Group *res);
	bool is_equal(Group * other);
	bool unite(Group * other);
	void print();
private:
	set<pair<int, int>> parent_cells;
	set<Cell*> cells;
	int bomb;
};
class MineSweeper
{
public:
	MineSweeper(const vector<vector<int>> &matrix, int max_bomb);
	~MineSweeper();
	void init();
	bool substract();
	bool intersect();
	bool unite_all_equal_groups();
	bool is_row_valid(int row);
	bool is_col_valid(int col);
	vector<pair<int, int>> get_not_opened_neighbors(int row, int col);
	set<Group*> getSetOfGroups();
	void print_groups();
	bool getLogicSolution(pair<int, int>&res);
	bool getProbablisticSolution(pair<int, int>&res);
	bool getSolution(pair<int, int>&res);
private:
	int max_bomb;
	set<Group*> groups;//открытая клетка - группа которую она создает
	vector<vector<int>> matrix;//количество бомб, если закрыта, то -1
	map<pair<int, int>, Cell*> board; //граница из неоткрытых клеток, граничащих с открытыми
};
#endif //MINESWEEPER_H_
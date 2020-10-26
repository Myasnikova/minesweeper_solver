#include "minesweeper.h"
void Group::addParent(set<pair<int, int>> parent_c)
{
	for (auto p : parent_c)
	{
		addParent(p);
	}
}
set<Cell*> Group::getIntersection(const Group & other)
{
	/*set<Cell*> big(cells), little(other.cells), res;
	if (cells.size() < other.cells.size())
		swap(big, little);*/
	set<Cell*> res;
	for (auto c : other.cells)
	{
		auto search = cells.find(c);
		if (search != cells.end())
			res.insert(c);
	}
	return res;
}
int Group::size() const
{
	return cells.size();
}
void Group::addCell(Cell * new_cell)
{
	cells.insert(new_cell);
	new_cell->setGroup(this);
}
void Group::addParent(pair<int, int> parent_cell)
{
	parent_cells.insert(parent_cell);
}
void Group::addCells(set<Cell*> new_cells)
{
	for (auto c : new_cells)
	{
		addCell(c);
	}
}
void Group::removeCells(const set<Cell*> & del_cells)
{
	for (auto c : del_cells)
	{
		cells.erase(c);
		c->eraseJustGroup(this);
	}
}
bool Group::is_intersect(const Group & other, set<Cell*> & intersection)
{
	intersection = getIntersection(other);
	return intersection.size() != 0 && intersection.size() != other.size() && intersection.size() != size();
}
bool Group::is_include(const Group & little)
{
	if (little.size() >= this->size())
		return false;
	for (auto c : little.cells)
	{
		auto search = cells.find(c);
		if (search == cells.end())
			return false;
	}
	return true;
}
bool Group::subtraction(Group * little)
{
	if (!is_include(*little))
		return false;
	removeCells(little->cells);
	bomb -= little->bomb;
	return true;
}
bool Group::intersect(Group * other, Group *res)
{
	if (is_intersect(*other, res->cells))
	{
		Group*gb;
		Group*gl;
		if (bomb > other->bomb)
		{
			gb = this;
			gl = other;
		}
		else
		{
			gl = this;
			gb = other;
		}
		int k = gb->size() - res->size();
		if (k == (gb->bomb - gl->bomb))
		{
			//выделяем новую группу
			other->removeCells(res->cells);
			removeCells(res->cells);
			res->bomb = k;
			bomb -= k;
			other->bomb -= k;
			res->addParent(parent_cells);
			res->addParent(other->parent_cells);
			return true;
		}
		return false;
	}
	return false;
}

bool Group::is_equal(Group * other)
{
	if (other->cells == this->cells && other->bomb == this->bomb)
		return true;
	return false;
}
bool Group::unite(Group * other)
{
	if (this == other)
		return false;
	if (is_equal(other))
	{
		auto cset = other->cells;
		for (auto it = cset.begin(); it != cset.cend() /* not hoisted */; /* no increment */)
		{
			auto val = *it;
			val->removeGroup(other);
			val->setGroup(this);
			it++;
		}
		for (auto p : other->parent_cells)
		{
			this->addParent(p);
		}
		return true;
	}
	return false;
}

void Group::print()
{
	for (auto p : parent_cells)
	{
		cout << "[" << p.first << "," << p.second << "]";
	}
	cout << ":";
	for (auto c : cells)
	{
		cout << "(" << c->getCoords().first << ";" << c->getCoords().second << ")";
	}
	cout << " bomb: " << bomb;
}

MineSweeper::MineSweeper(const vector<vector<int>>& matrix, int max_bomb) :matrix(matrix), max_bomb(max_bomb)
{
	init();
	//cout << "after_init:";
	//print_groups();
	//cout << "after_unite:";
	unite_all_equal_groups();
	//print_groups();
	
}

MineSweeper::~MineSweeper()
{
	auto s = getSetOfGroups();
	for (auto g : s)
	{
		delete g;
	}
	for (auto c : board)
	{
		delete c.second;
	}
}

void MineSweeper::init()
{
	map<pair<int, int>, Group*> _groups;
	// проходим по матрице, просматриваем все клетки, если клетка открытая, то мы добавляем все ее закрытые граничные клетки в board
	for (int i = 0; i < matrix.size(); i++)
	{
		for (int j = 0; j < matrix[i].size(); j++)
		{
			int bomb = matrix[i][j];
			if (bomb >= 0) // клетка открытая
			{
				vector<pair<int, int>> n = get_not_opened_neighbors(i, j);//находим все неоткрытые клетки соседние с данной
				if (!n.size())
					continue;
				set<Cell*> cset;//граничные клетки с данной
				for (auto c : n)//проходим по соседним неоткрытым клеткам создаем для них Cell, добавляем в board и соответствующую группу
				{
					Cell * cell;
					auto search = board.find(c);
					if (search == board.end())//клетку еще не занесли в границу
					{
						cell = new Cell(c);
						board[c] = cell;
					}
					else
						cell = board[c];
					cset.insert(cell);
				}

				auto key = make_pair(i, j);
				auto search = _groups.find(key); //ищем группу данной клетки
				Group * group = nullptr; //указатель на группу нашей открытой клетки
				if (search == _groups.end()) //если данная группа не принадлежит никакой клетке
				{
					//ищем группу с таким же набором клеток и количеством бомб
					/*for (auto g : getSetOfGroups())
					{
						if (g->getCells() == cset && g->getBomb() == bomb)
						{
							group = g;
							group->addParent(key);
							break;
						}
					}*/
					/*if (!group)//если не нашли создаем новую
					{*/
					group = new Group(key, bomb);
					//}
					//добавляем в мэп групп в любом случае
					_groups[key] = group;
				}
				else
					group = _groups[key];
				group->addCells(cset);
			}
		}
	}
	for (auto g : _groups)
	{
		groups.insert(g.second);
	}
}

bool MineSweeper::substract()
{
	auto s = getSetOfGroups();
	bool res = false;
	for (auto bg : s)
	{
		for (auto lg : s)
		{
			if (bg == lg)
				continue;
			if(bg->subtraction(lg))
				res = true;
		}
	}
	return res;
}

bool MineSweeper::intersect()
{
	auto s = getSetOfGroups();
	bool res = false;
	for (auto bg : s)
	{
		for (auto lg : s)
		{
			if (bg == lg)
				continue;
			Group *new_group = new Group();
			if (bg->intersect(lg, new_group))
			{
				res = true;
				groups.insert(new_group);
				//new_groups[]=new_group;
			}
			else
			{
				delete new_group;
			}
		}
	}
	return res;
}

bool MineSweeper::unite_all_equal_groups()
{
	auto s = getSetOfGroups();
	bool res = false;
	for (auto bg : s)
	{
		if (!bg->size())
			continue;
		for (auto lg : s)
		{
			if (!lg->size())
				continue;
			if (bg->unite(lg))
				res = true;
		}
	}

		for (auto it = groups.cbegin(); it != groups.cend() /* not hoisted */; /* no increment */)
		{
			if (!(*it)->size())
			{
				groups.erase(it++);    // or "it = m.erase(it)" since C++11
			}
			else
			{
				++it;
			}
		}

	return res;
}

bool MineSweeper::is_row_valid(int row)
{
	return row >= 0 && row < matrix.size();
}

bool MineSweeper::is_col_valid(int col)
{
	return col >= 0 && col < matrix[0].size();
}

vector<pair<int, int>> MineSweeper::get_not_opened_neighbors(int row, int col)
{
	vector<pair<int, int>> res;
	for (int dr = -1; dr < 2; dr++)
	{
		int r = row + dr;
		if (!is_row_valid(r))
			continue;
		for (int dc = -1; dc < 2; dc++)
		{
			int c = col + dc;
			if (!is_col_valid(c))
				continue;
			if (matrix[r][c] < 0)
			{
				res.push_back(make_pair(r, c));
			}
		}
	}
	return res;
}

set<Group*> MineSweeper::getSetOfGroups()
{
	return groups;
}

void MineSweeper::print_groups()
{
	cout << endl;
	for (auto g : getSetOfGroups())
	{
		g->print();
		cout << endl;
	}
}

bool MineSweeper::getLogicSolution(pair<int, int>& res)
{
	auto gset = getSetOfGroups();
	for (auto g : gset)
	{
		if (g->getBomb() == 0)
		{
			res = (*(g->getCells().begin()))->getCoords();
			return true;
		}
	}
	return false;
}

bool MineSweeper::getProbablisticSolution(pair<int, int>& res)
{
	double min = 1;
	double epsilon = 0.1;
	Cell * minc = nullptr;
	map<Cell*, double> prob_map;
	for (auto g : groups)
	{
		double prob = g->getBomb() / (double)g->size();
		for (auto c : g->getCells())
		{
			auto search = prob_map.find(c);
			if (search == prob_map.end())
				prob_map[c] = prob;
			else
				prob_map[c] = 1 - (1 - prob)*(1 - prob_map[c]);
		}
	}
	bool repeat;
	int max_iter = 10;
	int i = 0;
	do 
	{
		repeat = false;
		for (auto g : groups)
		{
			double sum = 0;
			for (auto c : g->getCells())
			{
				sum += prob_map[c];
			}
			if (abs((double)(sum - g->getBomb())) > epsilon)
			{
				double k = g->getBomb() / sum;
				for (auto c : g->getCells())
				{
					prob_map[c] *= k;
				}
				repeat = true;
			}
		}
		i++;
	} while (repeat && i<max_iter);

	for (auto p : prob_map)
	{
		double val = p.second;
		if (val < min)
		{
			min = val;
			minc = p.first;
		}
	}
	res = minc->getCoords();
	return min < 1;
}

bool MineSweeper::getSolution(pair<int, int>& res)
{
	bool resolved = false;
	bool repeat = false;
	//int i = 0;
	do
	{
		resolved = getLogicSolution(res);
		if (resolved)
			break;
		repeat = false;
		if (substract())
		{
			resolved = getLogicSolution(res);
			if (resolved)
				break;
			repeat = true;
			unite_all_equal_groups();
		}
		//cout << "after_substract:";
		//print_groups();
		if (intersect())
		{
			repeat = true;
			unite_all_equal_groups();
		}
		//i++;
		//print_groups();
	} while (repeat);
	//cout << endl;
	//print_groups();
	if (resolved)
	{
		cout << "result: row: " << res.first << " column: " << res.second;
		return true;
	}
	if (getProbablisticSolution(res))
	{
		cout << "result: row: " << res.first << " column: " << res.second;
		return true;
	}
	cout << "I don't have solution :(";
	return false;
}

void Cell::setGroup(Group * g)
{
	if (g)
	{
		group.insert(g);
		//reculcProbability();
	}
	
}

void Cell::removeGroup(Group * g)
{
	g->removeCells(set<Cell*>{this});
}


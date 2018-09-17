#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "state.h"
using namespace std;

// reset the rubik to the initial state
void State::reset()
{
	for (int k = 0; k < 6; ++k)
		for (int i = 0; i < 3; ++i) 
			for (int j = 0; j < 3; ++j)
				color[k][i][j] = k;
}


State::State()
{
	reset();
}

// clear history
void State::clearHistory()
{
	history.clear();
}

// rotate the rubik randomly
void State::random(int times)
{
    srand(unsigned(time(0)));
	reset();
	while (times--)
		rotate(operations[rand() % 12]);
}

// rotate a face
void State::rotateFace(int face)
{
	int tmp[3][3];
	for (int i = 0; i < 3; ++i) 
		for (int j = 0; j < 3; ++j)
			tmp[i][j] = color[face][i][j];
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			color[face][j][2 - i] = tmp[i][j];
}

// get coordinates of cells in a bar
vector<pair<int, int> > State::getBarCells(int bar)
{
	vector<pair<int, int> > p;
	if (bar == barU)
	{
		for (int i = 0; i < 3; ++i)
			p.push_back(make_pair(0, i));
	}
	else if (bar == barD)
	{
		for (int i = 0; i < 3; ++i)
			p.push_back(make_pair(2, i));
	}
	else if (bar == barL)
	{
		for (int i = 0; i < 3; ++i)
			p.push_back(make_pair(i, 0));
	}
	else if (bar == barR)
	{
		for (int i = 0; i < 3; ++i)
			p.push_back(make_pair(i, 2));
	}
	return p;	
}

// transfer a bar
void State::transBar(int barTo, int faceTo, int barFrom, int faceFrom, int rev = false)
{
	vector<pair<int, int> > pTo = getBarCells(barTo);
	vector<pair<int, int> > pFrom = getBarCells(barFrom);
	if (rev) 
		reverse(pFrom.begin(), pFrom.end());
	for (int k = 0; k < 3; ++k)
		color[faceTo][pTo[k].first][pTo[k].second] = 
			color[faceFrom][pFrom[k].first][pFrom[k].second];
}

// rotate a layer
void State::rotate(string _d)
{
	history.push_back(_d);
	
	int f = 1;
	if (_d.size() > 1) f = 3;
	char d = _d[0];
	while (f--)
	{
		if (d == 'U')
		{
			rotateFace(faceU);
			transBar(barU, 6, barU, faceF);
			transBar(barU, faceF, barU, faceR);
			transBar(barU, faceR, barU, faceB);
			transBar(barU, faceB, barU, faceL);
			transBar(barU, faceL, barU, 6);
		}
		else if (d == 'D')
		{
			rotateFace(faceD);
			transBar(barD, 6, barD, faceF);
			transBar(barD, faceF, barD, faceL);
			transBar(barD, faceL, barD, faceB);
			transBar(barD, faceB, barD, faceR);
			transBar(barD, faceR, barD, 6);
		}
		else if (d == 'L')
		{
			rotateFace(faceL); 
			transBar(barL, 6, barL, faceF);
			transBar(barL, faceF, barL, faceU);
			transBar(barL, faceU, barR, faceB, true);
			transBar(barR, faceB, barL, faceD, true);
			transBar(barL, faceD, barL, 6);
		}
		else if (d == 'R')
		{
			rotateFace(faceR);
			transBar(barR, 6, barR, faceF);
			transBar(barR, faceF, barR, faceD);
			transBar(barR, faceD, barL, faceB, true);
			transBar(barL, faceB, barR, faceU, true);
			transBar(barR, faceU, barR, 6);
		}
		else if (d == 'F')
		{
			rotateFace(faceF);
			transBar(barD, 6, barD, faceU);
			transBar(barD, faceU, barR, faceL, true);
			transBar(barR, faceL, barU, faceD);
			transBar(barU, faceD, barL, faceR, true);
			transBar(barL, faceR, barD, 6);
		}
		else if (d == 'B')
		{
			rotateFace(faceB);
			transBar(barU, 6, barU, faceU);
			transBar(barU, faceU, barR, faceR);
			transBar(barR, faceR, barD, faceD, true);
			transBar(barD, faceD, barL, faceL);
			transBar(barL, faceL, barU, 6, true);
		}
	}
}

// rotate in batch
void State::rotateBatch(vector<string> o)
{
	for (auto k:o)
		rotate(k);
}

// is (x, y) on faceU blue
bool State::isTopBlue(int x, int y)
{
	return color[faceU][x][y] == blue;
}

// rotate in batch (param from string)
void State::rotateBatch(string seq)
{
	vector<string> o;
	string buf = "";
	for (int i = 0; i < (int)seq.size(); ++i)
		if (seq[i] == ' ')
		{
			o.push_back(buf);
			buf = "";
		}
		else buf += seq[i];
	if (buf != "")
		o.push_back(buf);
	rotateBatch(o);
}

void State::rotateH()
{
    rotateFace(faceU);
    for (int k = 0; k < 3; ++k)
        rotateFace(faceD);
    char tmp[10][10];
    memcpy(tmp, color[faceR], sizeof(tmp));
    memcpy(color[faceR], color[faceB], sizeof(color[faceR]));
    memcpy(color[faceB], color[faceL], sizeof(color[faceB]));
    memcpy(color[faceL], color[faceF], sizeof(color[faceL]));
    memcpy(color[faceF], tmp, sizeof(color[faceF]));
}

void State::rotateV()
{
    rotateFace(faceF);
    for (int k = 0; k < 3; ++k)
        rotateFace(faceB);  
    char tmp[10][10];
    memcpy(tmp, color[faceR], sizeof(tmp));
    memcpy(color[faceR], color[faceU], sizeof(color[faceR]));
    memcpy(color[faceU], color[faceL], sizeof(color[faceU]));
    memcpy(color[faceL], color[faceD], sizeof(color[faceL]));
    memcpy(color[faceD], tmp, sizeof(color[faceD]));
    rotateFace(faceR);
    rotateFace(faceD);
    rotateFace(faceL);
    rotateFace(faceU);
}

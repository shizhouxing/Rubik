#include <cstdio>
#include <vector>
#include <string>
#include "solver.h"
#include "rubik.h"
#include "ui_rubik.h"
using namespace std;

vector<string> Solver::nosol()
{
    valid = false;
    return vector<string>();
}

// get all possible solutions with no more than limit steps
void Solver::getPossibleSolutions(int limit, vector<vector<string> > &sol, string *cur, int cnt)
{
	if (cnt)
	{
		vector<string> tmp;
		for (int i = 0; i < cnt; ++i)
			tmp.push_back(cur[i]);
		sol.push_back(tmp);
        if (sol.size() % 100000 == 0) qApp->processEvents();
	}
	if (cnt == limit) return;
	for (int k = 0; k < 12; ++k)
	{
		cur[cnt] = operations[k];
		getPossibleSolutions(limit, sol, cur, cnt + 1);
	}
}

// check whether {e1,e2} in {c1,c2,c3}
int Solver::setMatch(int c1, int c2, int c3, int e1, int e2)
{
	return (c1 == e1 || c2 == e1 || c3 == e1) && (c1 == e2 || c2 == e2 || c3 == e2);
}


// deal with the first layer
vector<string> Solver::stepFirstLayer(State state)
{
	vector<string> res;
	
	vector<vector<requirement> > req;
	
	vector<requirement> tmpReq;
	
	tmpReq.clear();
	tmpReq.push_back(requirement(faceD, 0, 1, green));
	tmpReq.push_back(requirement(faceF, 2, 1, white));
	req.push_back(tmpReq);

	tmpReq.clear();
	tmpReq.push_back(requirement(faceD, 1, 2, green));
	tmpReq.push_back(requirement(faceR, 2, 1, red));
	req.push_back(tmpReq);

	tmpReq.clear();
	tmpReq.push_back(requirement(faceD, 2, 1, green));
	tmpReq.push_back(requirement(faceB, 2, 1, yellow));
	req.push_back(tmpReq);

	tmpReq.clear();
	tmpReq.push_back(requirement(faceD, 1, 0, green));
	tmpReq.push_back(requirement(faceL, 2, 1, orange));
	req.push_back(tmpReq);
	
	string tmp[10];
	getPossibleSolutions(6, sol, tmp, 0);
	
	State cur = state;
	cur.clearHistory();
    
    time_t startt = clock();
	
	for (int i = 0; i < 4; ++i)
	{
		int found = false;
        for (int j = 0; j < (int)sol.size(); ++j)
		{
            if (!asked && (clock() - startt > 15 * CLOCKS_PER_SEC))
            {
                asked = true;
                if (QMessageBox::question(NULL, "Warning", "I guess your input is not very correct...\nWould you like to pause calculation and check it again?") == QMessageBox::Yes)
                {
                    return nosol();
                }
            }
			State tmp = cur;
			tmp.rotateBatch(sol[j]);
			int valid = true;
			for (int k = 0; k <= i && valid; ++k)
				for (auto r:req[k])
				{
					valid &= tmp.color[r.face][r.x][r.y] == r.c;
					if (!valid) break;
				}
			if (valid)
			{
				cur.rotateBatch(sol[j]);
				found = true;
				break;
			}
            if (j % 1000 == 0)
                qApp->processEvents();
		}
		if (!found) 
            return nosol();
	}
	
	for (int _ = 0; _ < 4; ++_)
	{
		// check whether finished
		int finished = true;
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				if (cur.color[faceD][i][j] != green) finished = false;
		for (int k = 1; k < 5; ++k)
			for (int i = 0; i < 3; ++i)
				if (cur.color[k][2][i] != cur.color[k][1][1]) finished = false;
		if (finished) break;
		
		bool aside = false;
		for (int k = 1; k < 5; ++k)
			if (cur.color[k][0][0] == green || cur.color[k][0][2] == green) aside = true;
			
		if (!aside)
		{
			bool onTop = false;
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					if (cur.color[faceU][i][j] == green) onTop = true;
			if (!onTop)
			{
				if (cur.color[faceD][0][0] != green)
					cur.rotateBatch("F U F'");
				else if (cur.color[faceD][0][2] != green)
					cur.rotateBatch("F' U F");
				else if (cur.color[faceD][2][0] != green)
					cur.rotateBatch("B' U B");
				else if (cur.color[faceD][2][2] != green)
					cur.rotateBatch("B U B'");
			}
			for (int k = 1; k < 5; ++k)
				if (cur.color[k][0][0] == green || cur.color[k][0][2] == green) aside = true;
			if (!aside)
			{
				while (true)
				{
					bool ok = true;
					if (cur.color[faceU][2][2] == green && cur.color[faceD][0][2] != green)
						cur.rotateBatch("F' U F");
					else if (cur.color[faceU][2][0] == green && cur.color[faceD][0][0] != green)
						cur.rotateBatch("F U' F'");
					else if (cur.color[faceU][0][0] == green && cur.color[faceD][2][0] != green)
						cur.rotateBatch("B' U B");
					else if (cur.color[faceU][0][2] == green && cur.color[faceD][2][2] != green)
						cur.rotateBatch("B U' B'");
					else
						ok = false;
					if (ok)
						break;
					else
						cur.rotate("U");
                    if (cur.history.size() > maxSteps) 
                        return nosol();
				}
			}
		}
		while (true)
		{
			bool ok = true;
			if (cur.color[faceF][0][2] == green && cur.color[faceR][0][0] == red)
				cur.rotateBatch("U R U' R'");
			else if (cur.color[faceF][0][2] == white && cur.color[faceR][0][0] == green)
				cur.rotateBatch("U' F' U F");
			else if (cur.color[faceF][0][0] == green && cur.color[faceL][0][2] == orange)
				cur.rotateBatch("U' L' U L");
			else if (cur.color[faceF][0][0] == white && cur.color[faceL][0][2] == green)
				cur.rotateBatch("U F U' F'");
			else if (cur.color[faceB][0][0] == green && cur.color[faceR][0][2] == red)
				cur.rotateBatch("U' R' U R");
			else if (cur.color[faceB][0][0] == yellow && cur.color[faceR][0][2] == green)
				cur.rotateBatch("U B U' B'");
			else if (cur.color[faceB][0][2] == green && cur.color[faceL][0][0] == orange)
				cur.rotateBatch("U L U' L'");
			else if (cur.color[faceB][0][2] == yellow && cur.color[faceL][0][0] == green)
				cur.rotateBatch("U' B' U B");
			else 
				ok = false;
			if (ok)
				break;
			else
				cur.rotate("U");
            if (cur.history.size() > maxSteps) 
                return nosol();            
		}
	}
	
	for (auto k:cur.history)
		res.push_back(k);
	
	return res;
}

// deal with the second layer
vector<string> Solver::stepSecondLayer(State state)
{
	vector<string> res;
	
	State cur = state;
	cur.clearHistory();
	
	for (int _ = 0; _ < 4; ++_)
	{
		int finished = true, cntBlue = 0;
		for (int k = 1; k < 5; ++k)
			for (int i = 1; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
				{
					if (cur.color[k][i][j] != cur.color[k][1][1])
						finished = false;
					if (cur.color[k][i][j] == blue)
						cntBlue++;
				}
						
		if (finished) break;
		
		if (cntBlue == 0)
		{
			if (cur.color[faceF][1][2] != white || cur.color[faceR][1][0] != red)
				cur.rotateBatch("U' F' U F U R U' R'");
		
			else if (cur.color[faceR][1][2] != red || cur.color[faceB][1][0] != yellow)
				cur.rotateBatch("U' R' U R U B U' B'");
				
			else if (cur.color[faceB][1][2] != yellow || cur.color[faceL][1][0] != orange)
				cur.rotateBatch("U' B' U B U L U' L'");
				
			else if (cur.color[faceL][1][2] != orange || cur.color[faceF][1][0] != white)
				cur.rotateBatch("U' L' U L U F U' F'");
		}
		
		while (true)
		{
			int ok = true;
			
			if (cur.color[faceU][1][2] == white && cur.color[faceR][0][1] == red)
				cur.rotateBatch("U' F' U F U R U' R'");
			else if (cur.color[faceU][2][1] == red && cur.color[faceF][0][1] == white)
				cur.rotateBatch("U R U' R' U' F' U F");
		
			else if (cur.color[faceU][0][1] == red && cur.color[faceB][0][1] == yellow)
				cur.rotateBatch("U' R' U R U B U' B'");
			else if (cur.color[faceU][1][2] == yellow && cur.color[faceR][0][1] == red)
				cur.rotateBatch("U B U' B' U' R' U R");
				
			else if (cur.color[faceU][1][0] == yellow && cur.color[faceL][0][1] == orange)
				cur.rotateBatch("U' B' U B U L U' L'");
			else if (cur.color[faceU][0][1] == orange && cur.color[faceB][0][1] == yellow)
				cur.rotateBatch("U L U' L' U' B' U B");
				
			else if (cur.color[faceU][2][1] == orange && cur.color[faceF][0][1] == white)
				cur.rotateBatch("U' L' U L U F U' F'");
			else if (cur.color[faceU][1][0] == white && cur.color[faceL][0][1] == orange)
				cur.rotateBatch("U F U' F' U' L' U L");
			else 
				ok = false;
				
			if (ok) 
				break;
			else
				cur.rotate("U");
            if (cur.history.size() > maxSteps) 
                return nosol();            
		}
	}
	
	for (auto k:cur.history)
		res.push_back(k);
	
	return res;
}

// deal with the third layer
vector<string> Solver::stepThirdLayer(State state)
{
	vector<string> res;
	
	State cur = state;
	cur.clearHistory();

	while (true)
	{
		int cntBlue = cur.isTopBlue(0, 1) + cur.isTopBlue(1, 2) + cur.isTopBlue(2, 1) + cur.isTopBlue(1, 0);
		if (cntBlue == 4) break;
		if (cntBlue == 2)
		{
			if (cur.isTopBlue(0, 1) && cur.isTopBlue(2, 1))
				cur.rotate("U");
			else if (cur.isTopBlue(1, 0) && cur.isTopBlue(2, 1))
				cur.rotate("U");
			else if (cur.isTopBlue(2, 1) && cur.isTopBlue(1, 2))
				cur.rotateBatch("U U");
			else if (cur.isTopBlue(0, 1) && cur.isTopBlue(1, 2))
				cur.rotate("U'");	
		}
		cur.rotateBatch("F R U R' U' F'");
        if (cur.history.size() > maxSteps) 
            return nosol();        
	}
	
	while (true)
	{
		while (cur.color[faceF][0][1] != white) cur.rotate("U");
		int finished = true;
		for (int k = 1; k < 5; ++k)
			if (cur.color[k][0][1] != cur.color[k][1][1])
				finished = false;
		if (finished) break;
		
		if (cur.color[faceL][0][1] != orange && cur.color[faceR][0][1] != red)
			cur.rotateBatch("R U R' U R U U R'");
		else if (cur.color[faceL][0][1] != orange)
			cur.rotateBatch("F U F' U F U U F'");
		else
			cur.rotateBatch("L U L' U L U U L'");
        if (cur.history.size() > maxSteps) 
            return nosol();        
	}
		
	while (true)
	{
		int cntSetMatch = 0;
		cntSetMatch += setMatch(cur.color[faceF][0][2], cur.color[faceR][0][0], cur.color[faceU][2][2], white, red);
		cntSetMatch += setMatch(cur.color[faceR][0][2], cur.color[faceB][0][0], cur.color[faceU][0][2], red, yellow);
		cntSetMatch += setMatch(cur.color[faceB][0][2], cur.color[faceL][0][0], cur.color[faceU][0][0], yellow, orange);
		cntSetMatch += setMatch(cur.color[faceL][0][2], cur.color[faceF][0][0], cur.color[faceU][2][0], orange, white);
		if (cntSetMatch == 4) break;
		
		if (cntSetMatch == 0 || setMatch(cur.color[faceF][0][2], cur.color[faceR][0][0], cur.color[faceU][2][2], white, red))
			cur.rotateBatch("U R U' L' U R' U' L");
		else if (setMatch(cur.color[faceR][0][2], cur.color[faceB][0][0], cur.color[faceU][0][2], red, yellow))
			cur.rotateBatch("U B U' F' U B' U' F");
		else if (setMatch(cur.color[faceB][0][2], cur.color[faceL][0][0], cur.color[faceU][0][0], yellow, orange))
			cur.rotateBatch("U L U' R' U L' U' R");
		else if (setMatch(cur.color[faceL][0][2], cur.color[faceF][0][0], cur.color[faceU][2][0], orange, white))
			cur.rotateBatch("U F U' B' U F' U' B");
        if (cur.history.size() > maxSteps) 
            return nosol();        
	}
	
	while (true)
	{
		int finished = true;
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				if (cur.color[faceU][i][j] != blue) finished = false;
		if (finished) break;
		
		while (cur.color[faceU][2][2] == blue) cur.rotate("U");
		while (cur.color[faceU][2][2] != blue)
			cur.rotateBatch("R' D' R D");
        if (cur.history.size() > maxSteps) 
            return nosol();        
	}
	
	while (cur.color[faceF][0][0] != white) 
    {
		cur.rotate("U");
        if (cur.history.size() > maxSteps) 
            return nosol();
    }
		
	for (auto k:cur.history)
		res.push_back(k);
	
	return res;
}

// solve the Solver
vector<vector<string> > Solver::solve(State state)
{
	vector<vector<string> > res;
    
    valid = true;
    asked = false;
	
	vector<string> step = stepFirstLayer(state);
	res.push_back(step);
	state.rotateBatch(step);
	
	step = stepSecondLayer(state);
	res.push_back(step);
	state.rotateBatch(step);
	
	step = stepThirdLayer(state);
	res.push_back(step);
	state.rotateBatch(step);
    
    if (!valid) 
    {
        res.clear();
        res.push_back(vector<string>());
        if (!asked)
            QMessageBox::information(NULL, "Warning", "It seems your input is not correct...\nCalculation has been paused.");
    }
	
	return res;
}

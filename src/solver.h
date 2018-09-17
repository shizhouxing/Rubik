#include <cstdio>
#include <vector>
#include <string>
#include "state.h"
using namespace std;

#ifndef SOLVER_H

#define SOLVER_H

const int maxSteps = 5000;

class requirement 
{
public:
	int face, x, y, c;
	
	requirement(){}
	requirement(int _face, int _x, int _y, int _c): face(_face), x(_x), y(_y), c(_c) {}
};

class Solver
{
private:

    bool valid, asked;
    
	// possible solutions in limit times
	vector<vector<string> > sol;

	// deal with the first layer
	vector<string> stepFirstLayer(State state);

	// deal with the second layer
	vector<string> stepSecondLayer(State state);
	
	// deal with the second layer
	vector<string> stepThirdLayer(State state);

    // not a valid input
    vector<string> nosol();
    
	// get all possible solutions with no more than limit steps
	void getPossibleSolutions(int limit, vector<vector<string> > &sol, string *cur, int cnt);
	
	// check whether {e1,e2} in {c1,c2,c3}
	int setMatch(int c1, int c2, int c3, int e1, int e2);
	
public:


	// solve the rubik
	vector<vector<string> > solve(State state);
};

#endif

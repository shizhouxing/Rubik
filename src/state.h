#include <cstdio>
using namespace std;

#ifndef STATE_H

#define STATE_H

enum COLOR
{
	green, white, red, yellow, orange, blue, black
};

enum FACE
{
	faceD, faceF,faceR, faceB, faceL, faceU
};

enum BAR
{
	barU, barD, barL, barR
};

const string faceName[6] = {"BOTTOM", "FRONT", "RIGHT", "BACK", "LEFT", "TOP"};

const string colorString[6] = {"G", "W", "R", "Y", "O", "B"};

const string operations[] = {"U", "U'", "D", "D'", "L", "L'", "R", "R'", "F", "F'", "B", "B'"};	


// to store the state of the rubik
class State 
{
private:
	
	// get coordinates of cells in a bar
	vector<pair<int, int> > getBarCells(int bar);
	
	// transfer a bar
	void transBar(int barTo, int faceTo, int barFrom, int faceFrom, int rev);
	
	// rotate a face
	void rotateFace(int face);
	
public:

	int color[7][3][3];
	vector<string> history;
	
	// is (x, y) on faceU blue
	bool isTopBlue(int x, int y);		

	// reset the rubik to the initial state
	void reset();
	
	// rotate the rubik randomly
	void random(int times);
	
	// clear history
	void clearHistory();

	State();

	// rotate a layer
	void rotate(string d); 
	
	// rotate in batch (param from string)
	void rotateBatch(string seq);
	
	// rotate in batch
	void rotateBatch(vector<string> o);
    
    void rotateH();
    
    void rotateV();
};

#endif

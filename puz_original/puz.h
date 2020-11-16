using namespace std;
#include <vector>

#ifndef GRIDSIZE
#define GRIDSIZE 4
#endif

#define GENSIZE 30


// key values for kbhit getch
#define KB_PRE 224 //weird input that appears before every arrow key input
#define KB_UP 72
#define KB_LEFT 75
#define KB_DOWN 80
#define KB_RIGHT 77

enum direction {up, left, down, right, end};


vector<int> getBlocks(int row[]);
bool allLeft(   int board[][GRIDSIZE]);
bool allRight(  int board[][GRIDSIZE]);
bool allUp(     int board[][GRIDSIZE]);
bool allDown(   int board[][GRIDSIZE]);
bool rowLeft(int row[GRIDSIZE]);

bool drawTestGrid(int size);

void addRandomBlock(int board[][GRIDSIZE], int value);

float scoreState(int board[][GRIDSIZE]);
direction lookahead(int board[][GRIDSIZE]);

bool isFull(int board[][GRIDSIZE]);
bool drawBoard(int board[][GRIDSIZE]);
void test1();
void startGameUser();
void trainBotRun();
int startGameBot(unsigned short int weights, bool watch);
int objectiveFunction(unsigned short int weights);
void fitnessesProbs(unsigned short int vectors[GENSIZE], float fitnesses[GENSIZE]);
int testSGA();

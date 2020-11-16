//v2: removing x and y, place will be determined by their place in the 2d list (vector)
//v3: just use the vector array, no classes or objects needed I guess
//v4: actual gameplay loop implemented
//v5: trying out kbhit in <conio.h> and printing blanks instead of 0
//v6: adding tracking to see if values change, not spawning or printing if nothing changed
//v7: simple state lookahead bot
//v8: making bot good
//v9: I guess I was just making it somewhat functional
//v10: random isn't random?? whatever I implemented variable scoring weights
//v11: more stuff, still have the huge random gen thinger
//v12: "done"
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cstring>
#include <conio.h>
#include <chrono>
#include "puz.h"
#include "sga.cpp"
#include "util.cpp"
#define GRIDSIZE 4

using namespace std;

// MATCHMULT = 0.15; 
// POTMULT = 2; 
// COUNTMULT = 16/2; 

int main()
{

    // test1();
    // startGameUser();

    // int weights = encode(7, 2, 8);
    // int finalScore = startGameBot(weights);
    // printf("Final score: %d", finalScore);

    // crossover(0b1111111111111111, 0b0000000000000000, 4);
    // testSGA();




    printf("---- Doubling Game ----\n");
    printf("Enter the corresponding number to make a selection\n");
    printf("1: Play Game\n");
    printf("2: Watch Default Bot\n");
    printf("3: Train Bot (SGA)\n");

    char selection = getchar();
    switch (selection)
    {
    case '1':
        startGameUser();
        break;
    case '2':
        startGameBot(0, true);
        break;
    case '3':
        trainBotRun();

    default:
        break;
    }
    
    printf("Program complete.\n");
    system("pause");
    return 0;
}

// runs the game with the given weights a certain number of times and outputs the total score (totaled numbers)
//  "weights" is an encoded bit string of MATCH POTENTIAL and COUNT, 0000MMMMPPPPCCCC.
int objectiveFunction(unsigned short int weights){
    const int CYCLES = 75;
    // const int count_weight = 0b0000000000001111;
    int total_values = 0;

    for (int i = 0; i < CYCLES; i++)
    {
        total_values += startGameBot(weights, false);
    }
    
    return total_values;
}

// uses given list of vectors (weights) to fill in 
//     given list of fitnesses (split as probabilities of 100) corresponding to each vector.
void fitnessesProbs(unsigned short int vectors[GENSIZE], float fitnesses[GENSIZE]){
    int total = 0;
    int val;
    for (int i = 0; i < GENSIZE; i++){
        val = objectiveFunction(vectors[i]);
        total += val;
        fitnesses[i] = val;

        printf("Vector %6x fitness %6d\n",vectors[i], val);
    }
    printf("Generation average fitness: %f\n", (float)total/GENSIZE);
    for (int j = 0; j < GENSIZE; j++)//getting probabilities for biased roulette
    {
        fitnesses[j] = fitnesses[j]*100.0/(float)total;
    }
    
    return;
}

// runs functions to train and test a bot
void trainBotRun(){

    const int MUTATIONS = 15;
    const int GENERATIONS = 15;

    printf("\n# of mutations: %d",MUTATIONS);
    printf("\n# of generations: %d",GENERATIONS);
    printf("\nSize of generations: %d",GENSIZE);
    printf("\n");

    unsigned short int generation[GENSIZE];
    unsigned short int generation2[GENSIZE];
    float fitnesses[GENSIZE];
    int numOffspring[GENSIZE];
    initialGeneration(generation);
    printf("\n\nGENERATION 1: \n");
    fitnessesProbs(generation, fitnesses);
    spinBiasedRoulette(fitnesses, numOffspring);
    matingPool(generation,numOffspring,generation2);
    mutate(generation2, MUTATIONS);

    for (int i = 2; i < GENERATIONS; i++)
    {
        copy1dArray(generation2, generation, GENSIZE);
        printf("\n\nGENERATION %d: \n", i);
        fitnessesProbs(generation, fitnesses);
        spinBiasedRoulette(fitnesses, numOffspring);
        matingPool(generation,numOffspring,generation2);
        mutate(generation2, MUTATIONS);
    }
    return;
}

//runs testing on SGA functions
int testSGA(){
    const int IGSIZE = GENSIZE;
    int big = 0b101011110000;
    // struct inputVectors out = decode(big);
    int match = getMatch(big);
    int pot = getPot(big);
    int count = getCount(big);

    unsigned short int generation[IGSIZE];
    unsigned short int generation2[IGSIZE];
    float fitnesses[IGSIZE];
    int numOffspring[IGSIZE];

    initialGeneration(generation);
    printf("\n\nGENERATION 1: \n");
    fitnessesProbs(generation, fitnesses);
    spinBiasedRoulette(fitnesses, numOffspring);
    matingPool(generation,numOffspring,generation2);

    printf("\n\nGENERATION 2: \n");
    fitnessesProbs(generation2, fitnesses);

    mutate(generation2,5);


    return 0;
}

//TODO maybe have a general function with a switch statement, since they all use allLeft anyway

//combines horizontally adjacent same-value blocks and slides all blocks to the LEFT, returns true if values changed
bool allLeft(int board[][GRIDSIZE]){
    bool changed = false;
    for (int y = 0; y < GRIDSIZE; y++){
        if (rowLeft(board[y])) changed = true;
    }
    return changed;
}

//TODO probably have it return new arrays rather than doing it in place (inside the reverse)
//combines horizontally adjacent same-value blocks and slides all blocks to the RIGHT, returns true if values changed
bool allRight(int board[][GRIDSIZE]){
    bool changed = false;
    for (int y = 0; y < GRIDSIZE; y++){
        reverseArray(board[y], GRIDSIZE);
        if (rowLeft(board[y])) changed = true;
        reverseArray(board[y], GRIDSIZE);
    }
    return changed;
}

//combines horizontally adjacent same-value blocks and slides all blocks to the TOP, returns true if values changed
bool allUp(int board[][GRIDSIZE]){
    bool changed = false;
    for (int x = 0; x < GRIDSIZE; x++)
    {
        int* tempSlice;
        tempSlice = grabVert(board,x);
        if (rowLeft(tempSlice)) changed = true;
        insertVert(board,tempSlice,x);
    }
    return changed;
    
}

//combines horizontally adjacent same-value blocks and slides all blocks to the BOTTOM, returns true if values changed
bool allDown(int board[][GRIDSIZE]){
    bool changed = false;
    for (int x = 0; x < GRIDSIZE; x++)
    {
        int* tempSlice;
        tempSlice = grabVert(board,x);
        reverseArray(tempSlice,GRIDSIZE);
        if (rowLeft(tempSlice)) changed = true;
        reverseArray(tempSlice,GRIDSIZE);
        insertVert(board,tempSlice,x);
        //tempSlice should be freed when allDown's stack is removed, right??
    }
    return changed;
    
}

//performs left combining and sliding operations on a row, returns true if anything changes
bool rowLeft(int row[GRIDSIZE]){
    //for tracking whether any change was made
    //  for case where all are on one side 
    bool changed = false; 

    vector<int> rowblox = getBlocks(row);

    //collapse and add adjacent same-value blocks
    int a = 0, b = 1;//could probably separate this into another function
    while (b < rowblox.size()){
        if(row[rowblox[a]] == row[rowblox[b]]){
            row[rowblox[a]] *= 2;          //double leftmost block
            row[rowblox[b]] = 0;           //delete rightmost block
            rowblox.erase(rowblox.begin()+b);  //remove rightmost block from rowblox
            changed = true;
        }
        a++;
        b++;
    }

    //slide all blocks to the left
    int boardInd = 0, rowbloxInd = 0; //I'm just overwriting hot, logically it SHOULD be fine right??
    while (rowbloxInd < rowblox.size()){
        if (row[boardInd] != row[rowblox[rowbloxInd]] ){
            changed = true;
        }
        row[boardInd++] = row[rowblox[rowbloxInd++]];
    }
    while (boardInd < GRIDSIZE){//filling the right with 0s
        row[boardInd++] = 0;
    }
    return changed;
}


//returns x locations of blocks present in the passed board row
vector<int> getBlocks(int row[GRIDSIZE]){
    vector<int> blocks;
    for (int q = 0; q < GRIDSIZE; q++)
    {
        if (row[q] != 0)
        {
            blocks.push_back(q);
        }
        
    }
    return blocks;
    
}

//adds a random block of given value in a blank space of the given board
void addRandomBlock(int board[][GRIDSIZE], int value){
    srand((unsigned) time(0)); //comment out to have the same blocks every time
    int x = rand() % GRIDSIZE;
    int y = rand() % GRIDSIZE;

    while (board[y][x] != 0){
        x = rand() % GRIDSIZE;
        y = rand() % GRIDSIZE;     
    }
    board[y][x] = value;
}

//returns true if the board is full
bool isFull(int board[][GRIDSIZE]){
    for (int i = 0; i < GRIDSIZE; i++){
        for (int j = 0; j < GRIDSIZE; j++){
            if (board[j][i] == 0){
                return false;
            }
        }
    }
    return true;
}



//draws the contents of board in a grid
bool drawBoard(int board[][GRIDSIZE]){
    int size = GRIDSIZE;
    string line = ".";
    for (int t = 0; t < size * 5 - 1; t++)
    {
        line += "_";
    }cout << line + ".\n";
    
    for (int y = 0; y < size; y++)
    {
        line = "";
        for (int x = 0; x < size; x++)
        {
            // styledPrint(board[y][x]);
            if (board[y][x] == 0){
                printf("|    ");
            }
            else{
                printf("|%3d ",  board[y][x]  );
            }
        }cout <<"|\n";
        line = "";
    }
    line = ".";
    for (int b = 0; b < size * 5 - 1; b++)
        {
            line += "-";
        }cout << line + ".\n";
    return true;
    
}

//runs through several commands on a hardcoded board, printing results
void test1(){
    int board[4][4];
    for (int i = 0; i < GRIDSIZE; i++)
    {

        for (int j = 0; j < GRIDSIZE; j++)
        {
            board[i][j] = 0;
        }
        
    }
    

    // [00 01 02 03]
    // [10 11 12 13]
    // [20 21 22 23]
    // vector<string> msg {"Hewwo", "Cpwuspwus", "Wowwd", "fwom", "VS Code", "and the Cpwuspwus extension!"};
    // msg[1] = "Mr owbaba";
    // vector<vector<int>> todee {};
    // int lel[GRIDSIZE][GRIDSIZE];
    // 
    // lel[0][1] = 99;
    //
    // print2dlist(todee);
    //
    // for (const string& word : msg)
    // {
    //     cout << word << " ";
    // }
    // cout << endl;


    int board2[4][4] = 
    {
        {1, 1, 0, 1},
        {5, 5, 1, 1},
        {0, 4, 5, 2},
        {4, 4, 0, 23 },
    };



    drawBoard(board2);
    printf("allLeft()\n");
    allLeft(board2);
    drawBoard(board2);
    printf("allRight()\n");
    allRight(board2);
    drawBoard(board2);
    printf("allUp()\n");
    allUp(board2);
    drawBoard(board2);
    printf("allDown()\n");
    allDown(board2);
    drawBoard(board2);

    printf("addRandomBlock(7)\n");
    addRandomBlock(board2,7);
    drawBoard(board2);
    printf("addRandomBlock(7)\n");
    addRandomBlock(board2,7);
    drawBoard(board2);
    printf("addRandomBlock(7)\n");
    addRandomBlock(board2,7);
    drawBoard(board2);
}

// calculates the score value of a given board based on Average block value and opportunities. Should be used on future states generated as guess
float scoreState(int board[][GRIDSIZE], unsigned short int weights = 0){
    // uncomment for testing
    // printf("~~~~~~ Lookahead ~~~~~~\n");
    // drawBoard(board);
    float MATCHMULT;
    int POTMULT;
    float COUNTMULT;
    //knob's fitness normalized around maxval's value. the way to think about it at least.
    if (weights == 0){
        MATCHMULT = 0.15; //multiplier for higher-value matches' potential. TOTWEAK
        POTMULT = 2; //multiplier for potential. Think "maxval is worth this many adjacencies" TOTWEAK
        COUNTMULT = 16/2; // multiplier for (inverted) blockCount.
    }else{
        int match = getMatch(weights);
        if(match == 0){MATCHMULT = 0;} else
        MATCHMULT = 1.0/getMatch(weights);
        POTMULT = getPot(weights);
        COUNTMULT = getCount(weights);
        // MATCHMULT = 1;
        // POTMULT = 1;
        // COUNTMULT = 1;
    }
    int blockCount = 0; //total number of blocks
    int blockValues = 0; //all block values added up
    int maxVal = 1; // highest value on the board
    int val = 0; //current value scanned with board[y][x]
    int potential = 0; //potential *new* collapses. higher value matches contribute +MULT more (to tweak).
    // horizontal scan
    for (int y = 0; y < GRIDSIZE; y++)
    {
        for (int x = 0; x < GRIDSIZE; x++)
        {
            val = board[y][x];
            if (val != 0){
                blockCount ++;
                blockValues += val;
                (val > maxVal) ? maxVal = val : maxVal = maxVal;
                // if (val > maxVal){maxVal = val;}
                //checking horizontal neighbours for match
                for(int i = 1; i < GRIDSIZE - x; i+= 1 ){
                    if (board[y][x+i] != 0){
                        if (board[y][x+i] == val){
                            potential += 1+(val*MATCHMULT);
                        }
                        break;
                    }
                }
                //checking vertical neighbours for match
                for(int j = 1; j < GRIDSIZE - x; j+= 1 ){
                    if (board[y+j][x] != 0){
                        if (board[y+j][x] == val){
                            potential += 1+(val*MATCHMULT);
                        }
                        break;
                    }
                }
            }
        }
        
    }
    float score = ( COUNTMULT*(1/blockCount) + POTMULT*potential + maxVal );
    // uncomment for testing
    //printf("Score: %d \n\n", score);
    return score;
}

//takes a board, evaluates scores of each direction and returns which had highest score
direction lookahead(int board[][GRIDSIZE], unsigned short int weights = 0){
    float scoreUp, scoreLeft, scoreDown, scoreRight;
    bool changed;

    int boardUp[GRIDSIZE][GRIDSIZE];
    copy2DArray(board, boardUp);
    changed = allUp(boardUp);
    scoreUp = changed ? scoreState(boardUp, weights) : -1;

    int boardLeft[GRIDSIZE][GRIDSIZE];
    copy2DArray(board, boardLeft);
    changed = allLeft(boardLeft);
    scoreLeft = changed ? scoreState(boardLeft, weights) : -1;

    int boardDown[GRIDSIZE][GRIDSIZE];
    copy2DArray(board, boardDown);
    changed = allDown(boardDown);
    scoreDown = changed ? scoreState(boardDown, weights) : -1;

    int boardRight[GRIDSIZE][GRIDSIZE];
    copy2DArray(board, boardRight);
    changed = allRight(boardRight);
    scoreRight = changed ? scoreState(boardRight, weights) : -1;

    float scores[4] {scoreUp, scoreLeft, scoreDown, scoreRight};

    int max = -1, maxpos = 0;
    for (int i = 0; i < 4; i++)
    {
        //uncomment for testing
        // printf("%f ",scores[i]);
        if (scores[i] > max){
            max = scores[i];
            maxpos = i;
        }
    }
    //uncomment for testing
    // printf("\n\n");
    if (max == -1){// if there are no valid moves
        return direction::end;
    }
    else{
        return (direction)maxpos;
    }
}

//begins gameplay loop, returns final value total (bot-decided input/moves, random blocks, maybe more)
int startGameBot(unsigned short int weights = 0, bool watch = false){
    //initialize board
    int board[GRIDSIZE][GRIDSIZE];
    memset(board, 0, sizeof(board));
    addRandomBlock(board, 1);
    addRandomBlock(board, 1);

    //game start
    // drawBoard(board);
    chrono::milliseconds ms = chrono::duration_cast< chrono::milliseconds >(
    chrono::system_clock::now().time_since_epoch()
);

    int roundCtr = 0;
    int maxval;
    while (true)
    {
        // printf("Scores: ");
        direction nextDir = lookahead(board, weights);
        switch (nextDir)
        {
        case direction::up:
            allUp(board);
            break;
        case direction::left:
            allLeft(board);
            break;
        case direction::down:
            allDown(board);
            break;
        case direction::right:
            allRight(board);
            break;
        case direction::end:
            maxval = boardMax(board);
            // printf("Board is full, game over. Max: %d\n",maxval);
            return maxval;

        default:
            break;
        }
        roundCtr++;
        // if (roundCtr % 2 == 0)
        // {
            addRandomBlock(board, rand()%2 + 1);
        // }

        // printf("Direction: %s \n",dirToStringPrint(nextDir).c_str() ); 
        if (watch){
            drawBoard(board);
        }
    }
    
    printf("Board is full, game over. \n");
    return boardSum(board);
    
}

//begins gameplay loop (user input, random blocks, maybe more)
void startGameUser(){
    //initialize board
    int board[GRIDSIZE][GRIDSIZE];
    memset(board, 0, sizeof(board));
    addRandomBlock(board, 1);
    addRandomBlock(board, 1);

    //game start
    drawBoard(board);
    printf("wasd or arrow keys to play, x to quit \n");
    int roundCtr = 0;

    int input = (int)NULL;

    bool changed = false;

    //parsing user input
    while (input != 'x' && input != 'X'){
        if (kbhit()){
            input = getch();
            if (input == KB_PRE){
                continue;
            }
            switch (input)
            {
            case 'w':
            case KB_UP:
                changed = allUp(board); //execution happens in-place in function, but also returns "changed" for tracking purposes
                break;
            case 'a':
            case KB_LEFT:
                changed = allLeft(board);
                break;
            case 's':
            case KB_DOWN:
                changed = allDown(board);
                break;
            case 'd':
            case KB_RIGHT:
                changed = allRight(board);
                break;
            case KB_PRE:
                break;
            default:
                printf("Please enter W, A, S, or D, or arrow kays? X to quit. \n");
                break;
            }

            if (changed){    
                //deciding whether to add random block
                //TODO - game balance, when to add, how many, which ones etc
                roundCtr++;
                // if (roundCtr % 2 == 0)
                // {
                    addRandomBlock(board, rand()%2 + 1);
                // }
                drawBoard(board);
            }changed = false;

            if (isFull(board)){
                printf("Board is full, game over. \n");
                return;
            }else{
            
                // cin >> input;
            }
        }
    }
}
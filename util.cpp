#include <iostream>
#include <vector>
#include <string>
#ifndef GRIDSIZE
#define GRIDSIZE 5
#endif
using namespace std;

//TODO probably have it return new arrays rather than doing it in place (reverse)

// reverses the passed array in-place
void reverseArray(int array[], const int size){
    vector<int> temp;
    for (int i = size - 1; i >= 0; i--)
    {
        temp.push_back(array[i]);
    }for (int j = 0; j < size; j++)
    {
        array[j] = temp[j];
    }
}

//returns a 1-D array made of a vertical slice at provided x-value
int* grabVert(int board[][GRIDSIZE], int x){
    static int slice[GRIDSIZE];
    for (int i = 0; i < GRIDSIZE; i++)
    {
        slice[i] = board[i][x];
    }
    return slice;
}

//inserts the given vertical slice at provided x-value (in-place)
void insertVert(int board[][GRIDSIZE], int slice[], int x){
    for (int y = 0; y < GRIDSIZE; y++)
    {
        board[y][x] = slice[y];
    }
}

//copies the given 2d array (4x4) into place of second argument pointer
void copy2DArray(int source[][GRIDSIZE], int destination[][GRIDSIZE]){
    // int** copy;
    // copy = new int*[GRIDSIZE*GRIDSIZE];
    for (int y = 0; y < GRIDSIZE; y++)
    {
        for (int x = 0; x < GRIDSIZE; x++)
        {
            destination[y][x] = source[y][x];
        }
    }
    return;
}


//might just not work :^)
//sets console printing to DEFAULT
void colReset(){
    printf("\x1b[0m");
}
//sets console printing to RED
void colRed(){
    printf("\x1b[31m");
}
//sets console printing to GREEN
void colGreen(){
    printf("\x1b[32m");
}
//sets console printing to YELLOW
void colYellow(){
    printf("\x1b[33m");
}
//sets console printing to BLUE
void colBlue(){
    printf("\x1b[34m");
}
//sets console printing to MAGENTA
void colMagenta(){
    printf("\x1b[35m");
}
//sets console printing to CYAN
void colCyan(){
    printf("\x1b[36m");
}

//prints a value, setting colour and placing space instead of 0
void styledPrint(int value){
    switch (value)
    {
    case 0:
        printf("|%3d ",  " ");
        return;
    case 1:
        colReset();
        break;
    case 2:
        colCyan();
        break;
    case 4:
        colBlue();
        break;
    case 8:
        colGreen();
        break;
    case 16:
        colYellow();
        break;
    case 32:
        colRed();
        break;
    case 64:
        colMagenta();
        break;
    
    default:
        break;
    
    printf("|%3d ", value);
    colReset();
    return;
    }
}

//returns highest value block in 4x4 int array (actually GRIDSIZExGRIDSIZE)
int boardMax(int board[][GRIDSIZE]){
    int maxVal = 0;
    int maxX = 0, maxY = 0;
    for (int y = 0; y < GRIDSIZE; y++)
    {
        for (int x = 0; x < GRIDSIZE; x++)
        {
            if(board[y][x] > maxVal){
                maxVal = board[y][x];
                maxX = x; maxY = y;
            }
        }
        
    }
    return maxVal;
}

//returns total value sum (ie counts up all values in 4x4 int array)
int boardSum(int board[][GRIDSIZE]){
    int sumVal = 0;
    for (int y = 0; y < GRIDSIZE; y++)
    {
        for (int x = 0; x < GRIDSIZE; x++)
        {
            sumVal += board[y][x];
        }
        
    }
    return sumVal;
}

// returns string name of direction enum
string dirToStringPrint(direction dir){
    string names[4] = {"up", "left", "down", "right"};
    return names[(int)dir];
}

// copies the contents of unsigned short integer array1 to array2 in-place, of given size.
void copy1dArray(unsigned short int* array1, unsigned short int* array2, unsigned short int size){
    for (int i = 0; i < size; i++)
    {
        array2[i] = array1[i];
    }
    return;
}
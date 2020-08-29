#include <stdio.h>
#include <random>
#include <cstring>
#include <chrono>
#include "sga.h"
// #include "puz.h"
// #include "puz.h"

#define GENSIZE 30

using namespace std;



//   original values: 
// MATCHMULT = 0.15; //multiplier for higher-value matches' potential. (goes into POT)
// POTMULT = 2; //multiplier for potential. Think "maxval is worth this many adjacencies"
// COUNTMULT = 16/2; // multiplier for (inverted) blockCount.
    //  score = ( COUNTMULT*(1/blockCount) + POTMULT*potential + maxVal )

// for each: 4 bit value. MATCH is 1/m, POTENTIAL is p, and COUNT is c.
//           input encoding: sequence of nmp (12 bit value.)  ?????????????


// returns "match" value 'nibble' of encoded integer
int getMatch(unsigned short int weights){
    return (weights & 0b0000111100000000) >> 8;
}
// returns "potential" value 'nibble' of encoded integer
int getPot(unsigned short int weights){
    return (weights & 0b0000000011110000) >> 4;
}
// returns "count" value 'nibble' of encoded integer
int getCount(unsigned short int weights){
    return  weights & 0b0000000000001111 ;
}

// encodes 1/match, potential and count weight values into one integer 0bMMMMPPPPCCCC
unsigned short int encode(int match, int potential, int count){
    unsigned short int weights = (match << 8) + (potential << 4) + (count);
    return weights;
}

//generates the initial generation; an array of GENSIZE (50) random weights (single int) in place 
void initialGeneration(unsigned short int generation[GENSIZE]){

    srand((unsigned) time(0));//comment out to have the same values every time
    
    int val = 1 + (rand()%4096 - 1);//4096 is from the max numbers encoded together (12 bits)

    for (int i = 0; i < GENSIZE; i++)
    {
        generation[i] = 1 + (rand()%4096 - 1);
    }
    
    return;
    
}

// gives the random number of offpsring to create based on individuals' bias, in-place
void spinBiasedRoulette(float *fitnesses, int *numOffspring){
    float cumFitness[GENSIZE]; //cumulative fitness, like a running total to only "land on" one.
    
    for (int i = 0; i < GENSIZE; i++)
    {   numOffspring[i] = 0;
        if(i == 0)
            {cumFitness[0] = fitnesses[0];
            continue;}
        else{
            cumFitness[i] = fitnesses[i] + cumFitness[i-1];
        }
    }

    float land;
    int j = 0;
    for (int spin = 0; spin < GENSIZE; spin++)
    {   j = 0;
        // land = raandom(gen);
        land = rand()%(int)round(cumFitness[GENSIZE-1]); //not exact to the floats but eeehhhhhhh
        while (land > cumFitness[j]){ //TODO: should BST here instead
            j++;
        }
        numOffspring[j]++;
    }
    
    for (int q = 0; q < GENSIZE; q++)
    {
        printf("Num offspring for %d: %d \n",q, numOffspring[q]);
    }
    
}

// selects genes from generation1 to cross over, based on numOffspring, placing their offspring into generation2
void matingPool(unsigned short int *generation1, int *numOffspring, unsigned short int *generation2){
    unsigned short int matingPool[GENSIZE];

    //placing genes in the mating pool based on # of offspring granted
    int childCount = 0;
    int c=0, j = 0;
    for (int i = 0; i < GENSIZE; i++) {   
        childCount = numOffspring[i];
        while (c++ < childCount){
            matingPool[j++] = generation1[i];
        }c=0;
    }
    scramble(matingPool);

    srand((unsigned) time(0));//comment out to have the same crossover sites every launch
    int crossoverSite;
    for (int w = 1; w < GENSIZE; w = w+2)
    {
        // crossoverSite = rand()%(12 - 1)+1;//12 bit, 11 places to cross
        crossoverSite = 4 + rand()%(3)*4;//12 bit, 11 places to cross, "4+" so it hits actual bit values, 
                                         //"3*4" so it's along one of the three vectors

        generation2[w-1] = crossover(matingPool[w - 1], matingPool[w], crossoverSite, true);
        generation2[w-0] = crossover(matingPool[w - 1], matingPool[w], crossoverSite, false);

    }
    
}

//returns the value of two individuals (binary numbers) crossed over at the given crossover site, "left" as true to get first value, false for second.
int crossover(unsigned short int a, unsigned short int b, int crossoverSite, bool left){
    unsigned short int c, d; // result 
    unsigned short int cr; //crossover mask

    // crossoverSite = 4;
    // a = 0b1111111111111111;
    // //       ||
    // b = 0b0000000000000000;
    // c = 0b1111000000000000;
    // ^^^^just to visualize crossing over

    // cr = pow(2,12-4) -1;    // because 12 - 4 is the 4th from the left instead of from the right (little endian),
                              //  and 2^4 is 10000, -1 is 01111, we got our mask!
    // to visualize implementation of crossing over, with mask 
    // cr =0b0000000011111111; for 8
    //~cr =0b1111111100000000;
    cr = pow(2,16-crossoverSite) -1;
    c = (a & ~cr) + (b & cr); //first mask is inverted so it starts as 1s from the left for a
    d = (a & cr) + (b & ~cr);

    // unsigned short: 2 bytes: 00000000 00000000
    if (left){ //I know this is bad but I don't wanna pass a pointer or return in-place...
        return c;
    }else{
        return d;
    }
}

// flips randomly selected bit 'mutCount' # of times, inside generation (list of shorts)
void mutate(unsigned short int* generation, int mutCount){
    int x, y;
    
    for (int i = 0; i < mutCount; i++)
    {
        x = rand()%12 + 4;
        y = rand()%GENSIZE;
        generation[y] ^= ((int)pow(2,x))&0b0000111111111111;
    }
    
}

//rearranges a list of shorts in-place
void scramble(unsigned short int* generation){
    unsigned short int temp[GENSIZE];
    for (int i = 0; i < GENSIZE; i++){
        temp[i] = 0;
    }
    int newIndex;
    for (int j = 0; j < GENSIZE; j++)
    {
        newIndex = rand()%GENSIZE;
        while (temp[newIndex%GENSIZE] != 0){
            newIndex++;
        }
        temp[newIndex%GENSIZE] = generation[j];
    }
    
    for (int i = 0; i < GENSIZE; i++)
    {
        generation[i] = temp[i];
    }
    return;
}
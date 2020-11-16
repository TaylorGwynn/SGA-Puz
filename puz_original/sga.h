using namespace std;

struct inputVectors{
    unsigned int match      : 4  ;
    unsigned int potential  : 4  ;
    unsigned int count      : 4  ;
};


struct inputVectors decode(int big);

int getMatch(unsigned short int big);
int getPot(unsigned short int big);
int getCount(unsigned short int big);
unsigned short int encode(int match, int potential, int count);
void initialGeneration(unsigned short int generation[50]);
void matingPool(unsigned short int *generation1,unsigned short int *numOffspring, unsigned short int *generation2);
int crossover(unsigned short int a, unsigned short int b, int crossoverSite, bool left);
void mutate(unsigned short int* generation, int mutCount);
void scramble(unsigned short int* generation);
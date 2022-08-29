/*  Header files reference!  */
#include <iostream>
#include <ctime>
#include <random>
#include <liblightnvm.h>
#include "FrontEnd/MemoryAllocate/HashBlock.h"
#include "FrontEnd/HashTable/HashTable.h"
#include "Exhashing/ExtendibleHash.h"
#include "Linearhashing/L-Hashing.h"


/* Define some global variables. */
struct nvm_bp* bp = nullptr;
std::unordered_map<uint64_t,uint64_t> chunkusage;


int GlobalInitialize(int argc, char **argv)
{

    /* Initialize device information */
    bp = nvm_bp_init_from_args(argc,argv);
    if(!bp)
    {
        return -1;
    }

    /* Initialize chunk information. */
    for(int i=0;i<100;i++)
    {
        chunkusage[i] = 0;
    }

    return 0;
    
}

int main(int argc, char **argv)
{
    printf("%s\n",argv[1]);
    /* initialize some global variables and SSD components initialization. */ 
    int result = GlobalInitialize(argc,argv);
    if(result != 0)
    {
        printf("Initialization failure!\n");
        exit(101);
    }

    /* select different indexes. line 49- */

    EXHashing1();

    //ExtendHashTableInitialize();

    LHashPort();

    return 0;

}



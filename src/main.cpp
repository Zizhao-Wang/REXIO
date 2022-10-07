/**
 * @date:   8/4/2022
 * @author: zz.wang
 * @email:  zz.wang@siat.ac.cn
 * @details:
 *          This file is the unique port of this project.
 * IDENTIFICATION:
 *          src/main.cpp
 *
 * According to the Google CPP style guide, header file reference should follow the following order:
 * (To avoid hidden dependencies)
 * 1. directly related header 2. C library 3. C++ library,
 * 4. other libraries'.h  5. your project's .h.  
 **/

#include <ctime> //C system files
#include <iostream>//C++ system files 
#include <random> 
#include <liblightnvm.h> // other project's .h files
#include "TNCTree/TNCtree.h"//your project's .h files
#include "MultiHash/ExtendibleHash/ExHash.h"
#include "MultiHash/LazySplitHash/LSHash.h"
#include "MultiHash/LinearHash/LiHash.h"
#include "Auxizilary/Logo.h"
#include "LSM-Tree/LsmTree.h"

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
    for(size_t i=0;i<150;i++)
    {
        chunkusage[i] = 0;
    }
    printf(UCAS_SIAT);
    printf(Name);
    return 0;
    
}

int main(int argc, char **argv)
{
    /* initialize some global variables and SSD components initialization. */ 
    int result = GlobalInitialize(argc,argv);
    if(result != 0)
    {
        printf("Initialization failure!\n");
        exit(101);
    }

    /* select different indexes. line 49- */

    // EXHashing1();

       //TNCtreePort();

     //LHashPort();

     LSMTreeInit();

    // LSHashPort();

    return 0;

}



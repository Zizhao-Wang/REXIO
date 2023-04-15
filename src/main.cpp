#include <ctime> //C system files
#include <iostream>//C++ system files 
#include <random> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <cstring>
#include <cstdlib> 
#include "Ti-OCSSD/TiOCS.h"//  other project's .h files SPDK's .h files   your project's .h files
#include "Auxizilary/Logo.h"
#include "LOCS/locs.h"
#include "LOCS/global_variables.h"
#include "LOCS/io_scheduler.h"
#include "Backend/backend_variables.h"


/* Define some global variables. */

int GlobalInitialize(int argc, char **argv)
{
    printf(UCAS_SIAT);
    printf(Name);

    printf("\n ================ LOCS initialization information ================\n");
    if(environment_init() != 0)
    {
        printf("Variables initialization failure!\n");
        return -1;
    }

    if(create_queue()!=0)
    {
        printf("I/O queue initialization failure!\n");
        return -1;
    }
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

    locs_init();

    locs_close();
    return 0;


}
#include <ctime> //C system files
#include <iostream>//C++ system files 
#include <random> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <cstring>
#include <cstdlib> 
#include "utility/Logo.h" //other project's .h files SPDK's .h files   your project's .h files
#include "include/spdk_env_init.h"
#include "include/init.h"
#include "include/log.h"
#include "tests/nexio_test.h"


// ThreadPool pool(2);

int GlobalInitialize(int argc, char **argv)
{
    // Usage in your code:
    printf("%s", NEXIO_LOGO);
    printf("%s", UCAS_SIAT);
    printf("%s", PROJECT_INTRO);
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

    logger.log(nexio_logger::info, "Start to initialize the SSD components.");    

    TiOCSInit();

    // bench_testing();

	TiOCS_close();
    
    return 0;
}



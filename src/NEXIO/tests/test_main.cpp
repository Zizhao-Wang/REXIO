#include <ctime> //C system files
#include <iostream>//C++ system files 
#include "utility/Logo.h" //other project's .h files SPDK's .h files   your project's .h files
#include "include/init.h"
#include "include/log.h"
#include "nexio_test.h"
#include <gflags/gflags.h>

#include "../memlayer/MemTier.h"


int main(int argc, char **argv)
{

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    /* initialize some global variables and SSD components initialization. */ 
    logo_print();
    exit(0);
    logger.log(nexio_logger::info, "Start to initialize the SSD components.");    

    TiOCSInit();

    bench_testing();

	TiOCS_close();
    
    return 0;
}



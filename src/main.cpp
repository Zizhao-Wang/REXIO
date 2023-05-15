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
#define DEBUG00 

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



// std::map<int, ThreadInfo> thread_map;


// void destroy_threads() {
//     for (auto& pair : thread_map) {
//         ThreadInfo& info = pair.second;

//         spdk_thread_exit(info.thread);

//         while (!spdk_thread_is_exited(info.thread)) {
//             spdk_thread_poll(info.thread, 0, 0);
//         }

//         spdk_thread_destroy(info.thread);

//         delete info.io_queue;
//     }

//     thread_map.clear();
// }

// void PutValue(int thread_id, IORequest request) {
//     ThreadInfo& info = thread_map[thread_id];
//     info.io_queue->add_request(request);
// }

// int main() {
//     // Initialize SPDK environment...
//     spdk_env_init();

//     create_threads();

//     // PutValue...

//     destroy_threads();

//     return 0;
// }


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

    // TiOCSInit();

	// TiOCS_close();
    


    return 0;
}
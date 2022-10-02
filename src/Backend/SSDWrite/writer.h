/**
 * @date   29/9/2022 
 * @author zz.wang
 * @details
 *         There are two main processes in the backend. More details about them are as follows:
 * 
 * IDENTIFICATION:
 *   	   src/Backend/SSDWrite/writer.h
 */

#ifndef EXPERIMENT1_BACKEND_WRITER_H
#define EXPERIMENT1_BACKEND_WRITER_H


#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <liblightnvm.h>
#include "../../TNCTree/MemoryTier/Node.h"
#include "../../Auxizilary/VariablesTypes.h"
#include "../../Auxizilary/SysOutput.h"
#include "../../Auxizilary/GlobalVariable.h"


/*  Global variables for SSD write definition.  */
extern uint32_t blockoffset;
extern int indexs;
extern TNCEntry * Pagedata;



int InfoRenew(size_t scale);


/**
 * ============= writer methods module ===============
 *  Function declartion for writing data into one or more pages:
 **/

int SinglePageWrite(); // Insert page-level key-value pairs into physicals for TNC-tree.



#endif //EXPERIMENT1_WRITEDISK_H
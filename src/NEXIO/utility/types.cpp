/**
 * @date 2022.5.10 13:43
 * @author zz.wang
 * @details
 *          This file contains some global variables are used in Front-End files.
 *
 *         IDENTIFICATION
 *   Experiment1/FrontEnd/GlobalVar/FrontGlobalVar.cpp
 */

#include "types.h"
#include "include/pre_definition.h"
#include <fstream>

/*  Global variables initialization. */


int Globaldepth=0;

uint64_t value_size = 128;

std::vector<GlobalHashNode*> global;

float pro=0.5;


int EncodeLength(uint64_t data) 
{
    int length = 0;
    do 
    {
        length++;
        data >>= 7;
    } while(data > 0);

    return length;
}


void write_data(const std::string& file_path, const std::string& experiment_num, const std::string& workload_num, int count_point) 
{
    std::ofstream outfile(file_path, std::ios_base::app);

    if (!outfile) 
    {
        std::cerr << "Can not open the file " << file_path << "\n";
        return;
    }
}

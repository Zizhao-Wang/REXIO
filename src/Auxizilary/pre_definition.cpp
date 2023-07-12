#include "pre_definition.h"
#include <fstream>

int reads_io = 0;

int writes_io = 0;

int resets = 0;

int io_resets =	0;

int writes_ram = 0;



uint64_t test(const char* buffer)
{
    uint64_t result = 0;
    for (size_t j = 0; j < 8; ++j) 
    {
        result |= (static_cast<uint64_t>(static_cast<unsigned char>(buffer[KEY_SIZE-1-j])) << (8 * j));
    }
    return result;
}

void write_data(const std::string& file_path, const std::string& experiment_num, const std::string& workload_num, int count_point) 
{
    std::ofstream outfile(file_path, std::ios_base::app);

    if (!outfile) 
    {
        std::cerr << "Can not open the file " << file_path << "\n";
        return;
    }

    outfile << experiment_num 
            <<"\t"<<workload_num
            << "\tCount_Point\t" << count_point 
            << "\tRead_count:\t" << reads_io 
            << "\tio_write:\t" << writes_io 
            << "\twrite2:\t" << writes_ram 
            << "\terase:\t" << resets
            << "\tBlock_erase:\t"<< io_resets << "\n";

}
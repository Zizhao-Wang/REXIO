#include "pre_definition.h"
#include <fstream>

int reads_io = 0;

int writes_io = 0;

int resets = 0;

int io_resets =	0;

int writes_ram = 0;


uint64_t big_endian2little_endian(const char *big_endian, size_t len)
{
    uint64_t result = 0;
    len = len > 8 ? 8 : len;
    for (size_t i = 0; i < len; ++i) 
	{
        result |= (static_cast<uint64_t>(static_cast<unsigned char>(big_endian[i])) << (8 * (len - 1 - i)));
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
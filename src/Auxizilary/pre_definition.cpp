#include "pre_definition.h"



uint64_t test(const char* buffer)
{
    uint64_t result = 0;
    for (size_t j = 0; j < 8; ++j) 
    {
        result |= (static_cast<uint64_t>(static_cast<unsigned char>(buffer[KEY_SIZE-1-j])) << (8 * j));
    }
    return result;
}
#include "assistant.h"


int EncodeLength(uint32_t data) 
{
    int length = 0;

    do 
    {
        length++;
        data >>= 7;
    } while(data > 0);

    return length;
}
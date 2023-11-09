/**
 * @date    30/4/2022
 * @author  zz.wang
 * @details
 *          If this is the program's first run, the program will create a new special hash table in memory.
 *          Otherwise, the process will read the hash table from the solid-state disk (SSD).
 *
 * IDENTIFICATION:
 *   		src/FrontEnd/GlobalVar/FrontGlobalVar.h
 */

#ifndef EXPERIMENT1_FRONTGLOBALVAR_H
#define EXPERIMENT1_FRONTGLOBALVAR_H


#include "include/node.h"
#include <vector>

#define LOCAL_TABLE_SIZE 100000

extern int Globaldepth;

/**
 *  Hash table
 **/
extern std::vector<GlobalHashNode *> global;

extern float pro;


/**
 * \brief Encodes the given data length.
 * 
 * This function takes an integer representing the length of data and encodes it
 * in a manner suitable for specific use cases. The exact encoding mechanism is 
 * implementation-specific.
 * 
 * \param data The data length to be encoded.
 * \return An integer representing the encoded length.
 */
int EncodeLength(uint64_t data);

/**
 * \brief Converts a big-endian byte order number to little-endian.
 * 
 * This function takes a sequence of bytes representing a big-endian number and 
 * converts it to its little-endian representation. This is useful in scenarios 
 * where byte order needs to be swapped due to platform differences.
 * 
 * \param big_endian A pointer to the start of the byte sequence in big-endian order.
 * \param len The length of the byte sequence.
 * \return A 64-bit integer representing the little-endian representation of the number.
 */
uint64_t big_endian2little_endian(const char *big_endian, size_t len);



#endif //EXPERIMENT1_FRONTGLOBALVAR_H

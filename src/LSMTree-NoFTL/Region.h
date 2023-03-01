/**
 * @date:   1/3/2022
 * @author: zz.wang
 * @email:  zz.wang@siat.ac.cn
 * @details:
 *          This file is used to manage the "region" within NoFTL-KV.
 *
 * IDENTIFICATION:
 *          TiOCS/src/LSMTree-NoFTL/Region.h
 **/

#ifndef TiOCS_LSMTREENoFTL_H
#define TiOCS_LSMTREENoFTL_H

// acquire the hardware information of SSD, including the number of chips(dies), blocks, pages, and 
// calculate the number of regions. 
int GetSSDinformation();


// a region is abstraction of parallel unit
int CreateRegion();

int DeleteRegion();



#endif
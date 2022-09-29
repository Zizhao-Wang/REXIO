/**
 * @date    28/4/2022
 * @author  zz.wang
 * @details
 *          The front has a primary process used to reply to requests from users, including
 *          read, write and delete. In the meantime, frontend calls the backend process to read/write data
 *          from disk!
 * IDENTIFICATION:
 *             src/TNCTree/TNCtree.cpp
 **/

#include "TNCtree.h"

int TNCtreeInit()
{

     int Createflag;
     Createflag=ExtendHashTableCreate();
     if(CreateResult==1)
     {

     }
     return 0;

}

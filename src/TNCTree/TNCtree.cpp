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
#include "../Auxizilary/SysOutput.h"

int TNCtreeInit()
{

     int Createflag;
     Createflag=ExtendHashTableCreate();
     if(Createflag != 0)
     {
          return -1;
     }
     return 0;

}


void TNCtreeStart()
{
     int flag = 0;
     flag = TNCtreeInit();

     if(flag!=0)
     {
          EMessageOutput("Memeory tier(first tier) create failure",370);
     }


     

}

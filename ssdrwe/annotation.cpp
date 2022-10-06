for (size_t m = 0; m < 5; m++)
        {
            for(size_t k=0,j=buf_ofz+m*16;j<buf_ofz+m*16+16;j++,k++)
            {
                temp[k] = bp->bufs->read[j];
            }
            uint64_t *ML = (uint64_t*) temp;
            printf("%lu %lu \n",ML[0],ML[1]);
        }
        for (size_t m = 0; m < 5; m++)
        {
            for(size_t k=0,j=buf_ofz+m*16;j<buf_ofz+m*16+16;j++,k++)
            {
                temp[k] = bp->bufs->write[j];
            }
            uint64_t *ML = (uint64_t*) temp;
            printf("%lu %lu \n",ML[0],ML[1]);
        }
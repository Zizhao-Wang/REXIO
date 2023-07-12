#include <pthread.h>
#include <liblightnvm.h>
#include "parallel_writer.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../Auxizilary/pre_definition.h"
#include "../../global_variables/global_variables.h"
#include "../../Backend/backend_variables.h"
#include "../../Auxizilary/SysOutput.h"



/*   */
void* parallel_write_into_pu(void *args)
{

    struct thread_param *arg = (struct thread_param *)args;
    char *buffer = arg->buffer;
    uint64_t page_num = arg->page_num;
    int err = 0;
    size_t s = arg->size;
    char* buffer3 = new char[4*4096+10];
    memset(buffer3, 0, 4*4096+10);
    // printf("size start: %lu, Page %lu reading success\n", arg->size, page_num);
    entry_t *buffer2 = (entry_t*)buffer;
    for (size_t i = 0; i < 1024; i++)
    {
        if(buffer2[i].key != s+i)
        {
            printf("Page %lu reading failed\n", page_num);
            exit(0);
        }
        // if(s+i == 13409)
        // {
        //     printf("Error in writing data in page %lu, key %lu, val %lu \n",page_num,buffer2[i].key,buffer2[i].val);
        //     exit(EXIT_FAILURE);
        // }
    }
    
    // entry_t *buffer2 = (entry_t*)buffer; 
    // printf("Page %lu writing success \n",page_num);
    // printf("key:%lu, val:%lu \n",buffer2[0].key,buffer2[0].val);

    if(page_num != UINT64_MAX )
    { 
        struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, page_num);
        struct nvm_addr addrs[ws_min];
        for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
            addrs[aidx].val = addrs_chunk.val;
            addrs[aidx].l.sectr = (page_num%4096)+aidx;
        }

        for (size_t i = 0; i < 1024; i++)
        {
            if(buffer2[i].key != s+i)
            {
                printf("Page %lu reading failed\n", page_num);
                exit(0);
            }
        }
        err = nvm_cmd_write(bp->dev, addrs,ws_min,buffer, NULL,0x0, NULL);
        if(err == 0) 
        {
            //printf("Page %lu writing success \n",page_num);
            writes_io++;
        }
        else
        {
            EMessageOutput("Page writing failed in "+ Uint64toString(page_num)+"\n", 4598);
        }
        
        for (size_t i = 0; i < 1024; i++)
        {
            if(buffer2[i].key != s+i)
            {
                printf("Page %lu reading failed\n", page_num);
                exit(0);
            }
        }
        // //diff = nvm_buf_diff(bp->bufs->write, bp->bufs->read,bp->bufs->nbytes);
	    // if (diff) 
        // {
		//     nvm_buf_diff_pr(bp->bufs->write, bp->bufs->read,bp->bufs->nbytes);
		//     errno = EIO;
		//     return -1;
	    // }
        err = nvm_cmd_read(bp->dev, addrs, ws_min,buffer3, NULL,0x0, NULL);
        if(err == 0) 
        {
            //printf("Page %lu reading success \n",page_num);
            reads_io++;
        }
        else
        {
            EMessageOutput("Page writing failed in "+ Uint64toString(page_num)+"\n", 4598);
        }

        entry_t *buffer4 = (entry_t*)buffer3;
        for (size_t i = 0; i < 1024; i++)
        {
            if(buffer4[i].key != s+i)
            {
                printf("Page %lu reading failed, buffer2[i].key:%lu, buffer2[i].val:%lu, s+i:%lu\n", page_num, buffer4[i].key,buffer4[i].val,s+i);
                break;
            }
        }
        delete[] buffer3;
    }

    return NULL;
    
}


/*   */
void* parallel_read_from_pu(void *args)
{

    struct thread_param *arg = (struct thread_param *)args;
    char *buffer = arg->buffer;
    uint64_t page_num = arg->page_num;
    int err = 0;
    size_t s = arg->size;
    printf("size start: %lu, Page %lu reading success\n", arg->size, page_num);

    if(page_num != UINT64_MAX )
    { 
        struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, page_num);
        struct nvm_addr addrs[ws_min];
        for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
            addrs[aidx].val = addrs_chunk.val;
            addrs[aidx].l.sectr = (page_num%4096)+aidx;
        }

        err = nvm_cmd_read(bp->dev, addrs, ws_min,buffer, NULL,0x0, NULL);
        if(err == 0) 
        {
            //printf("Page %lu reading success \n",page_num);
            reads_io++;
        }
        else
        {
            EMessageOutput("Page writing failed in "+ Uint64toString(page_num)+"\n", 4598);
        }
        
    }

    entry_t *buffer2 = (entry_t*)buffer;
    for (size_t i = 0; i < 1024; i++)
    {
        if(buffer2[i].key != s+i)
        {
            printf("Page %lu reading failed, buffer2[i].key:%lu, buffer2[i].val:%lu, s+i:%lu\n", page_num, buffer2[i].key,buffer2[i].val,s+i);
            exit(0);
        }
    }

    return (void *)args;

}


/**
 * ============= NoFTL-KV module ===============
 *  Function declartion for writing data into one or more pages:
 **/
void* parallel_coordinator(std::vector<entry_t> run_data, uint64_t num_pug, int mode, coordinator_param* read_param)
{

    /* create thread pool for asynchornous write */
    printf("number of CHANNEL: %lu, Mode %d \n", num_pug,mode);
    const nvm_geo *geo = nvm_dev_get_geo(bp->dev);
    size_t page_size = ws_min * geo->sector_nbytes;
    size_t page_capacity = page_size / sizeof(entry_t);
    size_t lun_width=geo->l.nchunk*geo->l.nsectr;
    size_t channel_width = lun_width*geo->l.npunit;


    if(mode == PAOCS_WRITE_MODE && read_param == nullptr)
    {
        size_t cwrite_point_lun;
        pthread_t thread_id[geo->l.nchunk];
        char **buffer = new char*[geo->l.nchunk];
        size_t num_copy, vector_offset, batch_pages;
        int res = 0;
        struct thread_param args [geo->l.nchunk];
        int ret = 0;
        struct coordinator_param *param = new coordinator_param;
        param->start_page = channel_current_pointer[num_pug];
        

        for (size_t i = 0; i < run_data.size(); i+= (geo->l.nchunk * page_capacity))
        {
            if( (run_data.size()-i) < geo->l.nchunk * page_capacity)
            {
                batch_pages = (run_data.size()-i) / page_capacity;
            }
            else
            {
                batch_pages = geo->l.nchunk;
            }
             
            for (size_t j = 0; j < batch_pages; j++)
            {
                // buffer[j] = new char[page_size];
                char *temp = new char[page_size];
                vector_offset = i+j*page_capacity;
                memcpy(temp,run_data.data()+vector_offset,page_size);
                buffer[j] = temp;
                entry_t *buffer2 = (entry_t*)buffer[j];

                //printf("key:%lu, val:%lu \n",buffer2[1023].key,buffer2[1023].val);
                for (size_t k = 0; k < page_capacity; k++)
                {
                    if(i+j*page_capacity+k+1 != buffer2[k].key || buffer2[k].key ==0 || buffer2[k].val==0)
                    {
                        printf("Error in writing data into page %lu, key %lu, val %lu \n",k,buffer2[k].key,buffer2[k].val);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            
            
            for (size_t j = 0; j < batch_pages; j+=max_os_threads)
            {

                cwrite_point_lun = channel_current_pointer[num_pug];
                // printf("===========\n current write point: %lu \n", cwrite_point_lun);
                for (int k = 0; k < max_os_threads; k++)
                {
                    args[j+k].buffer = buffer[j+k];
                    args[j+k].page_num = cwrite_point_lun + k*lun_width;
                    args[j+k].size = i+j*page_capacity+k*page_capacity+1;
                    res = pthread_create(&thread_id[j+k], NULL, parallel_write_into_pu, &(args[j+k]));
                    if (res != 0)
                    {
                        EMessageOutput("Thread creation failed in"+ std::to_string(i)+"creation!", 4598);
                        exit(EXIT_FAILURE);
                    }
                }
                
                /* reclaim threads  */
                for (int k = 0; k < max_os_threads; k++)
                {
                    res = pthread_join(thread_id[j+k], NULL);
                    if (res != 0)
                    {
                        EMessageOutput("Thread join failed in"+ std::to_string(i)+"creation!", 4598);
                        exit(EXIT_FAILURE);
                    }
                    delete[] buffer[j+k];
                }

                // printf("Threads batch created and reclaim successfully!\n");
                // printf("current write point: %lu \n =========== \n", cwrite_point_lun);
                if (cwrite_point_lun + max_os_threads*lun_width >= ( (num_pug+1)*channel_width))
                {
                    channel_current_pointer[num_pug] = (cwrite_point_lun + max_os_threads*lun_width) % ((num_pug+1)*channel_width);
                    channel_current_pointer[num_pug] += ws_min;    
                }
                else
                {
                    channel_current_pointer[num_pug] = cwrite_point_lun + max_os_threads*lun_width;
                }  
            }
            ret += batch_pages;
        }
        // printf("current write point: %lu \n =========== \n", channel_current_pointer[num_pug]);
        // printf("%d pages have been written into CHANNEL %lu \n", ret,num_pug);
        param->end_page = channel_current_pointer[num_pug];
        return   (void*)param;
    }
    else if(mode == PAOCS_READ_MODE && run_data.size() == 0 && read_param != nullptr)
    {
        size_t num_buffer = 0;
        size_t num_copy, vector_offset, batchs, remainder_bacths, cread_point_lun, channel_start_point;
        int res = 0, ret = 0, j=0;
        pthread_t thread_id[max_os_threads];
        struct thread_param args [max_os_threads];
        char *buffer = new char[read_param->size*sizeof(entry_t) +5];
        size_t read_pages[max_os_threads];

        batchs = (read_param->size / page_capacity)/max_os_threads;
        remainder_bacths = (read_param->size / page_capacity)%max_os_threads;
        channel_start_point = cread_point_lun = read_param->start_page;


        for (size_t i = 0; i < batchs; i++)
        {
            printf("===========\ncurrent read point: %lu \n", cread_point_lun);
            for (size_t j = 0; j < max_os_threads; j++)
            {
                args[j].page_num = cread_point_lun;
                args[j].size = i*max_os_threads*page_capacity +j*page_capacity+1;
                res = pthread_create(&thread_id[j], NULL, parallel_read_from_pu, &(args[j]));
                if (res != 0)
                {
                    EMessageOutput("Thread creation failed in"+ std::to_string(i)+"creation!", 4598);
                    exit(EXIT_FAILURE);
                }

                if(cread_point_lun + lun_width >= (num_pug+1)*channel_width)
                {
                    cread_point_lun = (cread_point_lun+lun_width)%((num_pug+1)*channel_width);
                    cread_point_lun += ws_min; 
                }
                else
                {
                    cread_point_lun += lun_width;
                }

            }

            /* reclaim threads  */
            for (int j = 0; j < max_os_threads; j++)
            {
                res = pthread_join(thread_id[j], NULL);
                if (res != 0)
                {
                    EMessageOutput("Thread join failed in"+ std::to_string(i)+"creation!", 4598);
                }
            }
           
            for (int j = 0; j < max_os_threads; j++)
            {
                memcpy(buffer+num_buffer, args[j].buffer, page_size);
                num_buffer += page_size;
                delete[] args[j].buffer;
                // entry_t *temp = (entry_t*)args[j].buffer;
                // printf("entry.key: %lu, entry.value: %lu\n", temp[0].key, temp[0].val);
                // for (size_t k = 0; k < page_capacity; k++)
                // {
                //     if(i*(max_os_threads*page_capacity)+j*page_capacity+k+1 != temp[k].key || temp[k].key ==0 || temp[k].val == 0)
                //     {
                //         printf("Error in writing data into page %lu, key %lu, val %lu k:%lu \n",i*(max_os_threads*page_capacity)+j*page_capacity+k+1,temp[k].key,temp[k].val,k);
                //         exit(EXIT_FAILURE);
                //     }   
                // }
            }
        }
        
        for (size_t j = 0; j < remainder_bacths; j++)
        {
            
            args[j].buffer = new char[page_size];
            args[j].page_num = cread_point_lun;

            res = pthread_create(&thread_id[j], NULL, parallel_read_from_pu, &(args[j]));
            if (res != 0)
            {
                EMessageOutput("Thread creation failed in"+ std::to_string(j)+"creation!", 4598);
                exit(EXIT_FAILURE);
            }

            if(cread_point_lun + lun_width >= (num_pug+1)*channel_width)
            {
                cread_point_lun = (cread_point_lun+lun_width)%((num_pug+1)*channel_width);
                cread_point_lun += ws_min; 
            }
            else
            {
                cread_point_lun += lun_width;
            }
        } 

        /* reclaim threads  */
        for (int j = 0; j < remainder_bacths; j++)
        {
            res = pthread_join(thread_id[j], NULL);
            if (res != 0)
            {
                EMessageOutput("Thread join failed in"+ std::to_string(j)+"creation!", 4598);
            }
        }
        printf("%d pages have been read from CHANNEL %lu \n", reads_io, num_pug);
        return  (void*)buffer;   
    }
    else
    {

        EMessageOutput("Invalid mode for parallel coordinator", 4598);
    }


    /* acquire the LUN information */
    
    
    return NULL;

}
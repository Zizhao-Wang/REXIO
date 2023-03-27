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

    printf("Page %lu writing success \n",page_num);

    if(page_num != UINT64_MAX )
    { 
        struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, page_num);
        struct nvm_addr addrs[ws_min];
        for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
            addrs[aidx].val = addrs_chunk.val;
            addrs[aidx].l.sectr = (page_num%4096)+aidx;
        }

        err = nvm_cmd_write(bp->dev, addrs, ws_min,buffer, NULL,0x0, NULL);
        if(err == 0) 
        {
            printf("Page %lu writing success \n",page_num);
            chunk_write_pointer[page_num/4096] = page_num;
            writes++;
        }
        else
        {
            EMessageOutput("Page writing failed in "+ Uint64toString(page_num)+"\n", 4598);
        }
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

    printf("Page %lu reading success \n",page_num);

    // if(page_num != UINT64_MAX )
    // { 
    //     struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, page_num);
    //     struct nvm_addr addrs[ws_min];
    //     for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    //     {
    //         addrs[aidx].val = addrs_chunk.val;
    //         addrs[aidx].l.sectr = (page_num%4096)+aidx;
    //     }

    //     err = nvm_cmd_read(bp->dev, addrs, ws_min,buffer, NULL,0x0, NULL);
    //     if(err == 0) 
    //     {
    //         //printf("Page %lu reading success \n",page_num);
    //         chunk_write_pointer[page_num/4096] = page_num;
    //         reads++;
    //     }
    //     else
    //     {
    //         EMessageOutput("Page writing failed in "+ Uint64toString(page_num)+"\n", 4598);
    //     }
    // }
    return (void *)args;

}


/**
 * ============= NoFTL-KV module ===============
 *  Function declartion for writing data into one or more pages:
 **/
void* parallel_coordinator(std::vector<entry_t> run_data, uint64_t num_pug, int mode, void* read_param)
{

    /* create thread pool for asynchornous write */
    printf("number of LUNs: %lu \n", num_pug);
    const nvm_geo *geo = nvm_dev_get_geo(bp->dev);
    pthread_t thread_id[geo->l.nchunk];
    size_t page_size = ws_min * geo->sector_nbytes;
    size_t page_capacity = page_size / sizeof(entry_t);


    if(mode == PAOCS_WRITE_MODE && read_param == nullptr)
    {
        size_t cwrite_point_lun;
        char **buffer = new char*[geo->l.nchunk];
        size_t num_copy, vector_offset, batch_pages;
        int res = 0;
        struct thread_param args [geo->l.nchunk];
        int ret = 0;
        struct coordinator_param *param = new coordinator_param;
        param->start_page = lun_current_pointer[num_pug];

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
                buffer[j] = new char[page_size];
                char *temp = new char[page_size];
                num_copy = (run_data.size()-j*page_capacity) % page_capacity;
                vector_offset = num_copy * j;
                memcpy(temp,run_data.data()+vector_offset,num_copy*sizeof(entry_t));
                buffer[j] = temp;
            }

            printf("===========\n current write point: %lu \n", cwrite_point_lun);
            
            for (size_t j = 0; j < batch_pages; j+=max_os_threads)
            {

                cwrite_point_lun = lun_current_pointer[num_pug];
                
                for (int k = 0; k < max_os_threads; k++)
                {
                    args[j+k].buffer = buffer[j+k];
                    args[j+k].page_num = cwrite_point_lun + k*geo->l.nchunk*geo->l.nsectr;
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
                }

                printf("Threads batch created and reclaim successfully!\n");
                printf("current write point: %lu \n =========== \n", cwrite_point_lun);
                if (cwrite_point_lun + max_os_threads * geo->l.nchunk*geo->l.nsectr >= (geo->l.npunit * geo->l.nchunk*geo->l.nsectr))
                {
                    lun_current_pointer[num_pug] = (cwrite_point_lun + max_os_threads*geo->l.nchunk*geo->l.nsectr) % (geo->l.npunit* geo->l.nchunk*geo->l.nsectr);
                    lun_current_pointer[num_pug] += ws_min;    
                }
                else
                {
                    lun_current_pointer[num_pug] = cwrite_point_lun + max_os_threads*geo->l.nchunk*geo->l.nsectr;
                }   
                
            }
            
            ret += batch_pages;
            printf("current write point: %lu \n =========== \n", lun_current_pointer[num_pug]);
        }
        printf("%d pages have been written into LUN %lu \n", ret,num_pug);
        param->end_page = lun_current_pointer[num_pug];
        return   (void*)param;
    }
    else if(mode == PAOCS_READ_MODE && run_data.size() == 0)
    {
        entry_t *buffer = new entry_t[262145];
        size_t num_buffer = 0;
        size_t num_copy, vector_offset, batch_pages;
        int res = 0;
        struct thread_param args [geo->l.nchunk];
        int ret = 0;
        struct coordinator_param *param = (struct coordinator_param*)read_param;

        for (size_t i = param->start_page; i >= param->end_page ; )
        {

            // printf("===========\n current write point: %lu \n", cwrite_point_lun);
            for (int k = 0; k < max_os_threads; k++)
            {
                if(i+geo->l.nsectr >= (num_pug+1)*geo->l.nchunk*geo->l.nsectr)
                {
                    i = (i+geo->l.nsectr)%((num_pug+1)* geo->l.nchunk*geo->l.nsectr);
                }
                else
                {
                    i += geo->l.nsectr;
                }
                args[k].buffer = new char[page_size];
                args[k].page_num = i;

                res = pthread_create(&thread_id[k], NULL, parallel_read_from_pu, &(args[k]));

                if (res != 0)
                {
                    EMessageOutput("Thread creation failed in"+ std::to_string(i)+"creation!", 4598);
                    exit(EXIT_FAILURE);
                }
            }

            /* reclaim threads  */
            for (int k = 0; k < max_os_threads; k++)
            {
                res = pthread_join(thread_id[k], NULL);
                if (res != 0)
                {
                    EMessageOutput("Thread join failed in"+ std::to_string(i)+"creation!", 4598);
                }
            }
            
            for (int k = 0; k < max_os_threads; k++)
            {
                memcpy(buffer+num_buffer, args[k].buffer, page_size);
                num_buffer += page_capacity;
            }
        }

        return  (void*)buffer;
        
    }
    else
    {
        EMessageOutput("Invalid mode for parallel coordinator", 4598);
    }


    /* acquire the LUN information */
    
    
    return NULL;

}
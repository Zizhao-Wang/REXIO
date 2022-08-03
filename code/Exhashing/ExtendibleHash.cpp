/* header files references. */
#include "ExtendibleHash.h"
#include <ctime>


void EXHashing1()
{
    bool show_messages, show_duplicate_buckets;
    int bucket_size=2048, initial_global_depth=1;
    int key, mode;
    string choice, value;
    clock_t startTime,endTime;

    // Set show_messages to 0 when taking input using file redirection
    show_messages = 0;

    // Set show_duplicate_buckets to 1 to see all pointers instead of unique ones
    show_duplicate_buckets = 0;

    Directory d(initial_global_depth,bucket_size);
    printf("======Initialized directory structure completed!=======\n");

    /* Insert 10 values 
     */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
        uint64_t value = i;
        d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time : " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;

 

    /*do
    {
        cout<<endl;
        if(show_messages) { cout<<">>> "; }
        cin>>choice;
        if(choice=="insert")
        {
            cin>>key>>value;
            if(show_messages) { cout<<endl; }
            d.insert(key,value,0);
        }
        else if(choice=="delete")
        {
            cin>>key>>mode;
            if(show_messages) { cout<<endl; }
            d.remove(key,mode);
        }
        else if(choice=="update")
        {
            cin>>key>>value;
            if(show_messages) { cout<<endl; }
            d.update(key,value);
        }
        else if(choice=="search")
        {
            cin>>key;
            if(show_messages) { cout<<endl; }
            d.search(key);
        }
        else if(choice=="display")
        {
            if(show_messages) { cout<<endl; }
            d.display(show_duplicate_buckets);
        }
    } while(choice!="exit");*/
}
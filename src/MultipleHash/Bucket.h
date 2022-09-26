/**
 * @date:   26/9/2022
 * @author: zz.wang
 * @email:  zz.wang@siat.ac.cn
 * @details:
 *          This file shows how to construct a generalized bucket for xxx-hash.
 *
 * IDENTIFICATION:
 *          src/MultipleHash/Bucket.h
 **/

class Bucket 
{
private:

public:
    int depth,size; 
    uint64_t pageno, CurSize; //page number and current size 
    map<int, uint64_t> values;
    
    Bucket(int depth, int size);

   /**
	* As a rule, a complete index structure should at least include the methods for CURD and crash recovery.
	* Create, update, retrieval, delete and crash recovery.
	**/
    int insert(int key,uint64_t value);
    int remove(int key);
    int update(int key, uint64_t value);
    void search(int key);

    int isFull(void);
    int isEmpty(void);
	
    int getDepth(void);
    int increaseDepth(void);
    int decreaseDepth(void);
    std::map<int, uint64_t> copy(void);
    void clear(void);
    void display(void);
    int  PageWrite();

};
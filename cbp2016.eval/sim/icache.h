#ifndef _ICACHE_H_
#define _ICACHE_H_

#include <stdlib.h>
#include <string.h>
#include <string>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <iomanip>
#include "utils.h"
#include <vector>
typedef unsigned short  uns_t ;
typedef unsigned long long unll_t;
typedef unsigned int uni_t;

#define NumPredTables 3
// #define UINT8_MAX  255

class Icache {
    public:
    
    struct banks{
        uni_t **tag_bank;
        bool **valid_bank;
        bool **prediction_bank;
        uns_t **signature_bank;
        uns_t **LRU_stack;
        uns_t **RRIP_stack;
        
    };
    struct Addr_info{
        unll_t TagBits;
        unll_t IndexBits;;


    };
    
    struct GHRP{
        int* cntrsNew;
        int predTables[4096][NumPredTables]; // 12 bits hash functions 3 tables
        int* indices;
    };
    struct GHRP* metadata;
    unll_t IndexBitNum;
    unll_t BlockShift;
    unll_t TagBitNum;
    unll_t EntryNum;
    unll_t Iassoc;
    unll_t Isize;
    unll_t Iblock_size;
    banks *bank_ptr;
    
    uns_t history ;
    unll_t Misses;
    unll_t Hits;
    unll_t TotalAccess;
    unll_t conflictMisses, bypassed, replaced;
    
    char policy [100] = {0};
    std::hash <int> hashing;
    public:
    
    
    Icache(int size, int assoc, int block_size, string policy_in){
        Isize = (unll_t) size/4;
        IndexBitNum = log2(Isize/(block_size*assoc));
        BlockShift = log2(block_size);
        TagBitNum = 64 - BlockShift -IndexBitNum -2;
        EntryNum = Isize/(block_size*assoc); 
        Iassoc = assoc;
        Misses = TotalAccess = Hits= history=  conflictMisses= bypassed = replaced=0;
        Iblock_size = block_size;
        bank_ptr = new banks;
        bank_ptr->tag_bank = (uni_t **) calloc(EntryNum, sizeof(uni_t *));
        bank_ptr->valid_bank = (bool **) calloc(EntryNum, sizeof(bool *));
        bank_ptr->prediction_bank = (bool **) calloc(EntryNum, sizeof(bool *));
        bank_ptr->signature_bank = (uns_t **) calloc(EntryNum, sizeof(uns_t *));
        bank_ptr->LRU_stack = (uns_t **) calloc(EntryNum, sizeof(uns_t *));
        bank_ptr->RRIP_stack = (uns_t **) calloc(EntryNum, sizeof(uns_t *));

        //additional structures for GHRP policy
        metadata = new GHRP;
        metadata->indices = metadata->cntrsNew = NULL;
        for (int i = 0; i < 4096; i++)
        {
            for (int j = 0; j < NumPredTables; j++)
            {
                metadata->predTables[i][j] = 0;
            }
            
            
        }
        
        //prediction table initialization add!!!

        policy_in.copy(policy, 100,0);
        for (int i =0; i< EntryNum; i++){
            bank_ptr->tag_bank[i]  = (uni_t*) calloc(assoc, sizeof(uni_t));
            bank_ptr->valid_bank[i]  = (bool*) calloc(assoc, sizeof(bool));
            bank_ptr->prediction_bank[i]  = (bool*) calloc(assoc, sizeof(bool));
            bank_ptr->signature_bank[i]  = (uns_t*) calloc(assoc, sizeof(uni_t));
            bank_ptr->LRU_stack[i]  = (uns_t*) calloc(assoc, sizeof(uni_t));
            bank_ptr->RRIP_stack[i]  = (uns_t*) calloc(assoc, sizeof(uni_t));
        }

        for (size_t i = 0; i < EntryNum; i++)
        {
            for (size_t j = 0; j < Iassoc; j++)
            {
                bank_ptr->RRIP_stack[i][j] = 3;
            }
            
        }
        
        // uni_t Tag_bank[EntryNum][assoc] =  {0};
        // bool valid_bank[EntryNum][assoc] = {0};
        // bool prediction_bank[EntryNum][assoc] = {0};
        // uns_t signature_bank[EntryNum][assoc] = {0};
        // uns_t LRU_stack[EntryNum][assoc] = {0};
       

        assert(TagBitNum > 1);
    }
    // function to parce the address
    struct Addr_info* Get_Addr_info(unll_t Addr ){
        Addr_info* t = new Addr_info;
        unll_t MASK_INDEX = 0;
        for (int i=0; i<IndexBitNum;i++){
            MASK_INDEX |= (1<<i);


        }
        
        t->IndexBits = (Addr>>(BlockShift+2))& MASK_INDEX;
        t->TagBits = (Addr>>(BlockShift + IndexBitNum + 2));
        
        return t;
    }
    public:
    
    void checkprint();
    
    void printSimpleStats(){
        std::cout<<"\nMisses: "<<Misses<<std::endl;
        std::cout<<"Hits: "<<Hits<<std::endl;
        std::cout<<"Total Accesses: "<<TotalAccess<<std::endl;
        if (Misses>0){
            printf("MissRate (misses per 1000 insts): %10.9f\n",   1000.0*(double)(Misses)/(double)(Hits+Misses));   
            // std::cout<<"MissRate: "<< t<<std::endl;
            printf("Replaced: %d\n",   replaced);   
            printf("Bypassed: %d\n",   bypassed);   
            printf("ConflictMisses: %d\n",   conflictMisses);   
            

        }

    }

    int check_hit(struct Addr_info *t){
        // std::string s;
        for (int i = 0; i<Iassoc; i++){
            if(bank_ptr->valid_bank[t->IndexBits][i] == true && bank_ptr->tag_bank[t->IndexBits][i] ==  t->TagBits )
                // std::cout<<std::hex<<"Hit: index ox"<<t->IndexBits<<"tag "<<t->TagBits<<" assoc"<< i<<std::endl;  
                return i;
        }
        // std::cout<<std::hex<<"Miss: "<<"index hex"<<t->IndexBits<<"tag "<<t->TagBits<<std::endl; 
        // std::cin>>s;
        return -1;
        
    }

    void updateLRUstack(int hit_assoc, struct Addr_info *t){
        for (int i = 0; i<Iassoc; i++){
            if(bank_ptr->valid_bank[t->IndexBits][i] == true ){
                bank_ptr->LRU_stack[t->IndexBits][i] = SatIncrement(bank_ptr->LRU_stack[t->IndexBits][i],7);
                if (i==hit_assoc){
                    bank_ptr->LRU_stack[t->IndexBits][i] = 0;

                }

            }
                
        }
       


    }

    ~Icache(){
        for (int i =0; i< EntryNum; i++){
            free(bank_ptr->tag_bank[i] );
            free(bank_ptr->valid_bank[i]);
            free(bank_ptr->prediction_bank[i]);
            free(bank_ptr->signature_bank[i]);
            free(bank_ptr->LRU_stack[i]);
            free(bank_ptr->RRIP_stack[i]);
        }
        delete(bank_ptr);
        // free(metadata->indices);
        // free(metadata->cntrsNew);
        delete(metadata);
    }
    
    bool Iaccess(unll_t Addr ,unll_t PC );
    void clear(Addr_info* t);

    //GHRP specific functions
    void UpdatePathHist(unll_t PC);
    int Signature(unll_t PC);
    int* ComputeIndices(int sign);
    bool MajorityVote(int counters[], int threshold);
    int *GetCounters(int *indices);
    int VictimBlock(int LRU_ind, Addr_info* t);
    void UpdatePredTables(int *indices, bool isDead);

    //RRIP specific funtions

    void UpdateRRIP(int block_ind, Addr_info* t, bool hit);
    int RRIP_victimBlock( Addr_info* t);
};




#endif
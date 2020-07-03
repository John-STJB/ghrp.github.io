// Author: Shyngys Aitkazinov
// ICache simulator functions

#include "icache.h"
#define MAXINT 0xffffffff
#define THRESHOLD_BYPASS 12
#define THRESHOLD_DEAD 8

void Icache::checkprint(){
    std::cout<<"Cache size in KBytes "<<Isize<<std::endl;
    std::cout<<"Cache size in KBytes "<<Isize/pow(2,10)<<std::endl;
    std::cout<<"number of data "<<Isize/4<<std::endl;
    std::cout<<"Cache assoc "<<Iassoc<<std::endl;
    std::cout<<"Cache blocksize "<<Iblock_size<<std::endl;   
    std::cout<<"BlockShift "<< BlockShift<<std::endl;
    std::cout<<"TagBitNum "<< TagBitNum<<std::endl;
    std::cout<<"BlockShift "<< BlockShift<<std::endl;
    std::cout<<"EntryNum "<< EntryNum<<std::endl;
    std::cout<<"IndexBitNum "<< IndexBitNum<<std::endl;
    std::cout<<(policy)<<std::endl;
    std::cout<<(policy=="LRU")<<std::endl;
    



}




bool Icache::Iaccess(unll_t Addr, unll_t PC){
    // return true if cahce-hit and false if cache-miss 
    // also, makes corresponding updates of the cache metadata
    struct Addr_info *t;
    int hit_assoc;
    t =  Get_Addr_info(Addr);
    // std::cout<<std::hex<<t->TagBits<<" "<<t->IndexBits<<std::endl;

    TotalAccess++;
    if ( strcmp(policy, "LRU") == 0){
        hit_assoc = check_hit(t);
        // cache hit case
        if (hit_assoc >= 0){
            updateLRUstack(hit_assoc, t);
            Hits++;
            return true;
        }
        else{
            // cache misss case
            Misses++;
            bool found = false;
            int temp = -1;
            int minLRU, maxLRU, max_ind;
            maxLRU = -1; minLRU = MAXINT; max_ind = -1;
            for(int i = 0; i < Iassoc; i++){
            
                if (bank_ptr->valid_bank[t->IndexBits][i] == false){
                    // case when there is still empty way to take data
                    found = true;
                    temp = i;
                    bank_ptr->valid_bank[t->IndexBits][i] = true;
                    bank_ptr->tag_bank[t->IndexBits][i] = t->TagBits;
                    updateLRUstack(i, t);
                    return false;
                }

                if (maxLRU < bank_ptr->LRU_stack[t->IndexBits][i]){
                    // find the oldest data
                    maxLRU = bank_ptr->LRU_stack[t->IndexBits][i];
                    max_ind = i;
                }
            }
            //update LRUstack 
            if (found == false){
                bank_ptr->valid_bank[t->IndexBits][max_ind] = true;
                bank_ptr->tag_bank[t->IndexBits][max_ind] = t->TagBits;
                updateLRUstack(max_ind, t);
                return false;
            }


        }
    }
    else if(strcmp(policy, "MRU") == 0){

        hit_assoc = check_hit(t);
        // cache hit case
        if (hit_assoc >= 0){
            updateLRUstack(hit_assoc, t);
            Hits++;
            return true;
        }
        else{
            // cache misss case
            Misses++;
            bool found = false;
            int temp = -1;
            int minLRU, maxLRU, min_ind;
            maxLRU = -1; minLRU = MAXINT; min_ind = -1;
            for(int i = 0; i < Iassoc; i++){
            
                if (bank_ptr->valid_bank[t->IndexBits][i] == false){
                    // case when there is still empty way to take data
                    found = true;
                    temp = i;
                    bank_ptr->valid_bank[t->IndexBits][i] = true;
                    bank_ptr->tag_bank[t->IndexBits][i] = t->TagBits;
                    updateLRUstack(i, t);
                    return false;
                }

                if (minLRU > bank_ptr->LRU_stack[t->IndexBits][i]){
                    // find the oldest data
                    
                    minLRU = bank_ptr->LRU_stack[t->IndexBits][i];
                    min_ind = i;
                }
            }
            //update LRUstack 
            if (found == false){
                // std::cout<<std::hex<<"Miss: "<<"tag "<<t->TagBits<<"index hex"<<t->IndexBits<<std::endl; 
                // for (int i = 0; i<Iassoc; i++){
                //     std::cout<<std::hex<<"assoc: " <<i<<" tag "<<bank_ptr->tag_bank[t->IndexBits][i]<<" index hex "<<t->IndexBits<<std::endl; 
                    
                // }
                // std::cin>>maxLRU;
                bank_ptr->valid_bank[t->IndexBits][min_ind] = true;
                bank_ptr->tag_bank[t->IndexBits][min_ind] = t->TagBits;
                updateLRUstack(min_ind, t);
                return false;
            }


        }

    }
    else if(strcmp(policy, "GHRP") == 0) {
        TotalAccess++;
        int  sign = Signature(PC);
        metadata->indices = ComputeIndices(sign);
        metadata->cntrsNew = GetCounters( metadata->indices );
        hit_assoc = check_hit(t);
        if(hit_assoc >= 0){
            Hits++;
            // cache hit case
            // UpdatePredTables(metadata->indices, false);
            metadata->indices = ComputeIndices(bank_ptr->signature_bank[t->IndexBits][hit_assoc]);
            bool isDead = false;
            UpdatePredTables(metadata->indices, isDead);
            bool pred = MajorityVote(metadata->cntrsNew, THRESHOLD_DEAD);
            bank_ptr->prediction_bank[t->IndexBits][hit_assoc] = pred;
            bank_ptr->signature_bank[t->IndexBits][hit_assoc] = sign;
            UpdatePathHist(PC);
            updateLRUstack(hit_assoc, t);
            clear(t);
            return true;

        }
        else{
            //cache miss case
            Misses++;
            bool found = false;
            int temp = -1;
            int minLRU, maxLRU, max_ind, block_ind;
            maxLRU = -1; minLRU = MAXINT; max_ind = -1;
            for(int i = 0; i < Iassoc; i++){
            
                if (bank_ptr->valid_bank[t->IndexBits][i] == false){
                    // case when there is still empty way to take for  data
                    found = true;
                    temp = i;
                    bank_ptr->valid_bank[t->IndexBits][i] = true;
                    bank_ptr->tag_bank[t->IndexBits][i] = t->TagBits;
                    updateLRUstack(i, t);
                    bank_ptr->signature_bank[t->IndexBits][i] = sign;
                    UpdatePathHist(PC);
                    clear(t);
                    return false;
                }

                if (maxLRU < bank_ptr->LRU_stack[t->IndexBits][i]){
                    // find the oldest data
                    maxLRU = bank_ptr->LRU_stack[t->IndexBits][i];
                    max_ind = i;
                }
            }
            //update LRUstack 
            if (found == false){
                conflictMisses++;
                bool bypass = (MajorityVote(metadata->cntrsNew, THRESHOLD_BYPASS));
                if (bypass == false){
                    replaced++;
                    block_ind = VictimBlock(max_ind, t);
                    metadata->indices = ComputeIndices(bank_ptr->signature_bank[t->IndexBits][block_ind]);
                    bool isDead = true;
                    UpdatePredTables(metadata->indices, isDead);
                    bool pred = MajorityVote(metadata->cntrsNew, THRESHOLD_DEAD);
                    bank_ptr->prediction_bank[t->IndexBits][block_ind] = pred;
                    bank_ptr->valid_bank[t->IndexBits][block_ind] = true;
                    bank_ptr->tag_bank[t->IndexBits][block_ind] = t->TagBits;
                    bank_ptr->signature_bank[t->IndexBits][block_ind] = sign;
                    updateLRUstack(block_ind, t);
                    
                }
                else{
                    bypassed++;
                }
                // bank_ptr->valid_bank[t->IndexBits][max_ind] = true;
                // bank_ptr->tag_bank[t->IndexBits][max_ind] = t->TagBits;
                // updateLRUstack(max_ind, t);
                
                
                UpdatePathHist(PC);
                clear(t);
                return false;
            }


        }
        // return false;
    }

    else if (strcmp(policy, "RANDOM") == 0){

        hit_assoc = check_hit(t);
        // cache hit case
        if (hit_assoc >= 0){
            // updateLRUstack(hit_assoc, t);
            Hits++;
            delete(t);
            return true;
        }
        else{
            // cache misss case
            Misses++;
            bool found = false;
            int temp = -1, rand_ind;
            
            for(int i = 0; i < Iassoc; i++){
            
                if (bank_ptr->valid_bank[t->IndexBits][i] == false){
                    // case when there is still empty way to take data
                    found = true;
                    temp = i;
                    bank_ptr->valid_bank[t->IndexBits][i] = true;
                    bank_ptr->tag_bank[t->IndexBits][i] = t->TagBits;
                    // updateLRUstack(i, t);
                    delete(t);
                    return false;
                }

                
            }
            //update LRUstack 
            if (found == false){
                // std::cout<<std::hex<<"Miss: "<<"tag "<<t->TagBits<<"index hex"<<t->IndexBits<<std::endl; 
                // for (int i = 0; i<Iassoc; i++){
                //     std::cout<<std::hex<<"assoc: " <<i<<" tag "<<bank_ptr->tag_bank[t->IndexBits][i]<<" index hex "<<t->IndexBits<<std::endl; 
                    
                // }
                // std::cin>>maxLRU;
                rand_ind = (rand()%Iassoc);
                bank_ptr->valid_bank[t->IndexBits][rand_ind] = true;
                bank_ptr->tag_bank[t->IndexBits][rand_ind] = t->TagBits;
                // updateLRUstack(rand_ind, t);
                delete(t);
                return false;
            }


        }



    }
    else if(strcmp(policy,"RRIP") == 0){
        hit_assoc = check_hit(t);
        // cache hit case
        if (hit_assoc >= 0){
            UpdateRRIP(hit_assoc, t, true);
            Hits++;
            delete(t);
            return true;
        }
        else{
            // cache misss case
            Misses++;
            bool found = false;
            int temp = -1, rand_ind;
            
            for(int i = 0; i < Iassoc; i++){
            
                if (bank_ptr->valid_bank[t->IndexBits][i] == false){
                    // case when there is still empty way to take data
                    found = true;
                    temp = i;
                    bank_ptr->valid_bank[t->IndexBits][i] = true;
                    bank_ptr->tag_bank[t->IndexBits][i] = t->TagBits;
                    UpdateRRIP(i,t,false);
                    delete(t);
                    return false;
                }

                
            }
            //update RRPVbits 
            if (found == false){
                int block_ind  = RRIP_victimBlock(t);
                UpdateRRIP(block_ind, t, false);
                bank_ptr->valid_bank[t->IndexBits][block_ind] = true;
                bank_ptr->tag_bank[t->IndexBits][block_ind] = t->TagBits;
                delete(t);
                return false;
            }
        }
        
    }
    else{

        assert(0);
    }


    
}

void Icache::UpdatePathHist(unll_t PC){
    history = history<<4;
    uns_t mask = 0 ;
    PC = PC >> 2;
    mask = mask | (1) | (1 << 1)| (1 << 2);
    history =  history | ((PC & mask) << 1);

}
int Icache::Signature(unll_t PC){
    int signature = (history ^ PC)& UINT16_MAX;
    return signature;
}

int* Icache::ComputeIndices(int sign){
    int *ptr = (int *) calloc(NumPredTables,sizeof(int));
    for (int i = 0; i < NumPredTables; i++){
        // std::cout<<sign<<"\n";
        ptr[i] = hashing(((sign<<2)|i))%4096;
        // std::cout<<ptr[i]<<"hash\n";
    }
    // int l;
    // std::cin>>l;
    return ptr;

}
void Icache::clear(Addr_info* t){
    free(metadata->indices);
    free(metadata->cntrsNew);
    delete(t);

}
bool Icache::MajorityVote(int counters[], int threshold){


    int vote = 0;
    for (int i = 0; i < NumPredTables; i++ ){
        if (counters[i]> threshold){
            vote++;
        }
    }
    if (vote >= double(NumPredTables)/double(2)){ return true;}
    else {return false;}

}

int* Icache::GetCounters(int *indices){
    int * ptr = (int* ) calloc(NumPredTables,sizeof(int));
    for (int i = 0; i < NumPredTables; i++)
    {       
        ptr[i] = metadata->predTables[indices[i]][i];
    }
    return ptr;

}

int Icache::VictimBlock(int LRU_ind, Addr_info* t){
    for (int i = 0; i < Iassoc; i++)
    {
        if (bank_ptr -> prediction_bank[t->IndexBits][i] == true){
            return i;
        }
    }
    return LRU_ind;
    
}

void Icache::UpdatePredTables(int *indices, bool isDead){
    
    for (int t = 0; t < NumPredTables; t++)
    {
        if (isDead == true){
            metadata->predTables[indices[t]][t] = SatIncrement(metadata->predTables[indices[t]][t], 15);

        }
        else{
            metadata->predTables[indices[t]][t]= SatDecrement(metadata->predTables[indices[t]][t]); 


        }
    }

}
void Icache::UpdateRRIP(int block_ind, Addr_info* t, bool hit){
    if (hit){bank_ptr->RRIP_stack[t->IndexBits][block_ind] = 0;}
    else{bank_ptr->RRIP_stack[t->IndexBits][block_ind] = 2;}

}
int Icache::RRIP_victimBlock( Addr_info* t){
    while(1){
        for (int i = 0; i < Iassoc; i++)
        {
            if(bank_ptr->RRIP_stack[t->IndexBits][i] == 3){

                return i;
            }
        }

        for (int i = 0; i < Iassoc; i++)
        {
            bank_ptr->RRIP_stack[t->IndexBits][i]++;
        }


    }


}
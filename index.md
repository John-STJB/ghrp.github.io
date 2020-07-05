## Cache Replacement Policies

In the following sections, I will present and compare existing Instuction Cache(I-Cache) Replacement Policies (PRs) and briefly introduce proposed design for I-Cache mentioned in the paper


### Dead Block Issue

It has been observed that caches often retain dead blocks, i.e. blocks in the cache that will not be used again until they are evicted. Dead blocks waste
space and energy in the cache. 

Some techniques were introduced to tackle this problem:

1. **Trace-based dead block predictor.** In this technique, a trace of instruction addresses that make reference to a block is summarized in a block signature associated with that block. The signature is used to index a table of saturating counters. The corresponding counter is incremented when a block is evicted and
decremented when a block is reused, thus keeping track of the tendency of an instruction trace to lead to a dead block.
2. **Counter-based dead block predictor.**  In this technique, each cache block is associated with a counter keeping track of the number of accesses to a block before it is evicted. The live time and access time of a block are tracked using Live time (LvP) and access time predictor (AIP). When the counter reaches a threshold, the block is predicted as dead. [related paper](https://ieeexplore.ieee.org/document/4358260?section=abstract)
3. **Deadtimekeeping techniques.** In these techniques, the system learns the number of cycles the block is accessed. The block is considered to be dead is considered to be dead if it is not accessed twice the number of cycles. [related paper](https://dl.acm.org/doi/pdf/10.1145/545214.545239)
4. **Sampling-based Dead Block Prediction.** In this technique, the system uses ses only the address (PC) of the most recent instruction, allowing it to be useful in the last-level cache and eliminating the need to store signatures with blocks. In this method, a predictor learns the pattern of accesses and evictions from a
small number of sets. [related paper](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=5695535)

**_Note:_**
  - Sampling-based dead block predictors such as SDBP[[2]](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=5695535) are not suitable for the instruction cache. PC-based predictors exploit the observation that, if a block in the data cache is accessed by a given PC and becomes dead, other blocks accessed by the same PC in other sets are likely to become dead as well. Set-sampling allows the predictor to learn from only a small number of sets, allowing the metadata store to be very small. Unfortunately, instruction streams do not allow for set-sampling in this way since the PC itself forms the index into the I-cache.
![SDBP problem](https://github.com/shyngys-aitkazinov/ghrp.github.io/blob/master/DeadBlockProblem.png?raw=true)  


### Replacement Policies:

## LRU
Discards the least recently used items first. This algorithm requires keeping track of what was used when, which is expensive if one wants to make sure the algorithm always discards the least recently used item. General implementations of this technique require keeping "age bits" for cache-lines and track the "Least Recently Used" cache-line based on age-bits. In such an implementation, every time a cache-line is used, the age of all other cache-lines changes. LRU does not consider any reuse possibilites, it just replaces the oldest entry with newcoming one.
```python
Cache Hit:
  (i) move block to MRU  
Cache Miss:
  (i) replace LRU block
  (ii) move block to MRU
 ``` 

## Random
Randomly selects a candidate item and discards it to make space when necessary. This algorithm does not require keeping any information about the access history. For its simplicity, it has been used in ARM processors.[8] It admits efficient stochastic simulation

## SRRIP
SRRIP [[1]](https://people.csail.mit.edu/emer/papers/2010.06.isca.rrip.pdf) keeps track of the recency of blocks by predicting blocks that will be re-referenced again in the cache. Each block is associated with a two-bit re-reference prediction value. An initial prediction is made on block placement and revised when a block is reused or replace. The pseudocode and the simulation of the SRRIP with 2 bit counters are illustrated below.  
```python
Cache Hit:
  (i) set RRPV of block to 0
Cache Miss:
  (i) search for first 3 from left
  (ii) if 3 found go to step (v)
  (iii) increment all RRPVs
  (iv) goto step (i)
  (v) replace block and set nru-bit to 1 (v) replace block and set RRPV to 2
 ``` 
![RRIP problem](https://github.com/shyngys-aitkazinov/ghrp.github.io/blob/master/images/RRIP.png?raw=true)  

## GHRP (Global History Reuse Predictor)
This is the design proposed in the paper. The main idea is to keep the instructions in the cache which will be reused in the future and bypass the instructions which will soon evicted without cache-hits to that block. The design relies on the Global History of I-Cache accesses and correlates it with the reuse counters in prediction tables. This is required to predict whether the fetched blocked should be placed or bypassed. The pseudocode of the GHRP access algorithm is as following:


```python
Algorithm 1 GHRP 
1: int cntrsNew[numPredTables] 
2: int predTables[numCounts][numPredTables] 
3: int indices[numPredTables] 
4: procedure Access(int PC) 
5: 	sign = signature(PC, history) 
6:	indices = ComputeIndices(sign)
7: 	cntrsNew = GetCounters(predTables, indices) 
8: 	set = calcSet(PC, cache) 
9: 	tag = calcTag(PC, cache) 
10: 	isMissed = isTagMatch(PC, cache) 
11: 	if isMissed = true then           # miss 
12: 		bypass←majorityVote(cntrsNew, bypassThresh) 
13: 		if bypass = false then 
14: 			block = victimBlock(set) 
15: 			indices = ComputeIndices(block.signature) 
16: 			isDead = true 
17: 			updatePredTables(indices, isDead) 
18: 			pred←MajorityVote(cntrsNew, deadThresh) 
19:			block.dead = pred 
20: 			block.tag = tag 
21: 	else                          # hit 
22: 		block = matchedBlock(set, tag)
23: 		indices = ComputeIndices(block.signature) 
24: 		isDead = false 
25: 		updatePredTables(indices, isDead)
26: 		pred = MajorityVote(cntrsNew, deadThresh) 
27: 		block.dead = pred 
28: 	block.signature = sign 
29: 	history = UpdatePathHist(PC) 
30: 	updateLRUstackPosition()
```


### References
- papers mentioned in the links above
- [cache RPs](https://en.wikipedia.org/wiki/Cache_replacement_policies#:~:text=Caching%20improves%20performance%20by%20keeping,room%20for%20the%20new%20ones.)

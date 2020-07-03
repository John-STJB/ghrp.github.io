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

**__Note:__**
  - Sampling-based dead block predictors such as SDBP[[2]](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=5695535) are not suitable for the instruction cache. PC-based predictors exploit the observation that, if a block in the data cache is accessed by a given PC and becomes dead, other blocks accessed by the same PC in other sets are likely to become dead as well. Set-sampling allows the predictor to learn from only a small number of sets, allowing the metadata store to be very small. Unfortunately, instruction streams do not allow for set-sampling in this way since the PC itself forms the index into the I-cache.
![Figure of SDBP problem](https://github.com/shyngys-aitkazinov/ghrp.github.io/images/DeadBlockProblem.png)  

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/shyngys-aitkazinov/shyngys-aitkazinov.github.io/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.

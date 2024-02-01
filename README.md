# Cache
Author of cacher.c and Makefile: Jason Wu

==========[Introduction]

The program, written entirely in c, is a cache system that supports FIFO, LRU, and clock eviction policies. It should continuously take items from stdin until stdin is closed. After each lookup, the program should print to stdout specifying
whether the item that was accessed is a HIT or MISS. If the lookup is a miss, it will add the item to the cache and evict an item based upon the eviction policy that the user specified. After stdin is closed, the program will output a summary line that specifies the total number of compulsory and capacity misses.

==========[Installation]

You will need Linux x86_64 or macOS

==========[Usage]

$ ./cacher [-N size] \<policy\>

- size: the size of your cache, or number of items that can be stored in cache(not optional)
- policy: the policy that the cache will follow; the policy flag needs to be exactly -F (First on First out), -L (Least Recently Used), or -C (Clock); this argument is optional(First in First out if no policy is given).

==========[Todo]

- Allows user to specify the size of cache
- Allows user to specify the eviction policy that is going to be used by the cache
- Allows user to pass newline separated items to the cache

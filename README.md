# MEMGC
simple garbage collector
It's a garbage collector that will free memory that allocated.
How to use:
1, #include "memgc.hpp"

2, allocate memory with ```alloc<yourType>(yourArguments)```;
or
```alloc<arrayType[]>(length);```
this function will return a pointer to the memory that allocated, and also automaticly store the address in the MEMGC.
tips: don't worry about the array, the MEMGC will automaticly detect whether the address is an array or not.

3,now you can use the pointer just like a regular one, MEMGC will free the memory automaticly.

# MEMGC
Simple C++ garbage collector

It's a garbage collector that will free memory that allocated.

How to use:

1, #include "memgc.hpp"

2, allocate memory with ```alloc<yourType>(yourArguments)```;
or
```alloc<arrayType[]>(length);```
this function will return a pointer to the memory that allocated, and also automaticly store the address in the MEMGC.

tips: don't worry about the array, the MEMGC will automaticly detect whether the address is an array or not.

3,now you can use the pointer just like a regular one, MEMGC will free the memory automaticly.

Sample:

```cpp
#include <iostream>
#include <string>
#include "memgc.hpp"
int main()
{
	int* arr = alloc<int[]>(10);
	for (int i = 0; i < 10; ++i)
		arr[i] = i + 1;		//add 1 ~ 100 to the array

	for (int j = 0; j < 100; ++j)
		std::cout << arr[j] << " ";	//print array
	std::cout << std::endl;
	
	std::string* str_ptr = alloc<std::string>("Hello World!");
	std::cout << (*str_ptr) << std::endl;

}//MEMGC will free all the memory that alloc<>() allocated
```

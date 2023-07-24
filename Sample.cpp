#include <iostream>
#include <string>
#include "memgc.hpp"
int main()
{
	int* arr = alloc<int[]>(10);
	for (int i = 0; i < 10; ++i)
		arr[i] = i + 1;		  //add 1 ~ 100 to the array

	for (int j = 0; j < 100; ++j)
		std::cout << arr[j] << " ";	  //print array
	std::cout << std::endl;
	
	std::string* str_ptr = alloc<std::string>("Hello World!");
	std::cout << (*str_ptr) << std::endl;

}  //MEMGC will free all the memory that alloc<>() allocated

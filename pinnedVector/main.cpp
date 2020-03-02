#include <iostream>
#include "PinneVector.h"

int main()
{

	int *ptr;


	{
		PinnedVector<int> v;
		PinnedVector<int> v2;

		v.resize(10);
		

		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		v.push_back(4);

		for(auto &i : v)
		{
			std::cout << i;
		}

		
	}

	//std::cout << *ptr << "\n";

	std::cout << "test";

	return 0;
}
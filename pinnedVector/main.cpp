#include <iostream>
#include "PinneVector.h"

int main()
{

	int *ptr;


	{
		PinnedVector<int> v;
		PinnedVector<int> v2;

		v.resize(10);
		

		v[0] = 1;

		ptr = &v[0];

		std::cout << v[0] << "\n";

		v.resize(1200);

		PinnedVector<int> v3 = std::move(v);

		std::cout << v3[0] << "\n";
		
	}

	//std::cout << *ptr << "\n";

	std::cout << "test";

	return 0;
}
#include <iostream>
#include "PinneVector.h"

int main()
{
	PinneVector<int> v;

	v.resize(10);

	v[0] = 1;

	std::cout << v[0] << "\n";

	v.resize(1200);

	std::cout << v[0] << "\n";

	return 0;
}
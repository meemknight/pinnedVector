#include <iostream>
#include <algorithm>

//#define PINNED_VECTOR_MEMORY_CHECK

#include "PinnedVector.h"

#include <vector>

//todo add a compare with a standard container like vector

int counter = 0;
int totalCounter = 0;
struct Test
{
	Test() { counter++; totalCounter++; hasData = 1; a = 69; }
	Test(Test &other) { counter++; totalCounter++; hasData = other.hasData; a = 69; }
	Test(Test &&other) noexcept { hasData = other.hasData; other.hasData = 0; a = 69; }

	~Test() { if (hasData) { counter--; a = 69; hasData = 0; } }

	Test &operator=(const Test &other)
	{
		if (this == &other) { return *this; }

		this->~Test();
		a = 69;
		counter++; totalCounter++; hasData = 1;
		return *this;
	}

	Test &operator=(Test &&other) noexcept
	{
		if (this == &other) { return *this; }

		this->~Test();
		a = 69;
		hasData = other.hasData;
		other.hasData = 0;
		return *this;
	}

	int a=69;
	bool hasData = 0;
};

struct Test2
{
	//Test2(){x = 420;}
	int a[100];
	//int x = 420;
};

std::string data[10] = {};

int main()
{

	std::cout << "Starting program:\n";

	PinnedVector<int> vInt;
	vInt.push_back(5);
	vInt.push_back(4);
	vInt.push_back(10);
	vInt.push_back(3);
	vInt.push_back(2);
	vInt.push_back(6);
	vInt.push_back(9);
	vInt.push_back(1);
	vInt.push_back(8);
	vInt.push_back(7);

	std::sort(vInt.begin(), vInt.end());

	std::cout << "Sort test:\n";

	for(auto &i : vInt)
	{
		std::cout << i << " ";
	}

	std::cout << "\n\n";
	vInt.clear();

	for (int i = 0; i < 10000; i++)
	{
		{
			PinnedVector<Test> v;

			v.resize(1);
		
			v.push_back({});
			v.push_back({});
			v.push_back({});
			v.push_back({});
			v.pop_back();
			
			v.resize(15);
			//

			auto i1 = v.top();
			auto &i2 = v.top();
			const auto i3 = v.top();
			const auto &i4 = v.top();

			Test t;
			v.push_back(std::move(t));
		
			v.pop_back();
			v.pop_back();
			v.pop_back();
			v.pop_back();
		}


	}

	PinnedVector<std::string> test;
	//test.resize(1);
	//test.push_back("lolsdfghjhgfdsderftyhjsadfghjhgfdsasdfghjhgfdsafg");
	test.push_back({});

	//test on stl data types
	for (int i = 0; i < 10000; i++)
	{
		{
			PinnedVector<std::string> v;

			v.resize(1);
			v.push_back("test ------------------------------------------ --- ");
			v.push_back("test ------------------------------------------ --- ");
			v.push_back("test ------------------------------------------ --- ");
			v.push_back("test ------------------------------------------ --- ");
			v.push_back("");
			v.push_back({});
			v.push_back(std::move(std::string("test long...........................................")));
			v.push_back({});
			v.push_back({});
			v.pop_back();
			v.resize(15);
			//
			v[4] = "new test ------------------------------------------";

			for (auto &i : v)
			{
				//i = "---------------------------------------------";
				//std::cout << i.c_str() << "\n";
			}



		}

	}


	std::cout << "counter (non zero means leaks): " << counter << "\ntotal: " << totalCounter;

	std::cin.get();
	return 0;
}
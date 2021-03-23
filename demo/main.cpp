#include <iostream>
#include "PinneVector.h"
#include <vector>

int counter = 0;
int totalCounter = 0;

struct Test
{
	Test() { counter++; totalCounter++; hasData = 1; a = 69; }
	Test(Test &other) { counter++; totalCounter++; hasData = 1; a = 69; }
	Test(Test &&other) { hasData = 1; other.hasData = 0; a = 69; }

	~Test() { if (hasData) { counter--; a = 69; } }

	Test operator=(const Test &other)
	{
		a = 69;
		counter++; totalCounter++; hasData = 1;
		return *this;
	}

	Test operator=(Test &&other)
	{
		a = 69;
		hasData = 1;
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

int main()
{

	int *ptr;


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

		}

	}


	for (int i = 0; i < 10000; i++)
	{
		{
			PinnedVector<std::string> v;

			v.resize(1);
			v.push_back({});
			v.push_back("test ------------------------------------------ --- ");
			v.push_back("test ------------------------------------------ --- ");
			v.push_back("test ------------------------------------------ --- ");
			v.push_back("test ------------------------------------------ --- ");
			v.push_back({});
			//v.push_back({});
			//v.push_back({});
			//v.push_back({});
			//v.pop_back();
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



	std::cout << "counter: " << counter << "\ntotal: " << totalCounter;

	std::cin.get();
	return 0;
}
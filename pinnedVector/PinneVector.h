#pragma once
#include <Windows.h>
#include <assert.h>

#ifdef _DEBUG

#define PINNED_VECTOR_DEBUG(x) x
#define PINNED_VECTOR_ASSERT(x) assert(x)

#else

#define PINNED_VECTOR_DEBUG(x) 
#define PINNED_VECTOR_ASSERT(x) 

#endif // DEBUG



template <class T, unsigned int maxElCount = 12000>
class PinneVector
{
public:

	PinneVector()
	{
		initializeArena();
	}

	void *begin = 0;
	unsigned int size = 0;
	//size_t actualSizeInBytes = 0;


	void initializeArena();

	void resize(unsigned int elementCount);

	T &operator[] (unsigned int index)
	{
		PINNED_VECTOR_ASSERT(index < size);
		return static_cast<T*>(begin)[index];
	}

	T operator[] (unsigned int index) const
	{
		PINNED_VECTOR_ASSERT(index < size);
		return static_cast<T*>(begin)[index];
	}
};

template<class T, unsigned int maxElCount>
inline void PinneVector<T, maxElCount>::initializeArena()
{
	
	begin = VirtualAlloc(0, sizeof(T) * maxElCount, MEM_RESERVE, PAGE_READWRITE);
	PINNED_VECTOR_ASSERT(begin != nullptr);

}

template<class T, unsigned int maxElCount>
inline void PinneVector<T, maxElCount>::resize(unsigned int elementCount)
{
	if(elementCount > size)
	{
		VirtualAlloc(begin, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
		size = elementCount;
	}


}

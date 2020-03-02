#pragma once
#include <Windows.h>
#include <assert.h>

// /////////////////
// Pinned vector
// Define PINNED_VECTOR_MEMORY_CHECK to check if acces deleted memory areas
// 
//

//#define PINNED_VECTOR_MEMORY_CHECK

#ifdef _DEBUG

#define PINNED_VECTOR_DEBUG(x) x
#define PINNED_VECTOR_ASSERT(x) assert(x)

#else

#define PINNED_VECTOR_DEBUG(x) 
#define PINNED_VECTOR_ASSERT(x) 

#endif // DEBUG




template <class T, unsigned int maxElCount = 12000>
struct PinnedVector
{
	void *begin = 0;
	unsigned int size = 0;

	PinnedVector()
	{
		initializeArena();
	}

	PinnedVector(PinnedVector &&other)
	{
		this->begin = other.begin;
		this->size = other.size;

		other.size = 0;
		other.begin = nullptr;
	
		//?
		other.initializeArena();
	}

	PinnedVector(PinnedVector &other)
	{
		initializeArena();
		resize(other.size);

		memcpy(begin, other.begin, size * sizeof(T));
	}


	static constexpr unsigned int maxSize = maxElCount;

	void initializeArena();

	void resize(unsigned int elementCount);

	PinnedVector &operator= (const PinnedVector& other)
	{
		this->resize(other.size);
		memcpy(begin, other.begin, size * sizeof(T));
		return *this;
	}

	PinnedVector &operator= (const PinnedVector&& other)
	{
		free();
		this(std::move(other));
		return *this;
	}

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

	void push_back(const T& el);
	void pop_back();

	void clear();

	void free();

	~PinnedVector();
};

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::initializeArena()
{
	if(begin)
	{
		free();
	}

	begin = VirtualAlloc(0, sizeof(T) * maxElCount, MEM_RESERVE, PAGE_READWRITE);
	PINNED_VECTOR_ASSERT(begin != nullptr);

}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::resize(unsigned int elementCount)
{
	if(!begin)
	{
		initializeArena();
	}

	if(elementCount > size)
	{
		VirtualAlloc(begin, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
		size = elementCount;
	}else 
	{
		elementCount = size;
	}

}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::free()
{

#ifdef PINNED_VECTOR_MEMORY_CHECK

	if (begin)
	{
		//VirtualFree(begin, maxElCount * sizeof(T), MEM_DECOMMIT);
		VirtualAlloc(begin, sizeof(T) * maxElCount, MEM_COMMIT, PAGE_NOACCESS);
	}

#else

	if (begin)
	{
		VirtualFree(begin, maxElCount * sizeof(T), MEM_RELEASE);
	}

	size = 0;

#endif // PINNED_VECTOR_MEMORY_CHECK

}

template<class T, unsigned int maxElCount>
inline PinnedVector<T, maxElCount>::~PinnedVector()
{
	free();
}

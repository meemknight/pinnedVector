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
	T *beg = 0;
	unsigned int size = 0;

	typedef T* iterator;
	typedef const T* constIterator;

	iterator begin() { return &((T*)beg)[0]; }
	constIterator begin() const { return &((T*)beg)[0]; }
	iterator end() { return &((T*)beg)[size]; }
	constIterator end() const { return &((T*)beg)[size]; }

	static constexpr unsigned int maxSize = maxElCount;

	PinnedVector()
	{
		initializeArena();
	}

	PinnedVector(PinnedVector &&other)
	{
		this->beg = other.beg;
		this->size = other.size;

		other.size = 0;
		other.beg = nullptr;
	
		//?
		other.initializeArena();
	}

	PinnedVector(PinnedVector &other)
	{
		initializeArena();
		resize(other.size);

		memcpy(beg, other.beg, size * sizeof(T));
	}

	void initializeArena();

	void resize(unsigned int elementCount);

	PinnedVector &operator= (const PinnedVector& other)
	{
		this->resize(other.size);
		memcpy(beg, other.beg, size * sizeof(T));
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
		return static_cast<T*>(beg)[index];
	}

	T operator[] (unsigned int index) const
	{
		PINNED_VECTOR_ASSERT(index < size);
		return static_cast<T*>(beg)[index];
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
	if(beg)
	{
		free();
	}

	beg = (T*)VirtualAlloc(0, sizeof(T) * maxElCount, MEM_RESERVE, PAGE_READWRITE);
	PINNED_VECTOR_ASSERT(beg != nullptr);

}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::resize(unsigned int elementCount)
{
	if(!beg)
	{
		initializeArena();
	}

	if(elementCount > size)
	{
		VirtualAlloc(beg, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
		size = elementCount;
	}else 
	{
		elementCount = size;
	}

}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::push_back(const T & el)
{
	resize(size + 1);
	beg[size - 1] = el;
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::pop_back()
{
	///todo
	beg[size - 1].~T();
	resize(size - 1);
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::free()
{

#ifdef PINNED_VECTOR_MEMORY_CHECK

	if (beg)
	{
		//VirtualFree(beg, maxElCount * sizeof(T), MEM_DECOMMIT);
		VirtualAlloc(beg, sizeof(T) * maxElCount, MEM_COMMIT, PAGE_NOACCESS);
	}

#else

	if (beg)
	{
		VirtualFree(beg, maxElCount * sizeof(T), MEM_RELEASE);
	}

	size = 0;

#endif // PINNED_VECTOR_MEMORY_CHECK

}

template<class T, unsigned int maxElCount>
inline PinnedVector<T, maxElCount>::~PinnedVector()
{
	free();
}

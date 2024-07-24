#pragma once

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pinned vector 1.3
// Define PINNED_VECTOR_MEMORY_CHECK to check if acces deleted memory areas (not working yet)
// Define PINNED_VECTOR_BOUNDS_CHECK to check in bounds acces
// https://github.com/meemknight/pinnedVector
// licensed under MIT license, do not remove this notice https://github.com/meemknight/pinnedVector/blob/master/LICENSE
// Luta Vlad 2021
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
//#define PINNED_VECTOR_BOUNDS_CHECK
// It checks if you acces outside the array
/////////////////////////////////////////////
#ifdef _DEBUG
#define PINNED_VECTOR_BOUNDS_CHECK
#endif

//(not working yet)
/////////////////////////////////////////////////////////////////////////////
//#define PINNED_VECTOR_MEMORY_CHECK
// 
// this checks if you acces memory that was freed. 
// I does not release memory, but rather ivalidates it so it would
// fail on acces. Can consume a lot of memory due to this reason so use it 
// only to debug and it can crash quickly if you allocate and deallocate 
// often with this vector
/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//logs
//
//Pinnded vecroe 1.2 -> fixed some forwarding semantics problems
//Pinnded vecroe 1.3 -> moved max size from template to constructor
//
//

#ifdef PINNED_VECTOR_BOUNDS_CHECK

//you can use your own assert function here if you want.
//it is used to check if the vector gets out of bounds
#define PINNED_VECTOR_ASSERT(x) assert(x)

#else

#define PINNED_VECTOR_ASSERT(x) 

#endif

//you can use your own assert function here if you want.
//it is used to check if the allocations fail
#define PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(x) assert(x)


////////////////////////////////////////////////////////////////////////////////////////////////
//code

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define PINNED_VECTOR_WIN
#elif defined(__linux__) && !defined(__ANDROID__)
#define PINNED_VECTOR_LINUX
#else
#error pinned vector supports only windows and linux
#endif



#ifdef PINNED_VECTOR_WIN
#include <Windows.h>
#else
#include <sys/mman.h>
#include <cstdio>
#endif

#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>




#ifdef PINNED_VECTOR_WIN

#define PINNED_VECTOR_RESERVE_MEMORY(size)\
			VirtualAlloc(0, (size), MEM_RESERVE, PAGE_READWRITE)

#define PINNED_VECTOR_COMMIT_MEMORY(beg, size)\
			VirtualAlloc((beg), (size), MEM_COMMIT, PAGE_READWRITE)

#define PINNED_VECTOR_FREE_MEMORY(beg)\
			VirtualFree((beg), 0, MEM_RELEASE)


#else

	//#define PINNED_VECTOR_RESERVE_MEMORY(size) mmap(nullptr, (size), PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)
#define PINNED_VECTOR_RESERVE_MEMORY(size) mmap(nullptr, (size),  PROT_READ|PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)
//#define PINNED_VECTOR_RESERVE_MEMORY(size) malloc(size)

//#define PINNED_VECTOR_COMMIT_MEMORY(beg, size) true
//(mprotect((beg), (size), PROT_READ|PROT_WRITE)==0) && (mlock((beg), (size)) == 0)
#define PINNED_VECTOR_COMMIT_MEMORY(beg, size) \
		(mprotect((beg), (size), PROT_READ|PROT_WRITE)==0, 1)
		//(mprotect((beg), (size), PROT_READ|PROT_WRITE)==0) && (mlock((beg), (size)) == 0 && (((unsigned char*)beg)[size-1] = 0, 1))

#define PINNED_VECTOR_FREE_MEMORY(beg) munmap((beg), std::numeric_limits<uint64_t>::max()*sizeof(T))
//#define PINNED_VECTOR_FREE_MEMORY(beg) ::free((void*)beg)


#endif


//maxElCount is the maximum number of ELEMENTS that the vector can store.

template <class T>
struct PinnedVector
{

	T *beg_ = 0;
	unsigned int size_ = 0;

	typedef T *iterator;
	typedef const T *constIterator;

	iterator begin() { return &((T *)beg_)[0]; }
	constIterator begin() const { return &((T *)beg_)[0]; }
	iterator end() { return &((T *)beg_)[size_]; }
	constIterator end() const { return &((T *)beg_)[size_]; }

	PinnedVector(size_t maxElementCount = 1000)
	{
		initializeArena(maxElementCount);
	}

	PinnedVector(PinnedVector &&other)
	{
		this->beg_ = other.beg_;
		this->size_ = other.size_;

		other.size_ = 0;
		other.beg_ = nullptr;

		//?
		other.initializeArena();
	}

	PinnedVector(PinnedVector &other)
	{
		initializeArena();
		noConstructorCallResize(other.size_);

		memcpy(beg_, other.beg_, size_ * sizeof(T));
	}

	void initializeArena(size_t maxElementCount);

	void noConstructorCallResize(size_t elementCount);

	void resize(size_t elementCount);

	void reserve(size_t elementCount);

	size_t size()const { return size_; }

	bool empty() const
	{
		return (size_ == 0);
	}

	T *data();

	PinnedVector &operator= (const PinnedVector &other)
	{
		if (this == &other)
		{
			return *this;
		}

		free();

		this->noConstructorCallResize(other.size_);
		memcpy(beg_, other.beg_, size_ * sizeof(T));
		return *this;
	}

	PinnedVector &operator= (const PinnedVector &&other)
	{
		if (this == &other)
		{
			return *this;
		}

		free();
		
		this->beg_ = other.beg_;
		this->size_ = other.size_;

		other.size_ = 0;
		other.beg_ = nullptr;

		//?
		other.initializeArena();

		return *this;
	}

	T &operator[] (unsigned int index)
	{
		PINNED_VECTOR_ASSERT(index < size_);
		return static_cast<T *>(beg_)[index];
	}

	T operator[] (unsigned int index) const
	{
		PINNED_VECTOR_ASSERT(index < size_);
		return static_cast<T *>(beg_)[index];
	}

	void push_back(const T &el);
	void push_back(T &&el);
	void pop_back();

	T &top()
	{
		return (*this)[size_ - 1];
	}

	const T &top() const
	{
		return (*this)[size_ - 1];
	}

	void clear();

	void free();

	~PinnedVector();
};

template<class T>
inline void PinnedVector<T>::initializeArena(size_t maxElementCount)
{
	if (beg_)
	{
		free();
	}

	beg_ = (T *)PINNED_VECTOR_RESERVE_MEMORY(sizeof(T) * maxElementCount);
	PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(beg_ != 0 && beg_ != (void *)-1);

}

template<class T>
inline void PinnedVector<T>::noConstructorCallResize(size_t elementCount)
{
	//if (!beg_)
	//{
	//	initializeArena();
	//}

	if (elementCount > size_)
	{
		auto check = PINNED_VECTOR_COMMIT_MEMORY((beg_), (elementCount * sizeof(T)));
		PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(check);
		size_ = elementCount;
	}
	else
	{
		size_ = elementCount;
	}

}

template<class T>
inline void PinnedVector<T>::resize(size_t elementCount)
{
	//if (!beg_)
	//{
	//	initializeArena();
	//}

	if (elementCount > size_)
	{
		auto check = PINNED_VECTOR_COMMIT_MEMORY((beg_), (elementCount * sizeof(T)));
		PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(check);

		for (int i = size_; i < elementCount; i++)
		{
			new(&beg_[i])T{};
		}
	}
	else
	{
		if constexpr (!std::is_trivially_destructible<T>::value)
		{
			for (int i = elementCount; i < size_; i++)
			{
				beg_[i].~T();
			}
		}
	}

	size_ = elementCount;

}

template<class T>
inline void PinnedVector<T>::reserve(size_t elementCount)
{
	auto check = PINNED_VECTOR_COMMIT_MEMORY((beg_), (elementCount * sizeof(T)));
	PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(check);

}

template<class T>
inline T *PinnedVector<T>::data()
{
	return beg_;
}

template<class T>
inline void PinnedVector<T>::push_back(const T &el)
{
	noConstructorCallResize(size_ + 1);
	//new(&beg_[size_ - 1])T(el);
	new(&beg_[size_ - 1])T(std::forward<T>(el));
}

template<class T>
inline void PinnedVector<T>::push_back(T &&el)
{
	noConstructorCallResize(size_ + 1);
	//new(&beg_[size_ - 1])T(el);
	new(&beg_[size_ - 1])T(std::forward<T>(el));

}

template<class T>
inline void PinnedVector<T>::pop_back()
{
	if constexpr (!std::is_trivially_destructible<T>::value)
	{
		beg_[size_ - 1].~T();
	}

	noConstructorCallResize(size_ - 1);
}

template<class T>
inline void PinnedVector<T>::clear()
{
	if constexpr (!std::is_trivially_destructible<T>::value)
	{
		for (int i = 0; i < size_; i++)
		{
			beg_[i].~T();
		}
	}

	size_ = 0;
}

//todo add commited size_
template<class T>
inline void PinnedVector<T>::free()
{
	if constexpr (!std::is_trivially_destructible<T>::value)
	{
		for (int i = 0; i < size_; i++)
		{
			beg_[i].~T();
		}
	}

#ifdef PINNED_VECTOR_MEMORY_CHECK

	if (beg_)
	{
		//VirtualFree(beg, maxElCount * sizeof(T), MEM_DECOMMIT);
		DWORD last = {};
		VirtualProtect(beg_, sizeof(T) * size_, PAGE_NOACCESS, &last);
	}

#else

	if (beg_)
	{
		PINNED_VECTOR_FREE_MEMORY(beg_);
	}

	size_ = 0;

#endif // PINNED_VECTOR_MEMORY_CHECK

}

template<class T>
inline PinnedVector<T>::~PinnedVector()
{
	free();
}


#undef PINNED_VECTOR_WIN
#undef PINNED_VECTOR_LINUX
#undef PINNED_VECTOR_RESERVE_MEMORY	
#undef PINNED_VECTOR_COMMIT_MEMORY
#undef PINNED_VECTOR_FREE_MEMORY

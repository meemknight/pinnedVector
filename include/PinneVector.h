#pragma once
#include <Windows.h>
#include <assert.h>
#include <type_traits>

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pinned vector 1.0
// Define PINNED_VECTOR_MEMORY_CHECK to check if acces deleted memory areas
// Define PINNED_VECTOR_BOUNDS_CHECK to check in bounds acces
// https://github.com/meemknight/pinnedVector
// licensed under MIT license, do not remove this notice https://github.com/meemknight/pinnedVector/blob/master/LICENSE
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
//#define PINNED_VECTOR_BOUNDS_CHECK
// It checks if you acces outside the array
/////////////////////////////////////////////
#ifdef _DEBUG
	#define PINNED_VECTOR_BOUNDS_CHECK
#endif


/////////////////////////////////////////////////////////////////////////////
//#define PINNED_VECTOR_MEMORY_CHECK
// 
// this checks if you acces memory that was freed. 
// I does not release memory, but rather ivalidates it so it would
// fail on acces. Can consume a lot of memory due to this reason so use it 
// only to debug and it can crash quickly if you allocate and deallocate 
// often with this vector
/////////////////////////////////////////////////////////////////////////////


#ifdef PINNED_VECTOR_BOUNDS_CHECK

#define PINNED_VECTOR_ASSERT(x) assert(x)

#else

#define PINNED_VECTOR_ASSERT(x) 

#endif

#define PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(x) assert(x)


///////////////////////////////////////////////////////////////////////////////

//maxElCount is the maximum number of elements that the vector can store.
template <class T, unsigned int maxElCount = 12000>
struct PinnedVector
{

	T *beg_ = 0;
	unsigned int size_ = 0;

	typedef T* iterator;
	typedef const T* constIterator;

	iterator begin() { return &((T*)beg_)[0]; }
	constIterator begin() const { return &((T*)beg_)[0]; }
	iterator end() { return &((T*)beg_)[size_]; }
	constIterator end() const { return &((T*)beg_)[size_]; }

	static constexpr unsigned int maxSize = maxElCount;

	
	PinnedVector()
	{
		initializeArena();
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

	void initializeArena();

	void noConstructorCallResize(size_t elementCount);

	void resize(size_t elementCount);

	void reserve(size_t elementCount);

	size_t size()const {return size_;}

	bool empty() const
	{
		return (size_ == 0);
	}


	T *data();

	PinnedVector &operator= (const PinnedVector& other)
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

	PinnedVector &operator= (const PinnedVector&& other)
	{
		if(this == &other)
		{
			return *this;
		}

		free();
		this(std::move(other));
		return *this;
	}

	T &operator[] (unsigned int index)
	{
		PINNED_VECTOR_ASSERT(index < size_);
		return static_cast<T*>(beg_)[index];
	}

	T operator[] (unsigned int index) const
	{
		PINNED_VECTOR_ASSERT(index < size_);
		return static_cast<T*>(beg_)[index];
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

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::initializeArena()
{
	if(beg_)
	{
		free();
	}

	beg_ = (T*)VirtualAlloc(0, sizeof(T) * maxElCount, MEM_RESERVE, PAGE_READWRITE);
	PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(beg_);

}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::noConstructorCallResize(size_t elementCount)
{
	if(!beg_)
	{
		initializeArena();
	}

	if(elementCount > size_)
	{
		auto check = VirtualAlloc(beg_, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
		PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(check);
		size_ = elementCount;
	}
	else 
	{
		size_ = elementCount;
	}

}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::resize(size_t elementCount)
{
	if (!beg_)
	{
		initializeArena();
	}

	if (elementCount > size_)
	{
		auto check = VirtualAlloc(beg_, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
		PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(check);

		for(int i=size_; i<elementCount; i++)
		{
			new(&beg_[i])T{};
		}
		size_ = elementCount;
	}
	else
	{
		if constexpr(!std::is_trivially_destructible<T>::value)
		{
			for (int i = elementCount; i < size_; i++)
			{
				beg_[i].~T();
			}
		}
		
		elementCount = size_;
	}
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::reserve(size_t elementCount)
{
	auto check = VirtualAlloc(beg_, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
	PINNED_VECTOR_ALLOCATION_FAILED_ASSERT(check);

}

template<class T, unsigned int maxElCount>
inline T * PinnedVector<T, maxElCount>::data()
{
	return beg_;
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::push_back(const T & el)
{
	noConstructorCallResize(size_ + 1);
	beg_[size_ - 1] = el;
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::push_back(T &&el)
{
	noConstructorCallResize(size_ + 1);
	beg_[size_ - 1] = std::forward<T>(el);
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::pop_back()
{
	if constexpr (!std::is_trivially_destructible<T>::value)
	{
		beg_[size_ - 1].~T();
	}

	noConstructorCallResize(size_ - 1);
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::clear()
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
template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::free()
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
		VirtualFree(beg_, 0, MEM_RELEASE);
	}

	size_ = 0;

#endif // PINNED_VECTOR_MEMORY_CHECK

}

template<class T, unsigned int maxElCount>
inline PinnedVector<T, maxElCount>::~PinnedVector()
{
	free();
}

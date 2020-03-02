#pragma once
#include <Windows.h>

template <class T>
class PinneVector
{
	void *begin = 0;
	unsigned int size = 0;
	size_t actualSizeInBytes = 0;


	void initializeArena();

	void resize(unsigned int elementCount);


};

template<class T>
inline void PinneVector<T>::initializeArena()
{


}

template<class T>
inline void PinneVector<T>::resize(unsigned int elementCount)
{
}

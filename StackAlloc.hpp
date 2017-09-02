#pragma once

#include "MemoryConfig.hpp"
#include "Errors.hpp"

namespace moe { 
namespace memory {

class StackAlloc {
public:
	explicit StackAlloc(size_t size = ALLOC_DEFAULT_SIZE)
		: 	_size{ size },
			_head{ nullptr },
			_data{ nullptr }
	{
		_data = new byte[_size];
		_head = _data;		
	}

	~StackAlloc() {
		//to do: give user a hint if he deletes a non-empty Allocator
		delete[] _data;
		_data = nullptr;
		_head = nullptr;	
	}

	//no copy and no move:
	StackAlloc(const StackAlloc&) 				= delete;
	StackAlloc(StackAlloc&&) 					= delete;
	StackAlloc operator = (const StackAlloc&) 	= delete;
	StackAlloc operator = (StackAlloc&&) 		= delete;

	void* alloc(size_t numBytes, size_t alignment = 1) {
		byte* location = (byte*)nextMultiple(alignment, (size_t)_head);	
		byte* headPtr = location + numBytes;
		
		//check if we have enough memory:
		if (headPtr > _data + _size) {
			throw errors::MemoryError(("<StackAlloc::alloc> " + OUT_OF_MEMORY).c_str());
		}
		_head = headPtr;
		return location;
	}

	template <typename T, typename... ARGS>
	T* make(size_t numObjs = 1, ARGS&&... args) {
		byte* location = (byte*)nextMultiple(alignof(T), (size_t)_head);
		byte* headPtr = location + numObjs * sizeof(T);
		
		if (headPtr > _data + _size) {
			throw errors::MemoryError(("<StackAlloc::make> " + OUT_OF_MEMORY).c_str());
		}
		T* objPointer = reinterpret_cast<T*>(location);

		//create objs:
		T* objs;
		for (size_t i = 0; i < numObjs; i++) {
			objs = new(moe_addressof(objPointer[i])) T(std::forward<ARGS>(args)...);
		}
		return objPointer;

	}

private:
	size_t _size;
	byte* _head;
	byte* _data;
};	
}} //end of namespace moe::memory

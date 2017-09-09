/* 
	StackAlloc.hpp 
	Allows allocation for different data types
	Gives away memory like a stack, ie. it destroys its elements in reverse order of their creation
	
*/

#pragma once

#include <vector>
#include <iostream>
#include "MemoryConfig.hpp"
#include "Errors.hpp"

namespace moe { 
namespace memory {

/* class to "save" the destructors of the objects using Memory from StackAlloc */
class StackAllocDestructor {
public:
	template <typename T>
	explicit StackAllocDestructor(const T& data)
		: _data(moe_addressof(data))
	{
		_destructor = [](const void* lambdaData) {
			auto type = static_cast<const T*>(lambdaData);
			type->~T();
		};
	}

	void operator()() {
		_destructor(_data);
	}

private:
	const void* _data;
	void(*_destructor)(const void*);	//lambda-expression
};


struct StackAllocMarker {
	StackAllocMarker(byte* val, size_t handle)
		: value{ val }, destructorHandle{ handle } {}
	byte* value;
	size_t destructorHandle;
};


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
		_head = headPtr;

		//create objs:
		for (size_t i = 0; i < numObjs; i++) {
			T* obj = new(moe_addressof(objPointer[i])) T(std::forward<ARGS>(args)...);
			//keep track of the destructors of our objs:
			_destructors.push_back(StackAllocDestructor{ *obj });
		}
		return objPointer;
	}

	void destroyAll() {
		_head = _data;
		std::cout << "Destructors size: " << _destructors.size() << std::endl;
		for(size_t i = 0; i < _destructors.size(); i++) {
			_destructors[i]();
		}
		_destructors.clear();
	}

	void destroyToMarker(StackAllocMarker marker) {
		_head = marker.value;
		while (_destructors.size() > marker.destructorHandle) {
			_destructors.back()();
			_destructors.pop_back();
		}
	}

	StackAllocMarker getMarker() {
		return StackAllocMarker{ _head, _destructors.size() };
	}

private:
	//only store destructors non-trivially-destructable objs:
	template <typename T>
	inline typename std::enable_if<std::is_trivially_destructible<T>::value>::type
	addToList(T* obj) {
		//do nothing
	}

	template <typename T>
	inline typename std::enable_if<!std::is_trivially_destructible<T>::value>::type
	addToList(T* obj) {
		_destructors.push_back(StackAllocDestructor{ *obj });
	}

	size_t _size;
	byte* _head;
	byte* _data;

	std::vector<StackAllocDestructor> _destructors;
};	
}} //end of namespace moe::memory

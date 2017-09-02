#pragma once

#include "Errors.hpp"
#include "MemoryConfig.hpp" 

namespace moe {
namespace memory {

template <typename T>
union PoolChunk {
	T val;
	PoolChunk* next;

	PoolChunk() {}
	~PoolChunk() {}
};

template <typename T>
class PoolAlloc {
public:
	explicit PoolAlloc(size_t sz = ALLOC_DEFAULT_SIZE) 
		: 	_sz { sz },
			_data { nullptr },
			_head { nullptr }
	{
		_data = new PoolChunk<T>[_sz];
		_head = _data;
		for (size_t i = 0; i < sz-1; i++) {
			_data[i].next = moe_addressof(_data[i+1]);
		}
		_data[sz-1].next = nullptr;
	}

	virtual ~PoolAlloc() {
		delete[] _data; //no saftey check, if _data is null we want a crash
		_data = nullptr;
		_head = nullptr;
	}

	//no copy and no move:
	PoolAlloc(const PoolAlloc&) 			= delete;
	PoolAlloc(PoolAlloc&&) 					= delete;
	PoolAlloc operator = (const PoolAlloc&) = delete;
	PoolAlloc operator = (PoolAlloc&&)		= delete;

	template <typename... ARGS>
	T* make(ARGS... args) {
		if	(_head == nullptr) {
			throw errors::MemoryError(("<PoolAlloc::make> " + OUT_OF_MEMORY).c_str());
		}
		PoolChunk<T>* chunk = _head;
		_head = _head->next;
		T* newT = new(moe_addressof(chunk->val)) T(std::forward<ARGS>(args)...);
		return newT;		
	}

	void destroy(T* data) {
		//make sure data is stored within PoolAllocs memory:
		if ((size_t)data < (size_t)_data || (size_t)data > (size_t)_data + _sz-1) {
			throw errors::MemoryError(("<PoolAlloc::destroy> " + INVALID_DEALLOC).c_str());
		}
		data->~T();
		PoolChunk<T>* poolChunk = reinterpret_cast<PoolChunk<T>*>(data);
		poolChunk->next = _head;
		_head = poolChunk;	
	}


protected:
	size_t _sz;
	PoolChunk<T>* _data;
	PoolChunk<T>* _head;
};
} //end of namespace moe::memory
} //end of namespace moe

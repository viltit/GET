#include "MemoryConfig.hpp"
#include "Errors.hpp"


namespace moe {
namespace memory {

class GeneralAlloc {
public:


private:
	static const size_t MAX_ALIGN = 128;

	//private helper class:
	class GeneralAllocFreeChunk {
	public:
		GeneralAllocFreeChunk(byte* addr, size_t size)
			: _addr{ addr }, _size{ size }
		{}

		bool doesTouch(const GeneralAllocFreeChunk& other) const {
			if (_addr + _size == other._addr) return true; 		//other is right of this
			if (other._addr + other._size == _addr) return true;//other is left of this
			return false;	
		}

		bool operator > (const GeneralAllocFreeChunk& other) const {
			return _addr > other._addr;
		}
		bool operator < (const GeneralAllocFreeChunk& other) const {
			return _addr < other._addr;
		}
		
		template <typename T, typename... ARGS>
		T* alloc(size_t numObjs = 1, ARGS&&... args) {
			//assert that alignment is not too big:
			static_assert(alignof(T) <= MAX_ALIGN, "Max alignment exceeded.");
			byte* location = (byte*)nextMultiple(alignof(T), ((size_t)_addr)+1);
			size_t numBytes = numObjs * sizeof(T);
			byte* newAddr = location + numBytes;

			//check if this chunk has enough space:
			if (newAddr > _addr + _size) return nullptr;
			byte offset = (byte)(location - _addr);
			location[-1] = offset;
				
			//adjust this chunks addr and size:
			_size = newAddr - _addr;
			_addr = newAddr;

			//create objs:
			T* returnPtr = reinterpret_cast<T*>(location);
			for (size_t i = 0; i < numObjs; i++) {
				T* obj = moe_addressof(returnPtr[i]);
				new(obj) T(std::forward<ARGS>(args)...);
			}				

			return returnPtr;
		}


		byte* _addr;
		size_t _size;
		

	}; //End of Subclass GeneralAllocFreeChunk

}; //end of class GeneralAlloc

}
}


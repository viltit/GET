/* 
 *	MemoryConfig.hpp
 *	Holds all typedefs, consts and helper functions for namespace moe::memory
 */

#pragma once

#include <cstddef>
#include <string>
#include "StdWrapper.hpp"

namespace moe {
namespace memory {
	
	typedef unsigned char byte;

	constexpr size_t ALLOC_DEFAULT_SIZE { 1024 };

	const moe_string OUT_OF_MEMORY 		{ "Out of memory" };
	const moe_string INVALID_DEALLOC	{ "Invalid deallocation" };

	template <typename T>
	T nextMultiple(size_t multipleOf, T value) {
		T multiple = value + multipleOf - 1;
		multiple -= (multiple % multipleOf);
		return multiple;	
	}
}
}

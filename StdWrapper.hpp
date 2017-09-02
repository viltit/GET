/*
 *	StdWrapper.hpp
 *	wraps STL-Functions in custom functions to gain indepence
 */

#pragma once

#include <string>
#include <memory>

namespace moe {
	typedef std::string moe_string;

	template <typename T>
	inline T* moe_addressof(T& obj) {
		return std::addressof(obj);
	}
}

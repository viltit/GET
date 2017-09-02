#pragma once

#include <stdexcept>

namespace moe {
namespace errors {
	class MemoryError : public std::runtime_error {
	public:
		MemoryError(const char* message) : std::runtime_error(message) {}
	};
}
}

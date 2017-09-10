#include "PoolAlloc.hpp"
#include "StackAlloc.hpp"
#include "StdWrapper.hpp"
#include "GeneralAlloc.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <chrono>

using namespace moe;
using namespace moe::memory;
using namespace moe::errors;

class TestClass {
public:	
	TestClass(const std::string& name, float x = 0.f, float y = 0.f)
		: _name{ name }, _x{ x }, _y{ y }
	{ /*std::cout << _name << " constructed." << std::endl;*/ }
	~TestClass() { /*std::cout << _name << " destructed." << std::endl;*/ }

	void test() {
		std::cout << "Hi, my name is " << _name << std::endl;
	}

private:
	moe_string _name;
	float _x;
	float _y;
};

int main(int argc, char** args) {
	
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<double, std::milli> tick;
	
	const size_t ITS = 400000;
	
	TestClass* objs1[ITS];
	TestClass* objs2[ITS];
	TestClass* objs3;
	PoolAlloc<TestClass> pool{ ITS };
	StackAlloc stack{ ITS * sizeof(TestClass) };
	for (size_t j = 0; j < 10; j++) {
	try {
		//run 1: use new and delete:
		auto t0 = clock::now();
		for (size_t i = 0; i < ITS; i++) {
			objs1[i] = (new TestClass{ "test" });
		}	
		for (size_t i = 0; i < ITS; i++) delete objs1[i];

		auto dt = tick(clock::now() - t0).count();
		std::cout << "New and delete: " << dt  << std::endl;
	
		t0 = clock::now();
		//run 2: use poolAlloc:
		for (size_t i = 0; i < ITS; i++) {
			objs2[i] = pool.make("test");	
		}	
		for (size_t i = 0; i < ITS; i++) pool.destroy(objs2[i]);

		dt = tick(clock::now() - t0).count();
		std::cout << "Pool Alloc: " << dt << std::endl;
		
		t0 = clock::now();

		objs3 = stack.make<TestClass>(ITS, "test");
		stack.destroyAll();

		dt = tick(clock::now() - t0).count();
		std::cout << "StackAlloc: " << dt << std::endl;

	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
	}

	return 0;
}

#include "PoolAlloc.hpp"
#include "StdWrapper.hpp"

#include <iostream>
#include <vector>

using namespace moe;
using namespace moe::memory;
using namespace moe::errors;

class TestClass {
public:	
	TestClass(const std::string& name, float x = 0.f, float y = 0.f)
		: _name{ name }, _x{ x }, _y{ y }
	{ std::cout << _name << " constructed." << std::endl; }
	~TestClass() { std::cout << _name << " destructed." << std::endl; }

private:
	moe_string _name;
	float _x;
	float _y;
};

int main(int argc, char** args) {
	try {
		PoolAlloc<TestClass> poolAlloc{ 4 };
	///	
		TestClass* obj1 = poolAlloc.make("Agata");
		TestClass* obj2 = poolAlloc.make("Ines");

		//should throw an error
		std::vector<TestClass*> objs(10);
		for (size_t i = 0; i < 10; i++) {
			objs.push_back(poolAlloc.make("dummy"));
		}	

		poolAlloc.destroy(obj1);

		TestClass* obj3 = poolAlloc.make("Jessica");

		poolAlloc.destroy(obj2);
		poolAlloc.destroy(obj3);

		TestClass* obj4 = new TestClass("Tina");
		poolAlloc.destroy(obj4);	
	}

	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
	return 0;
}

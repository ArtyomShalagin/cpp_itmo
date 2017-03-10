#include <iostream>

namespace x {
	void f(int x) {
		std::cout << "int\n";
	}
}
using namespace x;
void f(float x) {
	std::cout << "float\n";
}

int main() {
	f(5);
	::f(5);
}
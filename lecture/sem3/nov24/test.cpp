#include <iostream>

// template<typename T>
// void func(T t) {
// 	std::cout << t << std::endl;
// }

// template<typename T, typename ...Params>
// void func(T t, Params... args) {
// 	std::cout << t << std::endl;

// 	func(args...);
// }

void foo(int a) {
	std::cout << a << std::endl;
}

void func(void(*f)(int)) {
	f(42);
}

int main() {
	func(foo);
	std::cout << sizeof(foo) << std::endl;
}
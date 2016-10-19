#include <iostream>
#include <string>

using namespace std;

template<typename T>
void f() {
	cout << sizeof(T) << endl;
};

struct x {
	int a, b, c;
};

int main() {
	f<x>();
	return 0;
}
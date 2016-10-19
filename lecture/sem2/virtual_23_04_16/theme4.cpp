#include <iostream>

using namespace std;

struct base {
	virtual ~base() {
		cout << "base\n";
	}

	base() {
		cout << "base\n";
	}
};

struct derived : base {
	virtual ~derived() {
		cout << "derived\n";
	}

	derived() {
		cout << "derived\n";
	}
};

struct derived2 : derived {
	virtual ~derived2() {
		cout << "derived2\n";
	}

	derived2() {
		cout << "derived2\n";
	}
};

int main() {
	// delete (base*) new derived2();
	// вызывает последовательно деструкторы

	cout << endl;
	derived2 d2;
}
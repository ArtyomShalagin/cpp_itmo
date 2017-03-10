#include <iostream>

using namespace std;

struct x {
	int *val1, *val2, *val3;

	x(int _val1, int _val2, int _val3) {
		val1 = new int(_val1);
		val2 = new int(_val2);
		val3 = new int(_val3);
	}

	~x() {
		cout << "x destr" << endl;
		delete val1;
		delete val2;
		delete val3;
	}
};

void foo() {
	x& value = *(new x(1, 2, 3));
	x* same_mem = new(&value) x(4, 5, 6);
	cout << *(same_mem->val2) << endl;
	delete &value;
	delete same_mem;
}

int main() {
	foo();
	return 0;
}
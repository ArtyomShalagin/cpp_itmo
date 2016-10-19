#include <iostream>

using namespace std;

int g() {
	//lalala
	return 1;
}

//как было раньше
// int f() {
// 	int err = g();
// 	if (err != ok) {
// 		return err;
// 	}
// 	if (err == ok) {
// 		return ok;
// 	}
// 	return 1;
// }

//как теперь

struct x {

};

void f() {
	throw 1;
}

int main() {
	long long cnt = 0;
a:
	cout << "nikita pidor " << cnt << endl;
	cnt++;
	goto a;

	try {
		f();
	} catch (int a) {
		cout << "err " << endl;
	}
}
#include <iostream>

using namespace std;



int main() {
	float y = 1.5f;
	long i = * (long *) &y; // лонг из битовой записи флота
	cout << i << endl;

}
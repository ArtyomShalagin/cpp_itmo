#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
 
using namespace std;
 
const int MAXN = 120;
typedef long long LL;

// виртульные функции -- если в предке есть виртуальная функция, то при наследовании и после этого касте
// к предку будет вызываться все равно новый метод

// статический и динамический типы
// статический -- тип переменной, который написан в коде
// derived x;
// base2* vv = &x;
// статический тип у vv -- указатель на base2, а динамический -- указатель на derived

// void* можно рассматривать, как самый базовый тип, можно всегда привести к нему и назад


 
// struct base {
//     int a, b, c;
// };

// // struct derived : base {
//     // int d, e;
// // };

// void test1() {
//     derived d;
//     base b = d;
//     // b.f();
//     // тут на самом деле base b((base&) d);
// }

// void test2() { // test2 и test3 делают одно и то же
//     derived d;
//     base& b = d;
//     // b.f()''
// }

// void test3() {
//     derived d;
//     base* b = &d;
//     // b -> f();®
// }


// еще примеров
struct base1 {
    int a;
    int b;
};

struct base2 {
    int c;
    int d;
};

struct derived : base1, base2 {

};

int main() {
    derived x;
    x.a = 1;
    x.b = 2;
    x.c = 3;
    x.d = 4;

    void* v = &x;
    derived* vv = (derived*) v; // если поменять эти строки местами, не будет работать
    cout << vv -> a << vv -> b << vv -> c << vv -> d << endl;





}
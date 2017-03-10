Когда объекты копируются, а когда нет

struct mytype {
	int* a, b;
}

mytype f() {
 	mytype result;
 	return result;
}

void f(mytype &result) {
	result = mytype();
}

С return value optimization генерится почти одинаковый код. При этом в первом
случае можно даже больше оптимизаций сделать.
Перепишется примерно в такое

void f(void* result)
Указатель точно ни с чем не алясится, компилятору приятно.

move конструкторы

struct mytype {
	mytype(mytype const&); -- lvalue
	mytype(mytype &&); -- rvalue reference
}

void bar(mytype const&); //1
void bar(mytype &&); //2

mytype foo();

int main() {
	mytype a;
	bar(a); // 1
	bar(foo()); //2
	mytype&& c = a; //err
}

void foo() {
	mytype a = bar();
	v.push_back(static_cast<mytype&&>(a));
	v.push_back(move(a)); вроде одно и то же
}

template<typename T>
T&& move(T& obj) {
	return static_cast<T&&>(obj);
}

как делать push_back

void push_back(T const& a) {
	++size;
	void* placement = ...;
	new(placement) T(a);
}

void push_back(T&& a) {
	++size;
	void* placement = ...;
	new (placement) T(std::move(a));
}
________

void push_back(T a) {
	++size;
	void* placement = ...;
	new (placement) T(std::move(a));
}

говорят, код делает одно и то же, разница только в количестве move-ов
Если передаешь rvalue в функцию, которая принимает T, оно не будет копироваться

оператор присваивания

mytype& operator=(mytype rhs) {
	//clear();
	swap(rhs);
	return *this;
}

такое все еще хорошо работает с rvalue

mytype a = ...;
mytype b = ...;

a = std::move(b);

Если не писать clear, то фактически там будет вызываться swap(a, b), потому что
передается ссылка, а не копия, а пользователь может захотеть, чтобы
память освобождалась, а не пихалась в b.

подробнее про operator=
вот пусть есть только

mytype& operator=(mytype const&);

mytype a, b;
a = std::move(b);
Что получится? move честно сделает rvalue, но вызовется все равно один
и тот же operator=. Поэтому правильнее будет сказать, что move на самом
деле move or copy.

________

что-то там дальше

struct file_descriptor {
	int fd;
}

vector<shared_ptr<file_descriptor>>
vector<file_descriptor*>
ptr_vector<file_descriptor>
vector<int> //hardcooore
либо неэффективно, либо стремно

В c++11 можно
vector<unique_ptr<file_descriptor>> -- аля последние 3 варианта
vector<file_descriptor> -- теперь можем, потому что он некопируемый, но понятно,
	как его мувать -> можем в векторе хранить некопируемые типы

unique_ptr<mytype> a;
mytype* b = a.get();

странная вещь

typedef int& x;
typedef x& y;
reference collapsing rule: y == int&
С rvalue:
& + & = &
& + && = &
&& + & = &
&& + && = &&

пример не знаю чего

template<typename T>
unique_ptr<T> make_unique() {
	return unique_ptr<T>(new T());
}

template<typename T, typename A0>
unique_ptr<T> make_unique(A0 const& a0) {
	return unique_ptr<T>(new T(a0));
}

и так далее...

По-хорошему надо бы еще делать перегрузки для всех комбинаций & и const&.

make_unique<mytype>(1, 2, 3);
			||
std::unique_ptr<mytype>(new mytype(1, 2, 3));

что можно сделать

template<typename T>
void f(T&);

template<typename T>
void g(T const&);

template<typename T>
void h(T&&);

int a;
int const b;
int c();
int const d();

f(a); T -> int
f(b); T -> int const
f(c()); error 
f(d()); error

g(a) T -> int
g(b) T -> int
g(c()) T -> int
g(d()) T -> int

h(a) T -> int& -- void h(int&) по reference collapsing rule
h(b) T -> int const& -- void h(int const&)
h(c()) T -> int -- void h(int&&)
h(d()) void h(int const&&)

template<typename T>
T&& forward(&& a) {
	return static_cast<T&&>(a);
}

template<typename U>
void f(U&& u) {
	g(forward<U>(u));
}

void f(int& u) {
	g(forward<int&>(u));
	T -> int&;
	//int& forward(int&);
}

В цпп11 можно делать типа
template<typename ...U>
void f(U&& ...u) {
	g(forward<U>(u)...);
}

variadic templates

perfect-forwarding -- "это когда пишем там && и форвардим"
_________________________________________________________

variadic functions

как работал принтф

printf("%d%d%d", 1, 2, 3)
сама функция не знала о том, сколько у нее аргументов. В стеке лежало 
подряд сверху вниз просто
3
2
1
msg
return code
Фукнция смотрела на msg и читала со стека, при этом ломалась, если 
msg не совпадала с переданными значениями. Сейчас все не так, потому что 
не передать пользовательские значения, да и вообще можно передавать через 
регистры и все не так просто. 

template <typename ... U>
void g(U ... args)
{
	h(args...);

	h0(h1(args...)); эквивалентно
	h0(h1(a0, a1, ...));

	hp(h1(args)...); эквивалентно
	hp(h1(a0), h1(a1), ...);
}
sizeof...(U) -- сколько аргументов у U.

template <typename B>
struct derived : B...
{};

template<typename ...T>
struct all_are_same;

template<typename T0>
struct all_are_same<T0>
{
	statuc const bool value = true;
}

template<typename T0, typename T1, typename ...Ts>
struct all_are_same<T0, T1, Ts...> {
	static const bool value = is_same<T0, T1>::value && 
		all_are_same<T0, Ts...>::value;
}

_________________________________________________________

тайпдеф на функцию
typedef void(*a)();

алиасы в c++11

using mytype = int;
using a = void(*)();

template<typename T>
using mmap_vactor = vecotr<T, mmap_allocator<T>>

template<typename T>
const T pi = pi_value<T>::value;

_________________________________________________________

auto
auto&
auto&&
ты пидорьбиаордыавр
















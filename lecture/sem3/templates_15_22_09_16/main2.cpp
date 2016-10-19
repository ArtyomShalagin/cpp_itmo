partial специализация -- можно специализировать не все аргументы

template<typename T>
struct vector {
	T* data;
	size_t size;
	size_t capacity;
}

template<typename U>
struct vector<U*> {
	vector_ptr impl;
}

Какие вообще бывают специализации:

primary:
template<...> 
struct name {

}

partial:
template<...>
struct name<...> {

}

explicit:
template<> 
struct name<...> {
	
}


Примерчики

template<typename U, typename V>
struct x;

template<typename A, typename B, typename C>
struct x<A (*)(B), B (*)(C)>;
или struct x<pair<A, B>, pair<B, C>>

Примерчики

template<typename U, typename V>
void f(U, V);

template<typename A, typename B, typename C>
void f(A(*)(B), B(*)(C));

можно еще так:

template<typename T, size_t N> // можно еще enum и nullptr_t
struct array {
	T data[N];
}

template<typename U>
struct array<U, 0> 
{};
Это надо, потому что нельзя int [0], поэтому можно так сделать, тогда будут
все операторы и все такое, только размер 0



считаем факториал, линейно офк:

template<unsigned N>
struct factorial {
	static const unsigned value = N * factorial<N - 1>::value;
}

template<>
struct factorial<0> {
	static const unsigned value = 1;
}

про умные указатели

template<typename T>
struct scoped_ptr {
	~scoped_ptr() {
		sizeof(*ptr);
		delete ptr;
	}

private:
	T* ptr;
}

хедер mytype.h

struct impl;

struct mytype {
	~mytype();
	scoped_ptr<impl> p;
}

скомпилится, потому что при инстанцировании класса не сразу инстанцируются методы
в цпп:

struct impl {};

mytype::~mytype() {
	тут impl уже не incomplete
}

примееееер:

template<typename T>
struct basic_string;

template<typename T>
struct string;

больше примеров:

template<typename D>
struct base : D::type 
{};

struct xxx 
{};

struct derived : base<derived> {
	typedef xxx type;
}

Такое не скомпилится, потому что derived будет incomplete. А так:

template<typename D>
struct base : D::type 
{};

struct xxx 
{};

template<typename T>
struct derived : base<derived<T>> {
	typedef xxx type;
}

такая же ошибка, только точка ошибки переехала:

!!!two-phase name lookup

a b(c); -- переменная или функция?

void f() {
	(a) - b;
	a (b) -- что это вообще такое?
	правило -- все, что может быть рассмотрено, как декларация, 
	является декларацией.
	a <b, c> d;
}

template<typename T>
void f() {
	int a(T::xxx);
	(T::xxx) - b;
	T::xxx(b);
	T::yyy <b, c> d;
}

вообще без доп информации непонятно, что это за дичь

Зависимые имена -- то, что зависит от темплейта

1. T::name
2. vector<T>::name , name -- либо переменная, либо typedef -- не знаем, 
	в какую специализацию войдем -- undetermined specialization

typename -- так компилятор понимает, что дальше тип, а не выражение

T::template yyy<b, c>::template zzz<int>

про двухфазную

template<typename T>
void f(T) {
	mytype a;
}

struct mytype 
{};

int main() {
	f(5);
}

В gcc это не будет работать, потому что на м  омент mytype a класс 
mytype еще incomplete. В майкрософтовом компиляторе это нормально будет, 
потому что он до инстанцирования темплейта не проверяет функцию. 
Первая фаза -- просто смотришь код, вторая фаза -- инстанцирование. 


export template -- c++98, c++03
поддерживался только в comeau
можно писать тело темплейтной функции в цпп, а объявлять в хедере.

пример: два класса

a.cpp:

#include <vector>

struct mytype {
	int a;
}

std::vector<mytype> x;

b.cpp:

#include <vector>

struct mytype {
	double a;
}

std::vector<mytype> y;

такое не получится слинковать, потому что для разных специализаций вектора 
сгенерится разный код. Если бы имена были разными, все было бы норм. 

explicit ассоциации

typedef basic_string<char> string
typedef basic_string<wchar_t> wstring

extern template struct basic_string<char>












































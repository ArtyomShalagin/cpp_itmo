namespace

void gnome_panel_applet_indicator_create()
а можно было
namespace gnome {
	namespace panel {
		namespace applet_indicator {
			void create();
		}
	}
}

gnome::panel::applet_indicator::create();

::f() -- начинай искать в глобальном неймспейсе

namespace alias: namespace fs=boost::filesystem

namespace n {
	void f();
}

using n::f;
void g() {
	f();
}

___

struct base {
	void f();
}

struct base2 {
	void f();
}

struct derived : base, base2 {
	using base::f;
}

___

namespace n {
	void f();
	void g();
	void h();
}

using namespace n;

___

namespace std {
	void f(int);
}

using namespace std;
void f(float);

int main() {
	f(5);  		вызовет f из std
	::f(5);		вызовет из глобала
}

поиск имен бывает qualified и unqualified
a::b::c -- a unqualified, ::b::c qualified

___

Argument-Dependent Lookup (ADL) (только для unqualified)

namespace mylib {
	struct mytype {

	};
}

mylib::mytype a, b;
a + b; -- обычно определен не в классе, чтобы разрешить неявную конверсию 
			левого аргумента
 
сейчас считают, что swap для своих классов надо писать не в std, а рядом со
своим классом, чтобы его нашел ADL

template <typename T>
void f(T& a, T& b) {
	std::swap(a, b); так не будет работать, потому что мб не в std
	swap(a, b); не будет работать для интов
	надо:
	using std::swap;
	swap(a, b); будет искать и в std, и по ADL
}

в хедерах не надо писать юзинг, ИНАЧЕ АЛЬЦГЕЙМЕР

анонимные неймспейсы
1.cpp:
struct x {
	int a;
};
std::vector<x> v;

2.cpp:
struct x {
	char a;
};
std::vector<x> u;

Так будет не работать, потому что сгенерится одинаковый код и
ничего не понятно короче

А в цпп можно так
1.cpp
namespace {
	struct x {
		int a;
	};
}
std::vector<x> v;

2.cpp
namespace {
	struct x {
		char a;
	};
}
std::vector<x> v;

Тогда все нормасики
Но в хедерах так нельзя

_____________________________________________________

C++ style casts

struct base {
	int a;
};

struct base2 {
	int b;
};

struct derived;

base2* f(derived* d) {
	return (base2*) d;
}

struct derived : base, base2 {

};

не будет работать, потому что на момент каста еще не знает, что оно наследник

Вообще касты это для опасных парней, поэтому в цпп их разбили на
несколько типов по стрёмности

1. static_cast<base2*>(d); это норм, он делает только хорошие касты, например
	в другие числа, объекты по иерархии
2. const_cast -- снимает консты, но надо быть аккурантее, чтобы не 
	сделать такое говно с переменной в read-only области
3. reinterpret_cast<int*>(b) -- касты между несвязанными типами
4. dynamic_cast<base*>(c); -- проверяет, корректен ли каст

if (base* b = dynamic_cast<base*>(c))


_____________________________________________________

undefined behavior

void sum(float* result, float const* arr, size_t n) {
	for (size_t i = 0; i != n; i++) {
		*result += arr[i];
	}
}

можно бы
float tmp = 0f;
for (...) {
	tmp += arr[i];
}
*result = tmp;
но компилятору нельзя так заменять, потому что вдруг там result массив

___

void memcpy(char* dst, char const* src, size_t n) {
	for (size_t i = 0; i != n; i++) {
		*dst++ = *src++;
	}
}
сказано, что [dst, dst + n] и [src, src + n] не должны пересекаться

Есть сишное ключевое слово restrict -- значит, не алясится ни с 
каким другим указателем
рестрикт позволял бы копировать большими кусками
Есть функция __assume_aligned -- говорит, типа, предполагай, что 
этот указатель выравнен. Тогда есть шанс, что векторизация будет
a = __assume_aligned(a, 16);

Примеры ub -- double delete, разыменовывание удаленного указателя, каст числа к указателю и
наоборот. в библиотеке bound на несорченном массиве
Примеры ub

int* result;
float* arr;
for (size_t i = 0; i != n; i++) {
	if (arr[i] > 0f)
		++*result;
}

strict aliasing rule -- никогда не обращаемся к ячейке float как double. 
То есть если записали одно то и обращаемся к тому же.
gcc спокойно переупорядочивает операции с разными типами, так как 
там используется это правило. В других компиляторах не всегда так.
Пример:
int* a;
float* b;
*a = 5;
??? = *b;
Если бы a и b указывали на одну память, то их нельзя переупорядочивать. 
А gcc будет.

Пример
struct vector {
	float* a;
	size_t n;
}
for (size_t i = 0; i != v.n; i++) {
	v.a[i] = 0;
}
компилятор не сможет это оптимайзить, потому что v.n надо пересчитывать
на каждом шаге, потому что они могут алиаситься. (не gcc).
по стандарту char может аляситься с чем угодно, с нем нельзя юзать
правило.
a + 1 > a всегда. Переполнение знаковых -- андеф. Можно было бы так 
не делать, но компляторы юзают это для оптимизаций.

пример из ядра линукса
void f(mytype* ptr) {
	int unused = ptr->data;
	if (!ptr) -- проверка всегда неправда, потому что уже было 
					разыменование указателя. В итоге проверка
					просто выбрасывалась.
		return;
}

еще пример, когда возникал бесконечный цикл
void f() {
	int arr[10];
	int i, v;
	for (i = 0, v = arr[i]; i != 10; v = arr[++i]) {
		
	}
}



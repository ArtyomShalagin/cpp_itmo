
TEMPLATES

Что делали до того, например, в С. 
1. Хранили void*, любой указатель можно 
скастить с void* и наоборот.
2. Макросы. 
#define DECLARE_VECTOR(type)			\
	struct vector_##type				\
	{									\
		void push_back(type const&);	\
		type const& back() const;		\
	};
Есть проблемы: например, если хочешь сделать с unsigned long или если
есть два typedef-а, и ты от обоих делаешь вектор.

3. Template

template <typename T>
struct vector {
	void push_back(T const&);
	T const& back() const;
}

template <typename T>
T max(T a, T b) {
	return a > b ? a : b;
}

deduction -- определение типа результата по аргументам
!все template функции по умолчанию inline
Функции и классы с template надо реализовывать прямо в хедере

template <typename T>
void swap(T& a, T& b) {
	T tmp = a;
	a = b;
	b = tmp;
} 

void swap(big_integer& a, big_integer& b) {
	...
}

Суть в том, что big_integer эффективнее свопать по-другому, просто перекидывая
указатели, поэтому можно так вот перегрузить своп.

Для классов есть "специализация"

template <typename T>
struct vector {
	...
}

template <>
struct vector<bool> {
	...
}

Вектор булов работает совсем по-другому






















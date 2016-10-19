SFINAE -- substitution failure is not an error

template<typename C>
void f(C& c, typename C::iterator pos);

template<typename T, size_t N>
void f(T(&c)[N], T* pos);

Ета ошибка при подстановке. Вроде как при этом ничего особо не ломается, 
потому что так договорились. 

template<typename U, typename V>
struct is_same {
	static bool const value = false;
};

template<typename T>
struct is_same<T, T> {
	static bool const value = true;
}

так можно проверять, одинаковые ли классы

template<bool Condition, typename T>
struct enable_if;

template<typename T>
struct enable_if<true, T> {
	typedef T type;
}

template<typename C, typename F>
typename enable_if<is_same<typename container_traits<C>::tag, node_based_tag>::value, void>::type
	erase_if(C& c, F pred) {

	}

Если кондишон не подойдет, он завалится, потому что enable_if будет incomplete,
но при этом по SFINAE не завалится, а пойдет искать дальше.

template<typename T>
void f(typename container_traits<T>::tag);

f(vector_like_tag());


template<typename T>
void f(vector<T> const& v, T const& value);

vector<unsigned> v;
f(v, 5);


template<typename T>
struct no_deduce {
	typedef T type;
};

Можно так
template<typename T>
void f(vector<T> const& v, typename no_deduce<T>::type const& value);
или typename vector<T>::value_type


template<typename U, typename V>
typename max_type<U, V>::type max(U u, V v);


template<typename T, typename V>
struct max_type {
	typedef typename rank_to_type<max_value<type_to_rank<U>::value,
		type_to_rank<V>::value>::value>::type type;
}


template<int a, int b>
struct max_value {
	static int const value = a > b ? a : b;
}

-O3 включает автовекторизацию
-Og для отладки
-Os
-Ofloat = -O3 -float-math

















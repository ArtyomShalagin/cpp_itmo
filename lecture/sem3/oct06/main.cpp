Темы: tag_dispatching, SFINAE, STL

STL != вся стандартная библиотека плюсов
Generic Programming
Книжка Alexander Stepanov, Paul McJones -- Elements of Programming
Notes on Programming

STL: 
1. containers -- vector, list, * map, * set
2. algorithms -- find, sort, lower_bound, upper_bound, copy, trandform, 
	remove, shuffle, partition, ...
3. iterators
	iterator categories -- 
	input iterators, output iterators -- представляют range, по которому
		можно пройти один раз, ++, * -- юзает copy
	forward iterators -- copy, ==, !=, ++, *
	bidirectional -- как forward, и еще --
	random access -- +=, -=, +N, -N, -

template<typename T> 
struct iterator_traints {
	typedef typename T::value_type value_type;
};

list<int>::iterator a, b;
find(a, b, 5);

template<typename U>
struct iterator_traints<U*> {
	typedef U value_type;
}

template<typename T>
It find(It first, It last, typename It::value_type value) {
	while (first != last && *first != value) {
		++first;
	}
	return first;
}

template<typename T, typename P>
It find(It first, It last, P pred) {
	while (first != last && !pred(*first)) {
		++first;
	}
	return first;
}


template<typename InputIterator, typename OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator out) {
	while (first != last)
		*out += *first++;
	return out;
}

Это грустно, потому что не получится копировать большими кусками.
В библиотеке предполагается, что out не  лежит внутри first и last. Зная
это, можно написать copy более эффективно.

template<typename T>
T* copy(T const* first, T const* last, T* out) {
	memcpy(out, first, (last - first) * sizeof(T));
	return out + (last - first);
}

Это ошибка, потму что T вообще любой может быть

template<typename ForwardIterator>
void advance(ForwardIterator& it, size_t n) {
	while (n != 0) {
		++it;
		--n;
	}
	//чоооо
}

template<typename RandomAccessIterator>
void advance(RandomAccessIterator& it, size_t n) {
	it += n;
}



template<typename ForwardIterator, typename Cat>
void advance_impl(ForwardIterator& it, size_t n, Cat) {
	while (n != 0) {
		++it;
		--n;
	}
}

template<typename RandomAccessIterator>
void advance_impl(RandomAccessIterator& it, size_t n, random_access_iterator_tag) {
	it += n;
}


template<typename T>
void advance(T& it, size_t n) {
	advance_impl(it, n, typename iterator_traits <T>::category());
}


template<typename T, typename V>
void f(U, V);

template<typename T>
void f(U, int);



template<typename T>
void f(T);

template<>
void f<int>(int);

template<typename T>
void f(T);

void f(int);


Разница в том, что в первом случае вызывается вторая функция, а во втором
генерится из первой. 
В первом случае есть одна перегрузка, оторая имеет две специализации
Во втором случае есть две перегрузки, у каждой из которых одна специализация

template<typename T>
void f(T*);

template<>
void f<int>(int*);

template<typename T>
void f(T*);

void f(int*);

Как будет работать f(0)? Не сможет понять, что такое T.































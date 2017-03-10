Всякие странные примеры

1. min_element

хотим штуку, которая принимает n аргументов типа T и находит среди них минимум

что делать, когда дали 0 элементов? Можно
- сказать, что есть специальный элемент -- максимальное
- сказать, что это undefined behavior
- бросить исключение

template<typename T>
T min_element(T const* data, size_t size) {
	T result = ???
	for (size_t i = 0; i < size; i++) {
		result = min(result, data[i]);
	}
	return result;
}

такая функция не работает, когда:
- тип Т нельзя копировать
- тип Т нельзя сравнить

что делать? 
можно говорить, что result = max_value, как это реализовать:

template<typename T>
struct max_value;

template<>
struct max_value<int> {
	static int const result = 0x7fffffff;
};

и так сделать специализацию для каждого нужного типа. Использовать:

T result = max_value<T>::result;

пример, когда мы не можем сказать, что встроенные типы должны об этом знать
хотим проверять, переполнится ли умножение

хотим уметь из Signed делать тип в два раза больше

template<typename Signed>
bool signed_mul_overflow(Signed a, Signed b) {
	typedef typename twice<Signed>::type twice_t;
	twice_t c = twice_t(a) * twice_t(b);
	return c < numeric_limits(Signed)::min() 
		|| c > numeric_limits(Signed)::max();
}

как делать сорт

template<typename T>
void sort(T* data, size_t size, bool (*less)(T const&, T const&));
+ объектный код генерится в одном экземпляре
+ значение less можно получать в рантайме
+ можно компилировать сортировку и компараторы независимо -- 
	динамический полиморфизм

template<typename T, typename C>
void sort(T* data, size_t size, C less) {
	less(data[i], data[j]);
}

template<typename T>
struct less {
	bool operator()(T a, T b) {
		return a < b;
	}
}

sort(a, b, less<int>());
+ тут будет оптимизация в компайл тайме, поэтому это будет быстрее 
	В ДВА СУКА С ПОЛОВИНОЙ РАЗА
	но тащемта не всегда это более эффективно, но часто
+ flexibility
	

полиморфизм бывает разный -- compile time, runtime (статический, динамический)

вот типа есть бимап, и там есть типа итераторы, и как там не копировать код

struct left_tag;
struct right_tag;

template<typename Left, typename Right>
struct bimap {
	template<typename Tag>
	struct iterator {
		node* current;
		typedef typename choose_type<Left, Right, Tag>::result value_type;
		value_type const& operator*() const {
			return current;
		}
	}

	typedef iterator<left_tag> left_iterator;
	typedef iterator<right_type> right_iterator;
}

template<typename L, typename R, typename Tag>
struct choose_type;

template<typename L, typename R>
struct choose_type<L, R, left_tag> {
	typedef L result;
}

и вторую специализацию

можно:
в бимапе
typedef typename tag_traits<Left, Right, Tag>::value_type value_type;

template<typename L, typename R, typename Tag>
struct tag_traits;

template<typename L, typename R>
struct tag_traits<L, R, left_tag> {
	typedef L value_type;
	static value_type& get_value(node* n) {
		return n->left_data;
	}
}

такую штуку можно улучшить в смысле количества генерируемого кода
сейчас будет много, потому что bimap<int, double> и bimap<int, float>
для первого дерева генерят по-разному, хотя можно одинаково

template<typename Data>
struct half_node {
	Data data;
	half_node* left;
	half_node* right;
	half_node* parent;
}	

template<typename Left, typename Right>
struct node {
	half_node<Left> left;
	half_node<Right> right;
}

Весёлый хак

#define container_of( ptr, type, member ) \
   ( \
      { \
         const decltype( ((type *)0)->member ) *__mptr = (ptr); \
         (type *)( (char *)__mptr - offsetof( type, member ) ); \
      } \
   )

half_node<int>* p;
node* q = container_of(p, node, left);

работает, откровенно говоря, хуёво:
если передать что-то не то, получится говно

как это можно сделать без анального секса:

можно было сказать, что есть нода, и у нее две базы -- левая и правая
(ладно, с "без анального" погорячился)
надо будет кастить node к left_node

template<typename Data> 
struct half_node {
	Data data;
	half_node *left, *right, *parent;
};

template<typename Left, typename Right>
struct node : half_node<Left>, half_node<Right> {

};

если Left = Right, то мы будем наследоваться от одного типа дважды

Это можно обойти:

template<typename Data, template Tag> 
struct half_node {
	Data data;
	half_node *left, *right, *parent;
};

template<typename Left, typename Right>
struct node : half_node<Left, left_tag>, half_node<Right, right_tag> {

};

Но это не имеет смысла, потому что все, что мы пытались сжать по количеству
генерируемого кода, разожмется

Давайте сделаем обертку над оберткой над оберткой нахуй

какая-то ебола про наследования

struct base {
	int a;
}

struct derived : base {
	int b;
}

int main() {
	derived d1;
	derived* d2 = &d1;
	base* b3 = d2;
	derived* d4 = (derived*) b3;
	assert(d2 == d4);
}

это работает. А сейчас напишем неработающее

int main() {
	derived d1;
	derived* d2 = &d1;
	base* b3 = d2;
	base b4 = *b3;
	derived* d5 = (derived*) &b4;
}




















Как получить возвращаемое значение функционального объекта
(зависит от того, насколько модный у тебя компилятор)

template<typename F>
void g(F const& f) {
	? val = f(); -- что поставить вместо "?"
	auto val = f();
	но если надо делать return, то непонятно
}

в C++11 есть decltype (по смыслу typeof)
decltype(f()) val = f();
чтобы делать return, можно

template<typename F>
decltype(F()()) g(F const& f) -- не будет работать, если нет дефолтного конструктора
decltype(f()) очевидно не работает -- завалится по sfinae
decltype(std::declval<F>()())
std::declval<F>() возвращает обжект типа F

trailing return type -- фича C++11
template<typename F>
auto g(F const& f)->decltype(f()) {...}

забавный оффтоп
x::value_type x::f(value_type) -- в момент парсинга возвращаемого значения
	еще не знает, что мы в x, а в момент парсинга возвращаемого значения
	уже понимаем, что мы внутри x

В C++03 юзали вместо trailing return type "протокол" -- договорились, что 
	внутри всех функциональных объектов делали F::result_type. Для 
	функциональных объектов можно запилить traits.

Кстати, для анонимных функций можно не писать тип возвращаемого значения
[]{ return 5.f; }
Получается, компилятор вообще-то умняш и умеет понимать, что возвращается?
Сильное заявление

Простой случай: (работает вроде в C++14)
auto f() {
	return 1 + 2;
}

Сложный случай:
template<typename T>
auto f(T const& obj) {
	return obj.get();
}

template<typename T>
typename T::value_type f(T const& obj) {
	return obj.get();
}

Есть ли разница? Есть. Во втором случае возвращаемое значение указано явно, 
	поэтому будет отсечение по sfinae. В первом случае так не получится.
Второй код эквивалентен:
template<typename T>
auto f(T const& obj)->decltype(obj.get()) {
	return obj.get();
}

Немного про decltype
decltype(var) -- type of var
decltype(expr) -- expr -- rvalue => type of expr
				  expr -- lvalue => type&

int a;
decltype(a) b; == int b;
decltype((a)) c; == int& c;

__________________________________________
Люди захотели объединить signal-ы и данные. Сделать штуку типа value/variable, 
которая будет хранить значение, на изменения которого можно подписаться.
Такая мысль: c = a + b, хочется изменять c при изменении a и b. Из таких штук
можно собрать ациклический граф, и изменять значения при обновлении одного изменять
листьев. Бывает плохо:
int[] a;
size_t b;
c = a[b];
При изменении a апдейт может сначала пойти либо в b, либо в c. 
"Это, блин, проблема" (с) Ванечка
Можно пофиксить -- сделаем топсорт во время апдейта.
Проблемы не заканчиваются:
int[] a;
int[] b;
b[i] = f(a[i]);
Решение --  incremental update, TODO:загуглить
Но приходит многопоточность, и вообще ничего не понятно становится.
Еще lazy/eager update

Бывает вообще концептуально ничего не понять:
width-height-aspect ratio -- что менять при изменении одного вообще хз

Оффтоп про decltype
decltype(auto) x = f();
auto&& x = f();
вроде это одно и то же.

В C++17 есть std::optional:
std::optional<T>
optional(T);
operator=(T);
optional();
operator bool() const; -- вызывается на if(x)
T& operator*(); -- UB, если !valid
T& value();		-- exception, если !valid
bool valid;
... data;

T& value();
T value_or(T const& default_value) const;

Вообще optional печально, потому что храним лишний bool, а из-за 
выравнивания вообще много получается.

в boost есть tribool -- true|false|indeterminate

template<typename... Args>
void emplace(Args... args); -- basic throw гарантия, если контруктор бросил,
							   то valid = false

Есть еще замена union -- variant, только может еще в конструкторы
variant<int, string, vector<float>>
int which;
... data;
по размерам хранит тоже максимум из возможных аргументов.

Есть еще optional in place:
template<typename... Args>
optional(in_place_t, Args... args);
optional<mytype> a(in_place, 1, 2, 3);
nullopt -- переменная типа nullopt_t, у optional есть всякие операторы копирования
от него.

можно делать new(&data), чтобы создавать новый обжект в том же месте.
Но может не сработать. 
Данные надо хранить так -- если n-байтный тип, то хранить его в адресе кратном n
В x86 это не обязательно, но желательно. Если будет невыравнено, то обращение к 
памяти долго.
В этой массив char-ов будет выравниваться по чарам (по 1 байту), а значит 
обращение к памяти будет долго.

Если такая структура легла на границе 64-байтных блоков, то будет вообще 
не работать. Будет два обращения к памяти, intel вроде отказывается с 
этим работать.

alignment структуры = max (alignment_members)

в с++11
появились alignof
alignas
int alignas(8) a; это аргумент объявления. Появился в c++11. 
Теперь можно так реализовывать:
bool valid;
std::aligned_storage<sizeof(T), alignof(T)>::type data;

reinterpret_cast<T*>(&data);
new (&data)T(...);
reinterpret_cast<T*>(&data)->~T();
Анонимные функции

std::vector<int> v;
std::find_if(v.begin(), v.end(), ??? компаратор ???);

можно было бы передать указатель на функцию

bool is_even(int a) {
	return a % 2 == 0;
}

std::find_if(v.begin(), v.end(), is_even);
Плохо, потому что динамический полиморфизм -- что именно будет вызываться
определяется в рантайме, поэтому оптимизации получаются плохо. Этого можно
избежать:

struct is_even_t {
	bool operator()(int a) const {
		return a % 2 == 0;
	}
}

std::find_if(v.begin(), v.end(), is_even_t());
Тогда статический полиморфизм, потому что find_if темплейтный, поэтому
отдельно инстанцируется, когда его вызывать от структуры
В первом случае будет вызываться функция, которая работает с 
_какой-то_ функцией, которая принимает int и возвращает bool, а во
втором сделает отдельный класс для работы именно с этой функцией, поэтому
оптимизации будут лучше

struct is_multiple_of {
	is_multiple_of(int n) : n(n) {}

	bool operator()(int a) {
		return a % n == 0;
	}

private: 
	int n;
}

std::find_if(v.begin(), v.end(), is_multiple_of(n));
проблема в том, что такой код трудно читать и его много

как делают нормальные пацаны

std::find_if(v.begin(), [n](int a) { return a % n == 0; });
Такой код преобразуется в то, что выше

[] -- capture list -- захват из контекста
() -- аргументы
{} -- код

auto f = [](int a) { };
[a, b, c]() -> int { return 42; }

Есть шорткаты
[=, &d, &e, &f](){} -- все переменные по значению, кроме описанных
[&, a, b, c](){} -- все по ссылке, кроме описанных

[]() mutable {} -- будет значить, что operator() не const

Как написать такое:

... func;
if (cond) {
	func = [...](int a){...};
} else {
	func = [...](int a){...};
}

Не хочется делать класс для каждой лямбды, потому что количество кода

std::function<void(int)> func; -- тащемта в C++03 был boost::function

мы в любом случае будем выделять память динамически, потому что функция может
быть большой
struct function {
	typedef void(*deleter_t)(void*);
	typedef void(*caller_t)(int, int);

	template <typename F>
	function(F func) : ptr(new F(std::move(func))), 
					deleter(&function::delete_obj<F>),
					caller(&function::call_obj<F>) {

	}

	~function() {
		deleter(ptr);
	}

	void operator()(int a, int b) const {
		return caller(ptr, a, b);
	}

private:
	template<typename F>
	static void delete_obj(void* p) {
		delete static_cast<F*>(p);
	}

	template<typename F>
	static void call_obj(void* p, int a, int b) {
		return static_cast<F&>(*p)(a, b);
	}

	void* ptr;
	deleter_t deleter;
	caller_t caller;
}

function<void(int, int)>
void(int, int) -- тип -- функция

Как сделать это нормально:

template<typename R, typename ...Params>
struct function<R(Params...)> 
{
	typedef void(*deleter_t)(void*);
	typedef R(*caller_t)(Params...)

	template <typename F>
	function(F func) : ptr(new F(std::move(func))), 
					deleter(&function::delete_obj<F>),
					caller(&function::call_obj<F>) {

	}

	~function() {
		deleter(ptr);
	}

	R operator()(Params... p) const {
		return caller(ptr, std::forward<Params>(p)...);
	}

private:
	template<typename F>
	static void delete_obj(void* p) {
		delete static_cast<F*>(p);
	}

	template<typename F>
	static void call_obj(void* ptr, Params... p) {
		return static_cast<F&>(*ptr)(std::forward<Params>(p)...);
	}

	void* ptr;
	deleter_t deleter;
	caller_t caller;
}

Давайте поговорим о Боге

Есть обертки any -- any_iterator, any_cast
any_iterator<int, forward_iterator>

Давайте поговорим о signal

signal<int, int> s;
connection c1 = s.connect([](int a, int b) {...});
s(1, 2);
connection c1 = s.connect([](int a, int b) {...});
s(3, 4);
c1.disconnect();
s(5, 6);
c2.disconnect();
s(7, 8); -- уже оба дисконнектились, поэтому ничего не произойдет

как реализован signal?

есть проблема -- функции нельзя сравнивать на ==, считать хеши и все такое
можно хранить id и мапить id в слот

template<typename ...Params>
struct signal {
	typedef std::function<void(Params...)> slot_t;
	typedef size_t id_t;

	struct connection {
		connection(signal sig, id_t id) : sig(sig), id(id) {}

		void disconnect() {
			sig->slots.erase(id);
		}

	private:
		signal sig;
		id_t id;
	}

	connection connect(slot_t slot) {
		id_t id = next++;
		slots.insert( {id, std::move(slot)} );
		return connection(this, id);
	}

	_____________________________________
	void operator()(Params... p) const {
		for (auto i = slots.begin(); i != slots.end(); i++) {
			i->second(p...);
		}
	}
	так делать нельзя, потому что можем вызвать connection, после чего он что-то
	вернет и закроется, а мы не сможем перейти к следующему, потому что сделаем
	erase 
	_____________________________________

	void operator()(Params... p) const {
		for (auto i = slots.begin(); i != slots.end(); ) {
			auto j = i;
			++j;
			i->second(p...);
			i = j;
		}
	}

private:
	id_t next;
	std::map<id_t, slot_t> slot;
}

Короче к черту это все, надо так

template<typename ...Params>
struct signal {
	typedef std::function<void(Params...)> slot_t;
	typedef size_t id_t;

	struct connection {
		connection(signal sig, id_t i) : sig(sig), i(i) {}

		void disconnect() {
			i->first = false;
		}

	private:
		signal sig;
		iterator i;
	}


	connection connect(slot_t slot) {
		auto i = slots.insert( {true, std::move(slot)} );
		return connection(this, i);
	}

	void operator()(Params... p) const {
		for (auto i = slots.begin(); i != slots.end(); ) {
			if (i->first()) {
				i->second(p...);
			}
		}
		for (auto i = slots.begin(); i != slots.end(); ) {
			if (!i->first) {
				i = slots.erase(i);
			} else {
				++i;
			}
		}
	}

private:
	mutable size_t rec_depth;
	mutable std::list<id_t, slot_t> slot;
}

это называется reentrancy.
сейчас люди, которые заводили глобальные переменные тогда, когда надо 
было заводить локальные, уже передохли




















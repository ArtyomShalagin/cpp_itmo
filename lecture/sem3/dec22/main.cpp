Минутка Скакова: при записи в память данные сначала сохраняются в
store_buffer, и работа процессора при этом не особо останавливается.
Все выглядит в духе
|    memory    |
       |
|      L3      |
       |
|  L2  ||  L2  |
    |       |
|L1||L1||L1||L1|
  |   |   |   |
|sb||sb||sb||sb|
 |    |   |   |
|C|  |C| |C| |C|

Но для программы поддерживается иллюзия, что все выглядит так
|  memory  |
 |  |  |  |
|C||C||C||C| 

Поддержка такого иногда бывает не очень дешевой и вообще не особо понятно,
как это делать. (особенно если ты не ботал Скакова)

Пример про потоки
std::atomic<bool> flag1, flag2;

1:
flag1 = true;
if (flag2)
	retry();
else
	work();

2:
flag2 = true;
if (flag1)
	retry();
else
	work();

Нам важно, чтобы read и write не менялись местами (первые 2 строки 
	были в таком порядке)
x86 слишком жесткий, он даже не позволяет менять местами 2 рида, поэтому
иногда работает меденнее, чем мог бы. Но при этом на x86 код выше работает, 
а на остальных если бы это был не atomic, а просто bool, все сломалось бы, 
потому что read и write по мнению некоторых архитектур (ARM v7, POWER)
можно переупорядочивать.
По этому поводу есть специальная инструкция, условно memory_barrier(), которая
говорит, что то, что до и после этого вызова, не может быть поменяно местами.
Но вроде memory_barrier практически никогда не надо использовать.

int x, y, z;

void f() {
	x = 1;
	m.lock();
	y = 2;

	m.unlock();
	z = 3;
}

Можно ли тут что-то переставить местами? Можно поставить x под lock.
Тут возникает race condition. А можно x перенести после m.unlock()?

void g() {
	m.lock();
	int y_copy = y;
	m.unlock();
	if (y == 2) {
		assert(x == 1);
	}
}

Поэтому переносить после unlock-а нельзя.
А можно ли z = 3 засунуть до unlock или до lock? До unlock-а можно, 
до lock-а нельзя.
В итоге нельзя перемещать операции до лока, после анлока и нельзя 
менять lock и unlock местами.
// почитать про relax memory model
Какие бывают ограничения на ordering?
memory_order_seq_cst(default);
- memory_order_relaxed
std::atomic<int> counter;
void work() {
	counter.fetch_add(1, memory_order_relaxed);
}
Само число увеличится, но в какой момент это произойдет, нам не важно.

std::thread th(work);
th.join();
counter.load(memory_order_relaxed);

- consume
- acquire
- release
- acquire_release

std::atomic<int> data;
std::atomic<bool> ready;

1:
data.store(42, relaxed);
ready.store(true, release);

2:
if (ready.load(acquire)) {
	data.load(relaxed);
}

По идее если все load-ы заменить на acquire, а все store-ы на relaxed, 
то ничего не должно ломаться.
Пример, когда это не так (вообще надо минимум 3 потока, но мы
	сделаем 4, потому что мы можем):

1:
x.store(1);

2:
y.store(1);

3:
int xx = x.load();
int yy = y.load();
if (xx == 1 && yy == 0) {
	printf("x before y");
}

4:
int yy = y.load();
int xx = x.load();
if (yy == 1 && xx == 0) {
	printf("y before x");
}

Запускаем эти 4 треда и смотрим, что происходит. Утверждается, что 
при sequential consistant кодом невозможно, что они оба распечатают.
Но если все store-ы сделаем relaxed, все load-ы сделаем acquire,
то это не выполняется, один тред может считать, что сначала инкрементился x,
а второй, что сначала y.

___________________________________________________

std::condition_variable

struct concurrent_queue {
	void push(T data) {
		std::lock_guard<std::mutex> lg(m);
		q.push_back(std::move(data));
		cv.notify_one();
	}

	std::optional<T> try_pop() {
		std::lock_guard<std::mutex> lg(m);
		if (q.empty()) {
			return nullopt;
		}
		std::optional<T> result(q.front());
		q.pop_front();
		return result;
	}

	T pop() {
		std::lock_guard<...> lg(m);
		while (q.empty()) {
			cv.wait();
			алерт: мы сделали лок, но все еще держим mutex, поэтому
			никакой другой метод точно не сможет сделать notify

			можно сделать так:
			lg.unlock();
			cv.wait();
			lg.lock();
		}
		T result = q.front();
		q pop_front();
		return result; 
	}

private:
	std::deque<T> q;
	std::mutex m;
	std::condition_variable cv;
}

ошибка thundering herd -- надо погуглить

Почитать: c++ concurrency in action


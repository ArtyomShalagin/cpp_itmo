Multithreading

Multithreading -- shared memory -- concurrent
Multiprocessing -- no shared memory -- distributed

Вообще работает примерно как в джаве, есть 
std::thread, которому можно в конструктор передать функцию, для
него так же можно сделать join, и все такое. 

До многоядерных процессоров
ускорение выполнения программ 
out-of-order execution
SIMD
Многопоточность была и раньше и реализовалась с помощью прерываний.
Можно также несколько компьютеров использовать с одной расшареной памятью, 
или без шареной памяти (concurrent и distributed соответственно)
Программа может не выигрывать от распараллелирования, так как у процессор, 
диск и оперативная память в одном экземпляре и все может упираться в их ресурс
Привет Скакову

std::thread th([]()
{
	...
});
th.join() -- дождаться выполнения потока
th.detach() -- какая-то функция, до конца не понял. Функция нужна очень редко, 
в очень паталогических случаях.

Поток начинает выполнение сразу после объявления, если в него передана функция
Также можно создать с помощью пустого конструктора, а потом запустить уже на 
функции. Это легковесная обертка, у нее небольшой интерфейс

Проблемы многопоточности:
std::array<int, 10000> accounts;

void transfer(size_t from, size_t to, int account) 
{
	if (accounts[from] < amount)
		throw insufficient_fund();
	accounts[from] -= amount;
	accounts[to] += amount;
}

Проблемы в том, что можем списать одновременно x денег с одного аккаунта и у 
нас может стать меньше 0 денег. В разных потоках проерка прошла без проблем. 
Также если две операции на одном аккаунте, мы можем перекрыть запись в 
accounts[to], так как там три атомарные операции.

Mutexes

lock, unlock
std::mutex m;
Можно делать m.lock и m.unlock, но это вроде не эксепшен сейф.
Можно так
std::lock_guard<std::mutex> lg(m);

Закон Амдала -- насколько ускорится программа при использовании N потоков.
a -- serial, 1 - a -- parallel
1 / (a + (1 - a) / N)

Весёлые примерчики экран покажет ваш

struct account {
	int money;
	std::mutex m;	
}

std::array<account, 10000> accounts;

void transfer(size_t from, size_t to, int account) 
{
	std::lock_guard<std::mutex> lg(accounts[from], m);
	std::lock_guard<std::mutex> lg2(accounts[to], m);
	if (accounts[from] < amount)
		throw insufficient_fund();
	accounts[from] -= amount;
	accounts[to] += amount;
}

race condition -- когда результат работы программы зависит от того, в каком
порядке выполнятся потоки.
dead lock -- когда два потока залочатся и будут бесконечно ждать друг друга.

void transfer(size_t from, size_t to, int account) 
{
	if (from == to) return;
	std::lock_guard<std::mutex> lg(accounts[min(from, to)], m);
	std::lock_guard<std::mutex> lg2(accounts[max(from, to)], m);
	if (accounts[from] < amount)
		throw insufficient_fund();
	accounts[from] -= amount;
	accounts[to] += amount;
}

Можно сделать граф, в котором есть ребро из a в b, если а может, удерживая 
mutex а, залочить b. Надо, чтобы не было циклов, иначе все оч плохо.

Вообще mutex-ы бывают разные. Например, std::recursive_mutex

Есть еще такая штука:
std::atomic<int> a;
a.store(int);
int b = a.load();

busy-wait, spin-lock















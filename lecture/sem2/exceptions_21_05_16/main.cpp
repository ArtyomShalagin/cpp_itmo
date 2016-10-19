ВИРТУАЛЬНОЕ НАСЛЕДОВАНИЕ

struct a
{
	int x;
	virtual void f() = 0;
};

struct b : virtual a 
{};

struct c : virtual a
{}; 

struct d : b, c //будет два x, от b и от c
{
	virtual void f() {}
};

int main() {
	d dd;
	dd.x; //не работает
	dd.b::x; 
	:: -- синтаксис разыменования scope
	при виртуальном наследовании предки склеиваются. Если f была перегружена в c, 
	но не перегружена в b, то при обычном наследовании в d она тоже будет не 
	перегружена, потому что будет путь до не перегруженной через b. Если их склеить, 
	то перегрузка в одном из предков будет ок
	 a 			a a
	/ \ 		| |
	b c вместо 	b c
	 \/			\ /
	 d 			 d
	но при этом при виртуальном наследовании если реализовать и в b, и в с, но не 
	написать будет ошибка компиляции. Но если при этом написать отдельно 
	и в d, то будет норм
	если прописано в обоих предках, но не написано в d, то можно 
	сказать c::f() или d::f()


	a& aa = dd; 
	error, если наследование не virtual
	a& aa = (b&) dd; //будет работать и при обычном наследовании

	return 0;
}

Пример:

#include <iostream>

struct game_object
{
	virtual void update(double dt) = 0;
	virtual void render() = 0;
};

struct static_object : virtual game_object
{
	void update(double) {}
};

struct const_velocity_object : virtual game_object
{
	float x, y, dx, dy;
	void update(double dt) {
		x += dx * dt;
		y += dy * dt;
	}
};

struct billboard_object : virtual game_object
{
	void render();
};

struct model_object : virtual game_object
{
	void render();
};

struct projectile : const_velocity_object, billboard_object
{
	тут создать можно только projectile, потому что только у него реализованы все 
	нужные методы. без виртуального наследования так не получилось бы, потому что
	у одного из предков методы были бы не перегружены
};

в стандартной библиотеке тоже используется виртуальное наследование
есть istream и ostream -- имеют общего предка ios_base
и есть класс iostream, который наследуется от обоих

как в памяти лежит projectile -- game_object в конце, потому что общий
|const_velocity_object|billboard_object| |game_object|
предкам надо знать offset смещения до общего класса
делается таблица виртуальных баз -- хранит оффсеты, как таблица виртуальных функций
в gcc таблицы виртуальных функций и баз объединены

_______________

ИСКЛЮЧЕНИЯ

#include <iostream>

using namespace std;

int g() {
	//lalala
	return 1;
}

Как было раньше:

int f() {
	int err = g();
	if (err != ok) {
		return err;
	}
	if (err == ok) {
		return ok;
	}
	return 1;
}

Как теперь:

struct x {

};

void f() {
	throw 1;
}

int main() {
	long long cnt = 0;
a:
	cout << "nikita pidor " << cnt << endl;
	cnt++;
	goto a;

	try {
		f();
	} catch (int a) {
		cout << "err " << endl;
	}
}























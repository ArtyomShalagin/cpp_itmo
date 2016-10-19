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
	//тут создать можно только projectile, потому что только у него реализованы все 
	//нужные методы. без виртуального наследования так не получилось бы, потому что
	//у одного из предков методы были бы не перегружены
};

//в стандартной библиотеке тоже используется виртуальное наследование
//есть istream и ostream -- имеют общего предка ios_base
//и есть класс iostream, который наследуется от обоих

//как в памяти лежит projectile -- game_object в конце, потому что общий
// |const_velocity_object|billboard_object| |game_object|
//предкам надо знать offset смещения до общего класса
//делается таблица виртуальных баз -- хранит оффсеты, как таблица виртуальных функций
//в gcc таблицы виртуальных функций и баз объединены
#include <cstdlib>

int main()
{
	int* p = nullptr;

	delete p; // ок
	delete nullptr; //ошибка - надо удалять указатель???
	delete NULL; //ошибка - пытаемся удалить 0 а удалять надо указатель
}